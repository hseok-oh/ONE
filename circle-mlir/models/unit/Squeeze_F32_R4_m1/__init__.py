import torch


# Generate Squeeze operator with Float32, Rank-4, multiple 1 in input
class net_Squeeze(torch.nn.Module):
    def __init__(self):
        super().__init__()

    def forward(self, input):
        return torch.squeeze(input)

    def onnx_opset_version(self):
        # NOTE onnx-tf fails version >= 13
        return 13


_model_ = net_Squeeze()

_inputs_ = torch.randn(2, 1, 1, 3)
