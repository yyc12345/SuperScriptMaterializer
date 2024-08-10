#include "Utilities.hpp"

namespace VSW::Materializer::Utilities {

#pragma region Enhanced Reporter

	EnhancedReporter::EnhancedReporter(CKContext* ctx) :
		m_Ctx(ctx), m_OrderBeep(false) {}

	EnhancedReporter::~EnhancedReporter() {}

	void EnhancedReporter::EnableBeep() { m_OrderBeep = true; }

	void EnhancedReporter::DisableBeep() { m_OrderBeep = false; }

	void EnhancedReporter::PrePrint(const YYCC::yycc_char8_t* strl) const {
		// only write when CKContext is not nullptr and we are in Interface Mode (Dev mode).
		if (m_Ctx != nullptr && m_Ctx->IsInInterfaceMode())
			m_Ctx->OutputToConsole(const_cast<CKSTRING>(YYCC::EncodingHelper::UTF8ToChar(strl, CP_ACP).c_str()), m_OrderBeep);
	}

#pragma endregion

#pragma region Curve 2D Builder

	Curve2DBuilder::Curve2DBuilder(CK2dCurve* curve_2d) : m_Cache() {
		BuildCurve(curve_2d);
	}

	Curve2DBuilder::~Curve2DBuilder() {}

	const void* Curve2DBuilder::GetDataPtr() const { return m_Cache.c_str(); }
	size_t Curve2DBuilder::GetDataLength() const { return m_Cache.size(); }

	void Curve2DBuilder::BuildCurve(CK2dCurve* c) {
		// check curve
		if (c == nullptr) return;
		// get curve control point count
		int cp_count = c->GetControlPointCount();
		// reserve enough space
		// count * (x + y + is_linear + is_tcb + (io_tangent_tuple / tcb_tuple))
		m_Cache.reserve(static_cast<size_t>(cp_count) * (sizeof(float) * 2u + sizeof(float) * 4u + sizeof(uint32_t)  + sizeof(uint32_t)));

		// iterate control point
		for (int i = 0; i < cp_count; ++i) {
			BuildCurvePoint(c->GetControlPoint(i));
		}
	}

	void Curve2DBuilder::BuildCurvePoint(CK2dCurvePoint* cp) {
		// check control point
		if (cp == nullptr) return;
		// prepare variable
		uint32_t int_cache;
		Vx2DVector vector_cache;
		float float_cache;

#define APPEND_DATA(data) m_Cache.append(reinterpret_cast<decltype(m_Cache)::value_type*>(&data), sizeof(data))
		
		// x y value
		vector_cache = cp->GetPosition();
		APPEND_DATA(vector_cache);

		// is linear
		int_cache = static_cast<uint32_t>(cp->IsLinear());
		APPEND_DATA(int_cache);
		// is tcb
		int_cache = static_cast<uint32_t>(cp->IsTCB());
		APPEND_DATA(int_cache);

		if (cp->IsTCB()) {
			// TCB control point
			float_cache = cp->GetTension();
			APPEND_DATA(float_cache);
			float_cache = cp->GetContinuity();
			APPEND_DATA(float_cache);
			float_cache = cp->GetBias();
			APPEND_DATA(float_cache);
			// To keep balance with non-TCB control point,
			// We add a blank 0.0f in there
			float_cache = 0.0f;
			APPEND_DATA(float_cache);
		} else {
			// non-TCB control point
			vector_cache = cp->GetInTangent();
			APPEND_DATA(vector_cache);
			vector_cache = cp->GetOutTangent();
			APPEND_DATA(vector_cache);
		}

#undef APPEND_DATA

	}

#pragma endregion

	void RelativeAddress(const EnhancedReporter& reporter, YYCC::yycc_u8string& relative_addr_str, const void* absolute_addr) {
		// If address is nullptr, return directly
		if (absolute_addr == nullptr) {
			relative_addr_str = YYCC_U8("<nullptr>");
			return;
		}

		// Get the module handle which given function address belongs to
		// Reference: https://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code
		HMODULE hModule = NULL;
		GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,	// get address and do not inc ref counter.
			(LPCWSTR)absolute_addr,
			&hModule);
		if (hModule == NULL) {
			reporter.ErrF(YYCC_U8("Fail to get module of given absolute address 0x%" PRI_XPTR_LEFT_PADDING PRIXPTR ". Some relative address may be empty."), absolute_addr);
			relative_addr_str.clear();
			return;
		}

		// Get full path to module
		YYCC::yycc_u8string u8_module_path;
		if (!YYCC::WinFctHelper::GetModuleFileName(hModule, u8_module_path)) {
			reporter.ErrF(YYCC_U8("Fail to get file name of given module 0x%" PRI_XPTR_LEFT_PADDING PRIXPTR ". Some relative address may be empty."), hModule);
			relative_addr_str.clear();
			return;
		}
		// Then get its file name part
		auto module_path = YYCC::FsPathPatch::FromUTF8Path(u8_module_path.c_str());
		auto u8_module_name = YYCC::FsPathPatch::ToUTF8Path(module_path.filename());

		// Get the base address of current module
		// HMODULE is the base address of loaded module
		// Reference: https://stackoverflow.com/questions/4298331/exe-or-dll-image-base-address
		uintptr_t relative_addr = reinterpret_cast<uintptr_t>(absolute_addr) - reinterpret_cast<uintptr_t>(hModule);

		// get final result
		if (!YYCC::StringHelper::Printf(relative_addr_str, YYCC_U8("%s+0x%" PRI_XPTR_LEFT_PADDING PRIXPTR), u8_module_name.c_str(), relative_addr)) {
			reporter.Err(YYCC_U8("Fail to format relative address. Some relative address may be empty."));
			relative_addr_str.clear();
		}
	}

	//void GetBase64(YYCC::yycc_u8string& dst, const char* data, size_t data_len) {
	//	// Reference: https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
	//	static const YYCC::yycc_char8_t* BASE64_TABLE = YYCC_U8("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	//	static const int MOD_TABLE[] = { 0, 2, 1 };

	//	// compute size
	//	size_t output_length = 4u * ((data_len + 2u) / 3u);
	//	dst.resize(output_length);

	//	// compute
	//	for (size_t i = 0, j = 0; i < data_len;) {

	//		uint32_t octet_a = i < data_len ? (unsigned char)data[i++] : 0;
	//		uint32_t octet_b = i < data_len ? (unsigned char)data[i++] : 0;
	//		uint32_t octet_c = i < data_len ? (unsigned char)data[i++] : 0;

	//		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

	//		dst[j++] = BASE64_TABLE[(triple >> 3 * 6) & 0x3F];
	//		dst[j++] = BASE64_TABLE[(triple >> 2 * 6) & 0x3F];
	//		dst[j++] = BASE64_TABLE[(triple >> 1 * 6) & 0x3F];
	//		dst[j++] = BASE64_TABLE[(triple >> 0 * 6) & 0x3F];
	//	}

	//	// fill blank
	//	for (int i = 0; i < MOD_TABLE[data_len % 3]; i++)
	//		dst[output_length - 1 - i] = '=';
	//}

	void CopyStrGuid(const EnhancedReporter& reporter, YYCC::yycc_u8string& dst, const CKGUID& src) {
		if (!YYCC::StringHelper::Printf(dst, YYCC_U8("<0x%08" PRIX32 ", 0x%08" PRIX32 ">"), src.d1, src.d2)) {
			reporter.Err(YYCC_U8("Fail to format CKGUID. Some stringified GUID may be empty."));
			dst.clear();
		}
	}

	void CopyGuid(int64_t& dst, const CKGUID& src) {
		// reset dst to zero
		uint64_t* pdst = reinterpret_cast<uint64_t*>(&dst);
		// CKGUID.d1 to high 32 bits
		*pdst = static_cast<uint64_t>(src.d1);
		*pdst <<= 32u;
		// CKGUID.d2 to low 32 bits
		*pdst = (*pdst) | static_cast<uint64_t>(src.d2);
	}

	void CopyCKString(const EnhancedReporter& reporter, YYCC::yycc_u8string& storage, const char* str, UINT code_page, const YYCC::yycc_char8_t* fallback) {
		// check whether callback is nullptr.
		if (fallback == nullptr)
			throw std::invalid_argument("fallback string should not be nullptr!");

		// if given string is nullptr, use fallback instead
		if (str == nullptr) {
			storage = fallback;
			return;
		}

		// otherwise do encoding convertion of original string.
		if (!YYCC::EncodingHelper::CharToUTF8(str, storage, code_page)) {
			reporter.Err(YYCC_U8("Fail to convert encoding. Some string may use fallback string accidently."));
			storage = fallback;
		}
	}

}
