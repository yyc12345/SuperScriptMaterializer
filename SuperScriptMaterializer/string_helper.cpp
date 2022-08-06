#include "string_helper.hpp"
#include <cstdint>

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

		// Reference: https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
		void StdstringGetBase64(std::string& strl, const char* data, size_t datalen) {
			static const char* base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			static const int mod_table[] = { 0, 2, 1 };

			// compute size
			size_t output_length = 4 * ((datalen + 2) / 3);
			strl.resize(output_length);

			// compute
			for (size_t i = 0, j = 0; i < datalen;) {

				uint32_t octet_a = i < datalen ? (unsigned char)data[i++] : 0;
				uint32_t octet_b = i < datalen ? (unsigned char)data[i++] : 0;
				uint32_t octet_c = i < datalen ? (unsigned char)data[i++] : 0;

				uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

				strl[j++] = base64_table[(triple >> 3 * 6) & 0x3F];
				strl[j++] = base64_table[(triple >> 2 * 6) & 0x3F];
				strl[j++] = base64_table[(triple >> 1 * 6) & 0x3F];
				strl[j++] = base64_table[(triple >> 0 * 6) & 0x3F];
			}

			// fill blank
			for (int i = 0; i < mod_table[datalen % 3]; i++)
				strl[output_length - 1 - i] = '=';

		}

	}
}
