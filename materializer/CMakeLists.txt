# Try to initialize build environment
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/init_materializer_env.cmake)

# Create library or executable according to build type
if (MATERIALIZER_BUILD_TYPE STREQUAL "plugin")
    add_library(VSWMaterializer SHARED "")
else ()
    add_executable(VSWMaterializer "")
endif ()
# Setup source files
target_sources(VSWMaterializer
PRIVATE
    # Sources
    # Main
    main.cpp
    PluginMain.cpp
    StandaloneMain.cpp
    # Exporter
    ExportContext.cpp
    ExportEnvironment.cpp
    ExportScript.cpp
    # Utilities
    Utilities.cpp
    Database.cpp
    # Defination file
    "$<$<STREQUAL:${MATERIALIZER_BUILD_TYPE},plugin>:Materializer.def>"
)
# Setup header files
target_sources(VSWMaterializer
PRIVATE
FILE_SET HEADERS
FILES
    # Headers
    # Misc
    stdafx.hpp
    resource.h
    # Main
    PluginMain.hpp
    StandaloneMain.hpp
    # Exporter
    ExportContext.hpp
    ExportEnvironment.hpp
    ExportScript.hpp
    # Utilities
    Utilities.hpp
    Database.hpp
    DataTypes.hpp
)
# Setup header infomations
target_include_directories(VSWMaterializer
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    YYCC::YYCCommonplace
    SQLite3::SQLite3
    VirtoolsSDK
    VSWShared
)
# Setup linked library
target_link_libraries(VSWMaterializer
PRIVATE
    YYCC::YYCCommonplace
    SQLite3::SQLite3
    VirtoolsSDK
    VSWShared
)
# Setup C++ standard (we require C++ 17 because Virtools can hold more higher C++ standard)
set_target_properties(VSWMaterializer
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED 17
    CXX_EXTENSION OFF
)
# Setup MFC use
set_target_properties(VSWMaterializer
PROPERTIES
    CMAKE_MFC_FLAG 2
)
# MSVC specific correction
target_compile_definitions(VSWMaterializer
PRIVATE
    # MFC macro
    _AFXDLL
    # Materializer build type
    "MATERIALIZER_$<UPPER_CASE:${MATERIALIZER_BUILD_TYPE}>"
    # Materializer debug or release
    "$<$<CONFIG:Debug>:MATERIALIZER_DEBUG>"
    "$<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:MATERIALIZER_DEBUG>"
    # Multibyte encoding
    $<$<CXX_COMPILER_ID:MSVC>:MBCS>
    $<$<CXX_COMPILER_ID:MSVC>:_MBCS>
)
target_compile_options(VSWMaterializer
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Install binary and headers
install(TARGETS VSWMaterializer
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${VSW_INSTALL_BIN_PATH}
)
