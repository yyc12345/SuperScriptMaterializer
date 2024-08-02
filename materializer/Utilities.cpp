#include "Utilities.hpp"

namespace VSW::Materializer::Utilities {

	std::string RelativeAddress(const void* absolute_addr) {
		// prepare return value
		std::string ret("<error>");

		// If address is nullptr, return directly
		if (absolute_addr == nullptr) {
			ret = "<nullptr>";
			return ret;
		}

		// Get the module handle which given function address belongs to
		// Reference: https://stackoverflow.com/questions/557081/how-do-i-get-the-hmodule-for-the-currently-executing-code
		HMODULE hModule = NULL;
		GetModuleHandleExW(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,	// get address and do not inc ref counter.
			(LPCWSTR)absolute_addr,
			&hModule);
		if (hModule == NULL)
			return ret;

		// Get full path to module
		YYCC::yycc_u8string u8_module_path;
		if (!YYCC::WinFctHelper::GetModuleFileName(hModule, u8_module_path))
			return ret;
		// Then get its file name part
		auto module_path = YYCC::FsPathPatch::FromUTF8Path(u8_module_path.c_str());
		auto u8_module_name = YYCC::FsPathPatch::ToUTF8Path(module_path.filename());
		
		// Get the base address of current module
		// HMODULE is the base address of loaded module
		// Reference: https://stackoverflow.com/questions/4298331/exe-or-dll-image-base-address
		uintptr_t relative_addr = reinterpret_cast<uintptr_t>(absolute_addr) - reinterpret_cast<uintptr_t>(hModule);
		
		// get final result
		auto u8_ret = YYCC::StringHelper::Printf(YYCC_U8("%s+%" PRI_XPTR_LEFT_PADDING PRIXPTR), u8_module_name.c_str(), relative_addr);
		ret = YYCC::EncodingHelper::ToOrdinaryView(u8_ret);
		return ret;
	}

	void CopyGuid(int64_t& dst, const CKGUID& src) {
		// todo: use template argument to implement this to improve performance
		if (sizeof(dst) != sizeof(src))
			throw std::invalid_argument("CKGUID size error");
		std::memcpy(&dst, &src, sizeof(int64_t));
	}

	void CopyCKString(std::string& storage, const char* str) {
		if (str == nullptr) storage = "<unamed>";
		else storage = str;
	}

}
