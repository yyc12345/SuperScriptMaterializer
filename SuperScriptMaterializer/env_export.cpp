#include "env_export.h"
//disable shit tip
#pragma warning(disable:26812)

void IterateParameterOperation(CKParameterManager* parameterManager, envDatabase* db, dbEnvDataStructHelper* helper) {
	int count = parameterManager->GetParameterOperationCount();
	CKOperationDesc* opList = NULL;
	int listCount = 0, cacheListCount = 0;
	CKGUID _guid;
	for (int i = 0; i < count; i++) {
		//fill basic data
		helper->_db_envOp->op_code = i;
		_guid = parameterManager->OperationCodeToGuid(i);
		cp_guid(helper->_db_envOp->op_guid, _guid);
		strcpy(helper->_db_envOp->op_name, parameterManager->OperationCodeToName(i));

		//allocate mem
		cacheListCount = parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, NULL);
		if (cacheListCount > listCount) {
			listCount = cacheListCount;
			opList = (CKOperationDesc*)realloc(opList, listCount * sizeof(CKOperationDesc));
			assert(opList != NULL);
		}

		parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, opList);
		for (int j = 0; j < cacheListCount; j++) {
			cp_guid(helper->_db_envOp->in1_guid, opList[j].P1Guid);
			cp_guid(helper->_db_envOp->in2_guid, opList[j].P2Guid);
			cp_guid(helper->_db_envOp->out_guid, opList[j].ResGuid);
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
		cp_guid(helper->_db_envParam->guid, desc->Guid);
		cp_guid(helper->_db_envParam->derived_from, desc->DerivedFrom);
		strcpy(helper->_db_envParam->type_name, desc->TypeName.CStr());
		helper->_db_envParam->default_size = desc->DefaultSize;
		helper->_db_envParam->func_CreateDefault = desc->CreateDefaultFunction;
		helper->_db_envParam->func_Delete = desc->DeleteFunction;
		helper->_db_envParam->func_SaveLoad = desc->SaveLoadFunction;
		helper->_db_envParam->func_Check = desc->CheckFunction;
		helper->_db_envParam->func_Copy = desc->CopyFunction;
		helper->_db_envParam->func_String = desc->StringFunction;
		helper->_db_envParam->func_UICreator = desc->UICreatorFunction;
		helper->_db_envParam->creator_plugin_id = desc->CreatorDll != NULL ? desc->CreatorDll->m_PluginDllIndex : -1;
		helper->_db_envParam->dw_param = desc->dwParam;
		helper->_db_envParam->dw_flags = desc->dwFlags;
		helper->_db_envParam->cid = desc->Cid;
		cp_guid(helper->_db_envParam->saver_manager, desc->Saver_Manager);

		db->write_envParam(helper->_db_envParam);
	}
}