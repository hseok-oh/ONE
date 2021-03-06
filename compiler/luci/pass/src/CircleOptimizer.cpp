/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "luci/CircleOptimizer.h"

#include "luci/Pass/FoldDequantizePass.h"
#include "luci/Pass/FuseActivationFunctionPass.h"
#include "luci/Pass/FuseAddWithTConvPass.h"
#include "luci/Pass/FuseBatchNormWithTConv.h"
#include "luci/Pass/FuseBCQPass.h"
#include "luci/Pass/FuseInstanceNormPass.h"
#include "luci/Pass/FusePreActivationBatchNormPass.h"
#include "luci/Pass/MakeBatchNormGammaPositivePass.h"
#include "luci/Pass/PropagateQuantParamPass.h"
#include "luci/Pass/RemoveRedundantTransposePass.h"
#include "luci/Pass/ReplaceMulAddWithDepthwiseConvPass.h"
#include "luci/Pass/ResolveCustomOpAddPass.h"
#include "luci/Pass/ResolveCustomOpBatchMatMulPass.h"
#include "luci/Pass/ResolveCustomOpMatMulPass.h"
#include "luci/Pass/RequantizePass.h"
#include "luci/Pass/QuantizeWithMinMaxPass.h"
#include "luci/Pass/QuantizeDequantizeWeightsPass.h"
#include "luci/Pass/SparsifyTensorPass.h"
#include "luci/Pass/ShuffleWeightTo16x1Float32Pass.h"
#include "luci/Pass/SubstitutePackToReshapePass.h"
// TODO add more passes

#include "luci/Pass/ShapeInferencePass.h"
#include "luci/Pass/ShapeSignatureInferencePass.h"
#include "luci/Pass/TypeInferencePass.h"

// Following passes will be removed after refactoring is finished
#include "luci/Pass/MigrateLegacyShapeDtypePass.h"

// logo passes
#include <logo/RemoveDeadNodeWithQueryPass.h>

#include "ModulePhase.h"
#include "ProgressReporter.h"
#include "CircleOptimizerUtils.h"

#include <luci/IR/CircleNodes.h>
#include <logo/Phase.h>

#include <memory>
#include <sstream>

namespace
{

std::vector<int> parseIntFromCommadelimitedStr(std::string str)
{
  std::vector<int> ret;
  std::istringstream is(str);
  for (uint32_t i; is >> i;)
  {
    assert(i != ',');
    ret.push_back(i);
    if (is.peek() == ',')
      is.ignore();
  }
  return ret;
}

using namespace luci;

class OptimizeOptionsImpl final : public luci::CircleOptimizer::Options
{
public:
  void enable(Algorithm) final;
  void param(AlgorithmParameters, const std::string &) final;
  const std::string param(AlgorithmParameters) const final;
  bool query(Algorithm) final;

private:
  std::vector<Algorithm> _algorithms;
  std::map<AlgorithmParameters, const std::string> _algorithm_params;
};

void OptimizeOptionsImpl::enable(Algorithm algo) { _algorithms.push_back(algo); }

void OptimizeOptionsImpl::param(AlgorithmParameters param, const std::string &str)
{
  _algorithm_params.insert(std::pair<AlgorithmParameters, const std::string>(param, str));
}

const std::string OptimizeOptionsImpl::param(AlgorithmParameters param) const
{
  auto param_str = _algorithm_params.find(param);
  if (param_str != _algorithm_params.end())
  {
    return param_str->second;
  }
  else
  {
    return std::string();
  }
}

bool OptimizeOptionsImpl::query(Algorithm algo)
{
  std::vector<Algorithm>::iterator it = std::find(_algorithms.begin(), _algorithms.end(), algo);
  if (it == _algorithms.end())
    return false;

  return true;
}

} // namespace

namespace luci
{

CircleOptimizer::Options *CircleOptimizer::options(void)
{
  if (_options == nullptr)
  {
    _options = std::make_unique<OptimizeOptionsImpl>();
  }

  return _options.get();
}

void CircleOptimizer::optimize(luci::Module *m) const
{
  luci::Phase phase;

  // Following passes will be deprecated after refactoring is finished.
  phase.emplace_back(std::make_unique<luci::MigrateLegacyShapeDtypePass>());

  // Following passes are needed everytime when other passes create new node or modify some nodes.
  phase.emplace_back(std::make_unique<luci::ShapeInferencePass>());
  phase.emplace_back(std::make_unique<luci::ShapeSignatureInferencePass>());
  phase.emplace_back(std::make_unique<luci::TypeInferencePass>());

  if (_options->query(Options::Algorithm::FuseBCQ))
  {
    phase.emplace_back(std::make_unique<FuseBCQPass>());
  }

  ModuleProgressReporter prog(m, logo::PhaseStrategy::Restart);
  PhaseRunner<logo::PhaseStrategy::Restart> phase_runner{m};
  phase_runner.attach(&prog);
  phase_runner.run(phase);
}

void CircleOptimizer::optimize(loco::Graph *g) const
{
  logo::Phase phase;

  /* TRANSFORM DECLARATION BEGIN */
  phase.emplace_back(std::make_unique<logo::RemoveDeadNodeWithQueryPass>());

  // Following passes will be deprecated after refactoring is finished.
  phase.emplace_back(std::make_unique<luci::MigrateLegacyShapeDtypePass>());

  // Following passes are needed everytime when other passes create new node or modify some nodes.
  phase.emplace_back(std::make_unique<luci::TypeInferencePass>());
  phase.emplace_back(std::make_unique<luci::ShapeInferencePass>());
  phase.emplace_back(std::make_unique<luci::ShapeSignatureInferencePass>());

  if (_options->query(Options::Algorithm::ResolveCustomOpAdd))
  {
    phase.emplace_back(std::make_unique<luci::ResolveCustomOpAddPass>());
  }
  if (_options->query(Options::Algorithm::ResolveCustomOpBatchMatMul))
  {
    phase.emplace_back(std::make_unique<luci::ResolveCustomOpBatchMatMulPass>());
  }
  if (_options->query(Options::Algorithm::ResolveCustomOpMatMul))
  {
    phase.emplace_back(std::make_unique<luci::ResolveCustomOpMatMulPass>());
  }
  if (_options->query(Options::Algorithm::FuseInstanceNorm))
  {
    phase.emplace_back(std::make_unique<FuseInstanceNormPass>());
  }
  if (_options->query(Options::Algorithm::FuseBatchNormWithTConv))
  {
    phase.emplace_back(std::make_unique<FuseBatchNormWithTConvPass>());
  }
  if (_options->query(Options::Algorithm::FuseAddWithTConv))
  {
    phase.emplace_back(std::make_unique<FuseAddWithTConvPass>());
  }
  if (_options->query(Options::Algorithm::FuseActivationFunction))
  {
    phase.emplace_back(std::make_unique<FuseActivationFunctionPass>());
  }
  if (_options->query(Options::Algorithm::FoldDequantize))
  {
    phase.emplace_back(std::make_unique<luci::FoldDequantizePass>());
  }
  if (_options->query(Options::Algorithm::FusePreActivationBatchNorm))
  {
    phase.emplace_back(std::make_unique<luci::FusePreActivationBatchNormPass>());
  }
  if (_options->query(Options::Algorithm::MakeBatchNormGammaPositive))
  {
    phase.emplace_back(std::make_unique<luci::MakeBatchNormGammaPositivePass>());
  }
  if (_options->query(Options::Algorithm::ShuffleWeightTo16x1Float32))
  {
    phase.emplace_back(std::make_unique<luci::ShuffleWeightTo16x1Float32Pass>());
  }
  if (_options->query(Options::Algorithm::RemoveRedundantTranspose))
  {
    phase.emplace_back(std::make_unique<luci::RemoveRedundantTransposePass>());
  }
  if (_options->query(Options::Algorithm::ReplaceMulAddWithDepthwiseConv))
  {
    phase.emplace_back(std::make_unique<luci::ReplaceMulAddWithDepthwiseConvPass>());
  }
  if (_options->query(Options::Algorithm::SubstitutePackToReshape))
  {
    phase.emplace_back(std::make_unique<luci::SubstitutePackToReshapePass>());
  }

  /* TRANSFORM DECLARATION END */

  ProgressReporter prog(g, logo::PhaseStrategy::Restart);
  logo::PhaseRunner<logo::PhaseStrategy::Restart> phase_runner{g};
  phase_runner.attach(&prog);
  phase_runner.run(phase);
}

void CircleOptimizer::quantize(loco::Graph *g) const
{
  // Fake quantization of weights
  if (_options->query(Options::Algorithm::QuantizeDequantizeWeights))
  {
    static const std::vector<std::string> fakeq_supported_input_dtype{"float32"};
    static const std::vector<std::string> fakeq_supported_output_dtype{"uint8", "int16"};
    static const std::vector<std::string> fakeq_supported_granularity{"layer", "channel"};

    auto input_dtype = _options->param(Options::AlgorithmParameters::Quantize_input_dtype);
    auto output_dtype = _options->param(Options::AlgorithmParameters::Quantize_output_dtype);
    auto granularity = _options->param(Options::AlgorithmParameters::Quantize_granularity);

    if (!in_array(to_lower_case(input_dtype), fakeq_supported_input_dtype))
      throw std::runtime_error("Unsupported input type. List of supported input type: " +
                               to_string(fakeq_supported_input_dtype));

    if (!in_array(to_lower_case(output_dtype), fakeq_supported_output_dtype))
      throw std::runtime_error("Unsupported output type. List of supported output type: " +
                               to_string(fakeq_supported_output_dtype));

    if (!in_array(to_lower_case(granularity), fakeq_supported_granularity))
      throw std::runtime_error("Unsupported granularity. List of supported granularity: " +
                               to_string(fakeq_supported_granularity));

    if (str_to_granularity(granularity) == QuantizationGranularity::LayerWise &&
        str_to_dtype(output_dtype) != loco::DataType::U8)
      throw std::runtime_error("Layer-wise quantization only supports uint8 dtype.");

    // Clear existing quantparams before doing fake quantization
    for (auto node : loco::active_nodes(loco::output_nodes(g)))
    {
      auto circle_node = loco::must_cast<luci::CircleNode *>(node);
      if (circle_node->quantparam() != nullptr)
        circle_node->quantparam(nullptr);
    }

    luci::QuantizeDequantizeWeightsPass fake_quantizer(
        str_to_dtype(input_dtype), str_to_dtype(output_dtype), str_to_granularity(granularity));
    fake_quantizer.run(g);
  }

  // Actual quantization of weights, bias, and activation
  if (_options->query(Options::Algorithm::QuantizeWithMinMax))
  {
    static const std::vector<std::string> qwmm_supported_input_dtype{"float32"};
    static const std::vector<std::string> qwmm_supported_output_dtype{"uint8", "int16"};
    static const std::vector<std::string> qwmm_supported_granularity{"layer", "channel"};

    auto input_dtype = _options->param(Options::AlgorithmParameters::Quantize_input_dtype);
    auto output_dtype = _options->param(Options::AlgorithmParameters::Quantize_output_dtype);
    auto granularity = _options->param(Options::AlgorithmParameters::Quantize_granularity);

    if (!in_array(to_lower_case(input_dtype), qwmm_supported_input_dtype))
      throw std::runtime_error("Unsupported input type. List of supported input types: " +
                               to_string(qwmm_supported_input_dtype));

    if (!in_array(to_lower_case(output_dtype), qwmm_supported_output_dtype))
      throw std::runtime_error("Unsupported output type. List of supported output types: " +
                               to_string(qwmm_supported_output_dtype));

    if (!in_array(to_lower_case(granularity), qwmm_supported_granularity))
      throw std::runtime_error("Unsupported granularity. List of supported granularity: " +
                               to_string(qwmm_supported_granularity));

    if (str_to_granularity(granularity) == QuantizationGranularity::LayerWise &&
        str_to_dtype(output_dtype) != loco::DataType::U8)
      throw std::runtime_error("Layer-wise quantization only supports uint8 dtype.");

    luci::QuantizeWithMinMaxPass quantizer(str_to_dtype(input_dtype), str_to_dtype(output_dtype),
                                           str_to_granularity(granularity));
    quantizer.run(g);

    // Post-quantization optimizations
    logo::Phase phase;

    phase.emplace_back(std::make_unique<luci::PropagateQuantParamPass>());

    phase.emplace_back(std::make_unique<luci::ShapeInferencePass>());
    phase.emplace_back(std::make_unique<luci::TypeInferencePass>());
    phase.emplace_back(std::make_unique<logo::RemoveDeadNodeWithQueryPass>());

    ProgressReporter prog(g, logo::PhaseStrategy::Saturate);
    logo::PhaseRunner<logo::PhaseStrategy::Saturate> phase_runner{g};
    phase_runner.attach(&prog);
    phase_runner.run(phase);
  }

  // Requantize
  if (_options->query(Options::Algorithm::Requantize))
  {
    static const std::vector<std::string> rq_supported_input_dtype{"int8"};
    static const std::vector<std::string> rq_supported_output_dtype{"uint8"};

    auto input_dtype = _options->param(Options::AlgorithmParameters::Quantize_input_dtype);
    auto output_dtype = _options->param(Options::AlgorithmParameters::Quantize_output_dtype);

    if (!in_array(to_lower_case(input_dtype), rq_supported_input_dtype))
      throw std::runtime_error("Unsupported input type. List of supported input types: " +
                               to_string(rq_supported_input_dtype));

    if (!in_array(to_lower_case(output_dtype), rq_supported_output_dtype))
      throw std::runtime_error("Unsupported output type. List of supported output types: " +
                               to_string(rq_supported_output_dtype));

    luci::RequantizePass requantizer(str_to_dtype(input_dtype), str_to_dtype(output_dtype));
    requantizer.run(g);
  }

  logo::Phase phase;

  // Do Shape/Type inference
  phase.emplace_back(std::make_unique<luci::ShapeInferencePass>());
  phase.emplace_back(std::make_unique<luci::TypeInferencePass>());

  ProgressReporter prog(g, logo::PhaseStrategy::Saturate);
  logo::PhaseRunner<logo::PhaseStrategy::Saturate> phase_runner{g};
  phase_runner.attach(&prog);
  phase_runner.run(phase);
}

void CircleOptimizer::sparsify(loco::Graph *g) const
{
  if (_options->query(Options::Algorithm::SparsifyTensorPass))
  {
    std::string tensor_name = _options->param(Options::AlgorithmParameters::Sparsify_tensor_name);
    std::string str_tarversal_order =
        _options->param(Options::AlgorithmParameters::Sparsify_traversal_order);
    std::string str_format = _options->param(Options::AlgorithmParameters::Sparsify_format);
    std::string str_block_size = _options->param(Options::AlgorithmParameters::Sparsify_block_size);
    std::string str_block_map = _options->param(Options::AlgorithmParameters::Sparsify_block_map);

    // traversal order
    std::vector<int32_t> traversal_order = parseIntFromCommadelimitedStr(str_tarversal_order);
    // format
    std::vector<DimensionType> format;
    std::istringstream is(str_format);
    for (char c; is >> c;)
    {
      assert(c != ',');
      if (c == 'd')
        format.push_back(DimensionType::DENSE);
      else if (c == 's')
        format.push_back(DimensionType::SPARSE_CSR);
      if (is.peek() == ',')
        is.ignore();
    }
    // block size
    std::vector<int32_t> block_size = parseIntFromCommadelimitedStr(str_block_size);
    // block map
    std::vector<int32_t> block_map = parseIntFromCommadelimitedStr(str_block_map);

    luci::SparsifyTensorPass sparsifier{tensor_name, traversal_order, format, block_size,
                                        block_map};
    sparsifier.run(g);
  }
}

} // namespace luci
