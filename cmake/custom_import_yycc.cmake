# Check YYCC path variable
if (NOT DEFINED YYCC_PATH)
    message(FATAL_ERROR "You must set YYCC_PATH variable to one of CMake distribution of YYCC installation path.")
endif()

# Find YYCC library
find_package(YYCCommonplace REQUIRED
    HINTS ${YYCC_PATH} NO_DEFAULT_PATH
)
