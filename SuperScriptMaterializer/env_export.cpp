#include "env_export.h"
//disable shit tip
#pragma warning(disable:26812)

#define copyGuid(guid,str) sprintf(helper->_stringCache,"%d,%d",guid.d1,guid.d2);str=helper->_stringCache;
#define safeStringCopy(storage,str) storage=(str)?(str):"";

void IterateParameterOperation(CKParameterManager* parameterManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = parameterManager->GetParameterOperationCount();
	CKOperationDesc* opList = NULL;
	int listCount = 0, cacheListCount = 0;
	CKGUID _guid;
	for (int i = 0; i < count; i++) {
		//fill basic data
		helper->_db_envOp->op_code = i;
		_guid = parameterManager->OperationCodeToGuid(i);
		copyGuid(_guid,helper->_db_envOp->op_guid);
		helper->_db_envOp->op_name = parameterManager->OperationCodeToName(i);

		//allocate mem
		cacheListCount = parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, NULL);
		if (cacheListCount > listCount) {
			listCount = cacheListCount;
			opList = (CKOperationDesc*)realloc(opList, listCount * sizeof(CKOperationDesc));
			assert(opList != NULL);
		}

		parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, opList);
		for (int j = 0; j < cacheListCount; j++) {
			copyGuid(opList[j].P1Guid, helper->_db_envOp->in1_guid);
			copyGuid(opList[j].P2Guid, helper->_db_envOp->in2_guid);
			copyGuid(opList[j].ResGuid, helper->_db_envOp->out_guid);
			helper->_db_envOp->funcPtr = opList[j].Fct;

			db->write_envOp(helper->_db_envOp);
		}
	}
	if (opList != NULL) free(opList);

}

void IterateParameter(CKParameterManager* parameterManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = parameterManager->GetParameterTypesCount();
	CKParameterTypeDesc* desc = NULL;
	for (int i = 0; i < count; i++) {
		desc = parameterManager->GetParameterTypeDescription(i);

		helper->_db_envParam->index = desc->Index;
		copyGuid(desc->Guid, helper->_db_envParam->guid);
		copyGuid(desc->DerivedFrom, helper->_db_envParam->derived_from);
		helper->_db_envParam->type_name = desc->TypeName.CStr();
		helper->_db_envParam->default_size = desc->DefaultSize;
		helper->_db_envParam->func_CreateDefault = desc->CreateDefaultFunction;
		helper->_db_envParam->func_Delete = desc->DeleteFunction;
		helper->_db_envParam->func_SaveLoad = desc->SaveLoadFunction;
		helper->_db_envParam->func_Check = desc->CheckFunction;
		helper->_db_envParam->func_Copy = desc->CopyFunction;
		helper->_db_envParam->func_String = desc->StringFunction;
		helper->_db_envParam->func_UICreator = desc->UICreatorFunction;
		CKPluginEntry* plgEntry = desc->CreatorDll;
		if (plgEntry != NULL) {
			helper->_db_envParam->creator_dll_index = plgEntry->m_PluginDllIndex;
			helper->_db_envParam->creator_plugin_index = plgEntry->m_PositionInDll;
		} else {
			helper->_db_envParam->creator_dll_index =-1;
			helper->_db_envParam->creator_plugin_index =-1;
		}
		helper->_db_envParam->dw_param = desc->dwParam;
		helper->_db_envParam->dw_flags = desc->dwFlags;
		helper->_db_envParam->cid = desc->Cid;
		copyGuid(desc->Saver_Manager, helper->_db_envParam->saver_manager);

		db->write_envParam(helper->_db_envParam);
	}
}

void IterateMessage(CKMessageManager* msgManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = msgManager->GetMessageTypeCount();
	for (int i = 0; i < count; i++) {
		helper->_db_envMsg->index = i;
		helper->_db_envMsg->name = msgManager->GetMessageTypeName(i);

		db->write_envMsg(helper->_db_envMsg);
	}
}

void IterateAttribute(CKAttributeManager* attrManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = attrManager->GetAttributeCount();
	for (int i = 0; i < count; i++) {
		helper->_db_envAttr->index = i;
		helper->_db_envAttr->name = attrManager->GetAttributeNameByType(i);
		helper->_db_envAttr->category_index = attrManager->GetAttributeCategoryIndex(i);
		helper->_db_envAttr->category_name = attrManager->GetAttributeCategory(i) != NULL ? attrManager->GetAttributeCategory(i) : "";
		helper->_db_envAttr->flags = attrManager->GetAttributeFlags(i);
		helper->_db_envAttr->param_index = attrManager->GetAttributeParameterType(i);
		helper->_db_envAttr->compatible_classid = attrManager->GetAttributeCompatibleClassId(i);
		helper->_db_envAttr->default_value = attrManager->GetAttributeDefaultValue(i) != NULL ? attrManager->GetAttributeDefaultValue(i) : "";

		db->write_envAttr(helper->_db_envAttr);
	}
}

void IteratePlugin(CKPluginManager* plgManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	for (int i = 0; i <= 7; i++) {
		int catCount = plgManager->GetPluginCount(i);
		helper->_db_envPlugin->category = plgManager->GetCategoryName(i);
		for (int j = 0; j < catCount; j++) {
			CKPluginEntry* plgEntry = plgManager->GetPluginInfo(i, j);

			helper->_db_envPlugin->dll_index = plgEntry->m_PluginDllIndex;
			helper->_db_envPlugin->dll_name = plgManager->GetPluginDllInfo(plgEntry->m_PluginDllIndex)->m_DllFileName.CStr();
			helper->_db_envPlugin->plugin_index = plgEntry->m_PositionInDll;
			helper->_db_envPlugin->active = plgEntry->m_Active;
			helper->_db_envPlugin->needed_by_file = plgEntry->m_NeededByFile;
			CKPluginInfo* plgInfo = &(plgEntry->m_PluginInfo);
			copyGuid(plgInfo->m_GUID, helper->_db_envPlugin->guid);
			helper->_db_envPlugin->desc = plgInfo->m_Description.CStr();
			helper->_db_envPlugin->author = plgInfo->m_Author.CStr();
			helper->_db_envPlugin->summary = plgInfo->m_Summary.CStr();
			helper->_db_envPlugin->version = plgInfo->m_Version;
			helper->_db_envPlugin->func_init = plgInfo->m_InitInstanceFct;
			helper->_db_envPlugin->func_exit = plgInfo->m_ExitInstanceFct;

			db->write_envPlugin(helper->_db_envPlugin);
		}
	}
}

void IterateVariable(CKVariableManager* varManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	CKVariableManager::Iterator it = varManager->GetVariableIterator();
	CKVariableManager::Variable* varobj = NULL;
	XString dataCopyCache;
	for (; !it.End(); it++) {
		varobj = it.GetVariable();
		helper->_db_envVariable->name = it.GetName();
		safeStringCopy(helper->_db_envVariable->desciption, varobj->GetDescription());
		helper->_db_envVariable->flags = varobj->GetFlags();
		helper->_db_envVariable->type = varobj->GetType();
		safeStringCopy(helper->_db_envVariable->representation, varobj->GetRepresentation());
		varobj->GetStringValue(dataCopyCache);
		helper->_db_envVariable->data = dataCopyCache.CStr();

		db->write_envVariable(helper->_db_envVariable);
	}
}