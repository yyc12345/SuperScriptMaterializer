#include "ExportCore.hpp"
#include "Database.hpp"
#include "DataTypes.hpp"
#include "Utilities.hpp"
#include <numeric>
#include <type_traits>

/**
 * @brief The namespace contain functions exporting scripts
 * @details
 * \c Generate mean that this function will analyse something to generate some objects which are not existed in original Virtools document.
 * \c Proc meath that this function only just export something which is already existed in original Virtools document, or with slight modifications.
*/
namespace VSW::Materializer::ExportScript {

	struct ExportContext {
		ExportContext(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) :
			db(db_path), cache(), reporter(ctx), cp(code_page), attr_set(), ctx(ctx), param_mgr(ctx->GetParameterManager()) {}
		Database::ScriptDatabase db;
		DataTypes::Script::DataCache cache;
		Utilities::EnhancedReporter reporter;
		UINT cp;

		/// @brief Variable for removing duplicated exported attributes.
		std::set<CK_ID> attr_set;
		CKContext* ctx;
		CKParameterManager* param_mgr;
	};

#pragma region Assist Functions

	static void DataDictWriter(ExportContext& expctx, const YYCC::yycc_u8string_view& field, const void* data, size_t data_length, CK_ID parent) {
		// check given data length
		using db_size_t = decltype(expctx.cache.data.data.length);
		if (data_length > static_cast<size_t>(std::numeric_limits<db_size_t>::max()))
			throw std::runtime_error("Too long data length when exporting to data dictionary.");
		// write data
		expctx.cache.data.field = field;
		expctx.cache.data.data.ptr = data;
		expctx.cache.data.data.length = static_cast<db_size_t>(data_length);
		expctx.cache.data.parent = parent;
		expctx.db.Write(expctx.cache.data);
	}

	static void DataDictWriter(ExportContext& expctx, const YYCC::yycc_u8string_view& field, const YYCC::yycc_u8string& data, CK_ID parent) {
		DataDictWriter(expctx, field, data.c_str(), data.length(), parent);
	}

	template<class _Ty, std::enable_if_t<!std::is_same_v<_Ty, YYCC::yycc_u8string>, int> = 0>
	static void DataDictWriter(ExportContext& expctx, const YYCC::yycc_u8string_view& field, const _Ty& data, CK_ID parent) {
		DataDictWriter(expctx, field, &data, sizeof(_Ty), parent);
	}

#define REC_RAW_DATA(field, data, len) DataDictWriter(expctx, YYCC_U8(field), data, len, parent)
#define REC_DATA(field, data) DataDictWriter(expctx, YYCC_U8(field), data, parent)

	void DigParameterData(ExportContext& expctx, CKParameter* p, CK_ID parent) {
		// According to our algorithm, CKParameter passed in this function can not be duplicated.
		// So we don't need check it anymore.
		// Get CKGUID and Parameter Type first
		CKGUID t = p->GetGUID();
		CKParameterType pt = p->GetType();

		// Record GUID
		int64_t exported_t;
		CP_GUID(exported_t, t);
		REC_DATA("dumper.guid", exported_t);
		// Record Parameter Type name
		YYCC::yycc_u8string exported_pt;
		CP_CKSTR(exported_pt, expctx.param_mgr->ParameterTypeToName(pt));
		REC_DATA("dumper.type-name", exported_pt);

		// Detect value object scenario (associated with an existing CKObject)
		if (p->GetParameterClassID() && p->GetValueObject(false)) {
			// Record CK_ID of associated object
			CKObject* assoc_obj = p->GetValueObject(false);
			REC_DATA("dumper.assoc-ckobj", assoc_obj->GetID());
			return;
		}

		// Nothing
		if (t == CKPGUID_NONE) return;

		// Float value
		if (t == CKPGUID_FLOAT || t == CKPGUID_ANGLE || t == CKPGUID_PERCENTAGE || t == CKPGUID_TIME
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
			|| t == CKPGUID_FLOATSLIDER
#endif
			) {
			REC_DATA("dumper.float", *static_cast<float*>(p->GetReadDataPtr(false)));
			return;
		}
		// Integral value
		if (t == CKPGUID_INT || t == CKPGUID_KEY || t == CKPGUID_BOOL || t == CKPGUID_ID || t == CKPGUID_POINTER
			|| t == CKPGUID_MESSAGE || t == CKPGUID_ATTRIBUTE || t == CKPGUID_BLENDMODE || t == CKPGUID_FILTERMODE
			|| t == CKPGUID_BLENDFACTOR || t == CKPGUID_FILLMODE || t == CKPGUID_LITMODE || t == CKPGUID_SHADEMODE
			|| t == CKPGUID_ADDRESSMODE || t == CKPGUID_WRAPMODE || t == CKPGUID_3DSPRITEMODE || t == CKPGUID_FOGMODE
			|| t == CKPGUID_LIGHTTYPE || t == CKPGUID_SPRITEALIGN || t == CKPGUID_DIRECTION || t == CKPGUID_LAYERTYPE
			|| t == CKPGUID_COMPOPERATOR || t == CKPGUID_BINARYOPERATOR || t == CKPGUID_SETOPERATOR
			|| t == CKPGUID_OBSTACLEPRECISION || t == CKPGUID_OBSTACLEPRECISIONBEH) {
			REC_DATA("dumper.int", *static_cast<int*>(p->GetReadDataPtr(false)));
			return;
		}
		// Vector value
		if (t == CKPGUID_VECTOR) {
			REC_DATA("dumper.vector", *static_cast<VxVector*>(p->GetReadDataPtr(false)));
			return;
		}
		if (t == CKPGUID_2DVECTOR) {
			REC_DATA("dumper.2dvector", *static_cast<Vx2DVector*>(p->GetReadDataPtr(false)));
			return;
		}
		if (t == CKPGUID_MATRIX) {
			REC_DATA("dumper.matrix", *static_cast<VxMatrix*>(p->GetReadDataPtr(false)));
			return;
		}
		if (t == CKPGUID_COLOR) {
			REC_DATA("dumper.color", *static_cast<VxColor*>(p->GetReadDataPtr(false)));
			return;
		}
		// 2D Curve value
		if (t == CKPGUID_2DCURVE) {
			// Get instance
			CK2dCurve* c = static_cast<CK2dCurve*>(p->GetReadDataPtr(false));
			// We build our unique binary 2d curve data.
			Utilities::Curve2DBuilder builder(c);
			REC_RAW_DATA("dumper.2d-curve", builder.GetDataPtr(), builder.GetDataLength());
			return;
		}
		// String value
		if (t == CKPGUID_STRING) {
			// Virtools internal data may not have null terminal, 
			// so we use need copy it into container first.
			std::string native_string(
				static_cast<char*>(p->GetReadDataPtr(false)),
				static_cast<size_t>(p->GetDataSize())
			);
			// Then do encoding convertion
			YYCC::yycc_u8string utf8_string;
			if (!YYCC::EncodingHelper::CharToUTF8(native_string, utf8_string, expctx.cp)) {
				// If failed, report error
				expctx.reporter.Err(YYCC_U8("Fail to convert string encoding for data of CKParameter. Some value may be empty!"));
				// reset to blank string
				utf8_string.clear();
			}
			REC_DATA("dumper.string", utf8_string);
			return;
		}
		// If it gets here, we have no idea what it really is. so simply dump it.
		// Buffer-like
		if (t == CKPGUID_VOIDBUF
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
			|| t == CKPGUID_SHADER || t == CKPGUID_TECHNIQUE || t == CKPGUID_PASS
#endif
			|| true // All unknown type goes here.
			) {
			// Raw data is similar with string,
			// but we don't need do encoding convertion.
			const void* data_ptr = p->GetReadDataPtr(false);
			size_t data_len = static_cast<size_t>(p->GetDataSize());
			REC_RAW_DATA("dumper.raw", data_ptr, data_len);
			return;
		}

	}

#pragma endregion
	
	/**
	 * @brief Process Attribute Parameter
	 * @details
	 * pAttr do not have any explicit interface to get them, so they only can be find via pLink analyse.
	 * 2 pLink analyse funcstions will call this function to record each pAttr.
	 * Due to this mechanism, it might cause a duplication issues,
	 * so we need check possible duplication in here.
	 * @param[in] expctx Reference to export context.
	 * @param[in] ckobj Pointer to CKParameter (actually is attribute parameter) for processing.
	*/
	static void Proc_pAttr(ExportContext& expctx, CKParameter* ckobj) {
		// Check duplication.
		// We should not export duplicated pAttr
		auto insert_result = expctx.attr_set.emplace(ckobj->GetID());
		if (!insert_result.second) return;

		// Write self first
		expctx.cache.pAttr.thisobj = ckobj->GetID();
		CP_CKSTR(expctx.cache.pAttr.name, ckobj->GetName());
		CP_GUID(expctx.cache.pAttr.type, ckobj->GetGUID());
		expctx.cache.pAttr.owner = ckobj->GetOwner()->GetID();
		expctx.db.Write(expctx.cache.pAttr);
	}

	/**
	 * @brief Generate pLink and eLink from pIn
	 * @param[in] expctx Reference to export context.
	 * @param[in] analysed Pointer to CKParameterIn for analysing.
	 * @param[in] parent 
	 * @param[in] grandparent 
	 * @param[in] pin_index 
	 * @param[in] executed_from_bb True if given CKParameterIn is belong to a CKBehavior, otherwise false (CKParamterOperation).
	 * @param[in] is_target True if this function is called from pTarget processor.
	*/
	static void Generate_pLink(ExportContext& expctx, CKParameterIn* analysed, CK_ID parent, CK_ID grandparent, int pin_index, bool executed_from_bb, bool is_target) {
		// First, we analyse eLink.
		// Check whether this is export parameter and write to database.
		// If the behavior graph where this pIn's parent locate, also include this pIn,
		// we can simply assume there is an eLink between them
		CKBehavior* ckobj_grandparent = static_cast<CKBehavior*>(expctx.ctx->GetObjectA(grandparent));
		if (ckobj_grandparent->GetInputParameterPosition(analysed) != -1) {
			expctx.cache.eLink.export_obj = analysed->GetID();
			expctx.cache.eLink.internal_obj = parent;
			expctx.cache.eLink.is_in = true;
			expctx.cache.eLink.index = pin_index;
			expctx.cache.eLink.parent = grandparent;
			expctx.db.Write(expctx.cache.eLink);

			// if a eLink has been detected successfully, we returned immediately
			// and do not analyse any pLink any more.
			return;
		}

		// start to analyse pLink
		// first, analyse \c direct_source
		CKParameter* direct_source = analysed->GetDirectSource();
		if (direct_source != nullptr) {
			expctx.cache.pLink.input = direct_source->GetID();

			// For almost pIn, they are connected to a pLocal, so we use \c if statement to test it first
			CK_CLASSID direct_source_cid = direct_source->GetClassID();
			if (direct_source_cid == CKCID_PARAMETERLOCAL || direct_source_cid == CKCID_PARAMETERVARIABLE) {
				// pLocal
				expctx.cache.pLink.input_obj = direct_source->GetID();	// the owner of pLocal is itself.
				expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::PLOCAL;
				expctx.cache.pLink.input_is_bb = false;
				expctx.cache.pLink.input_index = Utilities::INVALID_INDEX;
			} else {
				// According to Virtools SDK document, there are 4 possible value gotten by us:
				// bb pOut / pOper pOut / CKObject Attribute / CKDataArray.
				// However, the last 2 returned values have NOT been tested perfectly.
				CKObject* ds_owner = direct_source->GetOwner();
				switch (ds_owner->GetClassID()) {
					case CKCID_BEHAVIOR:
					{;
						CKBehavior* ds_owner_cast = static_cast<CKBehavior*>(ds_owner);
						CKParameterOut* direct_source_cast = static_cast<CKParameterOut*>(direct_source);
						expctx.cache.pLink.input_obj = ds_owner->GetID();
						expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::POUT;
						expctx.cache.pLink.input_is_bb = true;
						expctx.cache.pLink.input_index = ds_owner_cast->GetOutputParameterPosition(direct_source_cast);
						break;
					}
					case CKCID_PARAMETEROPERATION:
					{
						expctx.cache.pLink.input_obj = ds_owner->GetID();
						expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::POUT;
						expctx.cache.pLink.input_is_bb = false;
						expctx.cache.pLink.input_index = 0;	// pOper only have 1 pOut
						break;
					}
					case CKCID_DATAARRAY:
					{
						// CKDataArray, see as virtual bb pLocal shortcut
						expctx.cache.pLink.input_obj = direct_source->GetID();
						expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::PATTR;
						expctx.cache.pLink.input_is_bb = false;	// discard
						expctx.cache.pLink.input_index = Utilities::INVALID_INDEX;	// discard
						Proc_pAttr(expctx, direct_source);
						break;
					}
					default:
					{
						// CKObject, because CKDataArray also a CKObject, so we test it first.
						// see as virtual bb pLocal shortcut
						expctx.cache.pLink.input_obj = direct_source->GetID();
						expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::PATTR;
						expctx.cache.pLink.input_is_bb = false;	// discard
						expctx.cache.pLink.input_index = Utilities::INVALID_INDEX;	// discard
						Proc_pAttr(expctx, direct_source);
						break;
					}
				}
			}
		}

		// \c direct_source reflect the real source of current analysed pIn,
		// however \c direct_source can not reflect export link.
		// So we need to analyse \c shared_source now for export link.
		// 
		// If this pIn has established some export relation, its \c shared_source must be filled, so we can detect this in here. 
		// Once its \c shared_source is not nullptr, we should consider export link in here.
		// 
		// We do not need to analyse any export link here, 
		// we just need to export some infos to tell that there is an export link.
		CKParameterIn* shared_source = analysed->GetSharedSource();
		if (shared_source != nullptr) {
			// pIn from BB
			// Get the owner of shared_source
			CKObject* ss_owner = shared_source->GetOwner();
			// Setup fields
			expctx.cache.pLink.input = shared_source->GetID();
			expctx.cache.pLink.input_obj = ss_owner->GetID();

			switch (ss_owner->GetClassID()) {
				case CKCID_BEHAVIOR: // CKBehavior
				{
					CKBehavior* ss_owner_cast = static_cast<CKBehavior*>(ss_owner);
					if (ss_owner_cast->IsUsingTarget() && (ss_owner_cast->GetTargetParameter() == shared_source)) {
						// pTarget
						expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::PTARGET;
						expctx.cache.pLink.input_is_bb = true;
						expctx.cache.pLink.input_index = Utilities::INVALID_INDEX;	// discard

					} else {
						// pIn
						expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::PIN;
						expctx.cache.pLink.input_is_bb = true;
						expctx.cache.pLink.input_index = ((CKBehavior*)ss_owner)->GetInputParameterPosition(shared_source);
					}
					break;
				}
				case CKCID_PARAMETEROPERATION:	// CKParameterOperation
				{
					// pOper only can have pIn (there is no possibility to have pTarget).
					CKParameterOperation* ss_owner_cast = static_cast<CKParameterOperation*>(ss_owner);
					expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::PIN;
					expctx.cache.pLink.input_is_bb = false;
					expctx.cache.pLink.input_index = ss_owner_cast->GetInParameter1() == shared_source ? 0 : 1;
					break;
				}
				default:
					// The unexpected value. 
					// According to SDK manual, there are only 2 possible types.
					throw std::runtime_error("unexpected shared_source owner class id!");
					return;
			}
		}

		// If the head of pLink has been analysed successfully, 
		// we can add tail info and push into database
		if (shared_source != nullptr || direct_source != nullptr) {
			expctx.cache.pLink.output = analysed->GetID();
			expctx.cache.pLink.output_obj = parent;
			expctx.cache.pLink.output_type = is_target ? VSW::DataTypes::ParameterLinkIOType::PTARGET : VSW::DataTypes::ParameterLinkIOType::PIN;
			expctx.cache.pLink.output_is_bb = executed_from_bb;
			expctx.cache.pLink.output_index = pin_index;
			expctx.cache.pLink.parent = grandparent;

			expctx.db.Write(expctx.cache.pLink);
		}
	}

	/**
	 * @brief Generate pLink and eLink from pOut
	 * @param[in] expctx Reference to export context.
	 * @param[in] analysed Pointer to CKParameterIn for analysing.
	 * @param[in] parent 
	 * @param[in] grandparent 
	 * @param[in] pout_index 
	 * @param[in] executed_from_bb True if given CKParameterIn is belong to a CKBehavior, otherwise false (CKParamterOperation).
	*/
	static void Generate_pLink(ExportContext& expctx, CKParameterOut* analysed, CK_ID parent, CK_ID grandparent, int pout_index, bool executed_from_bb) {
		// Check eLink first
		// Check whether there is an export parameter and write to database
		// Same check method as another overload
		CKBehavior* ckobj_grandparent = static_cast<CKBehavior*>(expctx.ctx->GetObjectA(grandparent));
		if (ckobj_grandparent->GetOutputParameterPosition(analysed) != -1) {
			expctx.cache.eLink.export_obj = analysed->GetID();
			expctx.cache.eLink.internal_obj = parent;
			expctx.cache.eLink.is_in = false;
			expctx.cache.eLink.index = pout_index;
			expctx.cache.eLink.parent = grandparent;

			expctx.db.Write(expctx.cache.eLink);
			// if an eLink has been generated, skip following pLink generation
			return;
		}

		// Try to generate pLink
		for (int j = 0, j_count = analysed->GetDestinationCount(); j < j_count; j++) {
			CKParameter* dest = analysed->GetDestination(j);

			expctx.cache.pLink.input = analysed->GetID();
			expctx.cache.pLink.input_obj = parent;
			expctx.cache.pLink.input_type = VSW::DataTypes::ParameterLinkIOType::POUT;
			expctx.cache.pLink.input_is_bb = executed_from_bb;
			expctx.cache.pLink.input_index = pout_index;

			expctx.cache.pLink.output = dest->GetID();
			if (dest->GetClassID() == CKCID_PARAMETERLOCAL) {
				// pLocal
				expctx.cache.pLink.output_obj = dest->GetID();
				expctx.cache.pLink.output_type = VSW::DataTypes::ParameterLinkIOType::PLOCAL;
				expctx.cache.pLink.output_is_bb = false;	// discard
				expctx.cache.pLink.output_index = Utilities::INVALID_INDEX;		// discard

			} else {
				// pOut. It must belong to a graph BB 
				// (pOut can not be shared, and prototype bb or pOper do not have linkable pOut).
				CKObject* dest_owner = dest->GetOwner();
				switch (dest_owner->GetClassID()) {
					case CKCID_BEHAVIOR:
					{
						CKBehavior* dest_owner_cast = static_cast<CKBehavior*>(dest_owner);
						CKParameterOut* dest_cast = static_cast<CKParameterOut*>(dest);
						expctx.cache.pLink.output_obj = dest_owner->GetID();
						expctx.cache.pLink.output_type = VSW::DataTypes::ParameterLinkIOType::POUT;
						expctx.cache.pLink.output_is_bb = true;
						expctx.cache.pLink.output_index = dest_owner_cast->GetOutputParameterPosition(dest_cast);
						break;
					}
					case CKCID_DATAARRAY:
					{
						// CKDataArray, see as virtual BB pLocal shortcut
						expctx.cache.pLink.output_obj = dest->GetID();
						expctx.cache.pLink.output_type = VSW::DataTypes::ParameterLinkIOType::PATTR;
						expctx.cache.pLink.input_is_bb = false;	// discard
						expctx.cache.pLink.input_index = Utilities::INVALID_INDEX;	// discard
						Proc_pAttr(expctx, dest);
						break;
					}
					default:
					{
						// CKObject. Because CKDataArray also a CKObject, so we test it above this case statement.
						// See this as virtual BB pLocal shortcut
						expctx.cache.pLink.output_obj = dest->GetID();
						expctx.cache.pLink.output_type = VSW::DataTypes::ParameterLinkIOType::PATTR;
						expctx.cache.pLink.input_is_bb = false;	// discard
						expctx.cache.pLink.input_index = Utilities::INVALID_INDEX;	// discard
						Proc_pAttr(expctx, dest);
						break;
					}
				}
			}

			// setup parent and write into database
			expctx.cache.pLink.parent = grandparent;
			expctx.db.Write(expctx.cache.pLink);
		}
	}

	static void Proc_pTarget(ExportContext& expctx, CKParameterIn* ckobj, CK_ID parent, CK_ID grandparent) {
		expctx.cache.pTarget.thisobj = ckobj->GetID();
		CP_CKSTR(expctx.cache.pTarget.name, ckobj->GetName());
		CP_GUID(expctx.cache.pTarget.type, ckobj->GetGUID());
		expctx.cache.pTarget.parent = parent;
		expctx.cache.pTarget.direct_source = ckobj->GetDirectSource() ? ckobj->GetDirectSource()->GetID() : Utilities::INVALID_CK_ID;
		expctx.cache.pTarget.shared_source = ckobj->GetSharedSource() ? ckobj->GetSharedSource()->GetID() : Utilities::INVALID_CK_ID;
		expctx.db.Write(expctx.cache.pTarget);

		// try to generate pLink and eLink
		Generate_pLink(expctx, ckobj, parent, grandparent, Utilities::INVALID_INDEX, true, true);
	}

	static void Proc_pIn(ExportContext& expctx, CKParameterIn* ckobj, CK_ID parent, CK_ID grandparent, int index, bool executed_from_bb) {
		expctx.cache.pIn.thisobj = ckobj->GetID();
		expctx.cache.pIn.index = index;
		CP_CKSTR(expctx.cache.pIn.name, ckobj->GetName());
		CP_GUID(expctx.cache.pIn.type, ckobj->GetGUID());
		expctx.cache.pIn.parent = parent;
		expctx.cache.pIn.direct_source = ckobj->GetDirectSource() ? ckobj->GetDirectSource()->GetID() : Utilities::INVALID_CK_ID;
		expctx.cache.pIn.shared_source = ckobj->GetSharedSource() ? ckobj->GetSharedSource()->GetID() : Utilities::INVALID_CK_ID;
		expctx.db.Write(expctx.cache.pIn);

		// try to generate pLink and eLink
		Generate_pLink(expctx, ckobj, parent, grandparent, index, executed_from_bb, false);
	}

	static void Proc_pOut(ExportContext& expctx, CKParameterOut* ckobj, CK_ID parent, CK_ID grandparent, int index, bool executed_from_bb) {
		expctx.cache.pOut.thisobj = ckobj->GetID();
		expctx.cache.pOut.index = index;
		CP_CKSTR(expctx.cache.pOut.name, ckobj->GetName());
		CP_GUID(expctx.cache.pOut.type, ckobj->GetGUID());
		expctx.cache.pOut.parent = parent;
		expctx.db.Write(expctx.cache.pOut);

		// try to generate pLink and eLink
		Generate_pLink(expctx, ckobj, parent, grandparent, index, executed_from_bb);
	}

	static void Proc_bIn(ExportContext& expctx, CKBehaviorIO* ckobj, CK_ID parent, int index) {
		expctx.cache.bIn.thisobj = ckobj->GetID();
		expctx.cache.bIn.index = index;
		CP_CKSTR(expctx.cache.bIn.name, ckobj->GetName());
		expctx.cache.bIn.parent = parent;
		expctx.db.Write(expctx.cache.bIn);
	}

	static void Proc_bOut(ExportContext& expctx, CKBehaviorIO* ckobj, CK_ID parent, int index) {
		expctx.cache.bOut.thisobj = ckobj->GetID();
		expctx.cache.bOut.index = index;
		CP_CKSTR(expctx.cache.bOut.name, ckobj->GetName());
		expctx.cache.bOut.parent = parent;
		expctx.db.Write(expctx.cache.bOut);
	}

	static void Proc_bLink(ExportContext& expctx, CKBehaviorLink* ckobj, CK_ID parent) {
		// setup start terminal
		CKBehaviorIO* io = ckobj->GetInBehaviorIO();
		CKBehavior* beh = io->GetOwner();
		expctx.cache.bLink.input = io->GetID();
		expctx.cache.bLink.input_obj = beh->GetID();
		expctx.cache.bLink.input_type = (io->GetType() == CK_BEHAVIORIO_IN ? VSW::DataTypes::BehaviorLinkIOType::INPUT : VSW::DataTypes::BehaviorLinkIOType::OUTPUT);
		expctx.cache.bLink.input_index = (io->GetType() == CK_BEHAVIORIO_IN ? io->GetOwner()->GetInputPosition(io) : io->GetOwner()->GetOutputPosition(io));
		// setup end terminal
		io = ckobj->GetOutBehaviorIO();
		beh = io->GetOwner();
		expctx.cache.bLink.output = io->GetID();
		expctx.cache.bLink.output_obj = beh->GetID();
		expctx.cache.bLink.output_type = (io->GetType() == CK_BEHAVIORIO_IN ? VSW::DataTypes::BehaviorLinkIOType::INPUT : VSW::DataTypes::BehaviorLinkIOType::OUTPUT);
		expctx.cache.bLink.output_index = (io->GetType() == CK_BEHAVIORIO_IN ? io->GetOwner()->GetInputPosition(io) : io->GetOwner()->GetOutputPosition(io));
		// other properties
		expctx.cache.bLink.delay = ckobj->GetActivationDelay();
		expctx.cache.bLink.parent = parent;
		// write to database
		expctx.db.Write(expctx.cache.bLink);
	}

	static void Proc_pLocal(ExportContext& expctx, CKParameterLocal* ckobj, CK_ID parent, bool is_setting) {
		expctx.cache.pLocal.thisobj = ckobj->GetID();
		CP_CKSTR(expctx.cache.pLocal.name, ckobj->GetName());
		CP_GUID(expctx.cache.pLocal.type, ckobj->GetGUID());
		expctx.cache.pLocal.is_setting = is_setting;
		expctx.cache.pLocal.parent = parent;
		expctx.db.Write(expctx.cache.pLocal);

		// Export pLocal internal data
		DigParameterData(expctx, ckobj, ckobj->GetID());
	}

	static void Proc_pOper(ExportContext& expctx, CKParameterOperation* ckobj, CK_ID parent) {
		expctx.cache.pOper.thisobj = ckobj->GetID();
		CP_GUID(expctx.cache.pOper.op, ckobj->GetOperationGuid());
		expctx.cache.pOper.parent = parent;
		expctx.db.Write(expctx.cache.pOper);

		// Process associated 2 pIn and 1 pOut
		Proc_pIn(expctx, ckobj->GetInParameter1(), ckobj->GetID(), parent, 0, false);
		Proc_pIn(expctx, ckobj->GetInParameter2(), ckobj->GetID(), parent, 1, false);
		Proc_pOut(expctx, ckobj->GetOutParameter(), ckobj->GetID(), parent, 0, false);
	}

	/**
	 * @brief Process CKBehavior
	 * @param[in] expctx Reference to export context.
	 * @param[in] behavior Pointer to CKBehavior for processing.
	 * @param[in] parent The parent of this CKBehavior. Set to \c INVALID_CK_ID if it was called from script iterator.
	*/
	static void Proc_Behavior(ExportContext& expctx, CKBehavior* behavior, CK_ID parent) {
		// Write basic behavior infomation
		expctx.cache.behavior.thisobj = behavior->GetID();
		CP_CKSTR(expctx.cache.behavior.name, behavior->GetName());
		expctx.cache.behavior.type = behavior->GetType();
		CP_CKSTR(expctx.cache.behavior.proto_name, behavior->GetPrototypeName());
		CP_GUID(expctx.cache.behavior.proto_guid, behavior->GetPrototypeGuid());
		expctx.cache.behavior.flags = behavior->GetFlags();
		expctx.cache.behavior.priority = behavior->GetPriority();
		expctx.cache.behavior.version = behavior->GetVersion();
		expctx.cache.behavior.pin_count_ptarget = (behavior->IsUsingTarget() ? 1 : 0);
		expctx.cache.behavior.pin_count_pin = behavior->GetInputParameterCount();
		expctx.cache.behavior.pin_count_pout = behavior->GetOutputParameterCount();
		expctx.cache.behavior.pin_count_bin = behavior->GetInputCount();
		expctx.cache.behavior.pin_count_bout = behavior->GetOutputCount();
		expctx.cache.behavior.parent = parent;
		expctx.db.Write(expctx.cache.behavior);
		
		// pTarget
		if (behavior->IsUsingTarget())
			Proc_pTarget(expctx, behavior->GetTargetParameter(), behavior->GetID(), parent);

		// pIn
		for (int i = 0, count = behavior->GetInputParameterCount(); i < count; ++i)
			Proc_pIn(expctx, behavior->GetInputParameter(i), behavior->GetID(), parent, i, true);
		// pOut
		for (int i = 0, count = behavior->GetOutputParameterCount(); i < count; ++i)
			Proc_pOut(expctx, behavior->GetOutputParameter(i), behavior->GetID(), parent, i, true);
		// bIn
		for (int i = 0, count = behavior->GetInputCount(); i < count; ++i)
			Proc_bIn(expctx, behavior->GetInput(i), behavior->GetID(), i);
		// bOut
		for (int i = 0, count = behavior->GetOutputCount(); i < count; ++i)
			Proc_bOut(expctx, behavior->GetOutput(i), behavior->GetID(), i);
		// bLink
		for (int i = 0, count = behavior->GetSubBehaviorLinkCount(); i < count; ++i)
			Proc_bLink(expctx, behavior->GetSubBehaviorLink(i), behavior->GetID());
		// pLocal
		for (int i = 0, count = behavior->GetLocalParameterCount(); i < count; ++i)
			Proc_pLocal(expctx, behavior->GetLocalParameter(i), behavior->GetID(), behavior->IsLocalParameterSetting(i));
		// pOper
		for (int i = 0, count = behavior->GetParameterOperationCount(); i < count; ++i)
			Proc_pOper(expctx, behavior->GetParameterOperation(i), behavior->GetID());

		// Iterate sub behavior
		for (int i = 0, count = behavior->GetSubBehaviorCount(); i < count; ++i)
			Proc_Behavior(expctx, behavior->GetSubBehavior(i), behavior->GetID());
	}

	static void IterateScript(ExportContext& expctx) {
		// Get all CKBeObjects to try to get all scripts,
		// because only CKBeObject can own script.
		XObjectPointerArray obj_array = expctx.ctx->GetObjectListByType(CKCID_BEOBJECT, TRUE);
		int obj_count = obj_array.Size();
		for (int i = 0; i < obj_count; ++i) {
			// Get CKBeObject
			CKBeObject* beobj = static_cast<CKBeObject*>(obj_array.GetObjectA(i));

			// Get script count. If no script, skip this object.
			int script_count = beobj->GetScriptCount();
			if (script_count == 0) continue;

			// Iterate binding scripts
			for (int j = 0; j < script_count; ++j) {
				// Get associated behavior
				CKBehavior* behavior = beobj->GetScript(j);
				// Write to database
				expctx.cache.script.beobj = beobj->GetID();
				CP_CKSTR(expctx.cache.script.beobj_name, beobj->GetName());
				expctx.cache.script.behavior_index = j;
				expctx.cache.script.behavior = behavior->GetID();
				expctx.db.Write(expctx.cache.script);

				// Process this script
				Proc_Behavior(expctx, behavior, Utilities::INVALID_CK_ID);
			}
		}
	}

	void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) {
		// create export context
		ExportContext expctx(ctx, db_path, code_page);
		if (!expctx.db.IsValid()) {
			expctx.reporter.Err(YYCC_U8("Fail to open database. Export process aborted."));
			return;
		}

		// export script
		IterateScript(expctx);
	}

}
