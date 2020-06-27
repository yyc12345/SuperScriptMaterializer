# Super Script Materializer

[中文文档](./README_ZH.md)

The project consist of two parts:

  1. SuperScriptMaterializer - Custom Virtools Interface Plugin that allows you to export all scripts/schematics into a SQLite database file
  2. SuperScriptViewer - a python parser that creates a web page based on the exported databases to view the schematic in Behavior Graph mode similar to how Virtools Dev displays.

The purpose of the project is to allow users to have a backup graph of their scripts from Virtools while providing support for analyzing `--Script Hidden--` scripts by decoding the bytecode of `.NMS` virtools files from `.CMO` and `.VMO`. 

The inspiration came from:
  - [BearKidsTeam/Script-Materializer](https://github.com/BearKidsTeam/Script-Materializer) - Allows exporting specific scripts into a JSON file
  - [BearKidsTeam/VirtoolsScriptDeobfuscation](https://github.com/BearKidsTeam/VirtoolsScriptDeobfuscation) - A custom interface plugin that loads `.CMO` files with hidden scripts and makes them visible and editable.

**NOTE:** This project does not give ability to edit hidden scripts within the file. It simply exports and views.

## Requirements
  - Virtools Dev 3.5, 4.0, or 5.0 with SDK (May work for others)
  - Python 3.0 with Flask module
  - Visual Studio 2015 with at least v140 toolset

## Build

  1. Download repository.
  2. Get SQLite SDK (amalgamation) and binaries (sqlite-dll-win32-x86) from [sqlite.org](http://www.sqlite.org/)
  3. Extract both SQLite SDK and binaries to the same folder.
  4. Open Developer Command Prompt for VS 2015 by typing Developer Command in Windows Search
  5. Go to directory where you've extracted SDK and binaries and type `lib /DEF:sqlite3.def /OUT:sqlite3.lib /MACHINE:x86` which will build a lib file for SQLite.
  6. Open SuperScriptMaterializer solution.
  7. Edit SuperScriptMaterializer Properties page as necessary:
     - General -> Target Platform Version
     - General -> Output Directory
     - General -> Platform Toolset
     - C/C++ -> General -> Additional Include Directories (Add your SQLite path and Virtools Includes Path)
     - Linker -> General -> Output File
     - Linker -> General -> Additional Library Dependencies (Add your SQLite path and Virtools Lib/Win32/Release Path)
     - Linker -> Debugging -> Generate Program Database File
  8. Clean and Build as Release



## Usage

### Exporting Scripts from Virtools

  1. Make Sure Virtools Dev is closed.
  2. Copy compiled SuperScriptMaterializer.dll into Virtools's InterfacePlugins folder.
  3. Copy `sqlite3.dll` you downloaded into Virtool's base folder (where `devr.exe` exist)
  4. Open Virtools and load a `.CMO` of your choice.
  5. Go to the menu bar and click `Super Script Materializer` and `Export all script`. Name it `export.db` and save it into `SuperScriptViewer` folder
  6. Go to the menu bar and click `Super Script Materializer` and `Export environment`. Name it `env.db` and save it into `SuperScriptViewer` folder

### Viewing Exported Databases
  1. Run the viewer by `python3 SuperScriptViewer.py`
  2. It will generate a unique dectorated database (`decorated.db`) used for viewing based on `export.db` and `env.db`
  3. Once generated, a webpage will be hosted (default 127.0.0.1:5000) to view all graphs.

There is advanced command line switches if needed for the SuperScriptViewer
  - `-i filename.db` specify an input file to be used as `export.db`
  - `-e filename.db` specify an input file to be used as `env.db`
  - `-o filename.db` specify an output file to generate `decorated.db`. If it already exists, it will used that instead of generating a new one.
  - `-f` forces the output database to be regenerated (useful if you are unsure if decorated.db matches what export.db and env.db used)

## Troubleshooting

  - If `SuperScriptViewer.py` displays `TEXT` type decoding error, you will need to modify `CustomConfig.py` and edit the `database_encoding` to whatever encoding system your OS uses since Virtools uses multi-byte encoding and depends on your system's locale.
  - If Virtools closes upon exporting any databases, please create an issues page noting what Virtools version you have and a link to your file you used to export.
