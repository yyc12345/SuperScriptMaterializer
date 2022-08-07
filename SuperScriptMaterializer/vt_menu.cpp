#include "vt_menu.hpp"
#include "database.hpp"
#include "doc_export.hpp"
#include "env_export.hpp"

#if defined(VIRTOOLS_PLUGIN)

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

	s_MainMenu = s_Plugininterface->AddPluginMenu("SSMaterializer", 20, NULL, (VoidFunc1Param)PluginMenuCallback);
}

void RemoveMenu() {
	if (!s_Plugininterface || !s_MainMenu)
		return;

	s_Plugininterface->RemovePluginMenu(s_MainMenu);
}

void UpdateMenu() {
	s_Plugininterface->ClearPluginMenu(s_MainMenu);		//clear menu

	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 0, "Export document");
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 1, "Export environment");

	s_Plugininterface->AddPluginMenuItem(s_MainMenu, -1, NULL, TRUE);
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 2, "Report bug");
	s_Plugininterface->AddPluginMenuItem(s_MainMenu, 3, "Plugin homepage");

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
				//Init file
				std::string file;
				OpenFileDialog(&file);
				if (file.empty())
					break;
				DeleteFile(file.c_str());

				//Init resources
				SSMaterializer::Database::DocumentDatabase* db = 
					new SSMaterializer::Database::DocumentDatabase(file.c_str(), ctx->GetParameterManager());

				//iterate item
				SSMaterializer::DocumentExporter::IterateScript(ctx, db);
				SSMaterializer::DocumentExporter::IterateMessage(ctx, db);
				SSMaterializer::DocumentExporter::IterateArray(ctx, db);

				//Close all resources
				delete db;

				ctx->OutputToConsole("[SSMaterializer] Done");
			}
			break;
			case 1:
			{
				//Init file
				std::string file;
				OpenFileDialog(&file);
				if (file.empty())
					break;
				DeleteFile(file.c_str());

				//Init
				SSMaterializer::Database::EnvironmentDatabase* db =
					new SSMaterializer::Database::EnvironmentDatabase(file.c_str());

				//iterate parameter operation/param
				SSMaterializer::EnvironmentExporter::IterateParameterOperation(ctx->GetParameterManager(), db);
				SSMaterializer::EnvironmentExporter::IterateParameter(ctx->GetParameterManager(), db);
				SSMaterializer::EnvironmentExporter::IterateAttribute(ctx->GetAttributeManager(), db);
				SSMaterializer::EnvironmentExporter::IteratePlugin(CKGetPluginManager(), db);
#if !defined(VIRTOOLS_21)
				SSMaterializer::EnvironmentExporter::IterateVariable(ctx->GetVariableManager(), db);
#endif

				//release all
				delete db;

				ctx->OutputToConsole("[SSMaterializer] Done");
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

#endif