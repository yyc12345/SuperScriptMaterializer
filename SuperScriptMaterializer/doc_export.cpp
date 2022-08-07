#include "doc_export.hpp"
#include "string_helper.hpp"

// some shitty features process
//disable shit tip
#pragma warning(disable:26812)
// disable microsoft shitty macro to avoid build error
#undef GetClassName

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
			CopyCKString(mDb->mDbHelper.script_pTarget.name, cache->GetName());
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
			CopyCKString(mDb->mDbHelper.script_pIn.name, cache->GetName());
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
			CopyCKString(mDb->mDbHelper.script_pOut.name, cache->GetName());
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
			CopyCKString(mDb->mDbHelper.script_bIn.name, cache->GetName());
			mDb->mDbHelper.script_bIn.parent = parents;

			mDb->write_script_bIn(mDb->mDbHelper.script_bIn);
		}

		void Proc_bOut(CKBehaviorIO* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, int index) {
			mDb->mDbHelper.script_bOut.thisobj = cache->GetID();
			mDb->mDbHelper.script_bOut.index = index;
			CopyCKString(mDb->mDbHelper.script_bOut.name, cache->GetName());
			mDb->mDbHelper.script_bOut.parent = parents;

			mDb->write_script_bOut(mDb->mDbHelper.script_bOut);
		}

		void Proc_bLink(CKBehaviorLink* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			CKBehaviorIO* io = cache->GetInBehaviorIO();
			CKBehavior* beh = io->GetOwner();
			mDb->mDbHelper.script_bLink.input = io->GetID();
			mDb->mDbHelper.script_bLink.input_obj = beh->GetID();
			mDb->mDbHelper.script_bLink.input_type = (io->GetType() == CK_BEHAVIORIO_IN ? DataStruct::bLinkInputOutputType_INPUT : DataStruct::bLinkInputOutputType_OUTPUT);
			mDb->mDbHelper.script_bLink.input_index = (io->GetType() == CK_BEHAVIORIO_IN ? io->GetOwner()->GetInputPosition(io) : io->GetOwner()->GetOutputPosition(io));
			io = cache->GetOutBehaviorIO();
			beh = io->GetOwner();
			mDb->mDbHelper.script_bLink.output = io->GetID();
			mDb->mDbHelper.script_bLink.output_obj = beh->GetID();
			mDb->mDbHelper.script_bLink.output_type = (io->GetType() == CK_BEHAVIORIO_IN ? DataStruct::bLinkInputOutputType_INPUT : DataStruct::bLinkInputOutputType_OUTPUT);
			mDb->mDbHelper.script_bLink.output_index = (io->GetType() == CK_BEHAVIORIO_IN ? io->GetOwner()->GetInputPosition(io) : io->GetOwner()->GetOutputPosition(io));

			mDb->mDbHelper.script_bLink.delay = cache->GetActivationDelay();
			mDb->mDbHelper.script_bLink.parent = parents;

			mDb->write_script_bLink(mDb->mDbHelper.script_bLink);
		}

		void Proc_pLocal(CKParameterLocal* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents, BOOL is_setting) {
			mDb->mDbHelper.script_pLocal.thisobj = cache->GetID();
			CopyCKString(mDb->mDbHelper.script_pLocal.name, cache->GetName());
			CopyCKParamTypeStr(mDb->mDbHelper.script_pLocal.type, cache->GetType(), mDb->mDbHelper.param_manager);
			CopyGuid(mDb->mDbHelper.script_pLocal.type_guid, cache->GetGUID());
			mDb->mDbHelper.script_pLocal.is_setting = is_setting;
			mDb->mDbHelper.script_pLocal.parent = parents;

			mDb->write_script_pLocal(mDb->mDbHelper.script_pLocal);

			//export plocal metadata
			DigParameterData(cache, mDb, cache->GetID());
		}

		void Proc_pOper(CKContext* ctx, CKParameterOperation* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			mDb->mDbHelper.script_pOper.thisobj = cache->GetID();
			mDb->mDbHelper.script_pOper.op = mDb->mDbHelper.param_manager->OperationGuidToName(cache->GetOperationGuid());
			CopyGuid(mDb->mDbHelper.script_pOper.op_guid, cache->GetOperationGuid());
			mDb->mDbHelper.script_pOper.parent = parents;

			mDb->write_script_pOper(mDb->mDbHelper.script_pOper);

			//export 2 input param and 1 output param
			Proc_pIn(ctx, cache->GetInParameter1(), mDb, cache->GetID(), parents, 0, FALSE);
			Proc_pIn(ctx, cache->GetInParameter2(), mDb, cache->GetID(), parents, 1, FALSE);
			Proc_pOut(ctx, cache->GetOutParameter(), mDb, cache->GetID(), parents, 0, FALSE);
		}

		/*
		* pAttr do not have any explict interface to get them
		* so they only can be find via pLink analyse.
		* 2 pLink analyse funcstions will call this function to record each pAttr
		* due to this mechanism, it might cause a duplication problem so
		* we need check possible duplication here.
		*/
		void Proc_pAttr(CKContext* ctx, Database::DocumentDatabase* mDb, CKParameter* cache) {
			// check duplication
			if (mDb->is_attr_duplicated(cache->GetID())) return;

			// write self first to detect conflict
			mDb->mDbHelper.script_pAttr.thisobj = cache->GetID();
			CopyCKString(mDb->mDbHelper.script_pAttr.name, cache->GetName());
			CopyCKParamTypeStr(mDb->mDbHelper.script_pAttr.type, cache->GetType(), mDb->mDbHelper.param_manager);
			CopyGuid(mDb->mDbHelper.script_pAttr.type_guid, cache->GetGUID());

			mDb->write_script_pAttr(mDb->mDbHelper.script_pAttr);

			// continue write some properties to indicate the host of this attribute
			CKObject* host = cache->GetOwner();
			// write owner id
			DataDictWritter("attr.owner", (long)host->GetID(), mDb, cache->GetID());
			// write data for owner
			DigObjectData(host, mDb, host->GetID());
		}

		void Proc_Behavior(CKContext* ctx, CKBehavior* bhv, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			//write self data
			mDb->mDbHelper.script_behavior.thisobj = bhv->GetID();
			CopyCKString(mDb->mDbHelper.script_behavior.name, bhv->GetName());
			mDb->mDbHelper.script_behavior.type = bhv->GetType();
			CopyCKString(mDb->mDbHelper.script_behavior.proto_name, bhv->GetPrototypeName());
			CopyGuid(mDb->mDbHelper.script_behavior.proto_guid, bhv->GetPrototypeGuid());
			mDb->mDbHelper.script_behavior.flags = bhv->GetFlags();
			mDb->mDbHelper.script_behavior.priority = bhv->GetPriority();
			mDb->mDbHelper.script_behavior.version = bhv->GetVersion();
			mDb->mDbHelper.script_behavior.parent = parents;
			Utils::StdstringPrintf(mDb->mDbHelper.script_behavior.pin_count, "%d,%d,%d,%d,%d",
				(bhv->IsUsingTarget() ? 1 : 0),
				bhv->GetInputParameterCount(),
				bhv->GetOutputParameterCount(),
				bhv->GetInputCount(),
				bhv->GetOutputCount());
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
			// get all CKBeObject to try to get all scripts.
			// because only CKBeObject can load script.
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

#pragma region message

		void IterateMessage(CKContext* ctx, Database::DocumentDatabase* mDb) {
			CKMessageManager* msgManager = ctx->GetMessageManager();
			int count = msgManager->GetMessageTypeCount();
			for (int i = 0; i < count; i++) {
				mDb->mDbHelper.msg.index = i;
				CopyCKString(mDb->mDbHelper.msg.name, msgManager->GetMessageTypeName(i));

				mDb->write_msg(mDb->mDbHelper.msg);
			}
		}

#pragma endregion

#pragma region array

		void IterateArray(CKContext* ctx, Database::DocumentDatabase* mDb) {
			// get all array
			XObjectPointerArray objArray = ctx->GetObjectListByType(CKCID_DATAARRAY, TRUE);
			CKDataArray* darray = NULL;
			int len = objArray.Size();
			for (int i = 0; i < len; i++) {
				darray = (CKDataArray*)objArray.GetObjectA(i);

				// dump self data first
				mDb->mDbHelper._array.thisobj = darray->GetID();
				CopyCKString(mDb->mDbHelper._array.name, darray->GetName());
				mDb->mDbHelper._array.columns = darray->GetColumnCount();
				mDb->mDbHelper._array.rows = darray->GetRowCount();

				mDb->write_array(mDb->mDbHelper._array);

				// dump column and row data
				Proc_ArrayColumnRow(darray, mDb, darray->GetID());
			}
		}

		void Proc_ArrayColumnRow(CKDataArray* cache, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			int columns = cache->GetColumnCount(), rows = cache->GetRowCount();

			// we iterate columns to get data
			// because the value type of one column is not changed,
			// it is benefit to our export.
			for (int col = 0; col < columns; ++col) {
				// dump column self first
				mDb->mDbHelper.array_header.index = col;
				CopyCKString(mDb->mDbHelper.array_header.name, cache->GetColumnName(col));

				CK_ARRAYTYPE coltype = cache->GetColumnType(col);
				mDb->mDbHelper.array_header.type = coltype;
				if (coltype == CKARRAYTYPE_PARAMETER) {
					CKGUID guid = cache->GetColumnParameterGuid(col);
					CopyGuid(mDb->mDbHelper.array_header.param_type_guid, guid);
					CKSTRING pname = mDb->mDbHelper.param_manager->ParameterGuidToName(guid);
					CopyCKString(mDb->mDbHelper.array_header.param_type, pname);
				} else {
					mDb->mDbHelper.array_header.param_type = "";
					mDb->mDbHelper.array_header.param_type_guid = "";
				}

				mDb->mDbHelper.array_header.parent = parents;
				mDb->write_array_header(mDb->mDbHelper.array_header);

				// write row data
				switch (coltype) {
					case CKARRAYTYPE_INT:
						for (int row = 0; row < rows; ++row) {
							mDb->mDbHelper.array_cell.column = col;
							mDb->mDbHelper.array_cell.row = row;
							mDb->mDbHelper.array_cell.parent = parents;

							Utils::StdstringPrintf(mDb->mDbHelper.array_cell.showcase, "%d", *((int*)cache->GetElement(row, col)));
							mDb->mDbHelper.array_cell.inner_param = -1;

							mDb->write_array_cell(mDb->mDbHelper.array_cell);
						}
						break;
					case CKARRAYTYPE_FLOAT:
						for (int row = 0; row < rows; ++row) {
							mDb->mDbHelper.array_cell.column = col;
							mDb->mDbHelper.array_cell.row = row;
							mDb->mDbHelper.array_cell.parent = parents;

							Utils::StdstringPrintf(mDb->mDbHelper.array_cell.showcase, "%f", *((float*)cache->GetElement(row, col)));
							mDb->mDbHelper.array_cell.inner_param = -1;

							mDb->write_array_cell(mDb->mDbHelper.array_cell);
						}
						break;
					case CKARRAYTYPE_STRING:
						for (int row = 0; row < rows; ++row) {
							mDb->mDbHelper.array_cell.column = col;
							mDb->mDbHelper.array_cell.row = row;
							mDb->mDbHelper.array_cell.parent = parents;

							int count = cache->GetElementStringValue(row, col, NULL);
							mDb->mDbHelper.array_cell.showcase.resize(count);
							cache->GetElementStringValue(row, col, (char*)mDb->mDbHelper.array_cell.showcase.data());
							mDb->mDbHelper.array_cell.inner_param = -1;

							mDb->write_array_cell(mDb->mDbHelper.array_cell);
						}
						break;
					case CKARRAYTYPE_OBJECT:
						for (int row = 0; row < rows; ++row) {
							mDb->mDbHelper.array_cell.column = col;
							mDb->mDbHelper.array_cell.row = row;
							mDb->mDbHelper.array_cell.parent = parents;

							CKObject* obj = cache->GetElementObject(row, col);
							if (obj == NULL) continue;	// fail to get obj
							CopyCKString(mDb->mDbHelper.array_cell.showcase, obj->GetName());
							mDb->mDbHelper.array_cell.inner_param = obj->GetID();

							mDb->write_array_cell(mDb->mDbHelper.array_cell);

							// dig more data for it
							DigObjectData(obj, mDb, obj->GetID());
						}
						break;
					case CKARRAYTYPE_PARAMETER:
						for (int row = 0; row < rows; ++row) {
							mDb->mDbHelper.array_cell.column = col;
							mDb->mDbHelper.array_cell.row = row;
							mDb->mDbHelper.array_cell.parent = parents;

							CKParameter* p = (CKParameter*)cache->GetElementObject(row, col);
							if (p == NULL) continue;	// fail to get obj
							int count = p->GetStringValue(NULL, FALSE);
							mDb->mDbHelper.array_cell.showcase.resize(count);
							p->GetStringValue((char*)mDb->mDbHelper.array_cell.showcase.data(), FALSE);
							mDb->mDbHelper.array_cell.inner_param = p->GetID();

							mDb->write_array_cell(mDb->mDbHelper.array_cell);

							// dig more data for it.
							DigParameterData(p, mDb, p->GetID());
						}
						break;
				}

			}
		}

#pragma endregion

#pragma region data process

		void DigObjectData(CKObject* o, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			// check duplication
			// we use `parents` not o->GetID() because in some call they are not equal.
			if (mDb->is_obj_duplicated(parents)) return;

			DataDictWritter("obj.id", (long)o->GetID(), mDb, parents);
			DataDictWritter("obj.name", o->GetName() ? o->GetName() : "", mDb, parents);
			DataDictWritter("obj.classid", (long)o->GetClassID(), mDb, parents);
			DataDictWritter("obj.type", o->GetClassNameA(), mDb, parents);
		}

		void DigParameterData(CKParameter* p, Database::DocumentDatabase* mDb, DataStruct::EXPAND_CK_ID parents) {
			// due to our algorithm, parameter can not be duplicated
			// so we don't need to check its duplication.
			CKGUID t = p->GetGUID();
			CKParameterType pt = p->GetType();

			// export guid and type name corresponding with guid
			static std::string str_guid;
			static std::string str_typename;
			CopyGuid(str_guid, t);
			DataDictWritter("guid", str_guid.c_str(), mDb, parents);
			CopyCKParamTypeStr(str_typename, pt, mDb->mDbHelper.param_manager);
			DataDictWritter("typename", str_typename.c_str(), mDb, parents);

			// value object
			if (p->GetParameterClassID() && p->GetValueObject(false)) {
				CKObject* vobj = p->GetValueObject(false);
				// write its id
				DataDictWritter("vobj", (long)vobj->GetID(), mDb, parents);
				// write more data for its id
				DigObjectData(vobj, mDb, vobj->GetID());
				return;
			}

			//nothing
			if (t == CKPGUID_NONE) return;

			//float
			if (t == CKPGUID_FLOAT || t == CKPGUID_ANGLE || t == CKPGUID_PERCENTAGE || t == CKPGUID_TIME
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
				|| t == CKPGUID_FLOATSLIDER
#endif
				) {
				DataDictWritter("float", *(float*)(p->GetReadDataPtr(false)), mDb, parents);
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
				DataDictWritter("int", (long)(*(int*)(p->GetReadDataPtr(false))), mDb, parents);
				return;
			}
			if (t == CKPGUID_VECTOR) {
				VxVector vec;
				memcpy(&vec, p->GetReadDataPtr(false), sizeof(vec));
				static std::string str_vector;
				Utils::StdstringPrintf(str_vector, "%f, %f, %f",
					vec.x, vec.y, vec.z);
				DataDictWritter("vector", str_vector.c_str(), mDb, parents);
				return;
			}
			if (t == CKPGUID_2DVECTOR) {
				Vx2DVector vec;
				memcpy(&vec, p->GetReadDataPtr(false), sizeof(vec));
				static std::string str_2dvector;
				Utils::StdstringPrintf(str_2dvector, "%f, %f",
					vec.x, vec.y);
				DataDictWritter("2dvector", str_2dvector.c_str(), mDb, parents);
				return;
			}
			if (t == CKPGUID_MATRIX) {
				VxMatrix mat;
				memcpy(&mat, p->GetReadDataPtr(false), sizeof(mat));

				static std::string str_matrix;
				Utils::StdstringPrintf(str_matrix, "%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f",
					mat[0][0], mat[0][1], mat[0][2], mat[0][3],
					mat[1][0], mat[1][1], mat[1][2], mat[1][3],
					mat[2][0], mat[2][1], mat[2][2], mat[2][3],
					mat[3][0], mat[3][1], mat[3][2], mat[3][3]
				);
				DataDictWritter("matrix", str_matrix.c_str(), mDb, parents);
				return;
			}
			if (t == CKPGUID_COLOR) {
				VxColor col;
				memcpy(&col, p->GetReadDataPtr(false), sizeof(col));
				static std::string str_color;
				Utils::StdstringPrintf(str_color, "%f, %f, %f, %f",
					col.r, col.g, col.b, col.a);
				DataDictWritter("color", str_color.c_str(), mDb, parents);
				return;
			}
			if (t == CKPGUID_2DCURVE) {
				//CK2dCurve* c;
				CK2dCurve* c = (CK2dCurve*)p->GetReadDataPtr(false);

				// we need construct a fake json body as our data
				static std::string str_2dcurve;
				static std::string str_2dcurve_entry;

				str_2dcurve = "[";
				for (int i = 0, cc = c->GetControlPointCount(); i < cc; ++i) {
					if (i != 0) str_2dcurve += ',';	// extra splitter

					str_2dcurve += '{';	// object start
					CK2dCurvePoint* cp = c->GetControlPoint(i);

					Utils::StdstringPrintf(str_2dcurve_entry, "\"x\": %f, \"y\": %f,", cp->GetPosition().x, cp->GetPosition().y);
					str_2dcurve += str_2dcurve_entry.c_str();

					if (cp->IsLinear()) str_2dcurve += "\"linear\": true,";
					else str_2dcurve += "\"linear\": false,";

					if (cp->IsTCB()) {
						str_2dcurve += "\"tcb\": true,";

						Utils::StdstringPrintf(str_2dcurve_entry, "\"bias\": %f,", cp->GetBias());
						str_2dcurve += str_2dcurve_entry.c_str();
						Utils::StdstringPrintf(str_2dcurve_entry, "\"continuity\": %f,", cp->GetContinuity());
						str_2dcurve += str_2dcurve_entry.c_str();
						Utils::StdstringPrintf(str_2dcurve_entry, "\"tension\": %f", cp->GetTension());
						str_2dcurve += str_2dcurve_entry.c_str();

					} else {
						str_2dcurve += "\"tcb\": false,";

						Utils::StdstringPrintf(str_2dcurve_entry, "\"intangent\": {\"x\": %f, \"y\": %f},", cp->GetInTangent().x, cp->GetInTangent().y);
						str_2dcurve += str_2dcurve_entry.c_str();
						Utils::StdstringPrintf(str_2dcurve_entry, "\"outtangent\": {\"x\": %f, \"y\": %f}", cp->GetOutTangent().x, cp->GetOutTangent().y);
						str_2dcurve += str_2dcurve_entry.c_str();

					}

					str_2dcurve += '}';
				}
				DataDictWritter("2dcurve", str_2dcurve.c_str(), mDb, parents);
				return;
			}
			if (t == CKPGUID_STRING) {
				char* cptr = (char*)p->GetReadDataPtr(false);
				int cc = p->GetDataSize();

				// virtools internal data may not have null terminal, so we use raw add function here
				// resize data and copy
				mDb->mDbHelper.data.data.resize(cc);
				memcpy((void*)mDb->mDbHelper.data.data.data(), cptr, cc);
				mDb->mDbHelper.data.field = "string";
				mDb->mDbHelper.data.parent = parents;
				mDb->write_data(mDb->mDbHelper.data);
				return;
			}

			//if it gets here, we have no idea what it really is. so simply dump it.
			//buffer-like
			if (t == CKPGUID_VOIDBUF
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
				|| t == CKPGUID_SHADER || t == CKPGUID_TECHNIQUE || t == CKPGUID_PASS
#endif
				) {
				// raw data also need use raw export feature
				// we use base64 encode to encode all data
				char* cptr = (char*)p->GetReadDataPtr(false);
				int ds = p->GetDataSize();

				// dump data
				static std::string str_raw;
				if (ds > 10240) {
					// data is too big, clamp it to 10240
					Utils::StdstringGetBase64(str_raw, cptr, 10240);
					DataDictWritter("raw.partial_data", str_raw.c_str(), mDb, parents);
				} else {
					Utils::StdstringGetBase64(str_raw, cptr, ds);
					DataDictWritter("raw.data", str_raw.c_str(), mDb, parents);
				}

				//dump data length
				DataDictWritter("dump.length", (long)ds, mDb, parents);
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