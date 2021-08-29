#include "stdafx.h"
#include "vt_menu.h"
#include "vt_player.h"

#if defined(VIRTOOLS_PLUGIN)
PluginInterface* s_Plugininterface = NULL;
PluginInfo g_PluginInfo0;

int GetVirtoolsPluginInfoCount() {
	return 1;
}

PluginInfo* GetVirtoolsPluginInfo(int index) {
	switch (index) {
		case 0:
			return &g_PluginInfo0;
	}
	return NULL;
}

class SuperScriptMaterializer : CWinApp {
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

SuperScriptMaterializer theApp;

BOOL SuperScriptMaterializer::InitInstance() {
	CWinApp::InitInstance();

	strcpy(g_PluginInfo0.m_Name, "Super Script Materializer");
	g_PluginInfo0.m_PluginType = PluginInfo::PT_EDITOR;
	g_PluginInfo0.m_PluginType = (PluginInfo::PLUGIN_TYPE)(g_PluginInfo0.m_PluginType | PluginInfo::PTF_RECEIVENOTIFICATION);
	g_PluginInfo0.m_PluginCallback = PluginCallback;

	return TRUE;
}

int SuperScriptMaterializer::ExitInstance() {
	return CWinApp::ExitInstance();
}

#elif defined(VIRTOOLS_STANDALONE)

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("Arguments too less!\n");
		printf("Format: SuperScriptMaterializer.exe [virtools composition] [script db path] [env db path]\n");
		ExitProcess(1);
	}

	PlayerMain(argv[1], argv[2], argv[3]);
	return 0;
}

#endif