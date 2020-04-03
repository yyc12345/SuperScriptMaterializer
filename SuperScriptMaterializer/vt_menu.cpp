#include "vt_menu.h"
#include "database.h"
#include "script_export.h"

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
	switch (commandID) {
		case 0:
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());
			CKContext* ctx = s_Plugininterface->GetCKContext();

			OPENFILENAME ofn;
			char* file = (char*)malloc(1024 * sizeof(char));
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.lpstrFile = file;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = 1024;
			ofn.lpstrFilter = "Database file(*.db)\0*.db\0All files(*.*)\0*.*";
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_EXPLORER;

			if (GetSaveFileName(&ofn)) {
				//make sure file is not exist
				DeleteFile(file);

				//init resources
				database* _db = new database();
				dbDataStructHelper* _helper = new dbDataStructHelper();
				_db->open(file);
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
			free(file);
		}
		break;
	}
}
