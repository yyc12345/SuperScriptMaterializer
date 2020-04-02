#include "vt_menu.h"
#include "database.h"

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
			}
			free(file);

			ctx->OutputToConsole("[Super Script Materializer] Done");
		}
		break;
	}
}

void IterateScript(CKContext* ctx, database* db, dbDataStructHelper* helper) {
	CKBeObject* beobj = NULL;
	CKBehavior* beh = NULL;
	XObjectPointerArray objArray = ctx->GetObjectListByType(CKCID_BEOBJECT, TRUE);
	int len = objArray.Size();
	int scriptLen = 0;
	for (int i = 0; i < len; i++) {
		beobj = (CKBeObject*)objArray.GetObjectA(i);
		if ((scriptLen = beobj->GetScriptCount()) == 0) continue;
		for (int j = 0; j < scriptLen; j++) {
			//write script table
			beh = beobj->GetScript(j);

			helper->_dbCKScript->thisobj = beobj->GetID();
			strcpy(helper->_dbCKScript->host_name, beobj->GetName());
			helper->_dbCKScript->index = j;
			helper->_dbCKScript->behavior = beh->GetID();
			db->write_CKScript(helper->_dbCKScript);

			//iterate script
			IterateBehavior(beh, db, helper, -1);
		}
	}
}

void IterateBehavior(CKBehavior* bhv, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	//write self data
	helper->_dbCKBehavior->thisobj = bhv->GetID();
	strcpy(helper->_dbCKBehavior->name, bhv->GetName());
	helper->_dbCKBehavior->type = bhv->GetType();
	strcpy(helper->_dbCKBehavior->proto_name, bhv->GetPrototypeName() ? bhv->GetPrototypeName() : "");
	//todo: fix guid
	helper->_dbCKBehavior->proto_guid = bhv->GetPrototypeGuid().d1;
	helper->_dbCKBehavior->flags = bhv->GetFlags();
	helper->_dbCKBehavior->priority = bhv->GetPriority();
	helper->_dbCKBehavior->version = bhv->GetVersion();
	helper->_dbCKBehavior->parent = parents;
	db->write_CKBehavior(helper->_dbCKBehavior);

	//write target
	if (bhv->IsUsingTarget()) {
		CKParameterIn* cache = bhv->GetTargetParameter();

		helper->_db_pTarget->thisobj = cache->GetID();
		strcpy(helper->_db_pTarget->name, cache->GetName());
		strcpy(helper->_db_pTarget->type, helper->_parameterManager->ParameterTypeToName(cache->GetType()));
		//todo: fix guid
		helper->_db_pTarget->type_guid = cache->GetGUID().d1;
		helper->_db_pTarget->belong_to = bhv->GetID();
		helper->_db_pTarget->direct_source = cache->GetDirectSource() ? cache->GetDirectSource()->GetID() : -1;
		helper->_db_pTarget->shared_source = cache->GetSharedSource() ? cache->GetSharedSource()->GetID() : -1;

		db->write_pTarget(helper->_db_pTarget);
	}

	int count = 0, i = 0;
	CKParameterIn* cache_pIn = NULL;
	CKParameterOut* cache_pOut = NULL;
	CKBehaviorIO* cache_bIO = NULL;
	CKBehaviorLink* cache_bLink = NULL;
	//pIn
	for (i = 0, count = bhv->GetInputParameterCount(); i < count; i++) {
		cache_pIn = bhv->GetInputParameter(i);
		helper->_db_pIn->thisobj = cache_pIn->GetID();
		helper->_db_pIn->index = i;
		strcpy(helper->_db_pIn->name, cache_pIn->GetName());
		strcpy(helper->_db_pIn->type, helper->_parameterManager->ParameterTypeToName(cache_pIn->GetType()));
		//todo: fix guid
		helper->_db_pIn->type_guid = cache_pIn->GetGUID().d1;
		helper->_db_pIn->belong_to = bhv->GetID();
		helper->_db_pIn->direct_source = cache_pIn->GetDirectSource() ? cache_pIn->GetDirectSource()->GetID() : -1;
		helper->_db_pIn->shared_source = cache_pIn->GetSharedSource() ? cache_pIn->GetSharedSource()->GetID() : -1;
	
		db->write_pIn(helper->_db_pIn);
	}

	//pOut
	for (i = 0, count = bhv->GetOutputParameterCount(); i < count; i++) {
		cache_pOut = bhv->GetOutputParameter(i);
		helper->_db_pOut->thisobj = cache_pOut->GetID();
		helper->_db_pOut->index = i;
		strcpy(helper->_db_pOut->name, cache_pOut->GetName());
		strcpy(helper->_db_pOut->type, helper->_parameterManager->ParameterTypeToName(cache_pOut->GetType()));
		//todo: fix guid
		helper->_db_pOut->type_guid = cache_pOut->GetGUID().d1;
		helper->_db_pOut->belong_to = bhv->GetID();

		db->write_pOut(helper->_db_pOut);
	}

	//bIn
	for (i = 0, count = bhv->GetInputCount(); i < count; i++) {
		cache_bIO = bhv->GetInput(i);
		helper->_db_bIn->thisobj = cache_bIO->GetID();
		helper->_db_bIn->index = i;
		strcpy(helper->_db_bIn->name, cache_bIO->GetName());
		helper->_db_bIn->belong_to = bhv->GetID();

		db->write_bIn(helper->_db_bIn);
	}

	//bOut
	for (i = 0, count = bhv->GetOutputCount(); i < count; i++) {
		cache_bIO = bhv->GetOutput(i);
		helper->_db_bOut->thisobj = cache_bIO->GetID();
		helper->_db_bOut->index = i;
		strcpy(helper->_db_bOut->name, cache_bIO->GetName());
		helper->_db_bOut->belong_to = bhv->GetID();

		db->write_bOut(helper->_db_bOut);
	}

	//bLink
	for (i = 0, count = bhv->GetSubBehaviorLinkCount(); i < count; i++) {
		cache_bLink = bhv->GetSubBehaviorLink(i);
		helper->_db_bLlink->input = cache_bLink->GetInBehaviorIO()->GetID();
		helper->_db_bLlink->output = cache_bLink->GetOutBehaviorIO()->GetID();
		helper->_db_bLlink->delay = cache_bLink->GetActivationDelay();
		helper->_db_bLlink->belong_to = bhv->GetID();

		db->write_bLink(helper->_db_bLlink);
	}


	//iterate sub bb
	for (i = 0, count = bhv->GetSubBehaviorCount(); i < count; i++)
		IterateBehavior(bhv->GetSubBehavior(i), db, helper, bhv->GetID());

}