// Copyright 2015 Samsung Electronics Co, Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/string_utils.h"

#include <uuid/uuid.h>
#include <string>
#include <algorithm>

namespace wrt {
namespace utils {

std::string GenerateUUID() {
  char tmp[37];
  uuid_t uuid;
  uuid_generate(uuid);
  uuid_unparse(uuid, tmp);
  return std::string(tmp);
}

bool StartsWith(const std::string& str, const std::string& sub) {
  if (sub.size() > str.size()) return false;
  return std::equal(sub.begin(), sub.end(), str.begin());
}

bool EndsWith(const std::string& str, const std::string& sub) {
  if (sub.size() > str.size()) return false;
  return std::equal(sub.rbegin(), sub.rend(), str.rbegin());
}

std::string ReplaceAll(const std::string& replace,
                       const std::string& from, const std::string& to) {
  std::string str = replace;
  int pos = str.find(from);
  while (pos != std::string::npos) {
    str.replace(pos, from.length(), to);
    pos = str.find(from, pos+to.length());
  }
  return str;
}

}  // namespace utils
}  // namespace wrt
