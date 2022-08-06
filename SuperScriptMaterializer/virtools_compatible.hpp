#pragma once

#include "stdafx.h"

//void __declspec(noreturn) FAKE_THROW();

#if defined(VIRTOOLS_21)
#define UNIVERSAL_VAR_TYPE void*
#elif defined(VIRTOOLS_25) || defined(VIRTOOLS_35) || defined(VIRTOOLS_40) || defined(VIRTOOLS_50)
#define UNIVERSAL_VAR_TYPE CKVariableManager::Variable::Type
#endif
