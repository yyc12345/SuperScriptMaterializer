#if !defined(_YYCDLL_HELP_EXPORT_H__IMPORTED_)
#define _YYCDLL_HELP_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

#define cp_guid(x,y) x[0]=y.d1;x[1]=y.d2

void IterateParameterOperation(CKParameterManager* parameterManager, envDatabase* db, dbEnvDataStructHelper* helper);
void IterateParameter(CKParameterManager* parameterManager, envDatabase* db, dbEnvDataStructHelper* helper);
void IterateMessage(CKMessageManager* msgManager, envDatabase* db, dbEnvDataStructHelper* helper);
void IterateAttribute(CKAttributeManager* attrManager, envDatabase* db, dbEnvDataStructHelper* helper);
void IteratePlugin(CKPluginManager* plgManager, envDatabase* db, dbEnvDataStructHelper* helper);

#endif