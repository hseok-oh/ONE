/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd. All Rights Reserved
 * Copyright 2020 The TensorFlow Authors. All Rights Reserved.
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

// from tensorflow/compiler/mlir/utils/name_utils.cc

#include "circle-mlir/dialect/NameUtils.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringExtras.h"
#include "mlir/IR/BuiltinAttributes.h" // from @llvm-project

#include <cctype>

namespace mlir
{

std::string GetNameFromLoc(Location loc)
{
  llvm::SmallVector<llvm::StringRef, 8> loc_names;
  llvm::SmallVector<Location, 8> locs;
  locs.push_back(loc);
  bool names_is_nonempty = false;

  while (!locs.empty())
  {
    Location curr_loc = locs.pop_back_val();

    if (auto name_loc = mlir::dyn_cast<NameLoc>(curr_loc))
    {
      // Add name in NameLoc. For NameLoc we also account for names due to ops
      // in functions where the op's name is first.
      auto name = name_loc.getName().strref().split('@').first;
      // Skip if the name is for op type.
      if (!name.ends_with(":"))
      {
        loc_names.push_back(name);
        if (!name.empty())
          names_is_nonempty = true;
      }
      continue;
    }
    else if (auto call_loc = mlir::dyn_cast<CallSiteLoc>(curr_loc))
    {
      // Use location of the Callee to generate the name.
      locs.push_back(call_loc.getCallee());
      continue;
    }
    else if (auto fused_loc = mlir::dyn_cast<FusedLoc>(curr_loc))
    {
      // Push all locations in FusedLoc in reverse order, so locations are
      // visited based on order in FusedLoc.
      auto reversed_fused_locs = llvm::reverse(fused_loc.getLocations());
      locs.append(reversed_fused_locs.begin(), reversed_fused_locs.end());
      continue;
    }

    // Location is not a supported, so an empty StringRef is added.
    loc_names.push_back(llvm::StringRef());
  }

  if (names_is_nonempty)
    return llvm::join(loc_names.begin(), loc_names.end(), ";");

  return "";
}

} // namespace mlir
