/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd. All Rights Reserved
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

#ifndef ONERT_MICRO_TRAIN_TESTS_MODELS_BOSTON_HOUSING_H
#define ONERT_MICRO_TRAIN_TESTS_MODELS_BOSTON_HOUSING_H

#include <vector>
#include <cstring>

namespace onert_micro
{
namespace train
{
namespace test
{
namespace models
{

unsigned char boston_housing_model[] = {
  0x18, 0x00, 0x00, 0x00, 0x43, 0x49, 0x52, 0x30, 0x00, 0x00, 0x0e, 0x00, 0x14, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x08, 0x00, 0x10, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0xdc, 0x07, 0x00, 0x00, 0x5c, 0x0a, 0x00, 0x00, 0x78, 0x0a, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
  0xc8, 0x07, 0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x30, 0x01, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00,
  0x90, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0xee, 0xfe, 0xff, 0xff, 0x04, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00,
  0x08, 0x00, 0x0e, 0x00, 0x08, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
  0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x10, 0x00, 0x0c, 0x00,
  0x08, 0x00, 0x04, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x32, 0x2e, 0x31, 0x33, 0x2e, 0x31, 0x00, 0x00,
  0x4e, 0xff, 0xff, 0xff, 0x04, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x31, 0x2e, 0x35, 0x2e,
  0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xf6, 0xff, 0xff,
  0x7c, 0xf6, 0xff, 0xff, 0x72, 0xff, 0xff, 0xff, 0x04, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
  0x9c, 0xe2, 0x3c, 0x3e, 0x58, 0xe6, 0x55, 0x3e, 0x7a, 0x51, 0xce, 0x3e, 0xdd, 0x3d, 0x98, 0xbe,
  0x40, 0x98, 0xf7, 0xbd, 0xfc, 0x33, 0x70, 0x3e, 0xe7, 0x7a, 0x47, 0xbe, 0xa8, 0x1e, 0xbd, 0xbe,
  0x57, 0x17, 0xa7, 0xbe, 0xf0, 0x98, 0x08, 0x3e, 0x98, 0xfb, 0xc6, 0x3e, 0x34, 0xb0, 0xc9, 0xbd,
  0x19, 0xf1, 0xa6, 0xbe, 0xe0, 0x39, 0x6a, 0xbc, 0x10, 0xba, 0x0b, 0x3e, 0xc5, 0xb5, 0xd4, 0xbe,
  0x82, 0x29, 0xd3, 0xbe, 0x0c, 0xc7, 0x69, 0x3e, 0x48, 0x86, 0x6e, 0x3d, 0x83, 0x90, 0x8d, 0xbe,
  0x9c, 0x1b, 0x8a, 0xbd, 0x70, 0x01, 0x12, 0x3e, 0xf0, 0x89, 0xcd, 0x3c, 0xe1, 0x8f, 0x85, 0xbe,
  0x48, 0xd2, 0xdc, 0x3d, 0x1a, 0xac, 0x27, 0xbe, 0x50, 0xf6, 0xf0, 0x3d, 0x81, 0x27, 0xbc, 0xbe,
  0x92, 0xdb, 0xc0, 0xbe, 0xaa, 0x81, 0x60, 0xbe, 0x30, 0x9b, 0x56, 0x3e, 0x60, 0xce, 0xcb, 0x3e,
  0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x80, 0x06, 0x00, 0x00, 0x44, 0xdd, 0x88, 0xbd, 0x20, 0xba, 0x3a, 0xbd, 0x74, 0x3a, 0x65, 0x3e,
  0xb0, 0x40, 0xb6, 0x3e, 0x73, 0x3a, 0x51, 0xbe, 0x64, 0x00, 0xe5, 0xbd, 0x1e, 0xb2, 0x23, 0xbe,
  0x58, 0x4e, 0xaf, 0x3e, 0x9c, 0x47, 0xab, 0xbd, 0x00, 0xf3, 0x87, 0x3e, 0xc0, 0x18, 0x4c, 0x3c,
  0x18, 0xbf, 0xa7, 0xbd, 0xe6, 0x1f, 0xaf, 0x3e, 0xb8, 0xc2, 0x95, 0x3e, 0xd8, 0x64, 0xa0, 0x3e,
  0xd4, 0xb3, 0x84, 0xbe, 0x60, 0x32, 0x0a, 0x3e, 0x80, 0xef, 0x1f, 0xbb, 0x50, 0x95, 0x3e, 0x3d,
  0xe0, 0x7b, 0x1d, 0x3e, 0xc8, 0xa1, 0x39, 0x3e, 0x43, 0x46, 0x20, 0xbe, 0x18, 0x9f, 0xb1, 0x3e,
  0xe3, 0x42, 0x1a, 0xbe, 0x53, 0xd2, 0x17, 0xbe, 0xd2, 0x96, 0xa3, 0x3e, 0x70, 0x54, 0x5a, 0xbd,
  0x50, 0xb1, 0x61, 0x3e, 0x88, 0xbf, 0xe1, 0x3d, 0x40, 0x26, 0xc1, 0xbd, 0x88, 0xd0, 0xa4, 0x3e,
  0x4c, 0x65, 0x8d, 0x3e, 0x38, 0xce, 0x6c, 0x3d, 0x7d, 0x7d, 0x98, 0xbe, 0xe8, 0xc4, 0x00, 0x3e,
  0x36, 0x33, 0x4c, 0xbe, 0x80, 0xa4, 0xf2, 0x3c, 0xa4, 0x5f, 0xe5, 0xbd, 0x74, 0x3e, 0x13, 0x3e,
  0x4c, 0x98, 0x45, 0x3e, 0x40, 0x8e, 0x2c, 0xbc, 0xdc, 0xdf, 0x9f, 0xbe, 0xe6, 0xad, 0x89, 0x3e,
  0xb8, 0x69, 0x75, 0x3e, 0x04, 0xbe, 0x8a, 0xbe, 0xdf, 0x27, 0x14, 0xbe, 0xfa, 0xbe, 0x8a, 0xbe,
  0xda, 0x95, 0x51, 0xbe, 0x80, 0x4d, 0x99, 0x3b, 0x04, 0x50, 0xd1, 0xbd, 0xc0, 0xec, 0x62, 0x3e,
  0x90, 0xe2, 0x6e, 0x3e, 0x58, 0x12, 0x8b, 0xbe, 0xa0, 0x1c, 0x96, 0x3d, 0xe0, 0xc6, 0x2d, 0x3e,
  0x20, 0x49, 0x22, 0xbd, 0x00, 0xc9, 0xbf, 0x3b, 0x6e, 0x46, 0x5f, 0xbe, 0xfc, 0x63, 0x85, 0xbd,
  0x1c, 0xa7, 0x37, 0x3e, 0x52, 0x4b, 0xfd, 0xbd, 0xfe, 0x38, 0x9a, 0x3e, 0xd8, 0xac, 0xa8, 0x3d,
  0x32, 0x41, 0xaf, 0x3e, 0xd9, 0xe3, 0x88, 0xbe, 0x4a, 0x50, 0x5f, 0xbe, 0x9e, 0x83, 0x5f, 0xbe,
  0x40, 0x67, 0xf5, 0xbc, 0x20, 0x45, 0xa7, 0xbd, 0xb5, 0xba, 0x98, 0xbe, 0xcc, 0x4f, 0xb1, 0x3e,
  0x36, 0xd7, 0x87, 0xbe, 0x33, 0x41, 0x36, 0xbe, 0xd4, 0x40, 0x6f, 0x3e, 0xa8, 0x81, 0x4e, 0xbe,
  0x6e, 0x28, 0xa9, 0xbe, 0xed, 0xad, 0x9f, 0xbe, 0x10, 0xa8, 0x70, 0x3e, 0xff, 0xbf, 0x09, 0xbe,
  0x40, 0x80, 0xf8, 0xbb, 0xe0, 0xfd, 0x15, 0x3c, 0x65, 0x13, 0x25, 0xbe, 0x68, 0x69, 0x33, 0x3e,
  0x58, 0xa6, 0x7d, 0xbd, 0xec, 0x9a, 0x82, 0x3e, 0x15, 0xe3, 0x4b, 0xbe, 0xc0, 0x90, 0x72, 0xbc,
  0xcc, 0xad, 0x48, 0x3e, 0x11, 0xad, 0xb3, 0xbe, 0x74, 0xcf, 0x8a, 0xbe, 0x08, 0xa2, 0x72, 0x3d,
  0xa0, 0xd3, 0x3c, 0x3e, 0x35, 0xa4, 0x0e, 0xbe, 0xc0, 0x53, 0x70, 0x3d, 0x08, 0x52, 0x55, 0xbd,
  0x18, 0xce, 0x0c, 0x3e, 0xa4, 0xba, 0xa0, 0x3e, 0x58, 0x8f, 0x0b, 0x3e, 0x4a, 0x69, 0xb0, 0xbe,
  0xe6, 0x79, 0x64, 0xbe, 0x10, 0x1d, 0xff, 0x3d, 0xa4, 0x6a, 0xa0, 0xbe, 0x28, 0x50, 0x88, 0xbe,
  0xa8, 0x1c, 0x98, 0xbe, 0x60, 0xe7, 0x37, 0x3e, 0xf8, 0x9f, 0x1d, 0x3e, 0x4c, 0x81, 0x82, 0xbd,
  0x88, 0x11, 0x5a, 0x3d, 0x10, 0x27, 0x58, 0xbe, 0xa0, 0xa2, 0x45, 0xbc, 0xd0, 0x29, 0x8d, 0x3c,
  0x7c, 0x3d, 0x6b, 0x3e, 0xec, 0x23, 0x11, 0x3e, 0xd2, 0x71, 0x56, 0xbe, 0x55, 0x22, 0x2c, 0xbe,
  0xfe, 0xe1, 0x02, 0x3e, 0xf0, 0x5c, 0x6f, 0x3e, 0x87, 0x57, 0xb6, 0xbe, 0x3c, 0xec, 0x31, 0x3e,
  0xac, 0x4d, 0x2c, 0xbe, 0x62, 0xf5, 0xf3, 0xbd, 0x84, 0xe6, 0xae, 0xbe, 0x56, 0x30, 0x90, 0xbe,
  0x19, 0x6c, 0x91, 0xbe, 0xcc, 0x22, 0x39, 0xbe, 0xcc, 0xb0, 0x90, 0x3e, 0x9a, 0xfc, 0x6f, 0xbe,
  0x18, 0x6e, 0xc1, 0xbd, 0x39, 0xe0, 0x30, 0xbe, 0xcc, 0x72, 0xae, 0x3e, 0xab, 0x9c, 0x8c, 0xbe,
  0x30, 0xf2, 0xb3, 0x3c, 0x3d, 0x56, 0x0c, 0xbe, 0x4f, 0xe7, 0x56, 0xbe, 0x2b, 0x23, 0x49, 0xbe,
  0xc0, 0x29, 0xbe, 0xbc, 0x9c, 0x23, 0xe4, 0x3d, 0x9c, 0x65, 0x7d, 0xbe, 0xcc, 0x19, 0x81, 0x3e,
  0x90, 0xfc, 0xcc, 0xbd, 0x8a, 0x00, 0x83, 0xbe, 0xa4, 0x74, 0x9e, 0x3e, 0x88, 0x13, 0x54, 0x3e,
  0xdc, 0x0a, 0x80, 0x3d, 0x3c, 0x51, 0x71, 0x3e, 0x04, 0xf4, 0x89, 0x3d, 0x0c, 0x6b, 0x12, 0x3e,
  0xb3, 0xed, 0x53, 0xbe, 0x64, 0x7b, 0x9c, 0xbe, 0x4d, 0xaf, 0x88, 0xbe, 0xa2, 0xc9, 0x03, 0xbe,
  0x82, 0xe8, 0x8a, 0xbe, 0x14, 0x4b, 0x8e, 0x3e, 0xc0, 0x08, 0x47, 0x3c, 0x70, 0xf7, 0xa9, 0xbc,
  0x32, 0x3b, 0x99, 0x3e, 0x24, 0x62, 0xa4, 0xbe, 0x28, 0xb6, 0x62, 0x3d, 0x9c, 0x03, 0xdd, 0xbd,
  0x20, 0x52, 0xb1, 0x3e, 0x63, 0xc3, 0x0d, 0xbe, 0x5a, 0xbe, 0xae, 0x3e, 0x64, 0x20, 0x85, 0xbd,
  0xa6, 0x54, 0x94, 0x3e, 0x98, 0x4d, 0x3a, 0xbd, 0x19, 0xfc, 0xac, 0xbe, 0x38, 0x77, 0x8f, 0xbd,
  0x58, 0x41, 0xd9, 0x3d, 0x60, 0x4e, 0x18, 0xbd, 0xcc, 0x3b, 0x22, 0x3e, 0x51, 0x2d, 0x84, 0xbe,
  0xde, 0xe7, 0x65, 0xbe, 0xb6, 0x87, 0x26, 0xbe, 0x28, 0x15, 0x8c, 0x3e, 0x58, 0x8b, 0x2c, 0x3e,
  0x52, 0x09, 0x21, 0xbe, 0xaa, 0xf0, 0x64, 0xbe, 0xec, 0x91, 0x89, 0x3d, 0x91, 0xd1, 0x39, 0xbe,
  0x20, 0x5a, 0x84, 0xbc, 0xf0, 0xca, 0xb2, 0x3e, 0x37, 0x99, 0x5e, 0xbe, 0xf8, 0x14, 0xa9, 0x3d,
  0xc2, 0x00, 0x8f, 0xbe, 0x0c, 0x48, 0x22, 0x3e, 0x60, 0x78, 0x1f, 0xbc, 0xea, 0x55, 0x8e, 0x3e,
  0x6a, 0xc5, 0x95, 0x3e, 0x27, 0xa0, 0x8d, 0xbe, 0x4a, 0x00, 0x20, 0xbe, 0xae, 0xb3, 0x4e, 0xbe,
  0xb0, 0xaa, 0x24, 0x3e, 0x68, 0x1b, 0x18, 0x3d, 0x6f, 0xa4, 0x51, 0xbe, 0xea, 0xda, 0xa0, 0x3e,
  0x8c, 0x9d, 0x1e, 0xbe, 0x38, 0x7e, 0x08, 0xbd, 0xf4, 0xe0, 0xad, 0xbd, 0xa7, 0x36, 0x8b, 0xbe,
  0xf4, 0x6d, 0xab, 0x3e, 0x1c, 0xd4, 0x8b, 0x3e, 0x00, 0xf2, 0x37, 0x3d, 0xa8, 0x63, 0x94, 0xbd,
  0x8c, 0x5b, 0x15, 0x3e, 0x3a, 0xf4, 0x6f, 0xbe, 0xe9, 0xfc, 0x4c, 0xbe, 0x62, 0xc9, 0x54, 0xbe,
  0xe4, 0x8d, 0x88, 0x3e, 0x6e, 0xa9, 0x9e, 0x3e, 0xd0, 0xdc, 0x52, 0x3e, 0x8c, 0xe6, 0x8d, 0x3e,
  0xb0, 0x48, 0x86, 0x3e, 0x38, 0x7b, 0xa8, 0x3e, 0x40, 0xd1, 0xcf, 0xbd, 0x00, 0xf7, 0xcf, 0xbd,
  0xec, 0xb8, 0xc3, 0x3d, 0x04, 0x96, 0x9a, 0x3e, 0x6e, 0xf3, 0xa6, 0x3e, 0x6c, 0x58, 0x89, 0x3e,
  0x06, 0x33, 0xaf, 0x3e, 0x00, 0x0d, 0x22, 0xbb, 0xc0, 0x30, 0x94, 0xbc, 0x80, 0x0b, 0x8c, 0xbd,
  0x68, 0xfc, 0x30, 0x3e, 0xe0, 0x4f, 0xba, 0x3e, 0xc0, 0xd4, 0xb6, 0x3e, 0x32, 0xba, 0x2e, 0xbe,
  0xbe, 0x3f, 0x06, 0xbe, 0x3d, 0x70, 0xb4, 0xbe, 0xa2, 0x6b, 0x75, 0xbe, 0x54, 0xc6, 0x2c, 0x3e,
  0xa8, 0x1d, 0x14, 0x3d, 0x04, 0xd3, 0xaf, 0xbe, 0x88, 0xa2, 0xa4, 0x3e, 0x7a, 0x98, 0x9c, 0xbe,
  0xf8, 0x73, 0x65, 0xbd, 0x3e, 0x55, 0x95, 0xbe, 0x80, 0x9e, 0x7c, 0xbb, 0x50, 0xdd, 0xae, 0x3d,
  0xf5, 0xa9, 0x83, 0xbe, 0x1b, 0xf6, 0x0c, 0xbe, 0x5e, 0xf6, 0xae, 0x3e, 0x04, 0x42, 0x9d, 0x3e,
  0x50, 0x1d, 0x38, 0x3e, 0x33, 0x25, 0x54, 0xbe, 0xfc, 0x5f, 0x89, 0xbd, 0xde, 0xc7, 0x2b, 0xbe,
  0xae, 0x25, 0xb5, 0x3e, 0xbc, 0x8e, 0x9c, 0xbd, 0x70, 0xce, 0xc1, 0x3d, 0x2c, 0xa5, 0xae, 0x3d,
  0x3c, 0xcb, 0x4b, 0xbe, 0xd8, 0xe7, 0xac, 0xbe, 0xf0, 0xd4, 0x6a, 0x3e, 0xf6, 0x8b, 0x28, 0xbe,
  0xf4, 0xaf, 0x69, 0x3e, 0xed, 0xed, 0xb0, 0xbe, 0x68, 0x4d, 0xa4, 0xbe, 0x00, 0x21, 0x6a, 0x3d,
  0x78, 0x96, 0xd3, 0x3d, 0xea, 0x7c, 0x08, 0x3e, 0x78, 0xd4, 0x79, 0xbd, 0xf4, 0x00, 0x87, 0x3e,
  0x64, 0xd9, 0x58, 0x3e, 0x68, 0xea, 0x4b, 0x3e, 0xe0, 0x93, 0xa1, 0xbd, 0x8c, 0x8a, 0xee, 0x3d,
  0x68, 0xd1, 0x87, 0x3e, 0x79, 0x2f, 0xaa, 0xbe, 0xf0, 0xbc, 0x94, 0xbd, 0x48, 0x21, 0x38, 0x3d,
  0x9c, 0xf2, 0xd2, 0x3d, 0x0c, 0xc2, 0x7d, 0x3e, 0x84, 0x19, 0x83, 0xbd, 0xa8, 0x3a, 0xc1, 0xbd,
  0x65, 0xa1, 0x9b, 0xbe, 0x9c, 0xe4, 0x7a, 0x3e, 0xd6, 0xbc, 0x1e, 0xbe, 0xb8, 0x32, 0xb8, 0xbd,
  0x08, 0x0f, 0xf4, 0x3d, 0x3c, 0xc0, 0x8c, 0x3e, 0x40, 0x68, 0xb0, 0xbc, 0xf8, 0xa3, 0x12, 0x3e,
  0x14, 0xfe, 0x08, 0x3e, 0xb0, 0x2d, 0x83, 0x3d, 0xc0, 0xe6, 0x10, 0x3c, 0x85, 0x72, 0x14, 0xbe,
  0x52, 0x13, 0xb7, 0x3e, 0xf4, 0xdd, 0xab, 0xbd, 0x48, 0x80, 0x1c, 0x3e, 0x9c, 0x9f, 0x36, 0x3e,
  0x88, 0x11, 0x48, 0x3e, 0xc6, 0x49, 0x80, 0xbe, 0x0d, 0x5b, 0xb8, 0xbe, 0x6a, 0x80, 0x04, 0x3e,
  0x68, 0x3e, 0x0d, 0xbe, 0xba, 0xd7, 0xb8, 0xbe, 0x88, 0x68, 0x99, 0xbd, 0x23, 0x44, 0x91, 0xbe,
  0xc8, 0xa9, 0xec, 0xbd, 0x9e, 0xf1, 0x99, 0xbe, 0x39, 0xf2, 0xb8, 0xbe, 0xa8, 0xbd, 0x35, 0x3d,
  0xe0, 0x1d, 0x64, 0xbc, 0xe8, 0xac, 0xc8, 0xbd, 0x88, 0xe6, 0x21, 0x3d, 0x3a, 0xaf, 0x95, 0xbe,
  0x74, 0x8b, 0xcf, 0x3d, 0xd8, 0x3d, 0xb2, 0x3e, 0x98, 0x86, 0x24, 0x3d, 0xd6, 0x9b, 0x82, 0x3e,
  0xa0, 0x37, 0x50, 0x3e, 0x44, 0xef, 0x00, 0x3e, 0xb8, 0x70, 0x56, 0x3e, 0xbc, 0x10, 0x51, 0x3e,
  0x66, 0xcb, 0x69, 0xbe, 0xa0, 0xdd, 0x11, 0x3e, 0x70, 0x73, 0xbb, 0x3d, 0xbe, 0xdf, 0x81, 0xbe,
  0xe9, 0x22, 0xab, 0xbe, 0x80, 0x11, 0xf9, 0x3b, 0x14, 0xba, 0x80, 0x3e, 0x76, 0x33, 0x0c, 0xbe,
  0x64, 0xe2, 0x65, 0xbe, 0x72, 0x23, 0x96, 0x3e, 0x0c, 0x60, 0x45, 0x3e, 0xf6, 0x4d, 0x91, 0x3e,
  0x3c, 0x9a, 0x59, 0xbe, 0x96, 0x6d, 0xff, 0xbd, 0x52, 0x45, 0xac, 0xbe, 0x57, 0xed, 0x47, 0xbe,
  0xa0, 0xfb, 0xa3, 0x3c, 0x7e, 0x1a, 0x36, 0xbe, 0xd8, 0x77, 0xb7, 0x3d, 0xe0, 0xd8, 0x0f, 0xbc,
  0xec, 0x47, 0xa4, 0xbd, 0x1c, 0x06, 0x14, 0x3e, 0x05, 0xb4, 0x83, 0xbe, 0x28, 0x6d, 0xa4, 0xbd,
  0x98, 0xeb, 0x87, 0xbe, 0xde, 0xb4, 0x5f, 0xbe, 0x04, 0x53, 0xd3, 0x3d, 0xd3, 0xd0, 0x43, 0xbe,
  0xd0, 0xa3, 0x29, 0xbd, 0x70, 0xce, 0x12, 0xbe, 0xb4, 0x1f, 0x00, 0xbe, 0xd0, 0x5b, 0x41, 0x3e,
  0x2c, 0x70, 0x84, 0x3e, 0x98, 0x0c, 0x57, 0xbd, 0xc8, 0x8e, 0x1f, 0x3d, 0xb4, 0xa9, 0x9e, 0x3e,
  0xbc, 0x8a, 0x94, 0x3e, 0x80, 0x03, 0x86, 0x3e, 0xa4, 0x67, 0xae, 0x3e, 0x00, 0x72, 0xf6, 0xbc,
  0xac, 0xb1, 0x8c, 0x3e, 0x87, 0xf2, 0x83, 0xbe, 0x0c, 0x3c, 0x80, 0xbe, 0x14, 0xa5, 0xaa, 0xbd,
  0x0c, 0x07, 0x81, 0xbe, 0x04, 0x9f, 0xbe, 0x3d, 0x10, 0xa3, 0x2d, 0x3e, 0x07, 0x8c, 0xa0, 0xbe,
  0x78, 0xa7, 0x38, 0xbe, 0xac, 0xae, 0x86, 0x3d, 0xa1, 0x65, 0xaa, 0xbe, 0x14, 0x87, 0x5e, 0x3e,
  0x2e, 0x56, 0xae, 0xbe, 0xd9, 0x44, 0xa4, 0xbe, 0x10, 0x1b, 0x4c, 0x3e, 0xb8, 0xff, 0x19, 0x3e,
  0x7a, 0x57, 0xb0, 0x3e, 0xf8, 0xe7, 0x01, 0xbd, 0x28, 0x41, 0x26, 0x3d, 0xb4, 0x33, 0x90, 0x3e,
  0xc8, 0x56, 0xe8, 0x3d, 0x4e, 0xe0, 0x1b, 0xbe, 0x60, 0xef, 0x71, 0x3e, 0xc0, 0xc8, 0xe4, 0xbd,
  0x58, 0xc2, 0x68, 0x3e, 0x74, 0xf0, 0x11, 0x3e, 0x4d, 0x0f, 0xa9, 0xbe, 0x5f, 0x73, 0x40, 0xbe,
  0x90, 0x80, 0x1f, 0x3e, 0x96, 0x71, 0x8d, 0xbe, 0x08, 0x20, 0x51, 0xbd, 0xf8, 0xd7, 0x16, 0xbe,
  0x50, 0x3b, 0x1c, 0x3e, 0x12, 0xef, 0x80, 0x3e, 0x54, 0xe1, 0xf8, 0x3d, 0xa4, 0x12, 0x85, 0x3e,
  0xd6, 0xb5, 0x94, 0x3e, 0x18, 0xd7, 0xb0, 0xbd, 0xa4, 0xc0, 0x33, 0x3e, 0x28, 0x83, 0xa8, 0x3e,
  0x40, 0x46, 0x88, 0xbc, 0x18, 0xbb, 0xc8, 0x3d, 0x3c, 0x50, 0x55, 0xbe, 0x2c, 0x51, 0x4c, 0x3e,
  0xb0, 0x62, 0x8e, 0x3e, 0xca, 0x36, 0x93, 0x3e, 0xd8, 0x18, 0x9c, 0xbe, 0xcc, 0x8a, 0x52, 0xbe,
  0xc7, 0x07, 0x5d, 0xbe, 0x00, 0x50, 0x95, 0xba, 0xf4, 0x9b, 0x12, 0x3e, 0xfc, 0x6a, 0xb0, 0x3e,
  0x66, 0xcb, 0xa5, 0xbe, 0xa0, 0x9d, 0x18, 0x3e, 0x30, 0x8e, 0xf3, 0x3c, 0xce, 0xac, 0x98, 0x3e,
  0x71, 0x55, 0x79, 0xbe, 0x10, 0x9e, 0xb8, 0x3c, 0xd2, 0x42, 0x41, 0xbe, 0xbb, 0xda, 0x24, 0xbe,
  0x00, 0x7d, 0x67, 0x3e, 0xa0, 0xfd, 0xff, 0xff, 0xa4, 0xfd, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00,
  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x18, 0x00, 0x14, 0x00, 0x10, 0x00, 0x0c, 0x00,
  0x08, 0x00, 0x04, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00,
  0xb0, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00,
  0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00,
  0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x14, 0x00, 0x00, 0x00, 0x10, 0x00, 0x0c, 0x00,
  0x07, 0x00, 0x08, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x0c, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x14, 0xfe, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00,
  0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x0e, 0x00, 0x16, 0x00, 0x00, 0x00, 0x10, 0x00, 0x0c, 0x00,
  0x07, 0x00, 0x08, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x14, 0x00, 0x00, 0x00,
  0x18, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x07, 0x00,
  0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
  0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x00, 0x00, 0x00, 0x5c, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00,
  0x4c, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0xc6, 0xfe, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0xb8, 0xfe, 0xff, 0xff,
  0x19, 0x00, 0x00, 0x00, 0x53, 0x74, 0x61, 0x74, 0x65, 0x66, 0x75, 0x6c, 0x50, 0x61, 0x72, 0x74,
  0x69, 0x74, 0x69, 0x6f, 0x6e, 0x65, 0x64, 0x43, 0x61, 0x6c, 0x6c, 0x3a, 0x30, 0x00, 0x00, 0x00,
  0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0a, 0xff, 0xff, 0xff,
  0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00,
  0xfc, 0xfe, 0xff, 0xff, 0x39, 0x00, 0x00, 0x00, 0x73, 0x65, 0x71, 0x75, 0x65, 0x6e, 0x74, 0x69,
  0x61, 0x6c, 0x5f, 0x32, 0x33, 0x2f, 0x64, 0x65, 0x6e, 0x73, 0x65, 0x5f, 0x35, 0x31, 0x2f, 0x4d,
  0x61, 0x74, 0x4d, 0x75, 0x6c, 0x3b, 0x73, 0x65, 0x71, 0x75, 0x65, 0x6e, 0x74, 0x69, 0x61, 0x6c,
  0x5f, 0x32, 0x33, 0x2f, 0x64, 0x65, 0x6e, 0x73, 0x65, 0x5f, 0x35, 0x31, 0x2f, 0x52, 0x65, 0x6c,
  0x75, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
  0x6e, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00,
  0x2c, 0x00, 0x00, 0x00, 0x60, 0xff, 0xff, 0xff, 0x1d, 0x00, 0x00, 0x00, 0x73, 0x65, 0x71, 0x75,
  0x65, 0x6e, 0x74, 0x69, 0x61, 0x6c, 0x5f, 0x32, 0x33, 0x2f, 0x64, 0x65, 0x6e, 0x73, 0x65, 0x5f,
  0x35, 0x32, 0x2f, 0x4d, 0x61, 0x74, 0x4d, 0x75, 0x6c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0xb6, 0xff, 0xff, 0xff, 0x10, 0x00, 0x00, 0x00,
  0x10, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0xa8, 0xff, 0xff, 0xff,
  0x1d, 0x00, 0x00, 0x00, 0x73, 0x65, 0x71, 0x75, 0x65, 0x6e, 0x74, 0x69, 0x61, 0x6c, 0x5f, 0x32,
  0x33, 0x2f, 0x64, 0x65, 0x6e, 0x73, 0x65, 0x5f, 0x35, 0x31, 0x2f, 0x4d, 0x61, 0x74, 0x4d, 0x75,
  0x6c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x0e, 0x00, 0x14, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x08, 0x00, 0x04, 0x00,
  0x0e, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
  0x24, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00,
  0x73, 0x65, 0x72, 0x76, 0x69, 0x6e, 0x67, 0x5f, 0x64, 0x65, 0x66, 0x61, 0x75, 0x6c, 0x74, 0x5f,
  0x78, 0x3a, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
  0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x0c, 0x00, 0x0b, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x04, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09,
  0x11, 0x00, 0x00, 0x00, 0x4f, 0x4e, 0x45, 0x2d, 0x74, 0x66, 0x6c, 0x69, 0x74, 0x65, 0x32, 0x63,
  0x69, 0x72, 0x63, 0x6c, 0x65, 0x00, 0x00, 0x00};
unsigned int boston_housing_model_size = 2744;

} // namespace models
} // namespace test
} // namespace train
} // namespace onert_micro

#endif // ONERT_MICRO_TRAIN_TESTS_MODELS_BOSTON_HOUSING_H
