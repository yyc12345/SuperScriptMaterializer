#include "Utilities.hpp"

namespace VSW::Materializer::Utilities {

#pragma region Enhanced Reporter

	EnhancedReporter::EnhancedReporter(CKContext* ctx) :
		m_Ctx(ctx) {}

	EnhancedReporter::~EnhancedReporter() {}

	void EnhancedReporter::PrePrint(const YYCC::yycc_char8_t* strl) const {
		if (m_Ctx != nullptr)
			m_Ctx->OutputToConsole(const_cast<CKSTRING>(YYCC::EncodingHelper::UTF8ToChar(strl, CP_ACP).c_str()), FALSE);
	}

#pragma endregion

	YYCC::yycc_u8string RelativeAddress(const EnhancedReporter& reporter, const void* absolute_addr) {
		// prepare return value
		YYCC::yycc_u8string ret;

		// If address is nullptr, return directly
		if (absolute_addr == nullptr) {
			ret = YYCC_U8("<nullptr>");
			return ret;
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
			return ret;
		}

		// Get full path to module
		YYCC::yycc_u8string u8_module_path;
		if (!YYCC::WinFctHelper::GetModuleFileName(hModule, u8_module_path)) {
			reporter.ErrF(YYCC_U8("Fail to get file name of given module 0x%" PRI_XPTR_LEFT_PADDING PRIXPTR ". Some relative address may be empty."), hModule);
			return ret;
		}
		// Then get its file name part
		auto module_path = YYCC::FsPathPatch::FromUTF8Path(u8_module_path.c_str());
		auto u8_module_name = YYCC::FsPathPatch::ToUTF8Path(module_path.filename());

		// Get the base address of current module
		// HMODULE is the base address of loaded module
		// Reference: https://stackoverflow.com/questions/4298331/exe-or-dll-image-base-address
		uintptr_t relative_addr = reinterpret_cast<uintptr_t>(absolute_addr) - reinterpret_cast<uintptr_t>(hModule);

		// get final result
		if (!YYCC::StringHelper::Printf(ret, YYCC_U8("%s+0x%" PRI_XPTR_LEFT_PADDING PRIXPTR), u8_module_name.c_str(), relative_addr)) {
			reporter.Err(YYCC_U8("Fail to format relative address. Some relative address may be empty."));
			ret.clear();
		}
		return ret;
	}

	void CopyStrGuid(const EnhancedReporter& reporter, YYCC::yycc_u8string& dst, const CKGUID& src) {
		if (!YYCC::StringHelper::Printf(dst, YYCC_U8("<0x%08" PRIX32 ", 0x%08" PRIX32 ">"), src.d1, src.d2)) {
			reporter.Err(YYCC_U8("Fail to format CKGUID. Some stringified GUID may be empty."));
			dst.clear();
		}
	}

	void CopyGuid(const EnhancedReporter& reporter, int64_t& dst, const CKGUID& src) {
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
