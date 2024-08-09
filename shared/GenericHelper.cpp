#include "GenericHelper.hpp"
#include <cstdarg>

namespace VSW {

	Reporter::Reporter() {}
	Reporter::~Reporter() {}

	void Reporter::PrePrint(const YYCC::yycc_char8_t* strl) const {}

#define GENERIC_REPORTER_WRITE(ty, data) YYCC::yycc_u8string cache(YYCC_U8("[" #ty "] ")); \
cache += data; \
this->PrePrint(cache.c_str()); \
YYCC::ConsoleHelper::WriteLine(cache.c_str());
#define GENERIC_REPORTER_FORMAT(ty, data) va_list argptr; \
va_start(argptr, data); \
YYCC::yycc_u8string cache(YYCC::StringHelper::VPrintf(data, argptr)); \
cache.insert(0u, YYCC_U8("[" #ty "] ")); \
this->PrePrint(cache.c_str()); \
YYCC::ConsoleHelper::WriteLine(cache.c_str()); \
va_end(argptr);

	void Reporter::Err(const YYCC::yycc_char8_t* strl) const {
		GENERIC_REPORTER_WRITE(Error, strl);
	}
	void Reporter::ErrF(const YYCC::yycc_char8_t* fmt, ...) const {
		GENERIC_REPORTER_FORMAT(Error ,fmt);
	}
	void Reporter::Warn(const YYCC::yycc_char8_t* strl) const {
		GENERIC_REPORTER_WRITE(Warning, strl);
	}
	void Reporter::WarnF(const YYCC::yycc_char8_t* fmt, ...) const {
		GENERIC_REPORTER_FORMAT(Warning ,fmt);
	}
	void Reporter::Info(const YYCC::yycc_char8_t* strl) const {
		GENERIC_REPORTER_WRITE(Info, strl);
	}
	void Reporter::InfoF(const YYCC::yycc_char8_t* fmt, ...) const {
		GENERIC_REPORTER_FORMAT(Info ,fmt);
	}

#undef GENERIC_REPORTER_WRITE
#undef GENERIC_REPORTER_FORMAT

}
