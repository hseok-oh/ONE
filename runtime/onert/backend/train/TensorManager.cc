/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd. All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "TensorManager.h"

#include <util/logging.h>

namespace
{

using namespace onert;

template <typename MemoryManager, typename TensorMap>
void allocateMemory(MemoryManager *mgr, const TensorMap &tensors, const std::string tensor_type)
{
  mgr->allocate();

  for (auto &&[index, tensor] : tensors)
  {
    assert(!tensor->is_dynamic());

    auto *buffer = mgr->getBuffer(index);
    tensor->setBuffer(buffer);
    VERBOSE(TensorManager) << tensor_type << index << " : " << static_cast<void *>(buffer)
                           << std::endl;
  }
}

inline size_t alignedSize(const size_t size, const uint64_t align)
{
  return (((size) + ((align)-1)) & ~((align)-1));
}

} // namespace

namespace onert::backend::train
{

TensorManager::TensorManager(const std::shared_ptr<TensorRegistry> &reg, uint32_t optim_vars_count)
  : _nonconst_mgr{new MemoryManager()},
    _trainable_mgr{new TrainableMemoryManager(optim_vars_count)},
    _back_prop_mgr{new MemoryManager()}, _gradient_mgr{new MemoryManager()},
    // TODO Find a suitable planner of disposable tensors to reduce peak memory usage
    _disposable_back_prop_mgr{new DisposableMemoryManager()},
    _layer_scope_mgr{new LayerScopeMemoryManager()}, _tensors{reg}
{
  // DO NOTHING
}

void TensorManager::allocateNonConstTensors()
{
  allocateMemory(_nonconst_mgr.get(), _tensors->nonconst_tensors(),
                 std::string{"               TENSOR "});
}

void TensorManager::allocateTrainableTensors()
{
  allocateMemory(_trainable_mgr.get(), _tensors->trainable_tensors(),
                 std::string{"     TRAINABLE TENSOR "});

  // Set buffers of optimizer variables
  // Allocating optimizer variables has been done when calling allocate() of TrainableMemoryManager
  for (const auto &[index, trainable_tensor] : _tensors->trainable_tensors())
  {
    for (uint32_t var_pos = 0; var_pos < trainable_tensor->optVars().size(); ++var_pos)
    {
      auto *buffer = _trainable_mgr.get()->getOptVarBuffer(index, var_pos);
      trainable_tensor->setOptVarBuffer(buffer, var_pos);
      VERBOSE(TensorManager) << std::string{"     OPTIM_VAR TENSOR "} << index << " : "
                             << static_cast<void *>(buffer) << std::endl;
    }
  }
}

void TensorManager::allocateBackPropTensors()
{
  allocateMemory(_back_prop_mgr.get(), _tensors->back_prop_tensors(),
                 std::string{"     BACK_PROP TENSOR "});
}

void TensorManager::allocateGradientTensors()
{
  allocateMemory(_gradient_mgr.get(), _tensors->gradient_tensors(),
                 std::string{"     GRADIENT TENSOR "});
}

void TensorManager::allocateDisposableBackPropTensors()
{
  allocateMemory(_disposable_back_prop_mgr.get(), _tensors->disposable_back_prop_tensors(),
                 std::string{"DISPOSABLE BACK_PROP TENSOR "});
}

void TensorManager::allocateLayerScopeTensors()
{
  allocateMemory(_layer_scope_mgr.get(), _tensors->layerscope_tensors(),
                 std::string{"   LAYERSCOPE TENSOR "});
}

void TensorManager::claimNonConstPlan(const ir::OperandIndex &index)
{
  auto tensor = _tensors->getNonConstTensor(index);
  assert(tensor && !tensor->is_dynamic());

  auto size = alignedSize(tensor->total_size(), _align);
  _nonconst_mgr->claimPlan(index, size);
}

void TensorManager::releaseNonConstPlan(const ir::OperandIndex &index)
{
  assert(_tensors->getNonConstTensor(index) && !_tensors->getNonConstTensor(index)->is_dynamic());

  _nonconst_mgr->releasePlan(index);
}

void TensorManager::claimTrainablePlan(const ir::OperandIndex &index)
{
  auto tensor = _tensors->getTrainableTensor(index);
  assert(tensor && !tensor->is_dynamic());

  auto size = alignedSize(tensor->total_size(), _align);
  _trainable_mgr->claimPlan(index, size);
}

void TensorManager::releaseTrainablePlan(const ir::OperandIndex &index)
{
  assert(_tensors->getTrainableTensor(index) && !_tensors->getTrainableTensor(index)->is_dynamic());

  _trainable_mgr->releasePlan(index);
}

void TensorManager::claimBackPropPlan(const ir::OperandIndex &index)
{
  auto tensor = _tensors->getBackPropTensor(index);
  assert(tensor && !tensor->is_dynamic());

  auto size = alignedSize(tensor->total_size(), _align);
  _back_prop_mgr->claimPlan(index, size);
}

void TensorManager::releaseBackPropPlan(const ir::OperandIndex &index)
{
  assert(_tensors->getBackPropTensor(index) && !_tensors->getBackPropTensor(index)->is_dynamic());

  _back_prop_mgr->releasePlan(index);
}

void TensorManager::claimGradientPlan(const ir::OperandIndex &index)
{
  auto tensor = _tensors->getGradientTensor(index);
  assert(tensor && !tensor->is_dynamic());

  auto size = alignedSize(tensor->total_size(), _align);
  _gradient_mgr->claimPlan(index, size);
}

void TensorManager::releaseGradientPlan(const ir::OperandIndex &index)
{
  assert(_tensors->getGradientTensor(index) && !_tensors->getGradientTensor(index)->is_dynamic());

  _gradient_mgr->releasePlan(index);
}

void TensorManager::claimDisposableBackPropPlan(const DisposableTensorIndex &index)
{
  const auto tensor = _tensors->getDisposableBackPropTensor(index);
  assert(tensor && !tensor->is_dynamic());

  auto size = alignedSize(tensor->total_size(), _align);
  _disposable_back_prop_mgr->claimPlan(index, size);
}

void TensorManager::releaseDisposableBackPropPlan(const DisposableTensorIndex &index)
{
  assert(_tensors->getDisposableBackPropTensor(index) &&
         !_tensors->getDisposableBackPropTensor(index)->is_dynamic());

  _disposable_back_prop_mgr->releasePlan(index);
}

void TensorManager::claimLayerScopePlan(const LayerScopeTensorIndex &index)
{
  const auto tensor = _tensors->getLayerScopeTensor(index);

  auto size = alignedSize(tensor->total_size(), _align);
  _layer_scope_mgr->claimPlan(index, size);
}

void TensorManager::releaseLayerScopePlan(const LayerScopeTensorIndex &index)
{
  assert(_tensors->getLayerScopeTensor(index));
  _layer_scope_mgr->releasePlan(index);
}

} // namespace onert::backend::train
