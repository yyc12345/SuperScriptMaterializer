# 编译与调试

本文档将指导您编译或调试`SuperScriptMaterializer`

## 需求

- Visual Studio 2010，即VC++ v100的工具集（不能使用Virtools 2015及以上版本进行编译，在编译Virtools 5.0 Standalone版本的时候亲测会出错且无法解决）
- Virtools SDK 或 Ballance Mod Loader（如果以Virtools 2.1作为目标）
- Python 3.x

## 编译之前

编译之前，您需要先进入项目根目录下，在此处打开终端，我们首先需要生成编译的参数。自上一个版本以来，我们将编译时需要的一些参数全部转为Visual Studio可识别的宏，定义在独立的`Virtools.props`中，此步操作便是生成这个文件，以确定编译文件的相关参数。

按如下格式运行指令

```

```

