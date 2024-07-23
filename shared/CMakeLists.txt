# Create static library
add_library(VSWShared STATIC "")
# Setup source files
target_sources(VSWShared
PRIVATE
    # Sources
)
# Setup header files
target_sources(VSWShared
PUBLIC
FILE_SET HEADERS
FILES
    # Headers
    DataTypes.hpp
)
# Setup include directory
target_include_directories(VSWShared
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
# Setup C++ standard
set_target_properties(VSWShared
PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED 20
    CXX_EXTENSION OFF
)
# MSVC specific correction
target_compile_definitions(VSWShared
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:UNICODE>
    $<$<CXX_COMPILER_ID:MSVC>:_UNICODE>
)
target_compile_options(VSWShared
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)
