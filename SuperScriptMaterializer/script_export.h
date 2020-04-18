#if !defined(_YYCDLL_SCRIPT_EXPORT_H__IMPORTED_)
#define _YYCDLL_SCRIPT_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

inline void generate_pLink_in_pIn(CKContext* ctx, CKParameterIn* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB, BOOL isTarget);
inline void proc_pTarget(CKContext* ctx, CKParameterIn* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents);
inline void proc_pIn(CKContext* ctx, CKParameterIn* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
inline void proc_pOut(CKContext* ctx, CKParameterOut* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
inline void proc_bIn(CKBehaviorIO* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, int index);
inline void proc_bOut(CKBehaviorIO* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, int index);
inline void proc_bLink(CKBehaviorLink* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
inline void proc_pLocal(CKParameterLocal* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents, BOOL is_setting);
inline void proc_pOper(CKContext* ctx, CKParameterOperation* cache, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);

inline void helper_pLocalDataExport(const char* field, long data, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
inline void helper_pLocalDataExport(const char* field, float data, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
inline void helper_pLocalDataExport(const char* field, const char* data, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);

void IterateBehavior(CKContext* ctx, CKBehavior* bhv, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void IterateScript(CKContext* ctx, scriptDatabase* db, dbScriptDataStructHelper* helper);
void IteratepLocalData(CKParameterLocal* pl, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);

#endif