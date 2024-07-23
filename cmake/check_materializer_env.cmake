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
# Check build type
set(VSW_MATERIALIZER_BUILD_TYPES
    "plugin" "standalone"
)
if (VSW_MATERIALIZER_BUILD_TYPE IN_LIST VSW_MATERIALIZER_BUILD_TYPES)
    message( "VirtoolsSchematicWeaver::Materializer build type: ${VSW_MATERIALIZER_BUILD_TYPE}")
else ()
    message(FATAL_ERROR 
        "VirtoolsSchematicWeaver::Materializer do not have correct type."
        "Please assign a legal value (\"plugin\" or \"standalone\") to VSW_MATERIALIZER_BUILD_TYPE."
    )
endif ()
