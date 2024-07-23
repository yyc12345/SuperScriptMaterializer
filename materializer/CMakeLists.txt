# Check build environment
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/check_materializer_env.cmake)
# Import virtools sdk
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/custom_import_virtools.cmake)

# Create library or executable according to build type
if (VSW_MATERIALIZER_BUILD_TYPE STREQUAL "plugin")
    add_library(VSWMaterializer STATIC "")
else ()
    add_executable(VSWMaterializer "")
endif ()
# Setup static library sources
target_sources(VSWMaterializer
PRIVATE
    # Sources
    main.cpp
)
target_sources(VSWMaterializer
PUBLIC
FILE_SET HEADERS
FILES
    # Headers
    stdafx.hpp
    resources.h
)
# Setup header infomations
target_include_directories(VSWMaterializer
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
# Setup C++ standard
set_target_properties(VSWMaterializer
PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED 20
    CXX_EXTENSION OFF
)
# MSVC specific correction
target_compile_definitions(VSWMaterializer
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:UNICODE>
    $<$<CXX_COMPILER_ID:MSVC>:_UNICODE>
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
