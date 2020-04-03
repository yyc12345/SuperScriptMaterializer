#include "script_export.h"

#define changeSuffix(a) prefix[endIndex]='\0';strcat(prefix,a)

#pragma region inline func

inline void proc_pTarget(CKParameterIn* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	helper->_db_pTarget->thisobj = cache->GetID();
	strcpy(helper->_db_pTarget->name, cache->GetName());
	strcpy(helper->_db_pTarget->type, helper->_parameterManager->ParameterTypeToName(cache->GetType()));
	helper->_db_pTarget->type_guid[0] = cache->GetGUID().d1;
	helper->_db_pTarget->type_guid[1] = cache->GetGUID().d2;
	helper->_db_pTarget->belong_to = parents;
	helper->_db_pTarget->direct_source = cache->GetDirectSource() ? cache->GetDirectSource()->GetID() : -1;
	helper->_db_pTarget->shared_source = cache->GetSharedSource() ? cache->GetSharedSource()->GetID() : -1;

	db->write_pTarget(helper->_db_pTarget);
}

inline void proc_pIn(CKParameterIn* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index) {
	helper->_db_pIn->thisobj = cache->GetID();
	helper->_db_pIn->index = index;
	strcpy(helper->_db_pIn->name, cache->GetName());
	strcpy(helper->_db_pIn->type, helper->_parameterManager->ParameterTypeToName(cache->GetType()));
	helper->_db_pIn->type_guid[0] = cache->GetGUID().d1;
	helper->_db_pIn->type_guid[1] = cache->GetGUID().d2;
	helper->_db_pIn->belong_to = parents;
	helper->_db_pIn->direct_source = cache->GetDirectSource() ? cache->GetDirectSource()->GetID() : -1;
	helper->_db_pIn->shared_source = cache->GetSharedSource() ? cache->GetSharedSource()->GetID() : -1;

	db->write_pIn(helper->_db_pIn);

	//=========try generate pLink
	EXPAND_CK_ID origin = -1;
	if (cache->GetDirectSource()) origin = cache->GetDirectSource()->GetID();
	if (cache->GetSharedSource()) origin = cache->GetSharedSource()->GetID();
	if (origin == -1) return;
	helper->_db_pLink->input = origin;
	helper->_db_pLink->output = cache->GetID();
	helper->_db_pLink->belong_to = index;

	db->write_pLink(helper->_db_pLink);
}

inline void proc_pOut(CKParameterOut* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index) {
	helper->_db_pOut->thisobj = cache->GetID();
	helper->_db_pOut->index = index;
	strcpy(helper->_db_pOut->name, cache->GetName());
	strcpy(helper->_db_pOut->type, helper->_parameterManager->ParameterTypeToName(cache->GetType()));
	helper->_db_pOut->type_guid[0] = cache->GetGUID().d1;
	helper->_db_pOut->type_guid[1] = cache->GetGUID().d2;
	helper->_db_pOut->belong_to = parents;

	db->write_pOut(helper->_db_pOut);

	//=========try generate pLink
	CKParameter* cache_Dest = NULL;
	for (int j = 0, jCount = cache->GetDestinationCount(); j < jCount; j++) {
		cache_Dest = cache->GetDestination(j);

		helper->_db_pLink->input = cache->GetID();
		helper->_db_pLink->output = cache_Dest->GetID();
		helper->_db_pLink->belong_to = parents;

		db->write_pLink(helper->_db_pLink);
	}
}

inline void proc_bIn(CKBehaviorIO* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index) {
	helper->_db_bIn->thisobj = cache->GetID();
	helper->_db_bIn->index = index;
	strcpy(helper->_db_bIn->name, cache->GetName());
	helper->_db_bIn->belong_to = parents;

	db->write_bIn(helper->_db_bIn);
}

inline void proc_bOut(CKBehaviorIO* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, int index) {
	helper->_db_bOut->thisobj = cache->GetID();
	helper->_db_bOut->index = index;
	strcpy(helper->_db_bOut->name, cache->GetName());
	helper->_db_bOut->belong_to = parents;

	db->write_bOut(helper->_db_bOut);
}

inline void proc_bLink(CKBehaviorLink* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	helper->_db_bLink->input = cache->GetInBehaviorIO()->GetID();
	helper->_db_bLink->output = cache->GetOutBehaviorIO()->GetID();
	helper->_db_bLink->delay = cache->GetActivationDelay();
	helper->_db_bLink->belong_to = parents;

	db->write_bLink(helper->_db_bLink);
}

inline void proc_pLocal(CKParameterLocal* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents, BOOL is_setting) {
	helper->_db_pLocal->thisobj = cache->GetID();
	strcpy(helper->_db_pLocal->name, cache->GetName() ? cache->GetName() : "");
	strcpy(helper->_db_pLocal->type, helper->_parameterManager->ParameterTypeToName(cache->GetType()));
	helper->_db_pLocal->type_guid[0] = cache->GetGUID().d1;
	helper->_db_pLocal->type_guid[1] = cache->GetGUID().d2;
	helper->_db_pLocal->is_setting = is_setting;
	helper->_db_pLocal->belong_to = parents;

	db->write_pLocal(helper->_db_pLocal);

	//export plocal metadata
	IteratepLocalData(cache, db, helper, cache->GetID());
}

inline void proc_pOper(CKParameterOperation* cache, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	helper->_db_pOper->thisobj = cache->GetID();
	strcpy(helper->_db_pOper->op, helper->_parameterManager->OperationGuidToName(cache->GetOperationGuid()));
	helper->_db_pOper->op_guid[0] = cache->GetOperationGuid().d1;
	helper->_db_pOper->op_guid[1] = cache->GetOperationGuid().d2;
	helper->_db_pOper->belong_to = parents;

	db->write_pOper(helper->_db_pOper);

	//export 2 input param and 1 output param
	proc_pIn(cache->GetInParameter1(), db, helper, cache->GetID(), 0);
	proc_pIn(cache->GetInParameter2(), db, helper, cache->GetID(), 1);
	proc_pOut(cache->GetOutParameter(), db, helper, cache->GetID(), 0);
}


//============================helper for pLocal data export
inline void helper_pLocalDataExport(const char* field, const char* data, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	strcpy(helper->_db_pLocalData->field, field);
	strcpy(helper->_db_pLocalData->data, data);
	helper->_db_pLocalData->belong_to = parents;

	db->write_pLocalData(helper->_db_pLocalData);
}
inline void helper_pLocalDataExport(const char* field, float data, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	char str[32];
	sprintf(str, "%f", data);
	helper_pLocalDataExport(field, str, db, helper, parents);
}
inline void helper_pLocalDataExport(const char* field, long data, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	char str[32];
	ltoa(data, str, 10);
	helper_pLocalDataExport(field, str, db, helper, parents);
}


#pragma endregion


#pragma region normal func

void IterateScript(CKContext* ctx, database* db, dbDataStructHelper* helper) {
	CKBeObject* beobj = NULL;
	CKBehavior* beh = NULL;
	XObjectPointerArray objArray = ctx->GetObjectListByType(CKCID_BEOBJECT, TRUE);
	int len = objArray.Size();
	int scriptLen = 0;
	for (int i = 0; i < len; i++) {
		beobj = (CKBeObject*)objArray.GetObjectA(i);
		if ((scriptLen = beobj->GetScriptCount()) == 0) continue;
		for (int j = 0; j < scriptLen; j++) {
			//write script table
			beh = beobj->GetScript(j);

			helper->_dbCKScript->thisobj = beobj->GetID();
			strcpy(helper->_dbCKScript->host_name, beobj->GetName());
			helper->_dbCKScript->index = j;
			helper->_dbCKScript->behavior = beh->GetID();
			db->write_CKScript(helper->_dbCKScript);

			//iterate script
			IterateBehavior(beh, db, helper, -1);
		}
	}
}

void IterateBehavior(CKBehavior* bhv, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	//write self data
	helper->_dbCKBehavior->thisobj = bhv->GetID();
	strcpy(helper->_dbCKBehavior->name, bhv->GetName());
	helper->_dbCKBehavior->type = bhv->GetType();
	strcpy(helper->_dbCKBehavior->proto_name, bhv->GetPrototypeName() ? bhv->GetPrototypeName() : "");
	helper->_dbCKBehavior->proto_guid[0] = bhv->GetPrototypeGuid().d1;
	helper->_dbCKBehavior->proto_guid[1] = bhv->GetPrototypeGuid().d2;
	helper->_dbCKBehavior->flags = bhv->GetFlags();
	helper->_dbCKBehavior->priority = bhv->GetPriority();
	helper->_dbCKBehavior->version = bhv->GetVersion();
	helper->_dbCKBehavior->parent = parents;
	db->write_CKBehavior(helper->_dbCKBehavior);

	//write target
	if (bhv->IsUsingTarget())
		proc_pTarget(bhv->GetTargetParameter(), db, helper, bhv->GetID());

	int count = 0, i = 0;
	//pIn
	for (i = 0, count = bhv->GetInputParameterCount(); i < count; i++)
		proc_pIn(bhv->GetInputParameter(i), db, helper, bhv->GetID(), i);
	//pOut
	for (i = 0, count = bhv->GetOutputParameterCount(); i < count; i++)
		proc_pOut(bhv->GetOutputParameter(i), db, helper, bhv->GetID(), i);
	//bIn
	for (i = 0, count = bhv->GetInputCount(); i < count; i++)
		proc_bIn(bhv->GetInput(i), db, helper, bhv->GetID(), i);
	//bOut
	for (i = 0, count = bhv->GetOutputCount(); i < count; i++)
		proc_bOut(bhv->GetOutput(i), db, helper, bhv->GetID(), i);
	//bLink
	for (i = 0, count = bhv->GetSubBehaviorLinkCount(); i < count; i++)
		proc_bLink(bhv->GetSubBehaviorLink(i), db, helper, bhv->GetID());
	//pLocal
	for (i = 0, count = bhv->GetLocalParameterCount(); i < count; i++)
		proc_pLocal(bhv->GetLocalParameter(i), db, helper, bhv->GetID(),
			bhv->IsLocalParameterSetting(i));
	//bOper
	for (i = 0, count = bhv->GetParameterOperationCount(); i < count; i++)
		proc_pOper(bhv->GetParameterOperation(i), db, helper, bhv->GetID());

	//iterate sub bb
	for (i = 0, count = bhv->GetSubBehaviorCount(); i < count; i++)
		IterateBehavior(bhv->GetSubBehavior(i), db, helper, bhv->GetID());
}

void IteratepLocalData(CKParameterLocal* p, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents) {
	CKGUID t = p->GetGUID(); 
	BOOL unknowType = FALSE;

	if (t.d1 & t.d2) unknowType = TRUE;

	//nothing
	if (t == CKPGUID_NONE) return;
	if (p->GetParameterClassID() && p->GetValueObject(false)) {
		helper_pLocalDataExport("id", (long)p->GetValueObject(false)->GetID(), db, helper, parents);
		helper_pLocalDataExport("name", p->GetValueObject(false)->GetName(), db, helper, parents);
		helper_pLocalDataExport("type", p->GetValueObject(false)->GetClassNameA(), db, helper, parents);
		return;
	}
	//float
	if (t == CKPGUID_FLOAT || t == CKPGUID_ANGLE || t == CKPGUID_PERCENTAGE || t == CKPGUID_TIME
		|| t == CKPGUID_FLOATSLIDER) {
		helper_pLocalDataExport("float-data", *(float*)(p->GetReadDataPtr(false)), db, helper, parents);
		return;
	}
	//int
	if (t == CKPGUID_INT || t == CKPGUID_KEY || t == CKPGUID_BOOL || t == CKPGUID_ID || t == CKPGUID_POINTER
		|| t == CKPGUID_MESSAGE || t == CKPGUID_ATTRIBUTE || t == CKPGUID_BLENDMODE || t == CKPGUID_FILTERMODE
		|| t == CKPGUID_BLENDFACTOR || t == CKPGUID_FILLMODE || t == CKPGUID_LITMODE || t == CKPGUID_SHADEMODE
		|| t == CKPGUID_ADDRESSMODE || t == CKPGUID_WRAPMODE || t == CKPGUID_3DSPRITEMODE || t == CKPGUID_FOGMODE
		|| t == CKPGUID_LIGHTTYPE || t == CKPGUID_SPRITEALIGN || t == CKPGUID_DIRECTION || t == CKPGUID_LAYERTYPE
		|| t == CKPGUID_COMPOPERATOR || t == CKPGUID_BINARYOPERATOR || t == CKPGUID_SETOPERATOR
		|| t == CKPGUID_OBSTACLEPRECISION || t == CKPGUID_OBSTACLEPRECISIONBEH) {
		helper_pLocalDataExport("int-data", (long)(*(int*)(p->GetReadDataPtr(false))), db, helper, parents);
		return;
	}
	if (t == CKPGUID_VECTOR) {
		VxVector vec;
		memcpy(&vec, p->GetReadDataPtr(false), sizeof(vec));
		helper_pLocalDataExport("vector.x", vec.x, db, helper, parents);
		helper_pLocalDataExport("vector.y", vec.y, db, helper, parents);
		helper_pLocalDataExport("vector.z", vec.z, db, helper, parents);
		return;
	}
	if (t == CKPGUID_2DVECTOR) {
		Vx2DVector vec;
		memcpy(&vec, p->GetReadDataPtr(false), sizeof(vec));
		helper_pLocalDataExport("2dvector.x", vec.x, db, helper, parents);
		helper_pLocalDataExport("2dvector.y", vec.y, db, helper, parents);
		return;
	}
	if (t == CKPGUID_MATRIX) {
		VxMatrix mat;
		char position[128];
		memcpy(&mat, p->GetReadDataPtr(false), sizeof(mat));

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				sprintf(position, "matrix[%d][%d]", i, j);
				helper_pLocalDataExport(position, mat[i][j], db, helper, parents);
			}
		}
		return;
	}
	if (t == CKPGUID_COLOR) {
		VxColor col;
		memcpy(&col, p->GetReadDataPtr(false), sizeof(col));
		helper_pLocalDataExport("color.r", col.r, db, helper, parents);
		helper_pLocalDataExport("color.g", col.g, db, helper, parents);
		helper_pLocalDataExport("color.b", col.b, db, helper, parents);
		helper_pLocalDataExport("color.a", col.a, db, helper, parents);
		return;
	}
	if (t == CKPGUID_2DCURVE) {
		CK2dCurve* c;
		char prefix[128];
		int endIndex = 0;
		memcpy(&c, p->GetReadDataPtr(false), sizeof(c));
		for (int i = 0, cc = c->GetControlPointCount(); i < cc; ++i) {
			sprintf(prefix, "2dcurve.control_point[%d]", i);
			endIndex = strlen(prefix);

			changeSuffix(".pos.x");
			helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetPosition().x, db, helper, parents);
			changeSuffix(".pos.y");
			helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetPosition().y, db, helper, parents);
			changeSuffix(".islinear");
			helper_pLocalDataExport(prefix, (long)c->GetControlPoint(i)->IsLinear(), db, helper, parents);
			if (c->GetControlPoint(i)->IsTCB()) {
				changeSuffix(".bias");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetBias(), db, helper, parents);
				changeSuffix(".continuity");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetContinuity(), db, helper, parents);
				changeSuffix(".tension");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetTension(), db, helper, parents);
			} else {
				changeSuffix(".intangent.x");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetInTangent().x, db, helper, parents);
				changeSuffix(".intangent.y");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetInTangent().y, db, helper, parents);
				changeSuffix(".outtangent.x");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetOutTangent().x, db, helper, parents);
				changeSuffix(".outtangent.y");
				helper_pLocalDataExport(prefix, c->GetControlPoint(i)->GetOutTangent().y, db, helper, parents);
			}
		}
		return;
	}

	unknowType = TRUE;
	//if it gets here, we have no idea what it really is. so simply dump it.
	//buffer-like
	if (unknowType || t == CKPGUID_VOIDBUF || t == CKPGUID_STRING || t == CKPGUID_SHADER || t == CKPGUID_TECHNIQUE || t == CKPGUID_PASS) {
		char* cptr = (char*)p->GetReadDataPtr(false);
		int cc = p->GetDataSize();
		for (int i = 0; i < cc; i++) 
			helper->_db_pLocalData->data[i] = cptr[i];
		helper->_db_pLocalData->data[cc] = '\0';
		strcpy(helper->_db_pLocalData->field, "dump-data");
		helper->_db_pLocalData->belong_to = p->GetID();
		db->write_pLocalData(helper->_db_pLocalData);

		return;
	}
}

#pragma endregion

