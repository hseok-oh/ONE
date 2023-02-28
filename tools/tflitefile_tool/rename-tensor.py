#!/usr/bin/env python

# Copyright (c) 2023 Samsung Electronics Co., Ltd. All Rights Reserved
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import argparse
import tflite.Buffer
import tflite.Model
import tflite.Operator
import tflite.OperatorCode
import tflite.SubGraph
import tflite.Tensor
import tflite.QuantizationParameters
import tflite.BuiltinOptions
import flatbuffers
import pkg_resources


# On flatbuffers 2.0, EndVector doesn't require length argument any more.
# But flatbuffers under 2.0 (ex. 1.12) requires length argument.
# We need this workaround until we abandon flatbuffers 1.12.
# Reference: https://github.com/google/flatbuffers/issues/6858
def EndVector(builder, len):
    flat_version = pkg_resources.get_distribution('flatbuffers').version
    if pkg_resources.parse_version(flat_version) < pkg_resources.parse_version("2.0"):
        return builder.EndVector(len)
    else:
        return builder.EndVector()


def GenerateVector(builder, offsets):
    for offset in reversed(offsets):
        builder.PrependUOffsetTRelative(offset)

    return EndVector(builder, len(offsets))


def GenerateOperatorCodes(new_builder, input_model):
    operator_code_num = input_model.OperatorCodesLength()
    operator_code_string_dic = {}
    operator_code_list = list()

    if operator_code_num == 0:
        return 0

    # Create custom operator_code string
    for operator_code_idx in range(operator_code_num):
        operator_code_string = input_model.OperatorCodes(operator_code_idx).CustomCode()
        if operator_code_string and (operator_code_string != "") and (
                not operator_code_string in operator_code_string_dic):
            operator_code_string_dic[operator_code_string] = new_builder.CreateString(
                operator_code_string)

    # Create tables of operator_code
    for operator_code_idx in range(operator_code_num):
        operator_code = input_model.OperatorCodes(operator_code_idx)
        # Create operator_code table
        tflite.OperatorCode.OperatorCodeStart(new_builder)
        tflite.OperatorCode.OperatorCodeAddBuiltinCode(new_builder,
                                                       operator_code.BuiltinCode())

        operator_code_string = operator_code.CustomCode()
        if operator_code_string in operator_code_string_dic:
            tflite.OperatorCode.OperatorCodeAddCustomCode(
                new_builder, operator_code_string_dic[operator_code_string])
        operator_code_list.append(tflite.OperatorCode.OperatorCodeEnd(new_builder))

    # Create operator_code vector
    tflite.Model.ModelStartOperatorCodesVector(new_builder, operator_code_num)
    return GenerateVector(new_builder, operator_code_list)


def GenerateQuantization(new_builder, quantization):
    # Create min vector
    min_num = quantization.MinLength()
    if min_num != 0:
        tflite.QuantizationParameters.QuantizationParametersStartMinVector(
            new_builder, min_num)
        for min_idx in reversed(range(min_num)):
            new_builder.PrependFloat32(quantization.Min(min_idx))
        new_min = EndVector(new_builder, min_num)

    # Create max vector
    max_num = quantization.MaxLength()
    if max_num != 0:
        tflite.QuantizationParameters.QuantizationParametersStartMaxVector(
            new_builder, max_num)
        for max_idx in reversed(range(max_num)):
            new_builder.PrependFloat32(quantization.Max(max_idx))
        new_max = EndVector(new_builder, max_num)

    # Create scale vector
    scale_num = quantization.ScaleLength()
    if scale_num != 0:
        tflite.QuantizationParameters.QuantizationParametersStartScaleVector(
            new_builder, scale_num)
        for scale_idx in reversed(range(scale_num)):
            new_builder.PrependFloat32(quantization.Scale(scale_idx))
        new_scale = EndVector(new_builder, scale_num)

    # Create zero_point vector
    zeropoint_num = quantization.ZeroPointLength()
    if zeropoint_num != 0:
        tflite.QuantizationParameters.QuantizationParametersStartZeroPointVector(
            new_builder, zeropoint_num)
        for zeropoint_idx in reversed(range(zeropoint_num)):
            new_builder.PrependInt64(quantization.ZeroPoint(zeropoint_idx))
        new_zeropoint = EndVector(new_builder, zeropoint_num)

    details = quantization.Details()
    if details != None:
        raise Exception("Cannot handle quantization details parameter yet")

    quantized_dimension = quantization.QuantizedDimension()

    # Create quantization
    tflite.QuantizationParameters.QuantizationParametersStart(new_builder)
    if min_num != 0:
        tflite.QuantizationParameters.QuantizationParametersAddMin(new_builder, new_min)
    if max_num != 0:
        tflite.QuantizationParameters.QuantizationParametersAddMax(new_builder, new_max)
    if scale_num != 0:
        tflite.QuantizationParameters.QuantizationParametersAddScale(
            new_builder, new_scale)
    if zeropoint_num != 0:
        tflite.QuantizationParameters.QuantizationParametersAddZeroPoint(
            new_builder, new_zeropoint)

    if quantized_dimension != 0:
        tflite.QuantizationParameters.QuantizationParametersAddQuantizedDimension(
            new_builder, quantized_dimension)

    return tflite.QuantizationParameters.QuantizationParametersEnd(new_builder)


def GenerateTensor(new_builder, tensor, name):

    # Create shape vector for tensor
    shape_num = tensor.ShapeLength()
    tflite.Tensor.TensorStartShapeVector(new_builder, shape_num)
    if shape_num != 0:
        for shape_idx in reversed(range(shape_num)):
            new_builder.PrependInt32(tensor.Shape(shape_idx))
    new_shape = EndVector(new_builder, shape_num)

    # Create tensor_type
    tensor_type = tensor.Type()

    # Create input vector for tensor
    buffer_idx = tensor.Buffer()

    # Create name string = name
    new_name = new_builder.CreateString(name)

    # Create quantization
    quantization = tensor.Quantization()
    if quantization != None:
        new_quantization = GenerateQuantization(new_builder, quantization)

    # Create IsVariable
    is_variable = tensor.IsVariable()

    # Create Sparsity
    sparsity = tensor.Sparsity()
    if sparsity != None:
        raise Exception("Cannot handle sparsity type tensor yet")

    # Create tensor
    tflite.Tensor.TensorStart(new_builder)
    tflite.Tensor.TensorAddShape(new_builder, new_shape)
    tflite.Tensor.TensorAddType(new_builder, tensor_type)
    tflite.Tensor.TensorAddBuffer(new_builder, buffer_idx)
    tflite.Tensor.TensorAddName(new_builder, new_name)
    if quantization != None:
        tflite.Tensor.TensorAddQuantization(new_builder, new_quantization)
    tflite.Tensor.TensorAddIsVariable(new_builder, is_variable)
    # tflite.Tensor.TensorAddSparsity(new_builder, sparsity)

    return tflite.Tensor.TensorEnd(new_builder)


def GenerateTensors(new_builder, input_subg, name_prefix):
    tensor_num = input_subg.TensorsLength()
    new_tensors = list()

    if tensor_num == 0:
        return 0

    for tensor_idx in range(tensor_num):
        new_name = name_prefix + "t" + str(tensor_idx)
        if name_prefix == "":
            for output_idx in range(input_subg.OutputsLength()):
                if input_subg.Outputs(output_idx) == tensor_idx:
                    new_name = "out" + str(output_idx)
            for input_idx in range(input_subg.InputsLength()):
                if input_subg.Inputs(input_idx) == tensor_idx:
                    new_name = "in" + str(input_idx)

        new_tensors.append(
            GenerateTensor(new_builder, input_subg.Tensors(tensor_idx), new_name))

    tflite.SubGraph.SubGraphStartTensorsVector(new_builder, tensor_num)
    return GenerateVector(new_builder, new_tensors)


def GenerateBuiltinOption(new_builder, selected_builtin_option, builtin_option_type):

    # Conv2D option
    import tflite.Conv2DOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().Conv2DOptions:

        conv2d_options = tflite.Conv2DOptions.Conv2DOptions()
        conv2d_options.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.Conv2DOptions.Conv2DOptionsStart(new_builder)
        tflite.Conv2DOptions.Conv2DOptionsAddPadding(new_builder,
                                                     conv2d_options.Padding())
        tflite.Conv2DOptions.Conv2DOptionsAddStrideW(new_builder,
                                                     conv2d_options.StrideW())
        tflite.Conv2DOptions.Conv2DOptionsAddStrideH(new_builder,
                                                     conv2d_options.StrideH())
        tflite.Conv2DOptions.Conv2DOptionsAddDilationWFactor(
            new_builder, conv2d_options.DilationWFactor())
        tflite.Conv2DOptions.Conv2DOptionsAddDilationHFactor(
            new_builder, conv2d_options.DilationHFactor())
        tflite.Conv2DOptions.Conv2DOptionsAddFusedActivationFunction(
            new_builder, conv2d_options.FusedActivationFunction())
        return tflite.Conv2DOptions.Conv2DOptionsEnd(new_builder)

    # DepthwiseConv2D option
    import tflite.DepthwiseConv2DOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).DepthwiseConv2DOptions:

        depthconv2d_option = tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptions()
        depthconv2d_option.Init(selected_builtin_option.Bytes,
                                selected_builtin_option.Pos)

        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsStart(new_builder)
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddPadding(
            new_builder, depthconv2d_option.Padding())
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddStrideW(
            new_builder, depthconv2d_option.StrideW())
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddStrideH(
            new_builder, depthconv2d_option.StrideH())
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddDepthMultiplier(
            new_builder, depthconv2d_option.DepthMultiplier())
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddFusedActivationFunction(
            new_builder, depthconv2d_option.FusedActivationFunction())
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddDilationWFactor(
            new_builder, depthconv2d_option.DilationWFactor())
        tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsAddDilationHFactor(
            new_builder, depthconv2d_option.DilationHFactor())
        return tflite.DepthwiseConv2DOptions.DepthwiseConv2DOptionsEnd(new_builder)

    # ConcatEmbeddingsOptions: not supported
    # LSHProjectionOptions: not supported

    # Pool2DPOption
    import tflite.Pool2DOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().Pool2DOptions:

        pool2d_option = tflite.Pool2DOptions.Pool2DOptions()
        pool2d_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.Pool2DOptions.Pool2DOptionsStart(new_builder)
        tflite.Pool2DOptions.Pool2DOptionsAddPadding(new_builder, pool2d_option.Padding())
        tflite.Pool2DOptions.Pool2DOptionsAddStrideW(new_builder, pool2d_option.StrideW())
        tflite.Pool2DOptions.Pool2DOptionsAddStrideH(new_builder, pool2d_option.StrideH())
        tflite.Pool2DOptions.Pool2DOptionsAddFilterWidth(new_builder,
                                                         pool2d_option.FilterWidth())
        tflite.Pool2DOptions.Pool2DOptionsAddFilterHeight(new_builder,
                                                          pool2d_option.FilterHeight())
        tflite.Pool2DOptions.Pool2DOptionsAddFusedActivationFunction(
            new_builder, pool2d_option.FusedActivationFunction())
        return tflite.Pool2DOptions.Pool2DOptionsEnd(new_builder)

    # SVDFOptions: not supported

    # RNNOptions
    import tflite.RNNOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().RNNOptions:

        rnn_option = tflite.RNNOptions.RNNOptions()
        rnn_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.RNNOptions.RNNOptionsStart(new_builder)
        tflite.RNNOptions.RNNOptionsAddFusedActivationFunction(
            new_builder, rnn_option.FusedActivationFunction())
        return tflite.RNNOptions.RNNOptionsEnd(new_builder)

    # FullyConnectedOptions
    import tflite.FullyConnectedOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).FullyConnectedOptions:

        fc_option = tflite.FullyConnectedOptions.FullyConnectedOptions()
        fc_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.FullyConnectedOptions.FullyConnectedOptionsStart(new_builder)
        tflite.FullyConnectedOptions.FullyConnectedOptionsAddFusedActivationFunction(
            new_builder, fc_option.FusedActivationFunction())
        return tflite.FullyConnectedOptions.FullyConnectedOptionsEnd(new_builder)

    # SoftmaxOptions
    import tflite.SoftmaxOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().SoftmaxOptions:

        softmax_option = tflite.SoftmaxOptions.SoftmaxOptions()
        softmax_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.SoftmaxOptions.SoftmaxOptionsStart(new_builder)
        tflite.SoftmaxOptions.SoftmaxOptionsAddBeta(new_builder, softmax_option.Beta())
        return tflite.SoftmaxOptions.SoftmaxOptionsEnd(new_builder)

    # ConcatenationOptions
    import tflite.ConcatenationOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ConcatenationOptions:

        concat_option = tflite.ConcatenationOptions.ConcatenationOptions()
        concat_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ConcatenationOptions.ConcatenationOptionsStart(new_builder)
        tflite.ConcatenationOptions.ConcatenationOptionsAddAxis(
            new_builder, concat_option.Axis())
        tflite.ConcatenationOptions.ConcatenationOptionsAddFusedActivationFunction(
            new_builder, concat_option.FusedActivationFunction())
        return tflite.ConcatenationOptions.ConcatenationOptionsEnd(new_builder)

    # AddOptions
    import tflite.AddOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().AddOptions:

        add_option = tflite.AddOptions.AddOptions()
        add_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.AddOptions.AddOptionsStart(new_builder)
        tflite.AddOptions.AddOptionsAddFusedActivationFunction(
            new_builder, add_option.FusedActivationFunction())
        return tflite.AddOptions.AddOptionsEnd(new_builder)

    # L2NormOptions
    import tflite.L2NormOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().L2NormOptions:

        l2norm_option = tflite.L2NormOptions.L2NormOptions()
        l2norm_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.L2NormOptions.L2NormOptionsStart(new_builder)
        tflite.L2NormOptions.L2NormOptionsAddFusedActivationFunction(
            new_builder, l2norm_option.FusedActivationFunction())
        return tflite.L2NormOptions.L2NormOptionsEnd(new_builder)

    # LocalResponseNormalizationOptions
    import tflite.LocalResponseNormalizationOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).LocalResponseNormalizationOptions:

        lrn_option = tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptions(
        )
        lrn_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptionsStart(
            new_builder)
        tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptionsAddRadius(
            new_builder, lrn_option.Radius())
        tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptionsAddBias(
            new_builder, lrn_option.Bias())
        tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptionsAddAlpha(
            new_builder, lrn_option.Alpha())
        tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptionsAddBeta(
            new_builder, lrn_option.Beta())
        return tflite.LocalResponseNormalizationOptions.LocalResponseNormalizationOptionsEnd(
            new_builder)

    # LSTMOptions: not supported

    # ResizeBilinearOptions
    import tflite.ResizeBilinearOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).ResizeBilinearOptions:

        resize_bilinear_option = tflite.ResizeBilinearOptions.ResizeBilinearOptions()
        resize_bilinear_option.Init(selected_builtin_option.Bytes,
                                    selected_builtin_option.Pos)

        tflite.ResizeBilinearOptions.ResizeBilinearOptionsStart(new_builder)
        tflite.ResizeBilinearOptions.ResizeBilinearOptionsAddAlignCorners(
            new_builder, resize_bilinear_option.AlignCorners())
        return tflite.ResizeBilinearOptions.ResizeBilinearOptionsEnd(new_builder)

    # CallOptions: not supported

    # ReshapeOptions
    import tflite.ReshapeOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ReshapeOptions:

        reshape_option = tflite.ReshapeOptions.ReshapeOptions()
        reshape_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        shape_num = reshape_option.NewShapeLength()
        if shape_num != 0:
            tflite.ReshapeOptions.ReshapeOptionsStartNewShapeVector(
                new_builder, shape_num)
            for new_shape_idx in reversed(range(shape_num)):
                new_shape_val = reshape_option.NewShape(new_shape_idx)
                new_builder.PrependInt32(new_shape_val)
            new_shape = EndVector(new_builder, shape_num)

        tflite.ReshapeOptions.ReshapeOptionsStart(new_builder)
        if shape_num != 0:
            tflite.ReshapeOptions.ReshapeOptionsAddNewShape(new_builder, new_shape)
        return tflite.ReshapeOptions.ReshapeOptionsEnd(new_builder)

    # SkipGramOptions: not supported

    # SpaceToDepthOptions
    import tflite.SpaceToDepthOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().SpaceToDepthOptions:

        space_to_depth_option = tflite.SpaceToDepthOptions.SpaceToDepthOptions()
        space_to_depth_option.Init(selected_builtin_option.Bytes,
                                   selected_builtin_option.Pos)

        tflite.SpaceToDepthOptions.SpaceToDepthOptionsStart(new_builder)
        tflite.SpaceToDepthOptions.SpaceToDepthOptionsAddBlockSize(
            new_builder, space_to_depth_option.BlockSize())
        return tflite.SpaceToDepthOptions.SpaceToDepthOptionsEnd(new_builder)

    # EmbeddingLookupSparseOptions: not supported

    # MulOptions
    import tflite.MulOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().MulOptions:

        mul_option = tflite.MulOptions.MulOptions()
        mul_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.MulOptions.MulOptionsStart(new_builder)
        tflite.MulOptions.MulOptionsAddFusedActivationFunction(
            new_builder, mul_option.FusedActivationFunction())
        return tflite.MulOptions.MulOptionsEnd(new_builder)

    # PadOptions
    import tflite.PadOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().PadOptions:

        tflite.PadOptions.PadOptionsStart(new_builder)
        return tflite.PadOptions.PadOptionsEnd(new_builder)

    # GatherOptions
    import tflite.GatherOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().GatherOptions:

        gather_option = tflite.GatherOptions.GatherOptions()
        gather_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.GatherOptions.GatherOptionsStart(new_builder)
        tflite.GatherOptions.GatherOptionsAddAxis(new_builder, gather_option.Axis())
        return tflite.GatherOptions.GatherOptionsEnd(new_builder)

    # BatchToSpaceNDOptions
    import tflite.BatchToSpaceNDOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).BatchToSpaceNDOptions:

        btsnd_option = tflite.BatchToSpaceNDOptions.BatchToSpaceNDOptions()
        btsnd_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.BatchToSpaceNDOptions.BatchToSpaceNDOptionsStart(new_builder)
        return tflite.BatchToSpaceNDOptions.BatchToSpaceNDOptionsEnd(new_builder)

    # SpaceToBatchNDOptions
    import tflite.SpaceToBatchNDOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).SpaceToBatchNDOptions:

        stbnd_option = tflite.SpaceToBatchNDOptions.SpaceToBatchNDOptions()
        stbnd_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.SpaceToBatchNDOptions.SpaceToBatchNDOptionsStart(new_builder)
        return tflite.SpaceToBatchNDOptions.SpaceToBatchNDOptionsEnd(new_builder)

    # TransposeOptions:
    import tflite.TransposeOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().TransposeOptions:

        transpose_option = tflite.TransposeOptions.TransposeOptions()
        transpose_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.TransposeOptions.TransposeOptionsStart(new_builder)
        return tflite.TransposeOptions.TransposeOptionsEnd(new_builder)

    # ReducerOptions
    import tflite.ReducerOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ReducerOptions:

        reducer_option = tflite.ReducerOptions.ReducerOptions()
        reducer_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ReducerOptions.ReducerOptionsStart(new_builder)
        tflite.ReducerOptions.ReducerOptionsAddKeepDims(new_builder,
                                                        reducer_option.KeepDims())
        return tflite.ReducerOptions.ReducerOptionsEnd(new_builder)

    # SubOptions
    import tflite.SubOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().SubOptions:

        sub_option = tflite.SubOptions.SubOptions()
        sub_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.SubOptions.SubOptionsStart(new_builder)
        tflite.SubOptions.SubOptionsAddFusedActivationFunction(
            new_builder, sub_option.FusedActivationFunction())
        return tflite.SubOptions.SubOptionsEnd(new_builder)

    # DivOptions
    import tflite.DivOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().DivOptions:

        div_option = tflite.DivOptions.DivOptions()
        div_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.DivOptions.DivOptionsStart(new_builder)
        tflite.DivOptions.DivOptionsAddFusedActivationFunction(
            new_builder, div_option.FusedActivationFunction())
        return tflite.DivOptions.DivOptionsEnd(new_builder)

    # SqueezeOptions
    import tflite.SqueezeOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().SqueezeOptions:

        squeeze_option = tflite.SqueezeOptions.SqueezeOptions()
        squeeze_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        squeeze_dims_num = squeeze_option.SqueezeDimsLength()
        if squeeze_dims_num != 0:
            tflite.SqueezeOptions.SqueezeOptionsStartSqueezeDimsVector(
                new_builder, squeeze_dims_num)
            for squeeze_dims_idx in reversed(range(squeeze_dims_num)):
                squeeze_dims_val = squeeze_option.SqueezeDims(squeeze_dims_idx)
                new_builder.PrependInt32(squeeze_dims_val)
            new_squeeze_dims = EndVector(new_builder, squeeze_dims_num)

        tflite.SqueezeOptions.SqueezeOptionsStart(new_builder)
        if squeeze_dims_num != 0:
            tflite.SqueezeOptions.SqueezeOptionsAddSqueezeDims(new_builder,
                                                               new_squeeze_dims)
        return tflite.SqueezeOptions.SqueezeOptionsEnd(new_builder)

    # SequenceRNNOptions: not supported

    # StridedSliceOptions
    import tflite.StridedSliceOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().StridedSliceOptions:

        stride_slice_option = tflite.StridedSliceOptions.StridedSliceOptions()
        stride_slice_option.Init(selected_builtin_option.Bytes,
                                 selected_builtin_option.Pos)

        tflite.StridedSliceOptions.StridedSliceOptionsStart(new_builder)
        tflite.StridedSliceOptions.StridedSliceOptionsAddBeginMask(
            new_builder, stride_slice_option.BeginMask())
        tflite.StridedSliceOptions.StridedSliceOptionsAddEndMask(
            new_builder, stride_slice_option.EndMask())
        tflite.StridedSliceOptions.StridedSliceOptionsAddEllipsisMask(
            new_builder, stride_slice_option.EllipsisMask())
        tflite.StridedSliceOptions.StridedSliceOptionsAddNewAxisMask(
            new_builder, stride_slice_option.NewAxisMask())
        tflite.StridedSliceOptions.StridedSliceOptionsAddShrinkAxisMask(
            new_builder, stride_slice_option.ShrinkAxisMask())

        return tflite.StridedSliceOptions.StridedSliceOptionsEnd(new_builder)

    # ExpOptions
    import tflite.ExpOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ExpOptions:

        exp_option = tflite.ExpOptions.ExpOptions()
        exp_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ExpOptions.ExpOptionsStart(new_builder)
        return tflite.ExpOptions.ExpOptionsEnd(new_builder)

    # TopKV2Options
    import tflite.TopKV2Options
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().TopKV2Options:

        topkv2_option = tflite.TopKV2Options.TopKV2Options()
        topkv2_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.TopKV2Options.TopKV2OptionsStart(new_builder)
        return tflite.TopKV2Options.TopKV2OptionsEnd(new_builder)

    # SplitOptions
    import tflite.SplitOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().SplitOptions:

        split_option = tflite.SplitOptions.SplitOptions()
        split_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.SplitOptions.SplitOptionsStart(new_builder)
        tflite.SplitOptions.SplitOptionsAddNumSplits(new_builder,
                                                     split_option.NumSplits())
        return tflite.SplitOptions.SplitOptionsEnd(new_builder)

    # LogSoftmaxOptions: not supported

    # CastOptions: not supported
    import tflite.CastOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().CastOptions:

        cast_option = tflite.CastOptions.CastOptions()
        cast_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.CastOptions.CastOptionsStart(new_builder)
        return tflite.CastOptions.CastOptionsEnd(new_builder)

    # DequantizeOptions:
    import tflite.DequantizeOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().DequantizeOptions:

        dequantize_option = tflite.DequantizeOptions.DequantizeOptions()
        dequantize_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.EqualOptions.DequantizeOptionsStart(new_builder)
        return tflite.DequantizeOptions.DequantizeOptionsEnd(new_builder)

    # MaximumMinimumOptions: not supported

    # ArgMaxOptions
    import tflite.ArgMaxOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ArgMaxOptions:

        arg_max_option = tflite.ArgMaxOptions.ArgMaxOptions()
        arg_max_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ArgMaxOptions.ArgMaxOptionsStart(new_builder)
        tflite.ArgMaxOptions.ArgMaxOptionsAddOutputType(new_builder,
                                                        arg_max_option.OutputType())
        return tflite.ArgMaxOptions.ArgMaxOptionsEnd(new_builder)

    # LessOptions: not supported
    import tflite.LessOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().LessOptions:

        equal_option = tflite.LessOptions.LessOptions()
        equal_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.LessOptions.LessOptionsStart(new_builder)
        return tflite.LessOptions.LessOptionsEnd(new_builder)

    # NegOptions
    import tflite.NegOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().NegOptions:

        neg_option = tflite.NegOptions.NegOptions()
        neg_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.NegOptions.NegOptionsStart(new_builder)
        return tflite.NegOptions.NegOptionsEnd(new_builder)

    # EqualOptions
    import tflite.EqualOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().EqualOptions:

        equal_option = tflite.EqualOptions.EqualOptions()
        equal_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.EqualOptions.EqualOptionsStart(new_builder)
        return tflite.EqualOptions.EqualOptionsEnd(new_builder)

    # PadV2Options: not supported

    # GreaterOptions
    import tflite.GreaterOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().GreaterOptions:

        equal_option = tflite.GreaterOptions.GreaterOptions()
        equal_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.GreaterOptions.GreaterOptionsStart(new_builder)
        return tflite.GreaterOptions.GreaterOptionsEnd(new_builder)

    # GreaterEqualOptions
    import tflite.GreaterEqualOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().GreaterEqualOptions:

        equal_option = tflite.GreaterEqualOptions.GreaterEqualOptions()
        equal_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.GreaterEqualOptions.GreaterEqualOptionsStart(new_builder)
        return tflite.GreaterEqualOptions.GreaterEqualOptionsEnd(new_builder)

    # LessEqualOptions
    import tflite.LessEqualOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().LessEqualOptions:

        equal_option = tflite.LessEqualOptions.LessEqualOptions()
        equal_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.LessEqualOptions.LessEqualOptionsStart(new_builder)
        return tflite.LessEqualOptions.LessEqualOptionsEnd(new_builder)

    # SelectOptions
    import tflite.SelectOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().SelectOptions:

        select_option = tflite.SelectOptions.SelectOptions()
        select_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.SelectOptions.SelectOptionsStart(new_builder)
        return tflite.SelectOptions.SelectOptionsEnd(new_builder)

    # SliceOptions: not supported

    # TransposeConvOptions
    import tflite.TransposeConvOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().TransposeConvOptions:

        transposeconv_option = tflite.TransposeConvOptions.TransposeConvOptions()
        transposeconv_option.Init(selected_builtin_option.Bytes,
                                  selected_builtin_option.Pos)

        tflite.TransposeConvOptions.TransposeConvOptionsStart(new_builder)
        tflite.TransposeConvOptions.TransposeConvOptionsAddPadding(
            new_builder, transposeconv_option.Padding())
        tflite.TransposeConvOptions.TransposeConvOptionsAddStrideW(
            new_builder, transposeconv_option.StrideW())
        tflite.TransposeConvOptions.TransposeConvOptionsAddStrideH(
            new_builder, transposeconv_option.StrideH())
        return tflite.TransposeConvOptions.TransposeConvOptionsEnd(new_builder)

    # SparseToDenseOptions: not supported

    # TileOptions
    import tflite.TileOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().TileOptions:

        tile_option = tflite.TileOptions.TileOptions()
        tile_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.TileOptions.TileOptionsStart(new_builder)
        return tflite.TileOptions.TileOptionsEnd(new_builder)

    # ExpandDimsOptions:
    import tflite.ExpandDimsOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ExpandDimsOptions:

        expanddims_option = tflite.ExpandDimsOptions.ExpandDimsOptions()
        expanddims_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ExpandDimsOptions.ExpandDimsOptionsStart(new_builder)
        return tflite.ExpandDimsOptions.ExpandDimsOptionsEnd(new_builder)

    # NotEqualOptions:
    import tflite.NotEqualOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().NotEqualOptions:

        notequal_option = tflite.NotEqualOptions.NotEqualOptions()
        notequal_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.NotEqualOptions.NotEqualOptionsStart(new_builder)
        return tflite.NotEqualOptions.NotEqualOptionsEnd(new_builder)

    # ShapeOptions:
    import tflite.ShapeOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ShapeOptions:

        shape_option = tflite.ShapeOptions.ShapeOptions()
        shape_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ShapeOptions.ShapeOptionsStart(new_builder)
        tflite.ShapeOptions.ShapeOptionsAddOutType(new_builder, shape_option.OutType())
        return tflite.ShapeOptions.ShapeOptionsEnd(new_builder)

    # PowOptions: not supported
    # ArgMinOptions: not supported
    # FakeQuantOptions: not supported

    # PackOptions:
    import tflite.PackOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().PackOptions:

        pack_option = tflite.PackOptions.PackOptions()
        pack_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.PackOptions.PackOptionsStart(new_builder)
        tflite.PackOptions.PackOptionsAddValuesCount(new_builder,
                                                     pack_option.ValuesCount())
        tflite.PackOptions.PackOptionsAddAxis(new_builder, pack_option.Axis())
        return tflite.PackOptions.PackOptionsEnd(new_builder)

    # LogicalOrOptions:
    import tflite.LogicalOrOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().LogicalOrOptions:

        logical_or_option = tflite.LogicalAndOptions.LogicalOrOptions()
        logical_or_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.LogicalOrOptions.LogicalOrOptionsStart(new_builder)
        return tflite.LogicalOrOptions.LogicalOrOptionsEnd(new_builder)

    # OneHotOptions: not supported
    import tflite.OneHotOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().OneHotOptions:

        one_hot_option = tflite.OneHotOptions.OneHotOptions()
        one_hot_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.OneHotOptions.OneHotOptionsStart(new_builder)
        tflite.OneHotOptions.OneHotOptionsAddAxis(new_builder, one_hot_option.Axis())
        return tflite.OneHotOptions.OneHotOptionsEnd(new_builder)

    # LogicalNotOptions
    import tflite.LogicalNotOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().LogicalNotOptions:

        logicalnot_option = tflite.LogicalNotOptions.LogicalNotOptions()
        logicalnot_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.LogicalNotOptions.LogicalNotOptionsStart(new_builder)
        return tflite.LogicalNotOptions.LogicalNotOptionsEnd(new_builder)

    # UnpackOptions:
    import tflite.UnpackOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().UnpackOptions:

        unpack_option = tflite.UnpackOptions.UnpackOptions()
        unpack_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.UnpackOptions.UnpackOptionsStart(new_builder)
        tflite.UnpackOptions.UnpackOptionsAddNum(new_builder, unpack_option.Num())
        tflite.UnpackOptions.UnpackOptionsAddAxis(new_builder, unpack_option.Axis())
        return tflite.UnpackOptions.UnpackOptionsEnd(new_builder)

    # FloorDivOptions: not supported
    # SquareOptions: not supported
    # ZerosLikeOptions: not supported
    # FillOptions: not supported

    # LogicalAndOptions
    import tflite.LogicalAndOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().LogicalAndOptions:

        logical_and_option = tflite.LogicalAndOptions.LogicalAndOptions()
        logical_and_option.Init(selected_builtin_option.Bytes,
                                selected_builtin_option.Pos)

        tflite.LogicalAndOptions.LogicalAndOptionsStart(new_builder)
        return tflite.LogicalAndOptions.LogicalAndOptionsEnd(new_builder)

    # LogicalNotOptions: not supported
    # UnpackOptions: not supported
    # FloorDivOptions: not supported
    # SquareOptions: not supported

    # ZerosLikeOptions
    import tflite.ZerosLikeOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().ZerosLikeOptions:

        zeroslike_option = tflite.ZerosLikeOptions.ZerosLikeOptions()
        zeroslike_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.ZerosLikeOptions.ZerosLikeOptionsStart(new_builder)
        return tflite.ZerosLikeOptions.ZerosLikeOptionsEnd(new_builder)

    # FillOptions: not supported
    # BidirectionalSequenceLSTMOptions: not supported
    # BidirectionalSequenceRNNOptions: not supported
    # FloorModOptions: not supported
    # RangeOptions: not supported
    # ResizeNearestNeighborOptions: not supported

    # LeakyReluOptions
    import tflite.LeakyReluOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().LeakyReluOptions:

        leaky_relu_option = tflite.LeakyReluOptions.LeakyReluOptions()
        leaky_relu_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.LeakyReluOptions.LeakyReluOptionsStart(new_builder)
        tflite.LeakyReluOptions.LeakyReluOptionsAddAlpha(new_builder,
                                                         leaky_relu_option.Alpha())
        return tflite.LeakyReluOptions.LeakyReluOptionsEnd(new_builder)

    # SquaredDifferenceOptions
    import tflite.SquaredDifferenceOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).SquaredDifferenceOptions:

        squared_difference_option = tflite.SquaredDifferenceOptions.SquaredDifferenceOptions(
        )
        squared_difference_option.Init(selected_builtin_option.Bytes,
                                       selected_builtin_option.Pos)

        tflite.SquaredDifferenceOptions.SquaredDifferenceOptionsStart(new_builder)
        return tflite.SquaredDifferenceOptions.SquaredDifferenceOptionsEnd(new_builder)

    # UnidirectionalSequenceLSTMOptions
    import tflite.UnidirectionalSequenceLSTMOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions(
    ).UnidirectionalSequenceLSTMOptions:

        unidirectional_sequence_lstm_option = tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptions(
        )
        unidirectional_sequence_lstm_option.Init(selected_builtin_option.Bytes,
                                                 selected_builtin_option.Pos)

        tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsStart(
            new_builder)
        tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsAddFusedActivationFunction(
            new_builder, unidirectional_sequence_lstm_option.FusedActivationFunction())
        tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsAddCellClip(
            new_builder, unidirectional_sequence_lstm_option.CellClip())
        tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsAddProjClip(
            new_builder, unidirectional_sequence_lstm_option.ProjClip())
        tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsAddTimeMajor(
            new_builder, unidirectional_sequence_lstm_option.TimeMajor())
        tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsAddAsymmetricQuantizeInputs(
            new_builder, unidirectional_sequence_lstm_option.AsymmetricQuantizeInputs())
        return tflite.UnidirectionalSequenceLSTMOptions.UnidirectionalSequenceLSTMOptionsEnd(
            new_builder)

    # MirrorPadOptions: not supported
    # AbsOptions: not supported
    # SplitVOptions: not supported

    # IfOptions
    import tflite.IfOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().IfOptions:

        if_option = tflite.IfOptions.IfOptions()
        if_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.IfOptions.IfOptionsStart(new_builder)
        tflite.IfOptions.IfOptionsAddElseSubgraphIndex(new_builder,
                                                       if_option.ElseSubgraphIndex())
        tflite.IfOptions.IfOptionsAddThenSubgraphIndex(new_builder,
                                                       if_option.ThenSubgraphIndex())
        return tflite.IfOptions.IfOptionsEnd(new_builder)

    # WhileOptions
    import tflite.WhileOptions
    if builtin_option_type == tflite.BuiltinOptions.BuiltinOptions().WhileOptions:

        while_option = tflite.WhileOptions.WhileOptions()
        while_option.Init(selected_builtin_option.Bytes, selected_builtin_option.Pos)

        tflite.WhileOptions.WhileOptionsStart(new_builder)
        tflite.WhileOptions.WhileOptionsAddBodySubgraphIndex(
            new_builder, while_option.BodySubgraphIndex())
        tflite.WhileOptions.WhileOptionsAddCondSubgraphIndex(
            new_builder, while_option.CondSubgraphIndex())
        return tflite.WhileOptions.WhileOptionsEnd(new_builder)

    # Cannot handle builtin option type yet
    raise Exception(
        "Cannot handle BuiltinOptions {} yet. See BuiltinOptions.py for op name".format(
            builtin_option_type))


def GenerateOperator(new_builder, operator):

    # define opcode_index
    opcode_index = operator.OpcodeIndex()

    # create input vector
    input_num = operator.InputsLength()
    new_input = 0
    if input_num != 0:
        tflite.Operator.OperatorStartInputsVector(new_builder, input_num)
        for input_idx in reversed(range(input_num)):
            tensor_idx = operator.Inputs(input_idx)
            new_builder.PrependInt32(tensor_idx)
        new_input = EndVector(new_builder, input_num)

    # create output_vector
    output_num = operator.OutputsLength()
    new_output = 0
    if output_num != 0:
        tflite.Operator.OperatorStartOutputsVector(new_builder, output_num)
        for output_idx in reversed(range(output_num)):
            tensor_idx = operator.Outputs(output_idx)
            new_builder.PrependInt32(tensor_idx)
        new_output = EndVector(new_builder, output_num)

    # Create builtin_option
    builtin_option_type = operator.BuiltinOptionsType()
    new_builtin_option = 0
    if builtin_option_type != 0:
        selected_builtin_option = operator.BuiltinOptions()
        new_builtin_option = GenerateBuiltinOption(new_builder, selected_builtin_option,
                                                   builtin_option_type)

    # Create custum option vector
    custom_option_num = operator.CustomOptionsLength()
    new_custom_option = 0
    if custom_option_num != 0:
        tflite.Operator.OperatorStartCustomOptionsVector(new_builder, custom_option_num)
        for custom_option_idx in reversed(range(custom_option_num)):
            new_builder.PrependUint8(operator.CustomOptions(custom_option_idx))
        new_custom_option = EndVector(new_builder, custom_option_num)

    # Create custum option type
    custom_option_type = operator.CustomOptionsFormat()

    # Create operator
    tflite.Operator.OperatorStart(new_builder)
    tflite.Operator.OperatorAddOpcodeIndex(new_builder, opcode_index)
    if input_num != 0:
        tflite.Operator.OperatorAddInputs(new_builder, new_input)
    if output_num != 0:
        tflite.Operator.OperatorAddOutputs(new_builder, new_output)
    tflite.Operator.OperatorAddBuiltinOptionsType(new_builder, builtin_option_type)
    if builtin_option_type != 0:
        tflite.Operator.OperatorAddBuiltinOptions(new_builder, new_builtin_option)
    if custom_option_num != 0:
        tflite.Operator.OperatorAddCustomOptions(new_builder, new_custom_option)
    tflite.Operator.OperatorAddCustomOptionsFormat(new_builder, custom_option_type)
    return tflite.Operator.OperatorEnd(new_builder)


def GenerateOperators(new_builder, subg):
    operator_num = subg.OperatorsLength()
    operators = list()

    if operator_num == 0:
        return 0

    for idx in range(operator_num):
        operators.append(GenerateOperator(new_builder, subg.Operators(idx)))

    tflite.SubGraph.SubGraphStartOperatorsVector(new_builder, operator_num)
    return GenerateVector(new_builder, operators)


def GenerateSubgraph(new_builder, subg, name_prefix):
    # Tensors
    tensors = GenerateTensors(new_builder, subg, name_prefix)

    # Create input vector for subgraph table
    input_num = subg.InputsLength()
    new_inputs = 0
    if input_num != 0:
        tflite.SubGraph.SubGraphStartInputsVector(new_builder, input_num)
        for input_idx in reversed(range(input_num)):
            new_builder.PrependInt32(subg.Inputs(input_idx))
        new_inputs = EndVector(new_builder, input_num)

    # Create output vector for subgraph table
    output_num = subg.OutputsLength()
    new_outputs = 0
    if output_num != 0:
        tflite.SubGraph.SubGraphStartOutputsVector(new_builder, output_num)
        for output_idx in reversed(range(output_num)):
            new_builder.PrependInt32(subg.Outputs(output_idx))
        new_outputs = EndVector(new_builder, output_num)

    # Name
    subgraph_name = subg.Name()
    new_subgraph_name = 0
    have_name = False
    if subgraph_name and subgraph_name != "":
        have_name = True
        new_subgraph_name = new_builder.CreateString(subgraph_name)

    # Operators
    operators = GenerateOperators(new_builder, subg)

    tflite.SubGraph.SubGraphStart(new_builder)
    tflite.SubGraph.SubGraphAddTensors(new_builder, tensors)
    tflite.SubGraph.SubGraphAddInputs(new_builder, new_inputs)
    tflite.SubGraph.SubGraphAddOutputs(new_builder, new_outputs)
    tflite.SubGraph.SubGraphAddOperators(new_builder, operators)
    if have_name:
        tflite.SubGraph.SubGraphAddName(new_builder, new_subgraph_name)

    return tflite.SubGraph.SubGraphEnd(new_builder)


def GenerateSubgraphs(new_builder, input_model):
    subgraph_num = input_model.SubgraphsLength()
    new_subgraphs = list()
    name_prefix = ""

    for subgraph_idx in range(subgraph_num):
        if subgraph_idx != 0:
            name_prefix = "subg" + str(subgraph_idx) + "-"
        input_subg = input_model.Subgraphs(subgraph_idx)
        new_subgraphs.append(GenerateSubgraph(new_builder, input_subg, name_prefix))

    tflite.Model.ModelStartSubgraphsVector(new_builder, subgraph_num)
    return GenerateVector(new_builder, new_subgraphs)


def GenerateBuffers(new_builder, input_model):
    buffer_num = input_model.BuffersLength()
    new_buffer_data_list = {}
    new_buffer_list = list()

    if buffer_num == 0:
        return 0

    # Create data vector for buffer table
    for buffer_idx in range(buffer_num):
        buffer = input_model.Buffers(buffer_idx)
        buffer_length = buffer.DataLength()

        if buffer_length != 0:
            tflite.Buffer.BufferStartDataVector(new_builder, buffer_length)
            for buffer_data_idx in reversed(range(buffer_length)):
                new_builder.PrependUint8(buffer.Data(buffer_data_idx))
            new_buffer = EndVector(new_builder, buffer_length)
            new_buffer_data_list[buffer_idx] = new_buffer

    # Create tables of buffer
    for buffer_idx in range(buffer_num):
        buffer = input_model.Buffers(buffer_idx)

        # Create buffer table
        tflite.Buffer.BufferStart(new_builder)
        if buffer.DataLength() != 0:
            tflite.Buffer.BufferAddData(new_builder, new_buffer_data_list[buffer_idx])
        new_buffer = tflite.Buffer.BufferEnd(new_builder)
        new_buffer_list.append(new_buffer)

    # Create buffer vector
    tflite.Model.ModelStartBuffersVector(new_builder, buffer_num)
    return GenerateVector(new_builder, new_buffer_list)


def GenerateModel(args, new_builder, input_model):
    # uint
    version = input_model.Version()

    # pointer of operator code 'table' vector
    operator_codes = GenerateOperatorCodes(new_builder, input_model)

    # subgraphs
    subgraphs = GenerateSubgraphs(new_builder, input_model)

    # skip description

    # buffers
    buffers = GenerateBuffers(new_builder, input_model)

    # Skip metadata and signature_defs: not used for test

    # Generate model
    tflite.Model.ModelStart(new_builder)
    tflite.Model.ModelAddVersion(new_builder, version)
    tflite.Model.ModelAddOperatorCodes(new_builder, operator_codes)
    tflite.Model.ModelAddSubgraphs(new_builder, subgraphs)
    tflite.Model.ModelAddBuffers(new_builder, buffers)

    return tflite.Model.ModelEnd(new_builder)


def main(args):
    input_model_file = args.input_model
    output_model_file = args.output_model

    # Read original model file
    input_buf = input_model_file.read()
    input_buf = bytearray(input_buf)
    input_model = tflite.Model.Model.GetRootAsModel(input_buf, 0)

    # Create new model file
    new_builder = flatbuffers.Builder(1024)
    new_model = GenerateModel(args, new_builder, input_model)
    new_builder.Finish(new_model, file_identifier=b'TFL3')
    new_buf = new_builder.Output()
    output_model_file.write(new_buf)


if __name__ == '__main__':
    # Define argument and read
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument(
        "input_model",
        type=argparse.FileType('rb'),
        help="input tflite model file to read")
    arg_parser.add_argument(
        "output_model", type=argparse.FileType('wb'), help="output tflite model file")

    args = arg_parser.parse_args()

    # Call main function
    main(args)
