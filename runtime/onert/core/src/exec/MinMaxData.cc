/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "MinMaxData.h"

#include <iostream>

namespace
{

void writeMMFile(const void *ptr, size_t size, size_t count, FILE *fp, const std::string &err_msg)
{
  if (std::fwrite(ptr, size, count, fp) != count)
  {
    std::fclose(fp);
    throw std::runtime_error(err_msg);
  }
}

void seekMMFile(FILE *fp, int64_t offset, int whence, const std::string &err_msg)
{
  if (std::fseek(fp, offset, whence) != 0)
  {
    std::fclose(fp);
    throw std::runtime_error(err_msg);
  }
}

} // namespace

namespace onert
{
namespace exec
{

RawMinMaxDumper::RawMinMaxDumper(const std::string &filename) : _filename(filename) {}

void RawMinMaxDumper::dump(const exec::IOMinMaxMap &input_minmax,
                           const exec::OpMinMaxMap &op_minmax) const
{
  // Find file is already exist for modifying
  auto file = std::fopen(_filename.c_str(), "rb+");
  uint32_t runs = 1;

  // Magic code and version
  // Match with runtime/onert/odc/MinMaxReader.cc
  // TODO Use util to share code and version
  const uint32_t MAGIC_CODE = 0x4F4D4D44;
  const uint32_t VERSION = 1;
  if (!file)
  {
    // If file is not exist, create new file
    file = std::fopen(_filename.c_str(), "wb+");
    if (!file)
      throw std::runtime_error{"RawMinMaxDumper: Failed to open minmax file " + _filename};

    // Write magic code and version
    writeMMFile(&MAGIC_CODE, sizeof(uint32_t), 1, file,
                "RawMinMaxDumper: Failed to write magic code");
    writeMMFile(&VERSION, sizeof(uint32_t), 1, file, "RawMinMaxDumper: Failed to write version");
  }
  else
  {
    // Check magic code and version
    seekMMFile(file, 0, SEEK_SET, "RawMinMaxDumper: Failed to seek file start position");
    ;
    uint32_t read_magic_code = 0;
    uint32_t read_version = 0;
    bool rewrite = true;
    if (std::fread(&read_magic_code, sizeof(uint32_t), 1, file) == 1 &&
        read_magic_code == MAGIC_CODE &&
        std::fread(&read_version, sizeof(uint32_t), 1, file) == 1 && read_version == VERSION)
      rewrite = false;

    // Destroy and create if file is not valid
    if (rewrite)
    {
      std::fclose(file);
      file = std::fopen(_filename.c_str(), "wb+");
      if (!file)
        throw std::runtime_error{"RawMinMaxDumper: Failed to rewrite minmax file " + _filename};

      // Write magic code and version
      writeMMFile(&MAGIC_CODE, sizeof(uint32_t), 1, file,
                  "RawMinMaxDumper: Failed to write magic code");
      writeMMFile(&VERSION, sizeof(uint32_t), 1, file, "RawMinMaxDumper: Failed to write version");
    }
  }

  // Read run count
  if (std::fread(&runs, sizeof(uint32_t), 1, file) == 1)
    runs++;
  else
    runs = 1;

  // TODO Verify file size

  // Overwrite run count
  seekMMFile(file, sizeof(MAGIC_CODE) + sizeof(VERSION), SEEK_SET,
             "RawMinMaxDumper: Failed to seek to header");
  writeMMFile(&runs, sizeof(uint32_t), 1, file, "RawMinMaxDumper: Failed to write run count");

  // Go to end of file to append new data
  seekMMFile(file, 0, SEEK_END, "RawMinMaxDumper: Failed to seek to end of file");

  uint32_t input_count = input_minmax.size();
  uint32_t op_count = op_minmax.size();

  // Write op_count and input_count
  writeMMFile(&op_count, sizeof(uint32_t), 1, file, "RawMinMaxDumper: Failed to write op_count");
  writeMMFile(&input_count, sizeof(uint32_t), 1, file,
              "RawMinMaxDumper: Failed to write input_count");

  // For each op
  for (auto &&elem : op_minmax)
  {
    const uint32_t model_idx = 0;
    const uint32_t subg_idx = elem.first.first.value();
    const uint32_t op_idx = elem.first.second.value();

    // Write model/subg/op index
    writeMMFile(&model_idx, sizeof(uint32_t), 1, file,
                "RawMinMaxDumper: Failed to write OP model index");
    writeMMFile(&subg_idx, sizeof(uint32_t), 1, file,
                "RawMinMaxDumper: Failed to write OP subg index");
    writeMMFile(&op_idx, sizeof(uint32_t), 1, file, "RawMinMaxDumper: Failed to write OP index");

    // Write min/max
    writeMMFile(elem.second.data, sizeof(float), 2, file,
                "RawMinMaxDumper: Failed to write OP min/max");
  }

  // For each input
  for (auto &&elem : input_minmax)
  {
    const uint32_t model_idx = 0;
    const uint32_t subg_idx = elem.first.first.value();
    const uint32_t input_idx = elem.first.second.value();

    // Write model/subg/input index
    writeMMFile(&model_idx, sizeof(uint32_t), 1, file,
                "RawMinMaxDumper: Failed to write input model index");
    writeMMFile(&subg_idx, sizeof(uint32_t), 1, file,
                "RawMinMaxDumper: Failed to write input subg index");
    writeMMFile(&input_idx, sizeof(uint32_t), 1, file,
                "RawMinMaxDumper: Failed to write input index");

    // Write min/max
    writeMMFile(elem.second.data, sizeof(float), 2, file,
                "RawMinMaxDumper: Failed to write min/max");
  }

  std::fclose(file);
}

} // namespace exec
} // namespace onert
