# Try to initialize build environment
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/init_materializer_env.cmake)

# Create library or executable according to build type
if (VSW_MATERIALIZER_BUILD_TYPE STREQUAL "plugin")
    add_library(VSWMaterializer STATIC "")
else ()
    add_executable(VSWMaterializer "")
endif ()
# Setup source files
target_sources(VSWMaterializer
PRIVATE
    # Sources
    main.cpp
)
# Setup header files
target_sources(VSWMaterializer
PRIVATE
FILE_SET HEADERS
FILES
    # Headers
    stdafx.hpp
    resource.h
)
# Setup header infomations
target_include_directories(VSWMaterializer
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    YYCCommonplace
    SQLite3::SQLite3
    VirtoolsSDK
    VSWShared
)
# Setup linked library
target_link_libraries(VSWMaterializer
PRIVATE
    YYCCommonplace
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
# MSVC specific correction
target_compile_definitions(VSWMaterializer
PRIVATE
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
