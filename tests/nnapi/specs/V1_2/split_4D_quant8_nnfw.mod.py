# model
model = Model()
i1 = Input("op1", "TENSOR_QUANT8_ASYMM", "{2,2,2,2}, 0.5f, 1")
axis = Int32Scalar("axis", 0)
num_out = Int32Scalar("num_out", 2)
i2 = Output("op2", "TENSOR_QUANT8_ASYMM", "{1,2,2,2}, 0.5f, 1")
i3 = Output("op3", "TENSOR_QUANT8_ASYMM", "{1,2,2,2}, 0.5f, 1")
model = model.Operation("SPLIT", i1, axis, num_out).To([i2, i3])

# Example 1. Input in operand 0,
input0 = {i1: # input 0
          [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16]}

output0 = {
    i2: # output 0
          [1, 2, 3, 4, 5, 6, 7, 8],
    i3: # output 1
          [9, 10, 11, 12, 13, 14, 15, 16]}

# Instantiate an example
Example((input0, output0))
