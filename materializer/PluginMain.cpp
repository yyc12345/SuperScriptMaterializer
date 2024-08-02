#include "PluginMain.hpp"
#include "ExportCore.hpp"

namespace VSW::Materializer::PluginMain {
	
#ifdef MATERIALIZER_PLUGIN
	
	PluginInterface* g_Plugininterface = nullptr;
	PluginInfo g_PluginInfo;
	CMenu* g_MainMenu = nullptr;

	static void PluginMenuCallback(int command_id) {
		AFX_MANAGE_STATE(AfxGetStaticModuleState());

		switch (command_id) {
			case 0:
			{
				MessageBoxW(nullptr, L"Work in Progress", L"WIP", MB_ICONINFORMATION + MB_OK);
				break;
			}
			case 1:
			{
				MessageBoxW(nullptr, L"Work in Progress", L"WIP", MB_ICONINFORMATION + MB_OK);
				break;
			}
			case 2:
			{
				MessageBoxW(nullptr, L"Work in Progress", L"WIP", MB_ICONINFORMATION + MB_OK);
				break;
			}
			case 3:
			{
				ShellExecuteW(NULL, L"open", L"https://github.com/yyc12345/SuperScriptMaterializer/issues", NULL, NULL, SW_SHOWNORMAL);
				break;
			}
			case 4:
			{
				const wchar_t* body = 
					L"VSW Materializer v2.0.0 - Virtools Schematic Weaver Materializer.\n"
					L"The exporter of universal Virtools scripts analyser.\n"
					L"Under GPL v3 License.\n"
					L"Project Home Page: https://github.com/yyc12345/SuperScriptMaterializer";
				MessageBoxW(nullptr, body, L"About VSW Materializer", MB_ICONINFORMATION + MB_OK);
				break;
			}
		}

	}

	static void InitMenu() {
		if (!g_Plugininterface)
			return;

		g_MainMenu = g_Plugininterface->AddPluginMenu("VSW Materializer", 20, NULL, (VoidFunc1Param)PluginMenuCallback);
	}

	static void RemoveMenu() {
		if (!g_Plugininterface || !g_MainMenu)
			return;
		g_Plugininterface->RemovePluginMenu(g_MainMenu);
	}

	static void UpdateMenu() {
		g_Plugininterface->ClearPluginMenu(g_MainMenu);

		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 0, "Export Script");
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 1, "Export Document");
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 2, "Export Environment");
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, -1, NULL, TRUE);
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 3, "Report Bug");
		g_Plugininterface->AddPluginMenuItem(g_MainMenu, 4, "About VSW Materializer");


		g_Plugininterface->UpdatePluginMenu(g_MainMenu);
	}

	static void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface) {
		switch (reason) {
			case PluginInfo::CR_LOAD:
			{
				g_Plugininterface = plugininterface;
				InitMenu();
				UpdateMenu();
				break;
			}
			case PluginInfo::CR_UNLOAD:
			{
				RemoveMenu();
				g_Plugininterface = nullptr;
				break;
			}
			case PluginInfo::CR_NEWCOMPOSITIONNAME:
				break;
			case PluginInfo::CR_NOTIFICATION:
				break;
		}
	}

	void InitializePluginInfo() {
		strcpy(g_PluginInfo.m_Name, "VSW Materializer");
		g_PluginInfo.m_PluginType = PluginInfo::PT_EDITOR;
		g_PluginInfo.m_PluginType = (PluginInfo::PLUGIN_TYPE)(g_PluginInfo.m_PluginType | PluginInfo::PTF_RECEIVENOTIFICATION);
		g_PluginInfo.m_PluginCallback = PluginCallback;
	}

	PluginInfo* GetPluginInfo() {
		return &g_PluginInfo;
	}
	PluginInterface* GetPluginInterface() {
		return g_Plugininterface;
	}

#endif

}
