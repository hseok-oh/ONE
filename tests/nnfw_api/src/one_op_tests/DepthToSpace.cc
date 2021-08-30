/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "GenModelTest.h"

// Input shape: {1, 1, 2, 4}
// Block size: 2
// Output shape: {1, 2, 4, 1}
TEST_F(GenModelTest, OneOp_DepthToSpace)
{
  struct DepthToSpaceExample
  {
    TestCaseData tcd;
    circle::TensorType type = circle::TensorType::TensorType_FLOAT32;
    float scale = 0.0f;
    int64_t zero_point = 0;
  };

  static const std::vector<DepthToSpaceExample> examples{
    // Float
    DepthToSpaceExample{uniformTCD<float>({{1, 2, 3, 4, 5, 6, 7, 8}}, {{1, 2, 5, 6, 3, 4, 7, 8}})},
    // Int32
    DepthToSpaceExample{uniformTCD<int32_t>({{1, 2, 3, 4, 5, 6, 7, 8}}, {{1, 2, 5, 6, 3, 4, 7, 8}}),
                        circle::TensorType::TensorType_INT32},
    // Int64
    DepthToSpaceExample{uniformTCD<int64_t>({{1, 2, 3, 4, 5, 6, 7, 8}}, {{1, 2, 5, 6, 3, 4, 7, 8}}),
                        circle::TensorType::TensorType_INT64},
    // Uint8
    DepthToSpaceExample{uniformTCD<uint8_t>({{1, 2, 3, 4, 5, 6, 7, 8}}, {{1, 2, 5, 6, 3, 4, 7, 8}}),
                        circle::TensorType::TensorType_UINT8, 1.0f, -2},
    // Int8
    DepthToSpaceExample{uniformTCD<int8_t>({{1, 2, 3, 4, 5, 6, 7, 8}}, {{1, 2, 5, 6, 3, 4, 7, 8}}),
                        circle::TensorType::TensorType_INT8, 1.0f, -2}};

  for (auto example : examples)
  {
    CircleGen cgen;
    int in = cgen.addTensor({{1, 1, 2, 4}, example.type}, example.scale, example.zero_point);
    int out = cgen.addTensor({{1, 2, 4, 1}, example.type}, example.scale, example.zero_point);
    cgen.addOperatorDepthToSpace({{in}, {out}}, 2);
    cgen.setInputsAndOutputs({in}, {out});

    _context = std::make_unique<GenModelTestContext>(cgen.finish());
    _context->addTestCase(example.tcd);
    _context->setBackends({"acl_cl", "acl_neon", "cpu"});
  }

  SUCCEED();
}

TEST_F(GenModelTest, neg_OneOp_DepthToSpace_Blocksize)
{
  CircleGen cgen;
  circle::TensorType data_type = circle::TensorType::TensorType_FLOAT32;
  int in = cgen.addTensor({{1, 1, 2, 4}, data_type});
  int out = cgen.addTensor({{1, 2, 4, 1}, data_type});
  cgen.addOperatorDepthToSpace({{in}, {out}}, -2);
  cgen.setInputsAndOutputs({in}, {out});

  _context = std::make_unique<GenModelTestContext>(cgen.finish());
  _context->expectFailModelLoad();

  SUCCEED();
}
