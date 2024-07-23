cmake_minimum_required(VERSION 3.23)
project(VirtoolsSchematicWeaver
    VERSION 2.0.0
    LANGUAGES CXX
)

# Provide options
option(VSW_BUILD_MATERIALIZER "Build Materializer part of Virtools Schematic Weaver." ON)
option(VSW_BUILD_DECORATOR "Build Decorator part of Virtools Schematic Weaver" ON)

# Setup install path
include(GNUInstallDirs)
set(VSW_INSTALL_BIN_PATH ${CMAKE_INSTALL_BINDIR} CACHE PATH
"Binary install path relative to CMAKE_INSTALL_PREFIX unless set to an absolute path.")

# Import shared library and essential library
if (VSW_BUILD_MATERIALIZER OR VSW_BUILD_DECORATOR)
    # Import YYCC and sqlite because all project use them
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/custom_import_yycc.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/custom_import_sqlite.cmake)
    # Import shared library
    add_subdirectory(shared)
endif ()
# Import build targets
if (VSW_BUILD_MATERIALIZER)
    add_subdirectory(materializer)
endif ()
if (VSW_BUILD_DECORATOR)
    add_subdirectory(decorator)
endif ()
