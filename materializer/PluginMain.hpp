#pragma once
#include "stdafx.hpp"

namespace VSW::Materializer::PluginMain {

#ifdef MATERIALIZER_PLUGIN

	class ConfigManager {
	public:
		static ConfigManager& GetSingleton();
	private:
		static YYCC::yycc_u8string GetConfigFilePath();
		ConfigManager();
		~ConfigManager();

	public:
		YYCC::ConfigManager::StringSetting m_LastFilePath;
		YYCC::ConfigManager::NumberSetting<UINT> m_Encoding;
		YYCC::ConfigManager::CoreManager m_Mgr;
	};

	void InitializePluginInfo();
	PluginInfo* GetPluginInfo();
	PluginInterface* GetPluginInterface();

#endif

}
