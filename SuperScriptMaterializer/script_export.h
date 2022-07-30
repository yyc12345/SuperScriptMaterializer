#if !defined(_YYCDLL_SCRIPT_EXPORT_H__IMPORTED_)
#define _YYCDLL_SCRIPT_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

void generate_pLink_in_pIn(CKContext* ctx, CKParameterIn* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB, BOOL isTarget);
void proc_pTarget(CKContext* ctx, CKParameterIn* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents);
void proc_pIn(CKContext* ctx, CKParameterIn* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
void proc_pOut(CKContext* ctx, CKParameterOut* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
void proc_bIn(CKBehaviorIO* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, int index);
void proc_bOut(CKBehaviorIO* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, int index);
void proc_bLink(CKBehaviorLink* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);
void proc_pLocal(CKParameterLocal* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents, BOOL is_setting);
void proc_pOper(CKContext* ctx, CKParameterOperation* cache, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);
void proc_pAttr(CKContext* ctx, DocumentDatabase* mDb, dbDocDataStructHelper* helper, CKParameter* cache);

void helper_pDataExport(const char* field, long data, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);
void helper_pDataExport(const char* field, float data, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);
void helper_pDataExport(const char* field, const char* data, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);

void IterateBehavior(CKContext* ctx, CKBehavior* bhv, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);
void IterateScript(CKContext* ctx, DocumentDatabase* mDb, dbDocDataStructHelper* helper);
void DigParameterData(CKParameterLocal* pl, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents);

#endif