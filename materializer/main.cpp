#include "stdafx.hpp"
#include "PluginMain.hpp"
#include "StandaloneMain.hpp"

#if defined(MATERIALIZER_PLUGIN)

int GetVirtoolsPluginInfoCount() {
	return 1;
}

PluginInfo* GetVirtoolsPluginInfo(int index) {
	switch (index) {
		case 0:
			return VSW::Materializer::PluginMain::GetPluginInfo();
		default:
			return nullptr;
	}
}

class CMaterializer : CWinApp {
public:
	virtual BOOL InitInstance() override {
		// register unhandler exception handler
#ifdef MATERIALIZER_RELEASE
		YYCC::ExceptionHelper::Register(vtobjplugin::VirtoolsMenu::UnhandledExceptionCallback);
#endif

		// init plugin info
		VSW::Materializer::PluginMain::InitializePluginInfo();

		return CWinApp::InitInstance();
	}
	virtual int ExitInstance() override {

		// unregister unhandler exception handler
#ifdef MATERIALIZER_RELEASE
		YYCC::ExceptionHelper::Unregister();
#endif

		return CWinApp::ExitInstance();
	}
};
CMaterializer theApp;

#else

int main(int argc, char* argv[]) {
	VSW::Materializer::StandaloneMain::PlayerMain(argc, argv);
	return 0;
}

#endif
