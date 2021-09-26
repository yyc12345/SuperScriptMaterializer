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
		helper->_db_op->op_code = i;
		_guid = parameterManager->OperationCodeToGuid(i);
		copyGuid(_guid,helper->_db_op->op_guid);
		helper->_db_op->op_name = parameterManager->OperationCodeToName(i);

		//allocate mem
		cacheListCount = parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, NULL);
		if (cacheListCount > listCount) {
			listCount = cacheListCount;
			opList = (CKOperationDesc*)realloc(opList, listCount * sizeof(CKOperationDesc));
			assert(opList != NULL);
		}

		parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, opList);
		for (int j = 0; j < cacheListCount; j++) {
			copyGuid(opList[j].P1Guid, helper->_db_op->in1_guid);
			copyGuid(opList[j].P2Guid, helper->_db_op->in2_guid);
			copyGuid(opList[j].ResGuid, helper->_db_op->out_guid);
			helper->_db_op->funcPtr = opList[j].Fct;

			db->write_op(helper->_db_op);
		}
	}
	if (opList != NULL) free(opList);

}

void IterateParameter(CKParameterManager* parameterManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = parameterManager->GetParameterTypesCount();
	CKParameterTypeDesc* desc = NULL;
	for (int i = 0; i < count; i++) {
		desc = parameterManager->GetParameterTypeDescription(i);

		helper->_db_param->index = desc->Index;
		copyGuid(desc->Guid, helper->_db_param->guid);
		copyGuid(desc->DerivedFrom, helper->_db_param->derived_from);
		helper->_db_param->type_name = desc->TypeName.CStr();
		helper->_db_param->default_size = desc->DefaultSize;
		helper->_db_param->func_CreateDefault = desc->CreateDefaultFunction;
		helper->_db_param->func_Delete = desc->DeleteFunction;
		helper->_db_param->func_SaveLoad = desc->SaveLoadFunction;
		helper->_db_param->func_Check = desc->CheckFunction;
		helper->_db_param->func_Copy = desc->CopyFunction;
		helper->_db_param->func_String = desc->StringFunction;
		helper->_db_param->func_UICreator = desc->UICreatorFunction;
		CKPluginEntry* plgEntry = desc->CreatorDll;
		if (plgEntry != NULL) {
			helper->_db_param->creator_dll_index = plgEntry->m_PluginDllIndex;
			helper->_db_param->creator_plugin_index = plgEntry->m_PositionInDll;
		} else {
			helper->_db_param->creator_dll_index =-1;
			helper->_db_param->creator_plugin_index =-1;
		}
		helper->_db_param->dw_param = desc->dwParam;
		helper->_db_param->dw_flags = desc->dwFlags;
		helper->_db_param->cid = desc->Cid;
		copyGuid(desc->Saver_Manager, helper->_db_param->saver_manager);

		db->write_param(helper->_db_param);
	}
}

void IterateMessage(CKMessageManager* msgManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = msgManager->GetMessageTypeCount();
	for (int i = 0; i < count; i++) {
		helper->_db_envMsg->index = i;
		helper->_db_envMsg->name = msgManager->GetMessageTypeName(i);

		db->write_msg(helper->_db_envMsg);
	}
}

void IterateAttribute(CKAttributeManager* attrManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = attrManager->GetAttributeCount();
	for (int i = 0; i < count; i++) {
		helper->_db_attr->index = i;
		helper->_db_attr->name = attrManager->GetAttributeNameByType(i);
		helper->_db_attr->category_index = attrManager->GetAttributeCategoryIndex(i);
		helper->_db_attr->category_name = attrManager->GetAttributeCategory(i) != NULL ? attrManager->GetAttributeCategory(i) : "";
		helper->_db_attr->flags = attrManager->GetAttributeFlags(i);
		helper->_db_attr->param_index = attrManager->GetAttributeParameterType(i);
		helper->_db_attr->compatible_classid = attrManager->GetAttributeCompatibleClassId(i);
		helper->_db_attr->default_value = attrManager->GetAttributeDefaultValue(i) != NULL ? attrManager->GetAttributeDefaultValue(i) : "";

		db->write_attr(helper->_db_attr);
	}
}

void IteratePlugin(CKPluginManager* plgManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	for (int i = 0; i <= 7; i++) {
		int catCount = plgManager->GetPluginCount(i);
		helper->_db_plugin->category = plgManager->GetCategoryName(i);
		for (int j = 0; j < catCount; j++) {
			CKPluginEntry* plgEntry = plgManager->GetPluginInfo(i, j);

			helper->_db_plugin->dll_index = plgEntry->m_PluginDllIndex;
			helper->_db_plugin->dll_name = plgManager->GetPluginDllInfo(plgEntry->m_PluginDllIndex)->m_DllFileName.CStr();
			helper->_db_plugin->plugin_index = plgEntry->m_PositionInDll;
			helper->_db_plugin->active = plgEntry->m_Active;
			helper->_db_plugin->needed_by_file = plgEntry->m_NeededByFile;
			CKPluginInfo* plgInfo = &(plgEntry->m_PluginInfo);
			copyGuid(plgInfo->m_GUID, helper->_db_plugin->guid);
			helper->_db_plugin->desc = plgInfo->m_Description.CStr();
			helper->_db_plugin->author = plgInfo->m_Author.CStr();
			helper->_db_plugin->summary = plgInfo->m_Summary.CStr();
			helper->_db_plugin->version = plgInfo->m_Version;
			helper->_db_plugin->func_init = plgInfo->m_InitInstanceFct;
			helper->_db_plugin->func_exit = plgInfo->m_ExitInstanceFct;

			db->write_plugin(helper->_db_plugin);
		}
	}
}

#if !defined(VIRTOOLS_21)
void IterateVariable(CKVariableManager* varManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	CKVariableManager::Iterator it = varManager->GetVariableIterator();
	CKVariableManager::Variable* varobj = NULL;
	XString dataCopyCache;
	for (; !it.End(); it++) {
		varobj = it.GetVariable();
		helper->_db_variable->name = it.GetName();
		safeStringCopy(helper->_db_variable->desciption, varobj->GetDescription());
		helper->_db_variable->flags = varobj->GetFlags();
		helper->_db_variable->type = varobj->GetType();
		safeStringCopy(helper->_db_variable->representation, varobj->GetRepresentation());
		varobj->GetStringValue(dataCopyCache);
		helper->_db_variable->data = dataCopyCache.CStr();

		db->write_variable(helper->_db_variable);
	}
}
#endif