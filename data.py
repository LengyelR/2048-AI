import torch
from torch.utils.data import Dataset


class GameHistory(Dataset):

    def __init__(self):
        self.states = []
        self.actions = []
        self.rewards = []


    def __len__(self):
        return len(self.states)

    def __getitem__(self, idx):
        return self.states[idx], self.actions[idx], self.rewards[idx]

    def append(self, *sar):
        self.states.append(sar[0])
        self.actions.append(sar[1])
        self.rewards.append(sar[2])

    def extend(self, *sar_list):
        self.states.extend(sar_list[0])
        self.actions.extend(sar_list[1])
        self.rewards.extend(sar_list[2])


if __name__ == "__main__":
    from torch.utils.data import DataLoader

    my_data = GameHistory()
    states = torch.randint(0, 16, (50, 1, 4, 4))
    action_pi = torch.rand(50, 4)
    zs = torch.ones(50)
    my_data.extend(states, action_pi, zs)

    data_loader = DataLoader(my_data, batch_size=5)
    for i, batch in enumerate(data_loader):
        print(i, batch)
