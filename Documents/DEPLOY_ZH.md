# 部署

本文档将指导您部署属于您的`SuperScriptMaterializer`。它可以让您摆脱Virtools的限制，在所有平台上都可以浏览到Virtools的脚本。您需要先了解README中标准的操作方式，然后，我们会根据部署的一些特殊性，对标准步骤进行修改。

## 导出脚本

首先我们需要使用`SuperScriptMaterializer`将我们需要查看的脚本导出。众所周知，Virtools各个版本之间的兼容性很差，为一个版本的所作的文档可能在另一个版本中就无法打开，或者是丢失相关的Building Block，因此，首先您需要根据您所需要查看的Virtools文件的对应版本来选取合适版本的插件，这样可以获得更好的导出效果，因为一旦文档中出现一些Building Block丢失之类的错误，那么Virtools会让一些本可读的字段变成完全不可读的Binary格式。这也是我们的`Materializer`提供如此多版本选项的原因，为的是让每一份可能的文档都可以被正确地加载。

此外，如果您遇到了NMS文件，那么您可能需要使用Virtools创作环境并配合插件模式的`Materializer`来导出，通过新建一个随意物件然后让脚本加载在其上，进而导出。您可能无法在NMS文件上使用独立模式的`Materializer`，因为它没有办法识别NMS文件。

当然，您如果遇到了很多文件的情况，您可以将需要的文件放在一起，然后编写一个Shell脚本，然后使用独立模式的`Materializer`来进行批量导出（如果可以的话）。这是一个针对Ballance游戏所有文件的脚本导出的Shell脚本和其生成器的示例，可供您参考。

// todo: 贴上链接

如果您有来自不同Virtools版本，您需要分别用对应的导出器进行导出，然后将他们整理归类以供下一级的输入。无需担心不同版本的Virtools导出会导致错误，`Decorator`会正确处理它们。

## 分析脚本

您需要准备好`SuperScriptDecorator`，和一份`import.txt`。正如标准操作方式中所言，您需要把需要综合的所有的导出数据库及各个文件的名字写入`import.txt`。我的操作是在上一步生成批处理的时候一并生成`import.txt`，因为它们之间联系很紧密。当然，您完全手写`import.txt`也是可以的。在完成之后，与标准操作方式内所述一致，直接运行即可。分析脚本的时间与您输入的脚本的数量和脚本的复杂度相关。

## 发布脚本

分析脚本完成后，我们就该将数据输入到`SuperScriptViewer`中了。与普通运行方式不同，我们需要以非调试模式启动Flask部署后端，并同时完成前端部署。您可以参阅[Flask部署文档](https://flask.palletsprojects.com/en/2.0.x/deploying/)来根据您的情况来进行选择。

我所作的Ballance脚本展示网页使用Gunicorn，部署指令是`gunicorn ServerCore:app`，其中唯一需要注意的是`ServerCore:app`这个程序入口点，在您选择不同于我的部署方式时，这个入口点将不会改变。与此同时我使用Nginx做了反向代理，具体配置可参考这里

// todo: 贴上链接
