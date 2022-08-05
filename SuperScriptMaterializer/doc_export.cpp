#include "doc_export.hpp"
#include "string_helper.hpp"

// some shitty features process
//disable shit tip
#pragma warning(disable:26812)
// disable microsoft shitty macro to avoid build error
#undef GetClassName

#define changeSuffix(a) prefix[endIndex]='\0';strcat(prefix,a)

namespace SSMaterializer {
	namespace DocumentExporter {

#pragma region script

		/*
		* `Generate` mean that this function will analyse something to generate some objects which are not existed in original Virtools document.
		* `Proc` meath that this function only just export something which is already existed in original Virtools document, or with slight modifications.
		*/

		/// <summary>
		/// Generate pLink and eLink from pIn
		/// </summary>
		/// <param name="ctx"></param>
		/// <param name="analysed_pin"></param>
		/// <param name="mDb"></param>
		/// <param name="parents"></param>
		/// <param name="grandparents"></param>
		/// <param name="pin_index"></param>
		/// <param name="executedFromBB"></param>
		/// <param name="isTarget"></param>
		void Generate_pLink(CKContext* ctx, CKParameterIn* analysed_pin, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int pin_index, BOOL executedFromBB, BOOL isTarget) {
			CKParameter* directSource = NULL;
			CKObject* ds_Owner = NULL;
			CKParameterIn* sharedSource = NULL;
			CKBehavior* ss_Owner = NULL;

			// first, we analyse eLink
			// check whether this is export parameter and write to database
			// if the behavior graph where this pIn locate, also include this pIn, we can simply
			// assume there is a eLink between them
			if (((CKBehavior*)ctx->GetObject(grandparents))->GetInputParameterPosition(analysed_pin) != -1) {
				mDb->mDbHelper.script_eLink.export_obj = analysed_pin->GetID();
				mDb->mDbHelper.script_eLink.internal_obj = parents;
				mDb->mDbHelper.script_eLink.is_in = TRUE;
				mDb->mDbHelper.script_eLink.index = pin_index;
				mDb->mDbHelper.script_eLink.parent = grandparents;

				mDb->write_script_eLink(mDb->mDbHelper.script_eLink);
				// if a eLink has been detected successfully, we returned immediately
				// and do not analyse any pLink anymore.
				return;
			}

			// start to analyse pLink
			// first, analyse direct_src
			if (directSource = analysed_pin->GetDirectSource()) {
				mDb->mDbHelper.script_pLink.input = directSource->GetID();
				// for almost pin, it is connected to a pLocal, so we use a if to test it first
				if (directSource->GetClassID() == CKCID_PARAMETERLOCAL || directSource->GetClassID() == CKCID_PARAMETERVARIABLE) {
					//pLocal
					mDb->mDbHelper.script_pLink.input_obj = directSource->GetID();	// the owner of pLocal is itself.
					mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_PLOCAL;
					mDb->mDbHelper.script_pLink.input_is_bb = FALSE;
					mDb->mDbHelper.script_pLink.input_index = -1;
				} else {
					// according to Virtools SDK document, there are 4 possible value gotten by us:
					// bb pOut / pOper pOut / CKObject Attribute / CKDataArray
					// however, the last 2 returned values have NOT been tested perfectly.
					ds_Owner = directSource->GetOwner();
					switch (ds_Owner->GetClassID()) {
						case CKCID_BEHAVIOR:
							mDb->mDbHelper.script_pLink.input_obj = ds_Owner->GetID();
							mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_POUT;
							mDb->mDbHelper.script_pLink.input_is_bb = TRUE;
							mDb->mDbHelper.script_pLink.input_index = ((CKBehavior*)ds_Owner)->GetOutputParameterPosition((CKParameterOut*)directSource);
							break;
						case CKCID_PARAMETEROPERATION:
							mDb->mDbHelper.script_pLink.input_obj = ds_Owner->GetID();
							mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_POUT;
							mDb->mDbHelper.script_pLink.input_is_bb = FALSE;
							mDb->mDbHelper.script_pLink.input_index = 0;	// pOper only have 1 pOut
							break;
						case CKCID_DATAARRAY:
							// CKDataArray, see as virtual bb pLocal shortcut
							mDb->mDbHelper.script_pLink.input_obj = directSource->GetID();
							mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_PATTR;
							mDb->mDbHelper.script_pLink.input_is_bb = FALSE;	// omit
							mDb->mDbHelper.script_pLink.input_index = -1;	// omit
							Proc_pAttr(ctx, mDb, directSource);
							break;
						default:
							// CKObject, because CKDataArray also a CKObject, so we test it first.
							// see as virtual bb pLocal shortcut
							mDb->mDbHelper.script_pLink.input_obj = directSource->GetID();
							mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_PATTR;
							mDb->mDbHelper.script_pLink.input_is_bb = FALSE;	// omit
							mDb->mDbHelper.script_pLink.input_index = -1;	// omit
							Proc_pAttr(ctx, mDb, directSource);
							break;
					}
				}
			}
			// direct_src reflect the real source of current analysed pIn,
			// however direct_src do not reflact export link
			// so we need to analyse shared_src now.
			// 
			// if this pIn has established some export relation, its shared_src must be filled.
			// so we can detect it here. once its shared_src is not NULL
			// we should consider export link here.
			// 
			// we do not need to analyse any export link here, we just need to export some info 
			// to indicate this phenomeno.
			if (sharedSource = analysed_pin->GetSharedSource()) {
				//pIn from BB
				mDb->mDbHelper.script_pLink.input = sharedSource->GetID();
				ss_Owner = (CKBehavior*)sharedSource->GetOwner();
				mDb->mDbHelper.script_pLink.input_obj = ss_Owner->GetID();

				if (ss_Owner->IsUsingTarget() && (ss_Owner->GetTargetParameter() == sharedSource)) {
					//pTarget
					mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_PTARGET;
					mDb->mDbHelper.script_pLink.input_is_bb = TRUE;
					mDb->mDbHelper.script_pLink.input_index = -1;	// omit

				} else {
					//pIn
					mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_PIN;
					mDb->mDbHelper.script_pLink.input_is_bb = TRUE;
					mDb->mDbHelper.script_pLink.input_index = ss_Owner->GetInputParameterPosition(sharedSource);
				}


			}

			// if the header of pLink has been analysed successfully, 
			// we can add tail info and push into database
			if (sharedSource != NULL || directSource != NULL) {
				mDb->mDbHelper.script_pLink.output = analysed_pin->GetID();
				mDb->mDbHelper.script_pLink.output_obj = parents;
				mDb->mDbHelper.script_pLink.output_type = isTarget ? DataStruct::pLinkInputOutputType_PTARGET : DataStruct::pLinkInputOutputType_PIN;
				mDb->mDbHelper.script_pLink.output_is_bb = executedFromBB;
				mDb->mDbHelper.script_pLink.output_index = pin_index;
				mDb->mDbHelper.script_pLink.parent = grandparents;

				mDb->write_script_pLink(mDb->mDbHelper.script_pLink);
			}
		}

		void Generate_pLink(CKContext* ctx, CKParameterOut* analysed_pout, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int pout_index, BOOL executedFromBB) {
			// check eLink first
			// check whether expoer parameter and write to database
			if (((CKBehavior*)ctx->GetObject(grandparents))->GetOutputParameterPosition(analysed_pout) != -1) {
				mDb->mDbHelper.script_eLink.export_obj = analysed_pout->GetID();
				mDb->mDbHelper.script_eLink.internal_obj = parents;
				mDb->mDbHelper.script_eLink.is_in = FALSE;
				mDb->mDbHelper.script_eLink.index = pout_index;
				mDb->mDbHelper.script_eLink.parent = grandparents;

				mDb->write_script_eLink(mDb->mDbHelper.script_eLink);
				// if an eLink has been generated, skip following pLink generation
				return;
			}

			// try generate pLink
			CKParameter* cache_Dest = NULL;
			CKObject* cache_DestOwner = NULL;
			for (int j = 0, jCount = analysed_pout->GetDestinationCount(); j < jCount; j++) {
				cache_Dest = analysed_pout->GetDestination(j);

				mDb->mDbHelper.script_pLink.input = analysed_pout->GetID();
				mDb->mDbHelper.script_pLink.input_obj = parents;
				mDb->mDbHelper.script_pLink.input_type = DataStruct::pLinkInputOutputType_POUT;
				mDb->mDbHelper.script_pLink.input_is_bb = executedFromBB;
				mDb->mDbHelper.script_pLink.input_index = pout_index;

				mDb->mDbHelper.script_pLink.output = cache_Dest->GetID();
				if (cache_Dest->GetClassID() == CKCID_PARAMETERLOCAL) {
					//pLocal
					mDb->mDbHelper.script_pLink.output_obj = cache_Dest->GetID();
					mDb->mDbHelper.script_pLink.output_type = DataStruct::pLinkInputOutputType_PLOCAL;
					mDb->mDbHelper.script_pLink.output_is_bb = FALSE;	// omit
					mDb->mDbHelper.script_pLink.output_index = -1;		// omit

				} else {
					//pOut, it must belong to a graph BB (pOut can not be shared, and prototype bb or pOper do not have link-able pOut).
					cache_DestOwner = cache_Dest->GetOwner();
					switch (cache_DestOwner->GetClassID()) {
						case CKCID_BEHAVIOR:
							mDb->mDbHelper.script_pLink.output_obj = cache_DestOwner->GetID();
							mDb->mDbHelper.script_pLink.output_type = DataStruct::pLinkInputOutputType_POUT;
							mDb->mDbHelper.script_pLink.output_is_bb = TRUE;
							mDb->mDbHelper.script_pLink.output_index = ((CKBehavior*)cache_DestOwner)->GetOutputParameterPosition((CKParameterOut*)cache_Dest);
							break;
						case CKCID_DATAARRAY:
							// CKDataArray, see as virtual bb pLocal shortcut
							mDb->mDbHelper.script_pLink.output_obj = cache_Dest->GetID();
							mDb->mDbHelper.script_pLink.output_type = DataStruct::pLinkInputOutputType_PATTR;
							mDb->mDbHelper.script_pLink.input_is_bb = FALSE;	// omit
							mDb->mDbHelper.script_pLink.input_index = -1;	// omit
							Proc_pAttr(ctx, mDb, cache_Dest);
							break;
						default:
							// CKObject, because CKDataArray also a CKObject, so we test it first.
							// see as virtual bb pLocal shortcut
							mDb->mDbHelper.script_pLink.output_obj = cache_Dest->GetID();
							mDb->mDbHelper.script_pLink.output_type = DataStruct::pLinkInputOutputType_PATTR;
							mDb->mDbHelper.script_pLink.input_is_bb = FALSE;	// omit
							mDb->mDbHelper.script_pLink.input_index = -1;	// omit
							Proc_pAttr(ctx, mDb, cache_Dest);
							break;
					}
				}

				mDb->mDbHelper.script_pLink.parent = grandparents;

				mDb->write_script_pLink(mDb->mDbHelper.script_pLink);
			}
		}

		void Proc_pTarget(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents) {
			mDb->mDbHelper.script_pTarget.thisobj = cache->GetID();
			mDb->mDbHelper.script_pTarget.name = cache->GetName();
			CopyCKParamTypeStr(mDb->mDbHelper.script_pTarget.type, cache->GetType(), mDb->mDbHelper.param_manager);
			CopyGuid(mDb->mDbHelper.script_pTarget.type_guid, cache->GetGUID());
			mDb->mDbHelper.script_pTarget.parent = parents;
			mDb->mDbHelper.script_pTarget.direct_source = cache->GetDirectSource() ? cache->GetDirectSource()->GetID() : -1;
			mDb->mDbHelper.script_pTarget.shared_source = cache->GetSharedSource() ? cache->GetSharedSource()->GetID() : -1;

			mDb->write_script_pTarget(mDb->mDbHelper.script_pTarget);
			
			// try generate pLink and eLink
			Generate_pLink(ctx, cache, mDb, parents, grandparents, -1, TRUE, TRUE);
		}

		void Proc_pIn(CKContext* ctx, CKParameterIn* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB) {
			mDb->mDbHelper.script_pIn.thisobj = cache->GetID();
			mDb->mDbHelper.script_pIn.index = index;
			mDb->mDbHelper.script_pIn.name = cache->GetName();
			CopyCKParamTypeStr(mDb->mDbHelper.script_pIn.type, cache->GetType(), mDb->mDbHelper.param_manager);
			CopyGuid(mDb->mDbHelper.script_pIn.type_guid, cache->GetGUID());
			mDb->mDbHelper.script_pIn.parent = parents;
			mDb->mDbHelper.script_pIn.direct_source = cache->GetDirectSource() ? cache->GetDirectSource()->GetID() : -1;
			mDb->mDbHelper.script_pIn.shared_source = cache->GetSharedSource() ? cache->GetSharedSource()->GetID() : -1;

			mDb->write_script_pIn(mDb->mDbHelper.script_pIn);

			// try generate pLink and eLink
			Generate_pLink(ctx, cache, mDb, parents, grandparents, index, executedFromBB, FALSE);

		}

		void Proc_pOut(CKContext* ctx, CKParameterOut* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, DataStruct::EXPAND_CK_ID grandparents, int index, BOOL executedFromBB) {
			mDb->mDbHelper.script_pOut.thisobj = cache->GetID();
			mDb->mDbHelper.script_pOut.index = index;
			mDb->mDbHelper.script_pOut.name = cache->GetName();
			CopyCKParamTypeStr(mDb->mDbHelper.script_pOut.type, cache->GetType(), mDb->mDbHelper.param_manager);
			CopyGuid(mDb->mDbHelper.script_pOut.type_guid, cache->GetGUID());
			mDb->mDbHelper.script_pOut.parent = parents;

			mDb->write_script_pOut(mDb->mDbHelper.script_pOut);

			// try generate pLink and eLink
			Generate_pLink(ctx, cache, mDb, parents, grandparents, index, executedFromBB);
		}

		void Proc_bIn(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index) {
			mDb->mDbHelper.script_bIn.thisobj = cache->GetID();
			mDb->mDbHelper.script_bIn.index = index;
			mDb->mDbHelper.script_bIn.name = cache->GetName();
			mDb->mDbHelper.script_bIn.parent = parents;

			mDb->write_script_bIn(mDb->mDbHelper.script_bIn);
		}

		void Proc_bOut(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index) {
			mDb->mDbHelper.script_bOut.thisobj = cache->GetID();
			mDb->mDbHelper.script_bOut.index = index;
			mDb->mDbHelper.script_bOut.name = cache->GetName();
			mDb->mDbHelper.script_bOut.parent = parents;

			mDb->write_script_bOut(mDb->mDbHelper.script_bOut);
		}

		void Proc_bLink(CKBehaviorLink* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			CKBehaviorIO* io = cache->GetInBehaviorIO();
			CKBehavior* beh = io->GetOwner();
			mDb->mDbHelper.script_bLink.input = io->GetID();
			mDb->mDbHelper.script_bLink.input_obj = beh->GetID();
			mDb->mDbHelper.script_bLink.input_type = (io->GetType() == CK_BEHAVIORIO_IN ? bLinkInputOutputType_INPUT : bLinkInputOutputType_OUTPUT);
			mDb->mDbHelper.script_bLink.input_index = (io->GetType() == CK_BEHAVIORIO_IN ? io->GetOwner()->GetInputPosition(io) : io->GetOwner()->GetOutputPosition(io));
			io = cache->GetOutBehaviorIO();
			beh = io->GetOwner();
			mDb->mDbHelper.script_bLink.output = io->GetID();
			mDb->mDbHelper.script_bLink.output_obj = beh->GetID();
			mDb->mDbHelper.script_bLink.output_type = (io->GetType() == CK_BEHAVIORIO_IN ? bLinkInputOutputType_INPUT : bLinkInputOutputType_OUTPUT);
			mDb->mDbHelper.script_bLink.output_index = (io->GetType() == CK_BEHAVIORIO_IN ? io->GetOwner()->GetInputPosition(io) : io->GetOwner()->GetOutputPosition(io));

			mDb->mDbHelper.script_bLink.delay = cache->GetActivationDelay();
			mDb->mDbHelper.script_bLink.belong_to = parents;

			mDb->write_script_bLink(mDb->mDbHelper.script_bLink);
		}

		void Proc_pLocal(CKParameterLocal* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, BOOL is_setting) {
			mDb->mDbHelper.script_pLocal.thisobj = cache->GetID();
			mDb->mDbHelper.script_pLocal.name = cache->GetName() ? cache->GetName() : "";
			CKParameterType vaildTypeChecker = cache->GetType();
			if (vaildTypeChecker != -1) mDb->mDbHelper.script_pLocal.type = helper->_parameterManager->ParameterTypeToName(cache->GetType()); //known types
			else mDb->mDbHelper.script_pLocal.type = "!!UNKNOW TYPE!!"; //unknow type
			CopyGuid(cache->GetGUID(), mDb->mDbHelper.script_pLocal.type_guid);
			mDb->mDbHelper.script_pLocal.is_setting = is_setting;
			mDb->mDbHelper.script_pLocal.belong_to = parents;

			mDb->write_script_pLocal(mDb->mDbHelper.script_pLocal);

			//export plocal metadata
			DigParameterData(cache, mDb, cache->GetID());
		}

		void Proc_pOper(CKContext* ctx, CKParameterOperation* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			mDb->mDbHelper.script_pOper.thisobj = cache->GetID();
			mDb->mDbHelper.script_pOper.op = helper->_parameterManager->OperationGuidToName(cache->GetOperationGuid());
			CopyGuid(cache->GetOperationGuid(), mDb->mDbHelper.script_pOper.op_guid);
			mDb->mDbHelper.script_pOper.belong_to = parents;

			mDb->write_script_pOper(mDb->mDbHelper.script_pOper);

			//export 2 input param and 1 output param
			Proc_pIn(ctx, cache->GetInParameter1(), mDb, cache->GetID(), parents, 0, FALSE);
			Proc_pIn(ctx, cache->GetInParameter2(), mDb, cache->GetID(), parents, 1, FALSE);
			Proc_pOut(ctx, cache->GetOutParameter(), mDb, cache->GetID(), parents, 0, FALSE);
		}

		void Proc_pAttr(CKContext* ctx, Database::DocumentDatabase* mDb, CKParameter* cache) {
			//write self first to detect conflict
			mDb->mDbHelper.script_pAttr.thisobj = cache->GetID();
			CopyCKString(mDb->mDbHelper.script_pAttr.name, cache->GetName());
			CKParameterType vaildTypeChecker = cache->GetType();
			if (vaildTypeChecker != -1) mDb->mDbHelper.script_pAttr.type = helper->_parameterManager->ParameterTypeToName(cache->GetType()); //known types
			else mDb->mDbHelper.script_pAttr.type = "!!UNKNOW TYPE!!"; //unknow type
			CopyGuid(cache->GetGUID(), mDb->mDbHelper.script_pAttr.type_guid);

			BOOL already_exist = FALSE;
			mDb->write_script_pAttr(mDb->mDbHelper.script_pAttr, &already_exist);
			if (!already_exist) {
				//not duplicated, continue write property
				CKObject* host = cache->GetOwner();
				DataDictWritter("attr.host_id", (long)host->GetID(), mDb, cache->GetID());
				DataDictWritter("attr.host_name", host->GetName(), mDb, cache->GetID());
			}

		}

		void Proc_Behavior(CKContext* ctx, CKBehavior* bhv, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			//write self data
			mDb->mDbHelper.script_behavior.thisobj = bhv->GetID();
			mDb->mDbHelper.script_behavior.name = bhv->GetName();
			mDb->mDbHelper.script_behavior.type = bhv->GetType();
			mDb->mDbHelper.script_behavior.proto_name = bhv->GetPrototypeName() ? bhv->GetPrototypeName() : "";
			CopyGuid(bhv->GetPrototypeGuid(), mDb->mDbHelper.script_behavior.proto_guid);
			mDb->mDbHelper.script_behavior.flags = bhv->GetFlags();
			mDb->mDbHelper.script_behavior.priority = bhv->GetPriority();
			mDb->mDbHelper.script_behavior.version = bhv->GetVersion();
			mDb->mDbHelper.script_behavior.parent = parents;
			sprintf(helper->_stringCache, "%d,%d,%d,%d,%d",
				(bhv->IsUsingTarget() ? 1 : 0),
				bhv->GetInputParameterCount(),
				bhv->GetOutputParameterCount(),
				bhv->GetInputCount(),
				bhv->GetOutputCount());
			mDb->mDbHelper.script_behavior.pin_count = helper->_stringCache;
			mDb->write_script_behavior(mDb->mDbHelper.script_behavior);

			//write target
			if (bhv->IsUsingTarget())
				Proc_pTarget(ctx, bhv->GetTargetParameter(), mDb, bhv->GetID(), parents);

			int count = 0, i = 0;
			//pIn
			for (i = 0, count = bhv->GetInputParameterCount(); i < count; i++)
				Proc_pIn(ctx, bhv->GetInputParameter(i), mDb, bhv->GetID(), parents, i, TRUE);
			//pOut
			for (i = 0, count = bhv->GetOutputParameterCount(); i < count; i++)
				Proc_pOut(ctx, bhv->GetOutputParameter(i), mDb, bhv->GetID(), parents, i, TRUE);
			//bIn
			for (i = 0, count = bhv->GetInputCount(); i < count; i++)
				Proc_bIn(bhv->GetInput(i), mDb, bhv->GetID(), i);
			//bOut
			for (i = 0, count = bhv->GetOutputCount(); i < count; i++)
				Proc_bOut(bhv->GetOutput(i), mDb, bhv->GetID(), i);
			//bLink
			for (i = 0, count = bhv->GetSubBehaviorLinkCount(); i < count; i++)
				Proc_bLink(bhv->GetSubBehaviorLink(i), mDb, bhv->GetID());
			//pLocal
			for (i = 0, count = bhv->GetLocalParameterCount(); i < count; i++)
				Proc_pLocal(bhv->GetLocalParameter(i), mDb, bhv->GetID(),
				bhv->IsLocalParameterSetting(i));
			//pOper
			for (i = 0, count = bhv->GetParameterOperationCount(); i < count; i++)
				Proc_pOper(ctx, bhv->GetParameterOperation(i), mDb, bhv->GetID());

			//iterate sub bb
			for (i = 0, count = bhv->GetSubBehaviorCount(); i < count; i++)
				Proc_Behavior(ctx, bhv->GetSubBehavior(i), mDb, bhv->GetID());
		}

		void IterateScript(CKContext* ctx, Database::DocumentDatabase* mDb) {
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

					mDb->mDbHelper.script.thisobj = beobj->GetID();
					mDb->mDbHelper.script.host_name = beobj->GetName();
					mDb->mDbHelper.script.index = j;
					mDb->mDbHelper.script.behavior = beh->GetID();
					mDb->write_script(mDb->mDbHelper.script);

					//iterate script
					Proc_Behavior(ctx, beh, mDb, -1);
				}
			}
		}

#pragma endregion

#pragma region array

#pragma endregion

#pragma region data process

		void DigParameterData(CKParameterLocal* p, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			CKGUID t = p->GetGUID();
			BOOL unknowType = FALSE;

			if (t.d1 & t.d2) unknowType = TRUE;

			//nothing
			if (t == CKPGUID_NONE) return;
			if (p->GetParameterClassID() && p->GetValueObject(false)) {
				DataDictWritter("id", (long)p->GetValueObject(false)->GetID(), mDb, parents);
				DataDictWritter("name", p->GetValueObject(false)->GetName(), mDb, parents);

				CK_CLASSID none_classid = p->GetValueObject(false)->GetClassID();
				CKParameterType none_type = helper->_parameterManager->ClassIDToType(none_classid);
				DataDictWritter("type", helper->_parameterManager->ParameterTypeToName(none_type), mDb, parents);
				return;
			}
			//float
			if (t == CKPGUID_FLOAT || t == CKPGUID_ANGLE || t == CKPGUID_PERCENTAGE || t == CKPGUID_TIME
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
				|| t == CKPGUID_FLOATSLIDER
#endif
				) {
				DataDictWritter("float-data", *(float*)(p->GetReadDataPtr(false)), mDb, parents);
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
				DataDictWritter("int-data", (long)(*(int*)(p->GetReadDataPtr(false))), mDb, parents);
				return;
			}
			if (t == CKPGUID_VECTOR) {
				VxVector vec;
				memcpy(&vec, p->GetReadDataPtr(false), sizeof(vec));
				DataDictWritter("vector.x", vec.x, mDb, parents);
				DataDictWritter("vector.y", vec.y, mDb, parents);
				DataDictWritter("vector.z", vec.z, mDb, parents);
				return;
			}
			if (t == CKPGUID_2DVECTOR) {
				Vx2DVector vec;
				memcpy(&vec, p->GetReadDataPtr(false), sizeof(vec));
				DataDictWritter("2dvector.x", vec.x, mDb, parents);
				DataDictWritter("2dvector.y", vec.y, mDb, parents);
				return;
			}
			if (t == CKPGUID_MATRIX) {
				VxMatrix mat;
				char position[128];
				memcpy(&mat, p->GetReadDataPtr(false), sizeof(mat));

				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						sprintf(position, "matrix[%d][%d]", i, j);
						DataDictWritter(position, mat[i][j], mDb, parents);
					}
				}
				return;
			}
			if (t == CKPGUID_COLOR) {
				VxColor col;
				memcpy(&col, p->GetReadDataPtr(false), sizeof(col));
				DataDictWritter("color.r", col.r, mDb, parents);
				DataDictWritter("color.g", col.g, mDb, parents);
				DataDictWritter("color.b", col.b, mDb, parents);
				DataDictWritter("color.a", col.a, mDb, parents);
				return;
			}
			if (t == CKPGUID_2DCURVE) {
				//CK2dCurve* c;
				CK2dCurve* c = (CK2dCurve*)p->GetReadDataPtr(false);
				char prefix[128];
				int endIndex = 0;
				//memcpy(&c, p->GetReadDataPtr(false), sizeof(c));
				for (int i = 0, cc = c->GetControlPointCount(); i < cc; ++i) {
					sprintf(prefix, "2dcurve.control_point[%d]", i);
					endIndex = strlen(prefix);

					changeSuffix(".pos.x");
					DataDictWritter(prefix, c->GetControlPoint(i)->GetPosition().x, mDb, parents);
					changeSuffix(".pos.y");
					DataDictWritter(prefix, c->GetControlPoint(i)->GetPosition().y, mDb, parents);
					changeSuffix(".islinear");
					DataDictWritter(prefix, (long)c->GetControlPoint(i)->IsLinear(), mDb, parents);
					if (c->GetControlPoint(i)->IsTCB()) {
						changeSuffix(".bias");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetBias(), mDb, parents);
						changeSuffix(".continuity");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetContinuity(), mDb, parents);
						changeSuffix(".tension");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetTension(), mDb, parents);
					} else {
						changeSuffix(".intangent.x");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetInTangent().x, mDb, parents);
						changeSuffix(".intangent.y");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetInTangent().y, mDb, parents);
						changeSuffix(".outtangent.x");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetOutTangent().x, mDb, parents);
						changeSuffix(".outtangent.y");
						DataDictWritter(prefix, c->GetControlPoint(i)->GetOutTangent().y, mDb, parents);
					}
				}
				return;
			}
			if (t == CKPGUID_STRING) {
				char* cptr = (char*)p->GetReadDataPtr(false);
				int cc = p->GetDataSize();

				helper->_db_pData->data.clear();
				helper->_db_pData->data.insert(0, cptr, 0, cc);
				helper->_db_pData->field = "str";
				helper->_db_pData->belong_to = p->GetID();
				mDb->write_pData(helper->_db_pData);
				return;
			}

			unknowType = TRUE;
			//if it gets here, we have no idea what it really is. so simply dump it.
			//buffer-like
			if (unknowType || t == CKPGUID_VOIDBUF
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
				|| t == CKPGUID_SHADER || t == CKPGUID_TECHNIQUE || t == CKPGUID_PASS
#endif
				) {
				//dump data
				unsigned char* cptr = (unsigned char*)p->GetReadDataPtr(false);
				char temp[8];
				int cc = 0, rcc = 0, pos = 0;
				rcc = cc = p->GetDataSize();
				if (rcc > 1024) rcc = 1024;

				helper->_db_pData->data.clear();
				for (int i = 0; i < rcc; i++) {
					sprintf(temp, "0x%02X", cptr[i]);

					helper->_db_pData->data += temp;
					if (i != rcc - 1)
						helper->_db_pData->data += ", ";
				}

				if (rcc == cc)
					helper->_db_pData->field = "dump.data";
				else
					helper->_db_pData->field = "dump.partial_data";
				helper->_db_pData->belong_to = p->GetID();
				mDb->write_pData(helper->_db_pData);

				//dump data length
				DataDictWritter("dump.length", (long)cc, mDb, parents);
				return;
			}
		}

		void DataDictWritter(const char* field, const char* data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			mDb->mDbHelper.data.field = field;
			mDb->mDbHelper.data.data = data;
			mDb->mDbHelper.data.parent = parents;

			mDb->write_data(mDb->mDbHelper.data);
		}
		void DataDictWritter(const char* field, float data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			static std::string float_conv;
			Utils::StdstringPrintf(float_conv, "%f", data);
			DataDictWritter(field, float_conv.c_str(), mDb, parents);
		}
		void DataDictWritter(const char* field, long data, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			static std::string int_conv;
			Utils::StdstringPrintf(int_conv, "%d", data);
			DataDictWritter(field, int_conv.c_str(), mDb, parents);
		}

#pragma endregion


	}
}