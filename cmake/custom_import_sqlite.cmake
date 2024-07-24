if (WIN32)
    # In Windows, we use custom way to import SQLite

    # Check variables# Check YYCC path variable
    if (NOT DEFINED SQLITE_AMALGAMATION_PATH)
        message(FATAL_ERROR "You must set SQLITE_AMALGAMATION_PATH variable to the header of SQLite library.")
    endif()
    if (NOT DEFINED SQLITE_DLL_PATH)
        message(FATAL_ERROR "You must set SQLITE_DLL_PATH variable the directory containing generated linking LIB file.")
    endif()
    # Add imported library
    add_library(SQLite3 INTERFACE IMPORTED)
    add_library(SQLite3::SQLite3 ALIAS SQLite3)
    # Setup header files
    set_target_properties(SQLite3 PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES
        "${SQLITE_AMALGAMATION_PATH}"
    )
    # Setup lib files
    set_target_properties(SQLite3 PROPERTIES
    INTERFACE_LINK_LIBRARIES
        "${SQLITE_DLL_PATH}/sqlite3.lib"
    )
else ()
    # In non-Windows, we import SQLite from CMake
    find_package(SQLite3 REQUIRED)
endif ()
