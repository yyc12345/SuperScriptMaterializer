#pragma once

#if defined(VIRTOOLS_PLUGIN)
#include "stdafx.h"
#include "database.hpp"

void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface);

void InitMenu();
void RemoveMenu();
void UpdateMenu();
void PluginMenuCallback(int commandID);

BOOL OpenFileDialog(std::string* returned_file);
#endif
