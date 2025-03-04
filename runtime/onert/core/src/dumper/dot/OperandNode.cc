/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include <sstream>

#include "OperandNode.h"
#include "ir/Graph.h"

namespace onert::dumper::dot
{

Operand::Operand(const ir::OperandIndex &index, Type type)
  : Node{"operand" + std::to_string(index.value())}
{
  {
    auto type_to_shape = [](Type type) -> const std::string & {
      switch (type)
      {
        case Type::MODEL_INPUT:
          return INPUT_SHAPE;
        case Type::MODEL_OUTPUT:
          return OUTPUT_SHAPE;
        case Type::UNDEFINED:
        case Type::INTERNAL:
        default:
          return OPERAND_SHAPE;
      }
    };
    setAttribute("shape", type_to_shape(type));
  }

  setAttribute("colorscheme", BG_COLOR_SCHEME);
}

} // namespace onert::dumper::dot
