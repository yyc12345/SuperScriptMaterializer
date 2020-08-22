# Super Script Materializer

[English document](./README.md)

---

![preview_image](./preview.png)

---

超级Virtools脚本物化器（机翻（确信））

本项目分为2个部分，`SuperScriptMaterializer`是一个C++工程，将生成一个Virtools界面插件用于导出初步数据，`SuperScriptViewer`是一个Python工程，将解析导出的数据，然后使用Flask提供一个本地Web界面进行脚本查看。

将Virtools文档中的所有脚本导出成一个SQLite数据库文件，然后经过Python进行排布处理，最后提供一个本地Web前端查看脚本。这同样适用于`Script Hidden`的Virtools脚本，也适用于其中含有不可展开的`Behavior Graph`的脚本。

物化器不能完全恢复脚本的原有排布，无论原有排布是否存在，物化器都将重新自动生成脚本中的各个元素的位置。某些结构的关系可能会改变（例如Export parameter），亦或者是与Virtools中的呈现不同，但是逻辑思路将不会改变。同时物化器不能将已经生成的结构回写成Virtools可接受的格式，因此物化器只能提供无视脚本隐藏的分析功能。

本工程代码源于另两个工程：[BearKidsTeam/Script-Materializer](https://github.com/BearKidsTeam/Script-Materializer)（该工程用于将指定脚本导出为JSON文档）和[BearKidsTeam/VirtoolsScriptDeobfuscation](https://github.com/BearKidsTeam/VirtoolsScriptDeobfuscation)（该工程能够在Virtools 3.5中提供内置的隐藏脚本解析功能，将解析结果解析为可以被Virtools识别的格式）

**注意事项**

* 当前最新的commit并不一定可以稳定使用，请访问Release界面获取可以稳定使用的版本。
* 如果您更换了使用的版本（包括第一个稳定版本之前的版本），则需要重新构建所有数据，因为各个版本之间的数据可能彼此不兼容。

### 使用

  - Virtools Dev 5.0
  - 至少Python 3.0，且需要安装Flask库
  - 常见浏览器（除了Safari）

### 编译

为了编译或者调试这个项目，以下工具也是必须的。

  - Virtools Dev 5.0 SDK（在其他版本上或许也能工作，已测试Virtools 4可以无修改编译并运行）
  - Visual Studio 2015（至少VC++ v140的工具集）

## 使用方法

### 基本方法

将Virtools插件投入`InterfacePlugins`目录下，将`sqlite3.dll`和`Dev.exe`放在同一目录。如果你使用了Release页面的打包文件，则直接在Virtools根目录解压然后允许合并和覆盖即可。然后启动Virtools，打开需要解析的文档，点击菜单栏的`Super Script Materializer`-`Export all script`，保存为`export.db`，然后等待Virtools提示你已经导出完成。然后同样的操作执行`Super Script Materializer`-`Export environment`，并将其保存为`env.db`。

将`export.db`和`env.db`与`SuperScriptViewer.py`放在一起。然后在此目录中运行`python ./SuperScriptViewer.py`，等待Python交互界面提示可以打开本地的网页即可。

`SuperScriptViewer.py`具有一些命令行开关：

- `-i`：指定输入的`export.db`
- `-o`：指定输出的`decorated.db`，如果已经存在将不考虑输入，直接使用输出数据库呈现
- `-e`：指定输入的`env.db`，环境数据库
- `-c`：指定数据库编码，可用的编码表可以在[这里](https://docs.python.org/3/library/codecs.html#standard-encodings)查看
- `-f`：无参数，用于强制重新生成输出数据库，无论输出数据库是否存在
- `-d`：无参数，启用调试模式，直接抛出异常，而不是捕获后在控制台输出，方便调试

以上选项在基本使用中应该不会用到，因此按上述规则直接放置好文件直接运行即可。

一份指导你如何使用Viewer的文档已内置在Viewer中，可以从Help页面进行查看

### 使用注意

- 导出插件选择完文件后弹出错误，或者Python交互界面弹出错误，请附带您引起bug的文件以及错误窗口的内容提交bug
- 如果Python交互界面提示数据库`TEXT`类型解码失败，或者解析的字符出现乱码，那么可能您需要手动使用`-c`开关指定数据库文本解码方式。因为Virtools使用多字节编码，依赖于当前操作系统的代码页，`SuperScriptMaterializer`做了特殊获取以保证大多数计算机可以直接运行，但仍然不能排除一些特殊情况。需要注意的是，指定的编码不是你计算机当前的代码页，而是制作这个Virtools文档的作者的计算机的代码页。

## 编译

只有Virtools界面插件需要编译，其余均为解释性语言无需编译。

需要手动配置Virtools插件的编译参数，例如包含路径等，需要指向您自己的Virtools SDK。对于SQLite SDK，您可以从[sqlite.org](http://www.sqlite.org/)下载，然后使用Visual C++的工具集执行`LIB /DEF:sqlite3.def /machine:IX86`以获取可以用于编译的文件。

## 开发计划

在之后的版本中，以下功能将逐步加入：

* 当前页面搜索和全局搜索
* Shortcut追踪
* 在Viewer中移动Block和BB