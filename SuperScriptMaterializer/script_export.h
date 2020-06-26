#if !defined(_YYCDLL_SCRIPT_EXPORT_H__IMPORTED_)
#define _YYCDLL_SCRIPT_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

void generate_pLink_in_pIn(CKContext* ctx, CKParameterIn* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB, BOOL isTarget);
void proc_pTarget(CKContext* ctx, CKParameterIn* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents);
void proc_pIn(CKContext* ctx, CKParameterIn* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
void proc_pOut(CKContext* ctx, CKParameterOut* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
void proc_bIn(CKBehaviorIO* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, int index);
void proc_bOut(CKBehaviorIO* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, int index);
void proc_bLink(CKBehaviorLink* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void proc_pLocal(CKParameterLocal* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, BOOL is_setting);
void proc_pOper(CKContext* ctx, CKParameterOperation* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void proc_pAttr(CKContext* ctx, scriptDatabase* db, dbScriptDataStructHelper* helper, CKParameter* cache);

void helper_pDataExport(const char* field, long data, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void helper_pDataExport(const char* field, float data, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void helper_pDataExport(const char* field, const char* data, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);

void IterateBehavior(CKContext* ctx, CKBehavior* bhv, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void IterateScript(CKContext* ctx, scriptDatabase* db, dbScriptDataStructHelper* helper);
void DigParameterData(CKParameterLocal* pl, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);

#endif