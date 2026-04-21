
# Check YYCC path environment variable
if (NOT DEFINED YYCC_PATH)
    message(FATAL_ERROR "You must set YYCC_PATH variable to one of YYCC CMake distribution installation path.")
endif()

# Find YYCC library
# It will produce YYCC::YYCCommonplace target for including and linking.
# 
# Please note we MUST set CMake variable YYCCommonplace_ROOT to make sure CMake can found YYCC in out given path.
# The cache status of YYCCommonplace_ROOT is doesn't matter.
# CMake will throw error if we use HINTS feature in find_package to find YYCC.
set(YYCCommonplace_ROOT ${YYCC_PATH} CACHE PATH
"The path to YYCC CMake distribution installation path.")
find_package(YYCCommonplace REQUIRED)
