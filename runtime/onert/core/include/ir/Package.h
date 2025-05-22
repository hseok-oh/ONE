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

#ifndef __ONERT_IR_PACKAGE_H__
#define __ONERT_IR_PACKAGE_H__

#include <memory>
#include <unordered_map>

#include "ir/IGraph.h"
#include "ir/Index.h"
#include "util/ObjectManager.h"

namespace onert::backend::custom
{
class IKernelBuilder;
} // namespace onert::backend::custom

namespace onert::ir
{

class Package
{
public:
  Package() = default;
  Package(const Package &obj) = default;
  Package(Package &&) = default;
  Package &operator=(const Package &) = default;
  Package &operator=(Package &&) = default;
  ~Package() = default;

  /**
   * @brief Put entry graph in the container with a new index for that
   *
   * This graph should be unique entry in the package if graph does not
   * have signature to distinguish itself from other graphs.
   * This function is used to push graph which represents model connection
   * in multimodel type package
   *
   * @param[in] graph Graph to be pushed
   * @return    Index of graph in package to be pushed
   */
  GraphIndex pushEntry(const std::shared_ptr<IGraph> &graph);

  /**
   * @brief     Put entry graph in the container with a new index for that
   *
   * This graph should be unique entry in the package if graph does not
   * have signature to distinguish itself from other graphs.
   *
   * @param[in] graph             Graph to be pushed
   * @param[in] model_index       Model index of the graph
   * @param[in] modelgraph_index  Graph index in model
   * @return    Index of graph in package to be pushed
   */
  GraphIndex pushEntry(const std::shared_ptr<IGraph> &graph, const ModelIndex &model_index,
                       const SubgraphIndex &modelgraph_index);

  /**
   * @brief     Put graph in the container with a new index for that
   *
   * This graph cannot be entry graph
   *
   * @param[in] graph             Graph to be pushed
   * @param[in] model_index       Model index of the graph
   * @param[in] modelgraph_index  Graph index in model
   * @return    Index of graph in package to be pushed
   */
  GraphIndex push(const std::shared_ptr<IGraph> &graph, const ModelIndex &model_index,
                  const SubgraphIndex &modelgraph_index);

  /**
   * @brief Remove the graph that is associated with the given index
   *
   * @param[in] index Index of the subgraph to be removed
   * @return N/A
   */
  void remove(const GraphIndex &index) { _graphs.at(index); }

  /**
   * @brief Get the graph that is associated with the given index
   *
   * @param[in] index Index of the subgraph to be returned
   * @return    IGraph
   */
  std::shared_ptr<IGraph> &at(const GraphIndex &index) { return _graphs.at(index); }

  /**
   * @brief Get the graph that is associated with the given model index and subgraph index
   * @param[in] model_index       Model index of the subgraph to be returned
   * @param[in] modelgraph_index  Graph index in model
   * @return    IGraph
   */
  std::shared_ptr<IGraph> &at(const ModelIndex &model_index, const SubgraphIndex &modelgraph_index);

  /**
   * @brief Get the subgraph that is associated with the given index
   *
   * @param[in] index Index of the subgraph to be returned
   * @return true if such entry exists otherwise false
   */
  bool exist(const GraphIndex &index) const
  {
    auto it = _graphs.find(index);
    return it != _graphs.end();
  }

  /**
   * @brief     Check whether the graph that is associated with the given model index and
   *            subgraph index exists
   * @param[in] model_index       Model index of the subgraph to be returned
   * @param[in] modelgraph_index  Graph index in model
   * @return    true if such entry exists otherwise false
   */
  bool exist(const ModelIndex &model_index, const SubgraphIndex &modelgraph_index) const;

  /**
   * @brief Iterate over the container with given function
   *
   * @param[in] fn Function to be run for every container entry
   * @return N/A
   */
  void iterate(const std::function<void(const GraphIndex &, const IGraph &)> &fn) const
  {
    for (const auto &[subg_idx, subg] : _graphs)
    {
      fn(subg_idx, *subg);
    }
  }

  /**
   * @brief Iterate over the container with given function
   *
   * @param[in] fn Function to be run for every container entry
   * @return N/A
   */
  void iterate(const std::function<void(const GraphIndex &, IGraph &)> &fn)
  {
    for (const auto &[subg_idx, subg] : _graphs)
    {
      fn(subg_idx, *subg);
    }
  }

  /**
   * @brief Get count of graph
   *
   * @return  Count of graph
   */
  size_t graphs_count() const { return _graphs.size(); }

  /**
   * @brief Return the entry graph
   *
   * @return std::shared_ptr<IGraph> entry graph
   */
  const IGraph *entryGraph() const
  {
    if (_entries.size() != 1)
      throw std::runtime_error("Entries is not unique");

    return _graphs.at(_entries[0]).get();
  }

  const IGraph *entryGraph(const std::string &signature) const
  {
    for (auto idx : _entries)
    {
      if (_graphs.at(idx)->signature() == signature)
        return _graphs.at(idx).get();
    }

    return nullptr;
  }

  /**
   * @brief Return whether the model has only typename Graph
   *
   * @tparam Graph Type that inherits from IGraph
   *
   * @return true if the model has only typename Graph, otherwise false
   */
  template <typename Graph, std::enable_if_t<std::is_base_of_v<IGraph, Graph>, bool> = true>
  bool hasOnly()
  {
    for (const auto &e : _graphs)
    {
      if (std::dynamic_pointer_cast<Graph>(e.second) == nullptr)
        return false;
    }
    return true;
  }

private:
  // Generate a new index with `_next_index`
  GraphIndex generateIndex()
  {
    // No need to check if there is an entry with _next_index since
    // _next_index is always ("the highest index in the object map" + 1)
    if (GraphIndex{_next_index}.valid())
      return GraphIndex{_next_index++};
    else
      return GraphIndex{};
  }

private:
  std::unordered_map<GraphIndex, std::shared_ptr<IGraph>> _graphs;
  std::vector<GraphIndex> _entries;
  std::unordered_map<GraphIndex, std::pair<ModelIndex, SubgraphIndex>> _model_subgraph_indices;
  uint32_t _next_index = 0;

  // Custom operations support
public:
  void
  bindKernelBuilder(const std::shared_ptr<onert::backend::custom::IKernelBuilder> &kernel_builder)
  {
    _kernel_builder = kernel_builder;
  }

  const std::shared_ptr<backend::custom::IKernelBuilder> &getKernelBuilder() const
  {
    return _kernel_builder;
  }

private:
  std::shared_ptr<backend::custom::IKernelBuilder> _kernel_builder;

public:
  void add_metadata(const std::string &name, std::unique_ptr<const ir::Data> data)
  {
    _metadatas.emplace(name, std::move(data));
  }

  bool exists_metadata(const std::string &name) const
  {
    return _metadatas.find(name) != _metadatas.end();
  }

  // NOTE The corresponding metadata is deleted from the model and returned
  std::unique_ptr<const ir::Data> extract_metadata(const std::string name)
  {
    auto m = _metadatas.find(name);

    if (m == _metadatas.end())
      throw std::out_of_range{"no meatdata named " + name};

    auto data = std::move(m->second);
    _metadatas.erase(m);
    return data;
  }

private:
  // TODO: Apply Heterogeneous lookup for unordered containers (transparent hashing) since C++20
  //       to use `std::string_view` with lookup functions in unordered containers
  std::unordered_map<std::string, std::unique_ptr<const ir::Data>> _metadatas;
};
} // namespace onert::ir

#endif // __ONERT_IR_PACKAGE_H__
