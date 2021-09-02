# Super Script Materializer

[English document](./README.md)

---

![preview_image](./preview.png)

---

## 简介

超级Virtools脚本物化器（机翻（确信））

本项目分为4个部分：

* `SuperScriptMaterializer`：一个C++工程，将生成一个Virtools界面插件或独立播放器用于导出初步数据。
* `SuperScriptDecorator`：一个Python工程，将解析导出的数据，并将其组合成易于浏览的格式。
* `SuperScriptViewer`：一个Python工程，使用Flask提供一个本地Web界面进行脚本以供快速查看，通常是用于本地快速查看解析后脚本的数据。
* `SuperScriptEnterprise`：一个PHP工程，相对于`SuperScriptViewer`更适合部署于服务器上进行数据的展示和浏览。

整个工程所作的事情就是，将Virtools文档中的所有脚本导出成一个SQLite数据库文件，然后经过Python进行排布处理，最后提供一个本地Web前端查看脚本。这同样适用于`Script Hidden`的Virtools脚本，也适用于其中含有不可展开的`Behavior Graph`的脚本。

物化器不能完全恢复脚本的原有排布，无论原有排布是否存在，物化器都将重新自动生成脚本中的各个元素的位置。某些结构的关系可能会改变（例如Export parameter），亦或者是与Virtools中的呈现不同，但是逻辑思路将不会改变。同时物化器不能将已经生成的结构回写成Virtools可接受的格式，因此物化器只能提供无视脚本隐藏的分析功能。

**注意事项**

* 当前最新的commit并不一定可以稳定使用，请访问Release界面获取可以稳定使用的版本。
* 如果您更换了使用的版本（包括第一个稳定版本之前的版本），则需要重新构建所有数据，因为各个版本之间的数据可能彼此不兼容。

## SuperScriptMaterializer

SuperScriptMaterializer分为两种类型，一种是*插件*模式，在Virtools内部，作为界面插件，由人手动点击菜单进行导出，一般用于本地快速查看。另一种是*独立*模式，使用独立的Virtools编译器加载文件并导出，通过命令行参数的方式获得需要解析的文件和解析后数据库的路径，一般用于编写Shell脚本批量进行脚本导出。目前，我们支持的Virtools版本和模式如下：

|已知的Virtools版本|插件模式|独立模式|
|:---|:---|:---|
|Virtools 2.1|× (0)|√ (1)|
|Virtools 2.5|× (2)|√|
|Virtools 3.0|× (3)|× (3)|
|Virtools 3.5|√ (4)|√ (4)|
|Virtools 4.0|√ (4)|√ (4)|
|Virtools 5.0|√|√|

0. 没有可用的Virtools Dev 2.1，因此没有插件模式  
0. 使用Ballance Mod Loader提供的逆向Virtools SDK进行编译  
0. Virtools SDK不支持在界面上添加菜单  
0. 缺少Virtools SDK，无法编译  
0. 正在编写中  

### 使用

无论您现在要使用何种版本，您都必须先下载一份可用的，匹配的`sqlite.dll`。通常来说，我们在每一次版本发布的时候都会附带对应的`sqlite.dll`，您只需要下载即可。

如果您将Virtools 2.1作为目标，则由于其只有独立模式，您需要准备一份Virtools 2.1的运行环境（通常而言是`CK2.dll`，`VxMath.dll`等一系列文件）。由于Virtools 2.1并没有创作环境，因此您只能从其他分发版本中获取到这个运行环境，通常来说，是一种游戏，在您的游戏文件夹内搜索相关dll即可找到，将下载好的独立模式的exe和`sqlite.dll`一起放在那里即可。

如果您选择其他Virtools版本，那么很幸运，它们的创作环境还能被下载到。因此，如果你选择独立模式，可以将下载到的exe和`sqlite.dll`一起放在Virtools创作环境的根目录下（通常有一个名为`Dev.exe`的可执行程序在那里）。当然，如果你是要解析某个游戏的内容，则可以和游戏所使用的Virtools环境放在一起。如果你选择插件模式，那么请仍然将`sqlite.dll`放在Virtools创作环境的根目录下，但是将下载到的dll插件文件放在名为`InterfacePlugins`的目录下。

#### 插件版本

启动Virtools，打开需要解析的文档，点击菜单栏的`Super Script Materializer`-`Export all script`，保存为`export.db`，然后等待Virtools提示你已经导出完成。这一步将导出所有脚本。然后同样的操作执行`Super Script Materializer`-`Export environment`，并将其保存为`env.db`。这一步将保存所有环境变量。环境变量很重要，尤其是分析脚本中的一些数据的时候。

#### 独立版本

在独立版本所在文件夹内启动命令行，按以下格式输入命令：`SuperScriptMaterializer.exe [virtools composition] [script db path] [env db path]`

* `virtools composition`：要导出的Virtools文档的地址
* `script db path`：导出所有脚本所用数据库的地址
* `env db path`：保存所有环境变量所用数据库的地址

## SuperScriptDecorator


### 使用

将上一步得到的`export.db`和`env.db`与`SuperScriptDecorator.py`放在一起。然后在此目录中运行`python3 ./SuperScriptDecorator.py`，等待Python交互界面提示可以打开本地的网页即可。

`SuperScriptDecorator.py`具有一些命令行开关：

- `-i`：指定输入的`export.db`
- `-o`：指定输出的`decorated.db`，如果已经存在将不考虑输入，直接使用输出数据库呈现
- `-e`：指定输入的`env.db`，环境数据库
- `-c`：指定数据库编码，可用的编码表可以在[这里](https://docs.python.org/3/library/codecs.html#standard-encodings)查看
- `-f`：无参数，用于强制重新生成输出数据库，无论输出数据库是否存在
- `-d`：无参数，启用调试模式，直接抛出异常，而不是捕获后在控制台输出，方便调试

### 提示

以上选项在基本使用中应该不会用到，因此按上述规则直接放置好文件直接运行即可。

如果Python交互界面提示数据库`TEXT`类型解码失败，或者解析的字符出现乱码，那么可能您需要手动使用`-c`开关指定数据库文本解码方式。因为Virtools使用多字节编码，依赖于当前操作系统的代码页，`SuperScriptDecorator`做了特殊获取以保证大多数计算机可以直接运行，但仍然不能排除一些特殊情况。需要注意的是，指定的编码不是你计算机当前的代码页，而是制作这个Virtools文档的作者的计算机的代码页。

## SuperScriptViewer

一份指导你如何使用Viewer的文档已内置在Viewer中，可以从Help页面进行查看

## 问题反馈

以下情况可能是程序陷入了错误，请附带您引起bug的文件（如果可以）以及错误窗口的内容提交bug

* `SuperScriptMaterializer`插件模式在选择完文件后弹出错误或长时间没有反应
* `SuperScriptMaterializer`独立模式输出了`[ERROR]`等类似内容
* Python交互界面弹出错误

## 常见场景

// todo

## 部署

参见[部署](./Documents/DEPLOY_ZH.md)

## 编译

参见[编译](./Documents/COMPILE_ZH.md)

<!-- 
只有Virtools界面插件需要编译，其余均为解释性语言无需编译。

需要手动配置Virtools插件的编译参数，例如包含路径等，需要指向您自己的Virtools SDK。对于SQLite SDK，您可以从[sqlite.org](http://www.sqlite.org/)下载，然后使用Visual C++的工具集执行`LIB /DEF:sqlite3.def /machine:IX86`以获取可以用于编译的文件。
-->

## 开发计划

在之后的版本中，以下功能将逐步加入：

* 当前页面搜索和全局搜索
* Shortcut追踪
* 在Viewer中移动Block和BB

## 特别感谢

* Dassault System：提供了Virtools 2.5及其可用的SDK
* [BearKidsTeam/Script-Materializer](https://github.com/BearKidsTeam/Script-Materializer)：该工程用于将指定脚本导出为JSON文档
* [BearKidsTeam/VirtoolsScriptDeobfuscation](https://github.com/BearKidsTeam/VirtoolsScriptDeobfuscation)：该工程能够在Virtools 3.5中提供内置的隐藏脚本解析功能，将解析结果解析为可以被Virtools识别的格式
* [Gamepiaynmo/BallanceModLoader](https://github.com/Gamepiaynmo/BallanceModLoader)：该工程提供了可以用于Virtools 2.1目标编译的，由逆向得到的Virtools SDK
* [phaicm](https://github.com/phaicm): 提供了许多测试文件用于修复程序Bug。以及说明文档翻译上的帮助。
