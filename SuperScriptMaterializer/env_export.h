#if !defined(_YYCDLL_ENV_EXPORT_H__IMPORTED_)
#define _YYCDLL_ENV_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

void IterateParameterOperation(CKParameterManager* parameterManager, EnvironmentDatabase* mDb, dbEnvDataStructHelper* helper);
void IterateParameter(CKParameterManager* parameterManager, EnvironmentDatabase* mDb, dbEnvDataStructHelper* helper);
void IterateMessage(CKMessageManager* msgManager, EnvironmentDatabase* mDb, dbEnvDataStructHelper* helper);
void IterateAttribute(CKAttributeManager* attrManager, EnvironmentDatabase* mDb, dbEnvDataStructHelper* helper);
void IteratePlugin(CKPluginManager* plgManager, EnvironmentDatabase* mDb, dbEnvDataStructHelper* helper);
void IterateVariable(CKVariableManager* varManager, EnvironmentDatabase* mDb, dbEnvDataStructHelper* helper);

#endif