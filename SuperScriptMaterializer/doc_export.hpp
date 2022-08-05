#pragma once

#include "stdafx.h"
#include "database.hpp"

namespace SSMaterializer {
	namespace DocumentExporter {

		void Generate_pLink(CKContext* ctx, CKParameterIn* analysed_pin, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int pin_index, BOOL executedFromBB, BOOL isTarget);
		void Generate_pLink(CKContext* ctx, CKParameterOut* analysed_pout, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int pout_index, BOOL executedFromBB);
		void Proc_pTarget(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents);
		void Proc_pIn(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
		void Proc_pOut(CKContext* ctx, CKParameterOut* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB);
		void Proc_bIn(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index);
		void Proc_bOut(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index);
		void Proc_bLink(CKBehaviorLink* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void Proc_pLocal(CKParameterLocal* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, BOOL is_setting);
		void Proc_pOper(CKContext* ctx, CKParameterOperation* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void Proc_pAttr(CKContext* ctx, Database::DocumentDatabase* mDb, CKParameter* cache);


		void Proc_Behavior(CKContext* ctx, CKBehavior* bhv, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void IterateScript(CKContext* ctx, Database::DocumentDatabase* mDb);

		void IterateArray(CKContext* ctx, Database::DocumentDatabase* mDb);

		void DigParameterData(CKParameterLocal* pl, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void DataDictWritter(const char* field, long data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void DataDictWritter(const char* field, float data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);
		void DataDictWritter(const char* field, const char* data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents);

	}
}
