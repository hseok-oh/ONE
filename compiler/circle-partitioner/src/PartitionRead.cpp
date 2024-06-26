/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd. All Rights Reserved
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

#include "PartitionRead.h"

#include <crew/PConfigIni.h>
#include <crew/PConfigIniDump.h>
#include <luci/Log.h>
#include <pepper/csv2vec.h>

#include <stdexcept>

namespace
{

using namespace partee;

const char *_section_partition = "partition";
const char *_section_OPCODE = "OPCODE";
const char *_section_OPNAME = "OPNAME";

const char *_comply_opcode = "opcode";
const char *_comply_opname = "opname";

const char *_key_backends = "backends";
const char *_key_default = "default";
const char *_key_comply = "comply";
const char *_key_underscore = "_";

luci::PartitionTable parse_table(const crew::Sections &sections)
{
  luci::PartitionTable table;

  // default comply as OPCODE
  table.comply = luci::PartitionTable::COMPLY::OPCODE;

  // read main '[partition]' first
  for (auto &section : sections)
  {
    if (section.name == _section_partition)
    {
      auto &items = section.items;
      if (items.find(_key_backends) == items.end())
      {
        throw std::invalid_argument("'backends' is required");
      }
      if (items.find(_key_default) == items.end())
      {
        throw std::invalid_argument("'default' is required");
      }

      table.groups = pepper::csv_to_vector<std::string>(items.at(_key_backends));
      table.default_group = items.at(_key_default);

      auto comply = items.at(_key_comply);

      // check valid comply types
      if (comply == _comply_opcode)
      {
        table.comply = luci::PartitionTable::COMPLY::OPCODE;
        continue;
      }
      if (comply == _comply_opname)
      {
        table.comply = luci::PartitionTable::COMPLY::OPNAME;
        continue;
      }
      throw std::runtime_error("Invalid or comply is not set");
    }
  }

  // read other sections
  for (auto &section : sections)
  {
    if (section.name == _section_OPCODE)
    {
      auto &items = section.items;

      for (auto &item : items)
      {
        if (item.first == _key_underscore)
        {
          if (table.comply == luci::PartitionTable::COMPLY::OPCODE)
            table.default_group = item.second;
        }
        else
        {
          table.byopcodes.emplace(item.first, item.second);
        }
      }
    }
    else if (section.name == _section_OPNAME)
    {
      auto &items = section.items;

      for (auto &item : items)
      {
        if (item.first == _key_underscore)
        {
          if (table.comply == luci::PartitionTable::COMPLY::OPNAME)
            table.default_group = item.second;
        }
        else
        {
          table.byopnames.emplace(item.first, item.second);
        }
      }
    }
  }

  return table;
}

} // namespace

namespace partee
{

luci::PartitionTable read(const std::string &path)
{
  LOGGER(l);

  INFO(l) << "PartitionConfig: " << path << std::endl;

  auto partition_config = crew::read_ini(path);

  INFO(l) << partition_config << std::endl;

  auto partition_table = parse_table(partition_config);

  return partition_table;
}

} // namespace partee
