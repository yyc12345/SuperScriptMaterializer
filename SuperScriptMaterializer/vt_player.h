#if !defined(_YYCDLL_VT_PLAYER_H__IMPORTED_)
#define _YYCDLL_VT_PLAYER_H__IMPORTED_

#if defined(VIRTOOLS_STANDALONE)
#include "stdafx.h"
#include "database.h"

void PlayerMain(const char* virtools_composition, const char* script_db_path, const char* env_db_path);
void CommonAssert(BOOL condition, const char* desc);

#endif

#endif