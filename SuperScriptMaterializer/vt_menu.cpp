#include "vt_menu.h"
#include "database.h"
#include "script_export.h"
#include "env_export.h"

extern PluginInterface* s_Plugininterface;
CMenu* s_MainMenu = NULL;

void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface) {
	switch (reason) {
		case PluginInfo::CR_LOAD:
		{
			s_Plugininterface = plugininterface;
			InitMenu();
			UpdateMenu();
		}break;
		case PluginInfo::CR_UNLOAD:
		{
			RemoveMenu();
			s_Plugininterface = NULL;
		}break;
		case PluginInfo::CR_NEWCOMPOSITIONNAME:
		{
		}break;
		case PluginInfo::CR_NOTIFICATION:
		{
		}break;
	}
}

void InitMenu() {
	if (!s_Plugininterface)
		return;

	s_MainMenu = s_Plugininterface->AddPluginMenu("Super Script Materializer", 20, NULL, (VoidFunc1Param)PluginMenuCallback);
}

void RemoveMenu() {
	if (!s_Plugininterface || !s_MainMenu)
		return;

	s_Plugininterface->RemovePluginMenu(s_MainMenu);
}

void UpdateMenu() {
	s_Plugininterface->ClearPluginMenu(s_MainMenu);		//clear menu

	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 0, "Export all scripts");
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 1, "Export environment");

	s_Plugininterface->AddPluginMenuItem(s_MainMenu, -1, NULL, TRUE);
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 2, "Report bug");
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 3, "Plugin homepage");
	//===========================freeze chirs241097 code for future expand
	//s_Plugininterface->AddPluginMenuItem(s_MainMenu, -1, NULL, TRUE);

	////note : sub menu must have independent command ids that must be >=0
	//CMenu* sub0 = s_Plugininterface->AddPluginMenuItem(s_MainMenu, 0, "Fsck /dev/sdb", FALSE, TRUE);//bb manipulation

	//s_Plugininterface->AddPluginMenuItem(sub0, 1, "Fsck /dev/sdb1");//modify bb proto
	//s_Plugininterface->AddPluginMenuItem(sub0, 2, "Fsck /dev/sdb2");//not implemented

	//s_Plugininterface->AddPluginMenuItem(s_MainMenu, -1, NULL, TRUE);
	//s_Plugininterface->AddPluginMenuItem(s_MainMenu, 10, "Exit Fsck");

	s_Plugininterface->UpdatePluginMenu(s_MainMenu);	//update menu,always needed when you finished to update the menu
														//unless you want the menu not to have Virtools Dev main menu color scheme.
}

void PluginMenuCallback(int commandID) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CKContext* ctx = s_Plugininterface->GetCKContext();

	//switch mode
#if defined(_RELEASE)
	try {
#endif
		switch (commandID) {
			case 0:
			{
				//init file
				std::string file;
				OpenFileDialog(&file);
				if (file.empty())
					break;
				DeleteFile(file.c_str());

				//init resources
				scriptDatabase* _db = new scriptDatabase();
				dbScriptDataStructHelper* _helper = new dbScriptDataStructHelper();
				_db->open(file.c_str());
				_helper->init(ctx->GetParameterManager());

				//iterate item
				IterateScript(ctx, _db, _helper);

				//close all resources
				_helper->dispose();
				_db->close();
				delete _helper;
				delete _db;

				ctx->OutputToConsole("[Super Script Materializer] Done");
			}
			break;
			case 1:
			{
				//init file
				std::string file;
				OpenFileDialog(&file);
				if (file.empty())
					break;
				DeleteFile(file.c_str());

				//init
				envDatabase* _db = new envDatabase();
				dbEnvDataStructHelper* _helper = new dbEnvDataStructHelper();
				_db->open(file.c_str());
				_helper->init();

				//iterate parameter operation/param
				IterateParameterOperation(ctx->GetParameterManager(), _db, _helper);
				IterateParameter(ctx->GetParameterManager(), _db, _helper);
				IterateMessage(ctx->GetMessageManager(), _db, _helper);
				IterateAttribute(ctx->GetAttributeManager(), _db, _helper);
				IteratePlugin(CKGetPluginManager(), _db, _helper);
				IterateVariable(ctx->GetVariableManager(), _db, _helper);

				//release all
				_helper->dispose();
				_db->close();
				delete _helper;
				delete _db;

				ctx->OutputToConsole("[Super Script Materializer] Done");
			}
			break;
			case 2:
				ShellExecute(NULL, "open", "https://github.com/yyc12345/SuperScriptMaterializer/issues", NULL, NULL, SW_SHOWNORMAL);
				break;
			case 3:
				ShellExecute(NULL, "open", "https://github.com/yyc12345/SuperScriptMaterializer", NULL, NULL, SW_SHOWNORMAL);
				break;
		}
#if defined(_RELEASE)
	} catch (const std::exception & e) {
		std::string errstr;
		errstr = "An error occurs, application will exit. Please report to developer with this window and reproduce step.\nError message: ";
		errstr += e.what();
		AfxMessageBox(errstr.c_str(), MB_OK | MB_ICONSTOP);
		exit(1);
	}
#endif
}


BOOL OpenFileDialog(std::string* returned_file) {
	returned_file->clear();

	char* file = (char*)malloc(1024 * sizeof(char));
	BOOL status;
	OPENFILENAME OpenFileStruct;
	ZeroMemory(&OpenFileStruct, sizeof(OPENFILENAME));
	OpenFileStruct.lStructSize = sizeof(OPENFILENAME);
	OpenFileStruct.lpstrFile = file;
	OpenFileStruct.lpstrFile[0] = '\0';
	OpenFileStruct.nMaxFile = 1024;
	OpenFileStruct.lpstrFilter = "Database file(*.db)\0*.db\0";
	OpenFileStruct.lpstrDefExt = "db";
	OpenFileStruct.lpstrFileTitle = NULL;
	OpenFileStruct.nMaxFileTitle = 0;
	OpenFileStruct.lpstrInitialDir = NULL;
	OpenFileStruct.Flags = OFN_EXPLORER;
	if (status = GetSaveFileName(&OpenFileStruct))
		*returned_file = file;

	free(file);
	return status;
}
