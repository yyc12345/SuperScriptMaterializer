#pragma once

#include "stdafx.h"
#include <string>

namespace SSMaterializer {
	namespace Utils {

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);
		void CopyGUID(std::string& strl, CKGUID& guid);

	}
}
