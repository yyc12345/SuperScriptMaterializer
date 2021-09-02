#include "virtools_compatible.h"

#if defined(VIRTOOLS_50) && defined(VIRTOOLS_STANDALONE)

//#define stdin  (__acrt_iob_func(0))
//#define stdout (__acrt_iob_func(1))
//#define stderr (__acrt_iob_func(2))
//
//FILE _iob[] = { *stdin, *stdout, *stderr };
//extern "C" FILE * __cdecl __iob_func(void) { return _iob; }

#endif

//void __declspec(noreturn) FAKE_THROW() {
//	throw "Unimplemented function called.";
//}
