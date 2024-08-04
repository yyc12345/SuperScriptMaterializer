#include "GenericHelper.hpp"
#include <cstdarg>

namespace VSW {

	Reporter::Reporter() {}
	Reporter::~Reporter() {}

	void Reporter::PrePrint(const YYCC::yycc_char8_t* strl) {}

#define GENERIC_REPORTER_WRITE(ty, data) YYCC::ConsoleHelper::Write(YYCC_U8("[" #ty "] ")); \
YYCC::ConsoleHelper::WriteLine(data);
#define GENERIC_REPORTER_FORMAT(ty, data) YYCC::ConsoleHelper::Write(YYCC_U8("[" #ty "] ")); \
va_list argptr; \
va_start(argptr, data); \
YYCC::ConsoleHelper::WriteLine(YYCC::StringHelper::VPrintf(data, argptr).c_str()); \
va_end(argptr);

	void Reporter::Err(const YYCC::yycc_char8_t* strl) {
		GENERIC_REPORTER_WRITE(Error, strl);
	}
	void Reporter::ErrF(const YYCC::yycc_char8_t* fmt, ...) {
		GENERIC_REPORTER_FORMAT(Error ,fmt);
	}
	void Reporter::Warn(const YYCC::yycc_char8_t* strl) {
		GENERIC_REPORTER_WRITE(Warning, strl);
	}
	void Reporter::WarnF(const YYCC::yycc_char8_t* fmt, ...) {
		GENERIC_REPORTER_FORMAT(Warning ,fmt);
	}
	void Reporter::Info(const YYCC::yycc_char8_t* strl) {
		GENERIC_REPORTER_WRITE(Info, strl);
	}
	void Reporter::InfoF(const YYCC::yycc_char8_t* fmt, ...) {
		GENERIC_REPORTER_FORMAT(Info ,fmt);
	}

#undef GENERIC_REPORTER_WRITE
#undef GENERIC_REPORTER_FORMAT

}
