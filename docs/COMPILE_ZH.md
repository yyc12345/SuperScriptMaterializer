# 编译

本文档将指导您编译或调试`SuperScriptMaterializer`以及对`SuperScriptViewer`的编译。

## SuperScriptMaterializer

### 需求

- Visual Studio 2010，即VC++ v100的工具集（不能使用Virtools 2015及以上版本进行编译，在编译Virtools 5.0 Standalone版本的时候亲测会出错且无法解决，但对于其他版本您可以尝试使用高版本的工具集进行编译）
- Virtools SDK 或 Ballance Mod Loader（如果以Virtools 2.1作为目标）
- SQlite SDK
- Python 3.x

### 编译之前

编译之前，对于SQLite SDK，您可以从[sqlite.org](http://www.sqlite.org/)下载，然后使用Visual C++的工具集执行`LIB /DEF:sqlite3.def /machine:IX86`以获取可以用于编译的文件。

然后您需要先进入项目根目录下，在此处打开终端，我们首先需要生成编译的参数。自上一个版本以来，我们将编译时需要的一些参数全部转为Visual Studio可识别的宏，定义在独立的`Virtools.props`中，此步操作便是生成这个文件，以确定编译文件的相关参数。

按如下格式运行指令：

```
python3 mk_materializer_cfg.py [plugin|standalone] [21|25|35|40|50] [virtools_root_path] [sqlite_header] [sqlite_lib] [bml path]
```

每一项参数的含义：

* `[plugin|standalone]`：编译为插件还是独立程序
* `[21|25|35|40|50]`：Virtools版本
* `[virtools_root_path]`：Virtools Dev的根目录，用于Virtools SDK的头文件，链接库的寻找，也决定程序编译后的输出位置
* `[sqlite_header]`：SQlite头文件路径，通常来说指向SQlite头文件所在的文件夹
* `[sqlite_lib]`：SQlite链接库的路径，通常来说是指向`sqlite3.lib`的路径
* `[bml path]`：BML的路径，只有在以Virtools 2.1为目标编译时才有用

以下是一些指令示例：

```
python3 .\mk_materializer_cfg.py plugin 50 "E:\Virtools\Virtools Dev 5.0" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py standalone 50 "E:\Virtools\Virtools Dev 5.0" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py plugin 40 "E:\Virtools\Virtools Dev 4.0" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py standalone 40 "E:\Virtools\Virtools Dev 4.0" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py plugin 35 "E:\Virtools\Virtools Dev 3.5" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py standalone 35 "E:\Virtools\Virtools Dev 3.5" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py standalone 25 "E:\Virtools\Virtools Dev 2.5" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
python3 .\mk_materializer_cfg.py standalone 21 "E:\Virtools\Virtools Dev 2.1" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"
```

### 编译

使用`Visual Studio`打开`SuperScriptMaterializer.sln`，选择`SuperScriptMaterializer`工程。然后选择编译模式（Debug或Release），然后编译即可，基本上只要配置无误，编译会很快完成

## SuperScriptViewer

### 需求

* Node.js
* gulp
* gulp插件：待补充

### 编译

`SuperScriptViewer`本身分为2个部分：由Python配合Flask所写的后端，以及前端。后端无需编译，而前端需要通过**gulp**进行操作，以减少文档本身的大小，进而减小传输开销。

编译的方法很简单，在`Viewer`目录下执行`gulp xxxx`即可让gulp自动完成相关操作。

