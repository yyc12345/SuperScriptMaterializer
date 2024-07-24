MKDIR bin
CD bin

cmake -G "Visual Studio 16 2019" -A Win32 -DVSW_BUILD_MATERIALIZER=ON -DVSW_BUILD_DECORATOR=OFF -DMATERIALIZER_BUILD_TYPE=plugin -DVIRTOOLS_VERSION=50 -DVIRTOOLS_PATH="E:\Virtools\Virtools Dev 5.0" -DSQLITE_AMALGAMATION_PATH="D:\CppLib\SQLite\sqlite-amalgamation-3450300" -DSQLITE_DLL_PATH="D:\CppLib\SQLite\sqlite-dll-win-x86-3450300" -DYYCC_PATH="J:\YYCCommonplace\bin\install\Win32_Debug" ../..
cmake --build . --config Debug
cmake -G "Visual Studio 16 2019" -A Win32 -DVSW_BUILD_MATERIALIZER=OFF -DVSW_BUILD_DECORATOR=ON -DSQLITE_AMALGAMATION_PATH="D:\CppLib\SQLite\sqlite-amalgamation-3450300" -DSQLITE_DLL_PATH="D:\CppLib\SQLite\sqlite-dll-win-x86-3450300" -DYYCC_PATH="J:\YYCCommonplace\bin\install\Win32_Debug" ../..
cmake --build . --config Debug

CD ..
ECHO Build Done!
