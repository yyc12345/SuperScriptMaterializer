#pragma once
#include "stdafx.hpp"

namespace VSW::Materializer::Utilities {
	
	/// @brief The value representing a invalid CK_ID.
	constexpr CK_ID INVALID_CK_ID = static_cast<CK_ID>(-1);

	/**
	 * @brief Get relative address from given absolute address
	 * @details This function is used when exporting function pointer into database.
	 * @param[in] absolute_addr The absolute address
	 * @return Module based relative address like \c xxx.dll+0x00000000.
	*/
	std::string RelativeAddress(const void* absolute_addr);

	void CopyGuid(int64_t& dst, const CKGUID& src);
	void CopyCKString(std::string& storage, const char* str);

}
