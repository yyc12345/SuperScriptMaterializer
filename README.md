# Super Script Materializer

Advanced Virtools Script Materializer.

Export all script into a SQLite database file.

Some code come from [BearKidsTeam/Script-Materializer](https://github.com/BearKidsTeam/Script-Materializer) (this project export specific script into a JSON file)

WIP

## Install

Drag built dll into Virtools's InterfacePlugins folder.

At the same time, you should put `sqlite3.dll` into Virtools folder. If you use the dll which downloaded form this project's release page, the sqlite dll version you needed is `sqlite-dll-win32-x86-3310100`.

## Build

You should configure build parameter to choose your Virtools SDK and SQLite SDK path. (SQLite SDK can be downloaded from [sqlite.org](http://www.sqlite.org/) and you need use Visual C++ tools and execute `LIB /DEF:sqlite3.def /machine:IX86 ` to generate `sqlite3.lib`)

Select VT5 config\(because I will migrate this project into VT3.5 version in the feature\) and then build.

## Usage

Load or create a file containing script, click `Super Script Materializer` on main menu and click `Export all script`, select a good file name and wait until Virtools tell you all things have done.
