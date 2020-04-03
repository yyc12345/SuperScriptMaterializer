#if !defined(_YYCDLL_SCRIPT_EXPORT_H__IMPORTED_)
#define _YYCDLL_SCRIPT_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

inline void proc_pTarget(CKParameterIn* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);
inline void proc_pIn(CKParameterIn* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index);
inline void proc_pOut(CKParameterOut* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index);
inline void proc_bIn(CKBehaviorIO* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index);
inline void proc_bOut(CKBehaviorIO* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index);
inline void proc_bLink(CKBehaviorLink* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);
inline void proc_pLocal(CKParameterLocal* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, BOOL is_setting);
inline void proc_pOper(CKParameterOperation* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);

inline void helper_pLocalDataExport(const char* field, long data, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);
inline void helper_pLocalDataExport(const char* field, float data, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);
inline void helper_pLocalDataExport(const char* field, const char* data, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);

void IterateBehavior(CKBehavior* bhv, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);
void IterateScript(CKContext* ctx, database* db, dbDataStructHelper* helper);
void IteratepLocalData(CKParameterLocal* pl, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);

#endif