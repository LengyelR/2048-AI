import os
import shutil


def try_delete(del_func, path):
    try:
        del_func(path)
    except:
        print(f'deleting {path} failed')
    
    
def del_folder(path):
    try_delete(shutil.rmtree, path)

    
def del_file(path):
    try_delete(os.remove, path)
        
    
if __name__ == "__main__":        
    cwd = os.path.dirname(os.path.abspath(__file__))
    
    del_folder(os.path.join(cwd, 'build'))
    del_file('simulator.cp36-win_amd64.pyd')
    del_file('simulator.cpp')
