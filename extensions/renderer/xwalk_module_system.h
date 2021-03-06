// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_EXTENSIONS_RENDERER_XWALK_MODULE_SYSTEM_H_
#define XWALK_EXTENSIONS_RENDERER_XWALK_MODULE_SYSTEM_H_

#include <v8/v8.h>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace extensions {

class XWalkExtensionModule;

// Interface used to expose objects via the requireNative() function in JS API
// code. Native modules should be registered with the module system.
class XWalkNativeModule {
 public:
  virtual v8::Handle<v8::Object> NewInstance() = 0;
  virtual ~XWalkNativeModule() {}
};


class XWalkModuleSystem {
 public:
  explicit XWalkModuleSystem(v8::Handle<v8::Context> context);
  ~XWalkModuleSystem();

  static XWalkModuleSystem* GetModuleSystemFromContext(
      v8::Handle<v8::Context> context);
  static void SetModuleSystemInContext(
      std::unique_ptr<XWalkModuleSystem> module_system,
      v8::Handle<v8::Context> context);
  static void ResetModuleSystemFromContext(v8::Handle<v8::Context> context);

  void RegisterExtensionModule(std::unique_ptr<XWalkExtensionModule> module,
                               const std::vector<std::string>& entry_points);
  void RegisterNativeModule(const std::string& name,
                            std::unique_ptr<XWalkNativeModule> module);
  v8::Handle<v8::Object> RequireNative(const std::string& name);

  void Initialize();

  v8::Handle<v8::Context> GetV8Context();

 private:
  struct ExtensionModuleEntry {
    ExtensionModuleEntry(const std::string& name, XWalkExtensionModule* module,
                         const std::vector<std::string>& entry_points);
    ~ExtensionModuleEntry();
    std::string name;
    XWalkExtensionModule* module;
    bool use_trampoline;
    std::vector<std::string> entry_points;
    bool operator<(const ExtensionModuleEntry& other) const {
      return name < other.name;
    }

    static bool IsPrefix(const ExtensionModuleEntry& first,
                         const ExtensionModuleEntry& second);
  };

  bool SetTrampolineAccessorForEntryPoint(
      v8::Handle<v8::Context> context,
      const std::string& entry_point,
      v8::Local<v8::External> user_data);

  static bool DeleteAccessorForEntryPoint(v8::Handle<v8::Context> context,
                                          const std::string& entry_point);

  bool InstallTrampoline(v8::Handle<v8::Context> context,
                         ExtensionModuleEntry* entry);

  static void TrampolineCallback(
      v8::Local<v8::String> property,
      const v8::PropertyCallbackInfo<v8::Value>& info);
  static void TrampolineSetterCallback(
      v8::Local<v8::String> property,
      v8::Local<v8::Value> value,
      const v8::PropertyCallbackInfo<void>& info);
  static void LoadExtensionForTrampoline(
      v8::Isolate* isolate,
      v8::Local<v8::Value> data);
  static v8::Handle<v8::Value> RefetchHolder(
    v8::Isolate* isolate,
    v8::Local<v8::Value> data);

  bool ContainsEntryPoint(const std::string& entry_point);
  void MarkModulesWithTrampoline();
  void DeleteExtensionModules();

  void EnsureExtensionNamespaceIsReadOnly(v8::Handle<v8::Context> context,
                                          const std::string& extension_name);

  typedef std::vector<ExtensionModuleEntry> ExtensionModules;
  ExtensionModules extension_modules_;
  typedef std::map<std::string, XWalkNativeModule*> NativeModuleMap;
  NativeModuleMap native_modules_;

  v8::Persistent<v8::FunctionTemplate> require_native_template_;
  v8::Persistent<v8::Object> function_data_;

  // Points back to the current context, used when native wants to callback
  // JavaScript. When WillReleaseScriptContext() is called, we dispose this
  // persistent.
  v8::Persistent<v8::Context> v8_context_;
};

}  // namespace extensions

#endif  // XWALK_EXTENSIONS_RENDERER_XWALK_MODULE_SYSTEM_H_
