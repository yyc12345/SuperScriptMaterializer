# Create executable
add_executable(VSWDecorator "")
# Setup source files
target_sources(VSWDecorator
PRIVATE
    # Sources
    main.cpp
)
# Setup header files
target_sources(VSWDecorator
PRIVATE
FILE_SET HEADERS
FILES
    # Headers
)
# Setup header infomations
target_include_directories(VSWDecorator
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    YYCC::YYCCommonplace
    SQLite3::SQLite3
    VSWShared
)
# Setup linked library
target_link_libraries(VSWDecorator
PRIVATE
    YYCC::YYCCommonplace
    SQLite3::SQLite3
    VSWShared
)
# Setup C++ standard
set_target_properties(VSWDecorator
PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED 17
    CXX_EXTENSION OFF
)
# MSVC specific correction
target_compile_definitions(VSWDecorator
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:UNICODE>
    $<$<CXX_COMPILER_ID:MSVC>:_UNICODE>
)
# Order build as UTF-8 in MSVC
target_compile_options(VSWDecorator
PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

# Install binary
install(TARGETS VSWDecorator
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${VSW_INSTALL_BIN_PATH}
)
