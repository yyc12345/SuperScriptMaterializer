#include "string_helper.hpp"

namespace SSMaterializer {
	namespace Utils {

		void StdstringPrintf(std::string& strl, const char* format, ...) {
			va_list argptr;
			va_start(argptr, format);
			StdstringVPrintf(strl, format, argptr);
			va_end(argptr);
		}
		void StdstringVPrintf(std::string& strl, const char* format, va_list argptr) {
			int count = _vsnprintf(NULL, 0, format, argptr);
			count++;

			strl.resize(count);
			strl[count - 1] = '\0';
			int write_result = _vsnprintf((char*)strl.data(), count, format, argptr);

			if (write_result < 0 || write_result >= count) {
				//something goes wrong
				strl.clear();
			}
		}

		void CopyGUID(std::string& strl, CKGUID& guid) {
			StdstringPrintf(strl, "0x%08x, 0x%08x", guid.d1, guid.d2);
		}

	}
}
