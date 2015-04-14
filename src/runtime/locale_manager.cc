// Copyright 2015 Samsung Electronics Co, Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "runtime/locale_manager.h"

#include <runtime_info.h>
#include <memory>
#include <algorithm>

#include "common/logger.h"
#include "common/file_utils.h"

namespace wrt {

namespace {

std::string localeToBCP47LangTag(
    const std::string locale) {
  // Cut off codepage information from given string (if any exists)
  // i.e. change en_US.UTF-8 into en_US */
  std::string lang = locale.substr(0, locale.find_first_of("."));

  // Replace all '_' with '-'
  std::replace(lang.begin(), lang.end(), '_', '-');
  return lang;
}

}  // namespace


LocaleManager::LocaleManager() {
  UpdateSystemLocale();
}

LocaleManager::~LocaleManager() {
}

void LocaleManager::SetDefaultLocale(const std::string& locale) {
  if (!default_locale_.empty() && system_locales_.size() > 0 &&
       system_locales_.back() == default_locale_) {
    system_locales_.pop_back();
  }
  default_locale_ = locale;
  if (!default_locale_.empty()) {
    system_locales_.push_back(locale);
  }
}

void LocaleManager::UpdateSystemLocale() {
  char* str = NULL;
  if (RUNTIME_INFO_ERROR_NONE !=
      runtime_info_get_value_string(RUNTIME_INFO_KEY_LANGUAGE, &str)
     || str == NULL) {
    return;
  }
  std::string lang = localeToBCP47LangTag(str);

  if (lang.length() == 0) {
    LoggerE("Language tag was invalid");
    return;
  }

  system_locales_.clear();
  while (true) {
    LoggerD("Processing language description: %s", lang.c_str());
    system_locales_.push_back(lang);

    // compatibility with lower language Tag by SDK
    std::string lower = lang;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower != lang) {
      system_locales_.push_back(lower);
    }
    size_t position = lang.find_last_of("-");
    if (position == std::string::npos) {
      break;
    }
    lang = lang.substr(0, position);
  }
  if (!default_locale_.empty()) {
    system_locales_.push_back(default_locale_);
  }
}

}  // namespace wrt
