#pragma once

#if defined(VIRTOOLS_STANDALONE)
#include "stdafx.h"
#include "database.hpp"

void PlayerMain(const char* virtools_composition, const char* script_db_path, const char* env_db_path);
void CommonAssert(BOOL condition, const char* desc);

#endif
