// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/renderer/xwalk_extension_renderer_controller.h"

#include <v8/v8.h>
#include <string>
#include <utility>

#include "common/logger.h"
#include "common/profiler.h"
#include "extensions/renderer/object_tools_module.h"
#include "extensions/renderer/widget_module.h"
#include "extensions/renderer/xwalk_extension_client.h"
#include "extensions/renderer/xwalk_extension_module.h"
#include "extensions/renderer/xwalk_module_system.h"
#include "extensions/renderer/xwalk_v8tools_module.h"

namespace extensions {

namespace {

void CreateExtensionModules(XWalkExtensionClient* client,
                            XWalkModuleSystem* module_system) {
  SCOPE_PROFILE();

  auto extensions = client->GetExtensions();
  for (auto it = extensions.begin(); it != extensions.end(); ++it) {
    std::unique_ptr<XWalkExtensionModule> module(
        new XWalkExtensionModule(client, module_system, it->first));
    module_system->RegisterExtensionModule(
        std::move(module), it->second->entry_points());
  }
}

}  // namespace

XWalkExtensionRendererController&
XWalkExtensionRendererController::GetInstance() {
  static XWalkExtensionRendererController instance;
  return instance;
}

XWalkExtensionRendererController::XWalkExtensionRendererController()
    : initialized_(false),
      extensions_client_(new XWalkExtensionClient()) {
}

XWalkExtensionRendererController::~XWalkExtensionRendererController() {
}

void XWalkExtensionRendererController::DidCreateScriptContext(
    v8::Handle<v8::Context> context) {
  SCOPE_PROFILE();
  XWalkModuleSystem* module_system = new XWalkModuleSystem(context);
  XWalkModuleSystem::SetModuleSystemInContext(
      std::unique_ptr<XWalkModuleSystem>(module_system), context);

  module_system->RegisterNativeModule(
        "v8tools",
        std::unique_ptr<XWalkNativeModule>(new XWalkV8ToolsModule));
  module_system->RegisterNativeModule(
        "WidgetModule",
        std::unique_ptr<XWalkNativeModule>(new WidgetModule));
  module_system->RegisterNativeModule(
        "objecttools",
        std::unique_ptr<XWalkNativeModule>(new ObjectToolsModule));

  CreateExtensionModules(extensions_client_.get(), module_system);
  module_system->Initialize();
}

void XWalkExtensionRendererController::WillReleaseScriptContext(
    v8::Handle<v8::Context> context) {
  v8::Context::Scope contextScope(context);
  XWalkModuleSystem::ResetModuleSystemFromContext(context);
}

void XWalkExtensionRendererController::InitializeExtensions() {
  if (initialized_) {
    LOGGER(DEBUG) << "already initialized";
    return;
  }
  extensions_client_->Initialize();
  initialized_ = true;
}

}  // namespace extensions
