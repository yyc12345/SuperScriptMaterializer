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
		helper->_db_envOp->op_guid[0] = _guid.d1;
		helper->_db_envOp->op_guid[1] = _guid.d2;
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
			helper->_db_envOp->in1_guid[0] = opList[j].P1Guid.d1;
			helper->_db_envOp->in1_guid[1] = opList[j].P1Guid.d2;
			helper->_db_envOp->in2_guid[0] = opList[j].P2Guid.d1;
			helper->_db_envOp->in2_guid[1] = opList[j].P2Guid.d2;
			helper->_db_envOp->out_guid[0] = opList[j].ResGuid.d1;
			helper->_db_envOp->out_guid[1] = opList[j].ResGuid.d2;
			helper->_db_envOp->funcPtr = opList[j].Fct;

			db->write_envOp(helper->_db_envOp);
		}
	}
	if (opList != NULL) free(opList);

}