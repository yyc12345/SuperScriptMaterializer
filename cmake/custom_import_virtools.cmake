# ========== Check Variables ==========
# Check Virtools version variable
set(SPECIAL_VIRTOOLS_VERSIONS
    "21"
)
set(LEGACY_VIRTOOLS_VERSIONS 
    "25"
    "30"
    "35"
    "40"
    "50"
)
if (VIRTOOLS_VERSION IN_LIST SPECIAL_VIRTOOLS_VERSIONS)
    message(
        "Specified Virtools version: ${VIRTOOLS_VERSION}."
        "Please note in this verions, your provided Virtools root path must be the root of GitHub repository: doyaGu/Virtools-SDK-2.1"
    )
elseif (VIRTOOLS_VERSION IN_LIST LEGACY_VIRTOOLS_VERSIONS)
    message("Specified Virtools version: ${VIRTOOLS_VERSION}.")
else ()
    message(FATAL_ERROR
        "You must set VIRTOOLS_VERSION variable to specify the version of your target Virtools before configurating this project."
        "The valid Virtools version are 21, 25, 30, 35, 40, 50."
    )
endif ()

# Check Virtools path variable
if (NOT DEFINED VIRTOOLS_PATH)
    message(FATAL_ERROR
        "You must set VIRTOOLS_PATH variable to specify the root folder of Virtools before configurating this project."
        "Please note this path is different when you picking Virtools 2.1 version."
    )
endif()

# ========== Import library ==========
