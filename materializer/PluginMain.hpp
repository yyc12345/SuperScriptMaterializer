#pragma once
#include "stdafx.hpp"

namespace VSW::Materializer::PluginMain {

#ifdef MATERIALIZER_PLUGIN
	void InitializePluginInfo();
	PluginInfo* GetPluginInfo();
	PluginInterface* GetPluginInterface();
#endif

}
