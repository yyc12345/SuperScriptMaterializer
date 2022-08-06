#pragma once

#include "stdafx.h"
#include <string>

#define CopyGuid(str, guid) SSMaterializer::Utils::StdstringPrintf((str), "0x%08X, 0x%08X", (guid).d1, (guid).d2);
#define CopyCKString(storage, str) storage = (str) ? (str) : "";
#define CopyCKParamTypeStr(strl, ckpt, pm) if ((ckpt) != -1) (strl) = (pm)->ParameterTypeToName(ckpt);\
else (strl) = "!!UNKNOW TYPE!!";

namespace SSMaterializer {
	namespace Utils {

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);
		void StdstringGetBase64(std::string& strl, const char* data, size_t datalen);

	}
}
