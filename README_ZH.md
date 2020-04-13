# Super Script Materializer

[English document](./README.md)

超级Virtools脚本物化器（机翻（确信））

将Virtools文档中的所有脚本导出成一个SQLite数据库文件，然后经过Python进行排布处理，最后提供一个本地Web前端查看脚本。这同样适用于`Script Hidden`的Virtools脚本，也适用于其中含有不可展开的`Behavior Graph`的脚本。

物化器不能完全恢复脚本的原有排布，无论原有排布是否存在，物化器都将重新自动生成脚本中的各个元素的位置。某些结构的关系可能会改变（例如Export parameter）但是逻辑思路将不会改变。同时物化器不能将已经生成的结构回写成Virtools可接受的格式，因此物化器只能提供无视脚本隐藏的分析功能。

本工程代码源于另两个工程：[BearKidsTeam/Script-Materializer](https://github.com/BearKidsTeam/Script-Materializer)（该工程用于将指定脚本导出为JSON文档）和[BearKidsTeam/VirtoolsScriptDeobfuscation](https://github.com/BearKidsTeam/VirtoolsScriptDeobfuscation)（该工程能够在Virtools 3.5中提供内置的隐藏脚本解析功能，将解析结果解析为可以被Virtools识别的格式）

本项目分为2个部分，`SuperScriptMaterializer`是一个C++工程，将生成一个Virtools界面插件用于导出初步数据，`SuperScriptViewer`是一个Python工程，将解析导出的数据，然后使用Flask提供一个本地Web界面进行脚本查看。

此项目仍在开发。

## 使用方法

### 基本方法

将Virtools插件投入`InterfacePlugins`目录下，将`sqlite3.dll`和`Dev.exe`放在同一目录。然后启动Virtools，打开需要解析的文档，点击菜单栏的`Super Script Materializer`-`Export all script`，选择要保存到的文件，然后等待Virtools提示你已经导出完成。

将导出文件重命名为`export.db`并和`SuperScriptViewer.py`放在一起。然后在此目录中运行`python ./SuperScriptViewer.py`，等待Python交互界面提示可以打开本地的网页即可。

### 使用注意

- 您需要先安装Virtools, Python和任意一种浏览器才能使用本工程。Python需要先安装`Flask`库。
- 导出插件目前只支持Virtools 5，后期会支持Virtools 3.5
- 导出插件选择完文件后卡住，或者Python交互界面弹出错误堆栈，这可能是设计失误，请附带您引起bug的文件提交bug
- 如果Python交互界面提示数据库`TEXT`类型解码失败，那么可能您需要手动在`DecoratorCore.py`中指定数据库文本解码方式。因为Virtools使用多字节编码，依赖于当前操作系统的代码页，`DecoratorCore.py`做了特殊获取以保证大多数计算机可以直接运行，但仍然不能排除一些特殊情况。
- 如果您使用本工程的Release页面中提供的已编译好的Virtools界面插件（现在因为仍然在开发所以没有），您需要放入的`sqlite3.dll`版本应为`sqlite-dll-win32-x86-3310100`

## 编译

只有Virtools界面插件需要编译，其余均为解释性语言无需编译。

需要手动配置Virtools插件的编译参数，例如包含路径等，需要指向您自己的Virtools SDK。对于SQLite SDK，您可以从[sqlite.org](http://www.sqlite.org/)下载，然后使用Visual C++的工具集执行`LIB /DEF:sqlite3.def /machine:IX86`以获取可以用于编译的文件。