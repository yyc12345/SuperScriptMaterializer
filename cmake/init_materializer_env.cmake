# ========== Check Basic Environment ==========
# Check Windows platform
if (NOT WIN32)
    message(FATAL_ERROR "VirtoolsSchematicWeaver::Materializer only supports Windows platform.")
endif ()
# Check MSVC toolchain
if (NOT MSVC)
    message(FATAL_ERROR "VirtoolsSchematicWeaver::Materializer only supports MSVC build toolchain.")
endif ()
# Check architecture
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "VirtoolsSchematicWeaver::Materializer only supports 32-bit platform.")
endif ()

# ========== Check Materializer Variables ==========
# Check build type
set(MATERIALIZER_BUILD_TYPES
    "plugin" "standalone"
)
if (MATERIALIZER_BUILD_TYPE IN_LIST MATERIALIZER_BUILD_TYPES)
    message( "VirtoolsSchematicWeaver::Materializer build type: ${MATERIALIZER_BUILD_TYPE}")
else ()
    message(FATAL_ERROR 
        "VirtoolsSchematicWeaver::Materializer do not have correct type."
        "Please assign a legal value (\"plugin\" or \"standalone\") to MATERIALIZER_BUILD_TYPE."
    )
endif ()

# ========== Check Virtools Variables ==========
# Check Virtools version variable
set(VIRTOOLS_VERSIONS "21" "25" "30" "35" "40" "50")
if (VIRTOOLS_VERSION IN_LIST VIRTOOLS_VERSIONS)
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

# Check the combination between materializer build type and Virtools version
set(STANDALONE_MATERIALIZER_SUPPORTED_VT "21" "25" "30" "35" "40" "50")
set(PLUGIN_MATERIALIZER_SUPPORTED_VT "30" "35" "40" "50")
if (MATERIALIZER_BUILD_TYPE STREQUAL "plugin")
    if (NOT (VIRTOOLS_VERSION IN_LIST PLUGIN_MATERIALIZER_SUPPORTED_VT))
        message(FATAL_ERROR
            "The combination between build type ${MATERIALIZER_BUILD_TYPE} and Virtools version ${VIRTOOLS_VERSION} is not compatible."
            "Please view our README for a legal combination."
        )
    endif ()
else ()
    if (NOT (VIRTOOLS_VERSION IN_LIST STANDALONE_MATERIALIZER_SUPPORTED_VT))
        message(FATAL_ERROR
            "The combination between build type ${MATERIALIZER_BUILD_TYPE} and Virtools version ${VIRTOOLS_VERSION} is not compatible."
            "Please view our README for a legal combination."
        )
    endif ()
endif ()

# ========== Import library ==========

# Build essential variables
# Virtools header path
if (VIRTOOLS_VERSION STREQUAL "21")
    set(VIRTOOLS_HEADER_PATH ${VIRTOOLS_PATH}/Include) # 2.1
elseif (VIRTOOLS_VERSION STREQUAL "25")
    set(VIRTOOLS_HEADER_PATH ${VIRTOOLS_PATH}/Virtools_SDK/Includes) # 2.5
else ()
    set(VIRTOOLS_HEADER_PATH ${VIRTOOLS_PATH}/Sdk/Includes) # 3.0, 3.5, 4.0, 5.0
endif ()
# Virtools lib path
if (VIRTOOLS_VERSION STREQUAL "21")
    set(VIRTOOLS_LIB_PATH ${VIRTOOLS_PATH}/Lib) # 2.1
elseif (VIRTOOLS_VERSION STREQUAL "25")
    set(VIRTOOLS_LIB_PATH ${VIRTOOLS_PATH}/Virtools_SDK/Lib) # 2.5
elseif (VIRTOOLS_VERSION STREQUAL "30")
    set(VIRTOOLS_LIB_PATH ${VIRTOOLS_PATH}/Sdk/Lib) # 3.0
else ()
    set(VIRTOOLS_LIB_PATH ${VIRTOOLS_PATH}/Sdk/Lib/Win32/Release) # 3.5, 4.0, 5.0
endif ()

# Real importer
add_library(VirtoolsSDK INTERFACE IMPORTED)
# Setup header files
set_target_properties(VirtoolsSDK PROPERTIES
INTERFACE_INCLUDE_DIRECTORIES
    "${VIRTOOLS_HEADER_PATH}"
)
# Setup lib files
set(VirtoolsSDK_LIBS_LIST
    # Both standalone and plugin build type needed
    "${VIRTOOLS_LIB_PATH}/CK2.lib"
    "${VIRTOOLS_LIB_PATH}/VxMath.lib"
    # Pugin build type only libraries
    "$<$<STREQUAL:${MATERIALIZER_BUILD_TYPE},plugin>:${VIRTOOLS_LIB_PATH}/DllEditor.lib>"
    "$<$<STREQUAL:${MATERIALIZER_BUILD_TYPE},plugin>:${VIRTOOLS_LIB_PATH}/InterfaceControls.lib>"
    "$<$<STREQUAL:${MATERIALIZER_BUILD_TYPE},plugin>:${VIRTOOLS_LIB_PATH}/CKControls.lib>"
    # Virtools 5.0 special
    "$<$<STREQUAL:${VIRTOOLS_VERSION},50>:${VIRTOOLS_LIB_PATH}/CKKernelInit.lib>"
)
set_target_properties(VirtoolsSDK PROPERTIES
INTERFACE_LINK_LIBRARIES "${VirtoolsSDK_LIBS_LIST}"
)
# Setup compile macros
target_compile_definitions(VirtoolsSDK
INTERFACE
    # Virtools version macro
    "VIRTOOLS_${VIRTOOLS_VERSION}"
    # Virtools 5.0 standalone mode need an extra macro
    "$<$<AND:$<STREQUAL:${MATERIALIZER_BUILD_TYPE},standalone>,$<STREQUAL:${VIRTOOLS_VERSION},50>>:VIRTOOLS_USER_SDK>"
)
# Setup compiler options
target_compile_options(VirtoolsSDK
INTERFACE
    # Permissive mode ordered.
    "/permissive"
)

