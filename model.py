import torch
import torch.nn as nn


class ResidualBlock(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = nn.Conv2d(16, 16, 3, padding=1)
        self.bn1 = nn.BatchNorm2d(16)

        self.conv2 = nn.Conv2d(16, 16, 3, padding=1)
        self.bn2 = nn.BatchNorm2d(16)

        self.relu = nn.ReLU(inplace=True)

    def forward(self, x):
        residual = x

        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)

        out = self.conv2(out)
        out = self.bn2(out)
        out = self.relu(out)

        out = out + residual

        out = self.relu(out)
        return out


class ValueHead(nn.Module):
    def __init__(self):
        super().__init__()
        self.relu = nn.ReLU(inplace=True)
        self.conv = nn.Conv2d(16, 1, 1)  # 16 channel input, 1 filter of kernel size 1x1
        self.bn = nn.BatchNorm2d(1)
        self.fc1 = nn.Linear(3*3, 3)
        self.fc2 = nn.Linear(3, 1)
        self.tanh = nn.Tanh()

    def forward(self, x):
        x = self.conv(x)
        x = self.bn(x)
        x = self.relu(x)

        x = x.view(-1, 3*3)
        x = self.fc1(x)
        x = self.relu(x)

        x = self.fc2(x)
        x = self.tanh(x)

        return x


class PolicyHead(nn.Module):
    def __init__(self):
        super().__init__()
        self.relu = nn.ReLU(inplace=True)

        self.conv = nn.Conv2d(16, 2, 1)  # 16 channel input, 2 filters of kernel size 1x1
        self.bn = nn.BatchNorm2d(2)
        self.fc = nn.Linear(2*3*3, 4)

    def forward(self, x):
        x = self.conv(x)
        x = self.bn(x)
        x = self.relu(x)
        x = x.view(-1, 2*3*3)
        x = self.fc(x)
        # returns logit probabilities
        return x

# similarly to alphazero: value + policy head, res tower, conv blocks
class Net(nn.Module):
    def __init__(self, res_block, value_head, policy_head):
        super().__init__()
        self.block = res_block
        self.relu = nn.ReLU(inplace=True)

        self.conv1 = self.conv_block()
        self.res_tower = self.residual_tower()
        self.policy_head = policy_head()
        self.value_head = value_head()

    def conv_block(self):
        conv1 = nn.Conv2d(1, 16, 2)  # 1 channel input, 16 filters of kernel size 2x2
        batch_norm = nn.BatchNorm2d(16)
        return nn.Sequential(conv1, batch_norm, self.relu)

    def residual_tower(self):
        layers = [self.block() for _ in range(10)]
        return nn.Sequential(*layers)

    def forward(self, x):
        x = self.conv1(x)
        x = self.res_tower(x)

        v = self.value_head(x)
        p = self.policy_head(x)

        return p, v


if __name__ == "__main__":
    import time

    def get_data(size):
        return torch.randint(0, 16, (size, 1, 4, 4))  # batch, channel, W, H

    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

    net = Net(ResidualBlock, ValueHead, PolicyHead).to(device)

    with torch.no_grad():
        data = get_data(10000).to(device)

        start = time.time()

        policy, value = net(data)
		
        print('elapsed:', time.time()-start)
        print('policy:', policy)
        print('value:', value[:5])
