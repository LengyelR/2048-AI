""" slightly modifed controller of nneonneo """"

import urllib.request as request
import json
import threading
import itertools
import time
import math
import websocket  # pip install websocket-client


class ChromeDebuggerControl:
    """ Control Chrome using the debugging socket.
    start chrome with:
    & "C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" --remote-debugging-port=9222  --incognito gabrielecirulli.github.io/2048/
    """

    def __init__(self, port):
        pages = json.loads(request.urlopen(f'http://localhost:{port}/json/list').read())
        if len(pages) == 0:
            raise Exception("No pages to attach to!")

        page = pages[0]

        wsurl = page['webSocketDebuggerUrl']
        self.ws = websocket.create_connection(wsurl)

        self.requests = {}  # dictionary containing in-flight requests
        self.results = {}
        self.req_counter = itertools.count(1)

        self.thread = threading.Thread(target=self._receive_thread)
        self.thread.daemon = True
        self.thread.start()

        self._send_cmd_noresult('Runtime.enable')

    def _receive_thread(self):
        """ Continually read events and command results """
        while 1:
            try:
                message = json.loads(self.ws.recv())
                if 'id' in message:
                    message_id = message['id']
                    event = self.requests.pop(message_id, None)
                    if event is not None:
                        self.results[message_id] = message
                        event.set()
            except Exception:
                break

    def _send_cmd_noresult(self, method, **params):
        """ Send a command and ignore the result. """
        id = next(self.req_counter)
        out = {'id': id, 'method': method}
        if params:
            out['params'] = params
        self.ws.send(json.dumps(out))

    def _send_cmd(self, method, **params):
        """ Send a command and wait for the result to be available. """
        id = next(self.req_counter)
        out = {'id': id, 'method': method}
        if params:
            out['params'] = params

        # Receive thread will signal us when the response is available
        event = threading.Event()
        self.requests[id] = event
        self.ws.send(json.dumps(out))
        event.wait()

        resp = self.results.pop(id)
        if 'error' in resp:
            raise Exception("error in response!")
        return resp['result']

    def execute(self, cmd):
        resp = self._send_cmd('Runtime.evaluate', expression=cmd)

        if 'exceptionDetails' in resp:
            raise Exception(f"JS evaluation threw an error: {resp['result']['description']}")

        result = resp['result']
        if 'value' in result:
            return result['value']

        if 'description' in result:
            return result['description']

        return None


class GameControl:
    """ Control 2048 by hooking the GameManager and executing its move() function.
    This is both safer and faster than the keyboard approach, but it is less compatible with clones. """

    def __init__(self, chrome):
        self.chrome = chrome
        self.setup()

    def _execute(self, cmd):
        return self.chrome.execute(cmd)

    def send_key_event(self, action, key):
        # Use generic events for compatibility with Chrome, which (for inexplicable reasons)
        # doesn't support setting keyCode on KeyboardEvent objects.
        # See http://stackoverflow.com/questions/8942678/keyboardevent-in-chrome-keycode-is-0.
        return self._execute(f'''
               var keyboardEvent = document.createEventObject 
                    ? document.createEventObject() 
                    : document.createEvent("Events");
                    
               if(keyboardEvent.initEvent)
                   keyboardEvent.initEvent("{action}", true, true);
                   
               keyboardEvent.keyCode = {key};
               keyboardEvent.which = {key};
               var element = document.body || document;
               
               element.dispatchEvent 
                    ? element.dispatchEvent(keyboardEvent) 
                    : element.fireEvent("on{action}", keyboardEvent);
               ''')

    def setup(self):
        # Obtain the GameManager instance by triggering a fake restart.
        self._execute(
            '''
            var _func_tmp = GameManager.prototype.isGameTerminated;
            GameManager.prototype.isGameTerminated = function() {
                GameManager._instance = this;
                return true;
            };
            ''')

        # Send an "up" event, which will trigger our replaced isGameTerminated function
        self.send_key_event('keydown', 38)
        time.sleep(0.1)
        self.send_key_event('keyup', 38)

        self._execute('GameManager.prototype.isGameTerminated = _func_tmp;')

    def get_status(self):
        """ Check if the game is in an unusual state. """
        return self._execute('''
            if(GameManager._instance.over) {"ended"}
            else if(GameManager._instance.won && !GameManager._instance.keepPlaying) {"won"}
            else {"running"}
            ''')

    def get_score(self):
        return self._execute('GameManager._instance.score')

    def get_board(self):
        # Chrome refuses to serialize the Grid object directly through the debugger.
        grid = json.loads(self._execute('JSON.stringify(GameManager._instance.grid)'))

        board = [[0]*4 for _ in range(4)]
        for row in grid['cells']:
            for cell in row:
                if cell is None:
                    continue
                pos = cell['x'], cell['y']
                tval = cell['value']
                board[pos[1]][pos[0]] = int(round(math.log(tval, 2)))

        return board

    def get_max_tile(self):
        board = self.get_board()
        return max([item for sublist in board for item in sublist])

    def execute_move(self, move):
        # We use UDLR ordering; 2048 uses URDL ordering
        move = [0, 2, 3, 1][move]
        self._execute(f'GameManager._instance.move({move})')

    def continue_game(self):
        """ Continue the game. Only works if the game is in the 'won' state. """
        self._execute('document.querySelector(".keep-playing-button").click();')

    def restart_game(self):
        self.send_key_event('keydown', 82)
        time.sleep(0.1)
        self.send_key_event('keyup', 82)

        self.send_key_event('keydown', 32)
        time.sleep(0.1)
        self.send_key_event('keyup', 32)


if __name__ == "__main__":
    import random
    import time

    ctrl = ChromeDebuggerControl(9222)
    game = GameControl(ctrl)
	
    while True:
        game.execute_move(random.randint(0,3))
        time.sleep(0.3)
        if game.get_status() == 'ended':
            break
