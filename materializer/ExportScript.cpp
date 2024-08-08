#include "ExportCore.hpp"
#include "Database.hpp"
#include "DataTypes.hpp"
#include "Utilities.hpp"
#include <numeric>
#include <type_traits>

namespace VSW::Materializer::ExportScript {

	struct ExportContext {
		ExportContext(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) :
			db(db_path), cache(), reporter(ctx), cp(code_page), attr_set(), param_mgr(ctx->GetParameterManager()) {}
		Database::ScriptDatabase db;
		DataTypes::Script::DataCache cache;
		Utilities::EnhancedReporter reporter;
		UINT cp;
		/// @brief Variable for removing duplicated exported attributes.
		std::set<CK_ID> attr_set;
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
		REC_DATA("dumper-guid", exported_t);
		// Record Parameter Type name
		YYCC::yycc_u8string exported_pt;
		CP_CKSTR(exported_pt, expctx.param_mgr->ParameterTypeToName(pt));
		REC_DATA("dumper-type-name", exported_pt);

		// Detect value object scenario (associated with an existing CKObject)
		if (p->GetParameterClassID() && p->GetValueObject(false)) {
			// Record CK_ID of associated object
			CKObject* assoc_obj = p->GetValueObject(false);
			REC_DATA("dumper-assoc-ckobj", assoc_obj->GetID());
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
			REC_DATA("dumper-float", *static_cast<float*>(p->GetReadDataPtr(false)));
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
			REC_DATA("dumper-int", *static_cast<int*>(p->GetReadDataPtr(false)));
			return;
		}
		// Vector value
		if (t == CKPGUID_VECTOR) {
			REC_DATA("dumper-vector", *static_cast<VxVector*>(p->GetReadDataPtr(false)));
			return;
		}
		if (t == CKPGUID_2DVECTOR) {
			REC_DATA("dumper-2dvector", *static_cast<Vx2DVector*>(p->GetReadDataPtr(false)));
			return;
		}
		if (t == CKPGUID_MATRIX) {
			REC_DATA("dumper-matrix", *static_cast<VxMatrix*>(p->GetReadDataPtr(false)));
			return;
		}
		if (t == CKPGUID_COLOR) {
			REC_DATA("dumper-color", *static_cast<VxColor*>(p->GetReadDataPtr(false)));
			return;
		}
		// 2D Curve value
		if (t == CKPGUID_2DCURVE) {
			// Get instance
			CK2dCurve* c = static_cast<CK2dCurve*>(p->GetReadDataPtr(false));
			// We build our unique binary 2d curve data.
			Utilities::Curve2DBuilder builder(c);
			REC_RAW_DATA("dumper-2d-curve", builder.GetDataPtr(), builder.GetDataLength());
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
			REC_DATA("dumper-string", utf8_string);
			return;
		}
		// If it gets here, we have no idea what it really is. so simply dump it.
		// Buffer-like
		if (t == CKPGUID_VOIDBUF
#if defined(VIRTOOLS_50) || defined(VIRTOOLS_40) || defined(VIRTOOLS_35)
			|| t == CKPGUID_SHADER || t == CKPGUID_TECHNIQUE || t == CKPGUID_PASS
#endif
			|| true // All unknown type goes there.
			) {
			// Raw data is similar with string,
			// but we don't need do encoding convertion.
			const void* data_ptr = p->GetReadDataPtr(false);
			size_t data_len = static_cast<size_t>(p->GetDataSize());
			REC_RAW_DATA("dumper-raw", data_ptr, data_len);
			return;
		}

	}


#pragma endregion



	void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) {
		// create export context
		ExportContext expctx(ctx, db_path, code_page);
		if (!expctx.db.IsValid()) {
			expctx.reporter.Err(YYCC_U8("Fail to open database. Export process aborted."));
			return;
		}

		// export script
	}

}
