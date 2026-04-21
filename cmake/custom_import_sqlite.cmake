if (WIN32)
    # In Windows, we use custom way to import SQLite.
    # Before using this CMake file in Windows, you should do following steps first.
    # 1. Get SQLite SDK (amalgamation) and binaries (sqlite-dll) from http://www.sqlite.org/
    # 2. Extract SQLite SDK and binaries to your preferred directory.
    # 3. Open Developer Command Prompt for Visual Studio (embedded with Visual Studio installation)
    # 4. Use `cd` command to switch work directory to the directory where you extract binaries.
    # 5. Execute `lib /DEF:sqlite3.def /OUT:sqlite3.lib /MACHINE:x86` (`x86` is the architecture of your SQLite3, you may change it according to your requirement).
    # 6. Now you have a generated LIB file for MSVC linking.

    # Check SQLite path variable
    if (NOT DEFINED SQLITE_AMALGAMATION_PATH)
        message(FATAL_ERROR "You must set SQLITE_AMALGAMATION_PATH to the directory where the SQLite3 header file locate.")
    endif()
    if (NOT DEFINED SQLITE_DLL_PATH)
        message(FATAL_ERROR "You must set SQLITE_DLL_PATH to the directory where the generated SQLite3 LIB file locate for linking.")
    endif()
    # Add imported library
    add_library(SQLite3 INTERFACE IMPORTED)
    # Add alias for it to let it has the same behavior with CMake imported SQLite3.
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
    # In non-Windows, we simply import SQLite3 from CMake preset.
    find_package(SQLite3 REQUIRED)
endif ()
