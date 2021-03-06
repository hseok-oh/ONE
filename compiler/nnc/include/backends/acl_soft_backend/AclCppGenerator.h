/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved
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

#ifndef _NNC_ACL_CPP_GENERATOR_H_
#define _NNC_ACL_CPP_GENERATOR_H_

#include "mir/Graph.h"

#include <string>

namespace nnc
{

/**
 * @brief Class for the ACL C++ code generator.
 * It is the entry point to the ACL C++ soft backend.
 */
class AclCppCodeGenerator final
{
public:
  AclCppCodeGenerator(std::string output_dir, std::string artifact_name);

  /**
   * @brief Method represents the generation sequence: analysis, serialization,
   *        header/code generation, etc
   * @param graph MIR graph
   */
  void run(mir::Graph *graph);

private:
  std::string _output_dir;
  std::string _artifact_name;
};

} // namespace nnc

#endif //_NNC_ACL_CPP_GENERATOR_H_
