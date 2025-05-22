
/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "ir/Package.h"

namespace onert::ir
{

GraphIndex Package::pushEntry(const std::shared_ptr<IGraph> &graph)
{
  if (graph->signature() == std::nullopt)
  {
    if (!_entries.empty())
      throw std::runtime_error("Multiple entry graphs without signatures are not allowed");
  }
  else
  {
    // Already have an unique entry graph
    if (_entries.size() == 1 && _graphs.at(_entries.front())->signature() == std::nullopt)
      throw std::runtime_error("Multiple entry graphs without signatures are not allowed");

    for (const auto &entry : _entries)
    {
      if (graph->signature() == _graphs.at(entry)->signature())
        throw std::runtime_error("Multiple entry graphs with same signature are not allowed");
    }
  }

  auto index = generateIndex();
  if (!index.valid())
    throw std::runtime_error("Failed to create a valid index");

  _entries.push_back(index);
  _graphs.emplace(index, graph);
  return index;
}

GraphIndex Package::pushEntry(const std::shared_ptr<IGraph> &graph, const ModelIndex &model_index,
                              const SubgraphIndex &modelgraph_index)
{
  // TODO Support multiple entry in modelfile(s)
  if (_entries.size() > 0)
    throw std::runtime_error("Only unique entry graph in modelfile is allowed");

  // TODO Support multiple entry in single modelfile
  if (model_index != ModelIndex{0} || modelgraph_index != SubgraphIndex{0})
    throw std::runtime_error("Entry graph should be placed at model_index=0, subgraph_index=0");

  auto index = generateIndex();
  if (!index.valid())
    throw std::runtime_error("Failed to create a valid index");

  _entries.push_back(index);
  _graphs.emplace(index, graph);
  _model_subgraph_indices.emplace(index, std::make_pair(model_index, modelgraph_index));
  return index;
}

GraphIndex Package::push(const std::shared_ptr<IGraph> &graph, const ModelIndex &model_index,
                         const SubgraphIndex &modelgraph_index)
{
  auto index = generateIndex();
  if (!index.valid())
    throw std::runtime_error("Failed to create a valid index");

  _graphs.emplace(index, graph);
  _model_subgraph_indices.emplace(index, std::make_pair(model_index, modelgraph_index));
  return index;
}

std::shared_ptr<IGraph> &Package::at(const ModelIndex &model_index,
                                     const SubgraphIndex &modelgraph_index)
{
  for (auto &pair : _model_subgraph_indices)
  {
    if (pair.second.first == model_index && pair.second.second == modelgraph_index)
      return _graphs.at(pair.first);
  }

  throw std::out_of_range("No graph found with given model_index and modelgraph_index");
}

bool Package::exist(const ModelIndex &model_index, const SubgraphIndex &modelgraph_index) const
{
  for (auto &pair : _model_subgraph_indices)
  {
    if (pair.second.first == model_index && pair.second.second == modelgraph_index)
      return true;
  }

  return false;
}

} // namespace onert::ir
