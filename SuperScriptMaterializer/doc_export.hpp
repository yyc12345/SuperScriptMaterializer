#pragma once

#include "stdafx.h"
#include "database.hpp"

namespace SSMaterializer {
	namespace DocumentExporter {

		void generate_pLink_in_pIn(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB, BOOL isTarget);
		void proc_pTarget(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents);
		void proc_pIn(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
		void proc_pOut(CKContext* ctx, CKParameterOut* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
		void proc_bIn(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index);
		void proc_bOut(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index);
		void proc_bLink(CKBehaviorLink* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void proc_pLocal(CKParameterLocal* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, BOOL is_setting);
		void proc_pOper(CKContext* ctx, CKParameterOperation* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void proc_pAttr(CKContext* ctx, Database::DocumentDatabase* mDb, CKParameter* cache);


		void IterateBehavior(CKContext* ctx, CKBehavior* bhv, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void IterateScript(CKContext* ctx, Database::DocumentDatabase* mDb);

		void IterateArray(CKContext* ctx, Database::DocumentDatabase* mDb);

		void DigParameterData(CKParameterLocal* pl, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void DataDictWritter(const char* field, long data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void DataDictWritter(const char* field, float data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void DataDictWritter(const char* field, const char* data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);

	}
}
