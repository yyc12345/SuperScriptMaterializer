#pragma once

#include "stdafx.h"
#include <string>

namespace SSMaterializer {
	namespace Utils {

		constexpr const char g_Unknow[] = "!!UNKNOW!!";

		void StdstringPrintf(std::string& strl, const char* format, ...);
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr);
		void StdstringGetBase64(std::string& strl, const char* data, size_t datalen);

		inline void CopyGuid(std::string& str, CKGUID& guid) {
			StdstringPrintf(str, "0x%08X, 0x%08X", guid.d1, guid.d2);
		}
		inline void CopyCKString(std::string& storage, const char* str) {
			storage = str == NULL ? g_Unknow : str;
		}
		inline void CopyCKParamTypeStr(std::string& strl, CKParameterType ckpt, CKParameterManager* pm) {
			if (ckpt != -1) strl = pm->ParameterTypeToName(ckpt);
			else strl = g_Unknow;
		}
		inline void CopyCKClassId(std::string& strl, CK_CLASSID clsid, CKParameterManager* pm) {
			CKSTRING ckstr = pm->ParameterGuidToName(pm->ClassIDToGuid(clsid));
			strl = ckstr == NULL ? g_Unknow : ckstr;
		}

	}
}
