# 开发帮助

此文档将叙述一些开发SuperScriptMaterializer中需要被记忆的一些原则。

# 可能的连线

该程序中最重要的事情之一是生成链接，而最困难的是pLink。 下图显示了将在此程序中分析的所有pLink情况。

```
 +--------------+              +-----------------+
 | pLocal       +-------------->                 |
 +--------------+              |                 |
                               |  pIn (bb/oper)  |
 +--------------+              |                 |
 | pIn (bb)     +-------------->       /         |
 +--------------+              |                 |
                               |   pTarget (bb)  |
 +----------------+            |                 |
 | pOut (bb/oper) +------------>                 |
 +----------------+            +-----------------+




+------------------+           +--------------+
|                  +-----------> pOut (bb)    |
|                  |           +--------------+
| pOut (bb/oper)   |
|                  |           +--------------+
|                  +-----------> pLocal       |
+------------------+           +--------------+

```

如果链接的一个端点不在当前图形中，则此端点将创建为快捷方式。 但是，如果链接的一个端点是当前分析的Building Graph的pIO，并且与另一个端点具有相同的CK_ID，则应创建一个eLink。

# 数据库

SuperScriptMaterializer处理流程里会涉及很多不同的数据库文件，以下是这些数据库和其内表的格式。  
其中这些是Materializer直接输出的基于文档的数据库：

* doc.db
* env.db

这些数据库是上面数据库经过Decorator处理可以被Viewer接受的数据库：

* decorated.db
* sheet.db
* query.db
* composition.db

## doc.db 格式

doc.db导出的是当前Virtools文档内的脚本及其它重要数据。

表列表：

- script：文档里的每个脚本
- script_behavior：behavior本身，包括behavior graph和prototype behavior之类的
- script_bIn：behavior的行为输入
- script_bOut：behavior的行为输出
- script_eLink：export link
- script_pAttr：attribute类型的parameter
- script_pIn：behavior的parameter输入
- script_pLink：parameter link
- script_pLocal：local parameter
- script_pOper：parameter operator
- script_pOut：behavior的parameter输出
- script_pTarget：behavior的target输入，实际上是一个特殊的parameter输入
- msg：文档内定义的message标号与数值的对应
- array：文档里的每个CKDataArray
- array_header：CKDataArray的表头
- array_cell：CKDataArray的数据体
- data：文档中所有涉及导出的parameter参数属性，以类似字典的形式存储每个parameter参数的各个部分

### script

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|脚本依附对象的`CK_ID`|
|name|TEXT|脚本依附对象的名字|
|index|INTEGER|脚本在依附对象的排序序号|
|behavior|INTEGER|真正的脚本Behavior的`CK_ID`|

### script_behavior

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|name|TEXT|当前对象的名字|
|type|INTEGER|`CKBehavior::GetType`|
|proto_name|TEXT|原型名称，没有原型则留空。`CKBehavior::GetPrototypeName`|
|proto_guid|TEXT|原型的GUID，如果没有原型则填写`0,0`，格式为`guid1,guid2`，用逗号分隔Virtools的GUID的两部分|
|flags|INTEGER|`CKBehavior::GetFlags`|
|priority|INTEGER|`CKBehavior::GetPriority`|
|version|INTEGER|`CKBehavior::GetVersion`|
|pin_count|TEXT|Behavior的接口个数，由5部分组成，用`,`分隔，分别代表：pTarget, pIn, pOut, bIn, bOut|
|parent|INTEGER|当前对象父级Behavior的`CK_ID`，如是顶层Behavior则填写-1|

### script_bIn / script_bOut

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|index|INTEGER|bIn/bOut在所属Behavior的次序号|
|name|TEXT|bIn/bOut名称|
|parent|INTEGER|当前对象所归属的Behavior的`CK_ID`|

### script_bLink

|字段|类型|含义|
|:---|:---|:---|
|input|INTEGER|连线输入端的`CK_ID`，不能确认此`CK_ID`是`bOut`还是`bIn`，由于连线也可以是从`parent`的`bIn`连接到子元素的`bIn`|
|output|INTEGER|连线输出端的`CK_ID`，不能确认是`bOut`还是`bIn`，理由同上|
|delay|INTEGER|连线时延`CKBehaviorLink::GetActivationDelay()`|
|input_obj|INTEGER|输入端所属Behavior的`CK_ID`|
|input_type|INTEGER|指示输入端类型，0为bIn，1为bOut|
|input_index|INTEGER|指示连线位于输入端所在Behavior的第几个bIO口|
|output_obj|INTEGER|输出端所属Behavior的`CK_ID`|
|output_type|INTEGER|指示输出端类型，0为bIn，1为bOut|
|output_index|INTEGER|指示连线位于输出端所在Behavior的第几个bIO口|
|parent|INTEGER|当前对象所归属的Behavior的`CK_ID`|

### script_eLink

|字段|类型|含义|
|:---|:---|:---|
|export_obj|INTEGER|需要被导出的`pIn`或`pOut`的`CK_ID`|
|internal_obj|INTEGER|导出连线中`pIn`或`pOut`所属Behavior中层次较深者的`CK_ID`|
|is_in|INTEGER|指示连线是对`pIn`还是`pOut`进行导出，如果是`pIn`则设置为1，否则设置为0|
|index|INTEGER|导出连线`pIn`或`pOut`在所属Behavior中层次较深者中的参数序号|
|parent|INTEGER|导出连线中`pIn`或`pOut`所属Behavior中层次较浅者的`CK_ID`，同时也就是导出连线所归属的Behavior的`CK_ID`|

导出连线eLink是一种将父级的pIn或pOut链接至器内部Behavior的pIn与pOut的连线。pIn只能链接pIn，pOut只能链接pOut。导出连线一旦链接完成，则意味着两端的pIn或pOut将具有相同的`CK_ID`。

### script_pAttr

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|name|TEXT|pAttr名称|
|type|TEXT|Attribute的类型名|
|type_guid|TEXT|Attribute类型的GUID|

### script_pIn

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|index|INTEGER|pIn在所属Behavior中的序号|
|name|TEXT|名称|
|type|TEXT|类型名|
|type_guid|TEXT|类型GUID|
|parent|INTEGER|当前对象所归属的Behavior或Operator的`CK_ID`|
|direct_source|INTEGER|`CKParameterIn::GetDirectSource()`|
|shared_source|INTEGER|`CKParameterIn::GetSharedSource()`|

### script_pLink

|字段|类型|含义|
|:---|:---|:---|
|input|INTEGER|pLink起点parameter的`CK_ID`|
|output|INTEGER|pLink终点parameter的`CK_ID`|
|input_obj|INTEGER|起点parameter所属behavior或operator的`CK_ID`|
|input_type|INTEGER|起点来源类型|
|input_is_bb|INTEGER|指示起点parameter所属是Behavior还是Operator，如果是Behavior，则填写为1，否则为0|
|input_index|INTEGER|起点parameter所属Behavior的序号值|
|output_obj|INTEGER|终点parameter所属behavior或operator的`CK_ID`|
|output_type|INTEGER|终点来源类型|
|output_is_bb|INTEGER|指示终点parameter所属是Behavior还是Operator|
|output_index|INTEGER|终点parameter所属Behavior的序号值|
|parent|INTEGER|当前对象所归属的Behavior的`CK_ID`|

`input_type`与`output_type`使用下列数值：

* pIn：0
* pOut：1
* pLocal：2
* pTarget：3
* pAttr：4

当其值不是`pIn`或`pOut`中任何一个时，忽略对应输入或输出的`is_bb`与`index`值。

### script_pLocal

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|name|TEXT|名称|
|type|TEXT|类型名|
|type_guid|TEXT|类型GUID|
|is_setting|INTEGER|是否是所属Behavior的Setting，调用所属Behavior的`CKBehavior::IsLocalParameterSetting()`|
|parent|INTEGER|当前对象所归属的Behavior的`CK_ID`|

### script_pOper

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|op|TEXT|名称|
|op_guid|TEXT|Operator采用的转换函数的GUID|
|parent|INTEGER|当前对象所归属的Behavior的`CK_ID`|

### script_pOut

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|index|INTEGER|在所属Behavior中的序号|
|name|TEXT|名称|
|type|TEXT|类型名|
|type_guid|TEXT|类型GUID|
|parent|INTEGER|当前对象所归属的Behavior或Operator的`CK_ID`|

### script_pTarget

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|name|TEXT|名称|
|type|TEXT|类型名|
|type_guid|TEXT|类型GUID|
|parent|INTEGER|当前对象所归属的Behavior的`CK_ID`|
|direct_source|INTEGER|`CKParameterIn::GetDirectSource()`|
|shared_source|INTEGER|`CKParameterIn::GetSharedSource()`|

与`pIn`类似，仅缺少`index`描述，因为对于一个Behavior，pTarget至多只能有1个。

### msg

|字段|类型|含义|
|:---|:---|:---|
|index|INTEGER|message的标号|
|name|TEXT|message的名称|

### array

|字段|类型|含义|
|:---|:---|:---|
|thisobj|INTEGER|当前对象的`CK_ID`|
|name|TEXT|表的名称|
|rows|INTEGER|表的行数|
|columns|INTEGER|表的列数|

### array_header

|字段|类型|含义|
|:---|:---|:---|
|index|INTEGER|当前表头的是第几列|
|name|TEXT|名称|
|type|INTEGER|列类型，`CKDataArray::GetColumnType`|
|param_type|TEXT|列的类型|
|param_type_guid|TEXT|列的类型的GUID|
|parent|INTEGER|表头所属表的`CK_ID`|

当列类型为Parameter时，`param_type`和`param_type_guid`为其所属CKParameterType的类型文本和GUID，当列类型为Object时，`param_type`为其对应的Class id，`param_type_guid`无效。列类型为其它类型时，`param_type`和`param_type_guid`均无效。

### array_cell

|字段|类型|含义|
|:---|:---|:---|
|row|INTEGER|当前单元格所在行|
|column|INTEGER|当前单元格所在列|
|showcase|TEXT|单元格可视化文本|
|inner_param|INTEGER|单元格内部数据，为对应CKParameter或CKObject的`CK_ID`|
|parent|INTEGER|单元格所属表的`CK_ID`|

当且仅当对应列的类型为Parameter或Object时，`inner_param`才有效。

### data

|字段|类型|含义|
|:---|:---|:---|
|field|TEXT|键值对的键|
|data|TEXT|键值对的值|
|parent|INTEGER|键值对所描述对象的`CK_ID`|

## env.db 格式

env.db导出的是当前Virtools环境的数据，与文档无关，这部分如果是多个文件共用一个环境，那么只需要导出和在`Decorator`里综合一次即可

表列表：

- attr：attribute
- op：operator，parameter转换的定义
- param：parameter的定义
- plugin：当前环境插件
- variable：全局变量数据

### attr

|字段|类型|含义|
|:---|:---|:---|
|index|INTEGER|序号，总数由`CKAttributeManager::GetAttributeCount()`获取|
|name|TEXT|`CKAttributeManager::GetAttributeNameByType()`|
|category_index|INTEGER|`CKAttributeManager::GetAttributeCategoryIndex()`|
|category_name|TEXT|`CKAttributeManager::GetAttributeCategory()`|
|flags|INTEGER|`CKAttributeManager::GetAttributeFlags()`|
|param_index|INTEGER|`CKAttributeManager::GetAttributeParameterType()`|
|compatible_classid|INTEGER|`CKAttributeManager::GetAttributeCompatibleClassId()`|
|default_value|TEXT|`CKAttributeManager::GetAttributeDefaultValue()`|

### op

|字段|类型|含义|
|:---|:---|:---|
|funcptr|INTEGER|`CKOperationDesc::Fct`|
|in1_guid|TEXT|`CKOperationDesc::P1Guid`|
|in2_guid|TEXT|`CKOperationDesc::P2Guid`|
|out_guid|TEXT|`CKOperationDesc::ResGuid`|
|op_guid|TEXT|`CKParameterManager::OperationCodeToGuid()`|
|op_name|TEXT|`CKParameterManager::OperationCodeToName`|
|op_code|INTEGER|序号，总数由`CKParameterManager::GetParameterOperationCount()`获取|

### param

|字段|类型|含义|
|:---|:---|:---|
|index|INTEGER|`CKParameterTypeDesc::Index`|
|guid|TEXT|`CKParameterTypeDesc::Guid`|
|derived_from|TEXT|`CKParameterTypeDesc::DerivedFrom`|
|type_name|TEXT|`CKParameterTypeDesc::TypeName`|
|default_size|INTEGER|`CKParameterTypeDesc::DefaultSize`|
|func_CreateDefault|INTEGER|`CKParameterTypeDesc::CreateDefaultFunction`|
|func_Delete|INTEGER|`CKParameterTypeDesc::DeleteFunction`|
|func_SaveLoad|INTEGER|`CKParameterTypeDesc::SaveLoadFunction`|
|func_Check|INTEGER|`CKParameterTypeDesc::CheckFunction`|
|func_Copy|INTEGER|`CKParameterTypeDesc::CopyFunction`|
|func_String|INTEGER|`CKParameterTypeDesc::StringFunction`|
|func_UICreator|INTEGER|`CKParameterTypeDesc::UICreatorFunction`|
|creator_dll_index|INTEGER|`CKParameterTypeDesc::CreatorDll`的`CKPluginEntry::m_PluginDllIndex`|
|creator_plugin_index|INTEGER|`CKParameterTypeDesc::CreatorDll`的`CKPluginEntry::m_PositionInDll`|
|dw_param|INTEGER|`CKParameterTypeDesc::dwParam`|
|dw_flags|INTEGER|`CKParameterTypeDesc::dwFlags`|
|cid|INTEGER|`CKParameterTypeDesc::Cid`|
|saver_manager|TEXT|`CKParameterTypeDesc::Saver_Manager`|

### plugin

|字段|类型|含义|
|:---|:---|:---|
|dll_index|INTEGER|`CKPluginEntry::m_PluginDllIndex`|
|dll_name|TEXT|`CKPluginEntry::m_PluginInfo`的`CKPluginInfo::m_DllFileName`|
|plugin_index|INTEGER|`CKPluginEntry::m_PositionInDll`|
|active|INTEGER|`CKPluginEntry::m_Active`|
|guid|TEXT|`CKPluginEntry::m_PluginInfo`的`CKPluginInfo::m_GUID`|
|desc|TEXT|`CKPluginEntry::m_Description`|
|author|TEXT|`CKPluginEntry::m_Author`|
|summary|TEXT|`CKPluginEntry::m_Summary`|
|version|INTEGER|`CKPluginEntry::m_Version`|
|func_init|INTEGER|`CKPluginEntry::m_InitInstanceFct`|
|func_exit|INTEGER|`CKPluginEntry::m_ExitInstanceFct`|

### variable

|字段|类型|含义|
|:---|:---|:---|
|name|TEXT|`CKVariableManager::Iterator::GetName()`|
|description|TEXT|`CKVariableManager::Variable::GetDescription()`|
|flags|INTEGER|`CKVariableManager::Variable::GetFlags()`|
|type|INTEGER|`CKVariableManager::Variable::GetType()`|
|representation|TEXT|`CKVariableManager::Variable::GetRepresentation()`|
|data|TEXT|`CKVariableManager::Variable::GetStringValue()`|

## composition.db 格式

composition.db是`Decorator`输出的所有被综合的文档与接下来三个数据库分别的内部的编号的的对应表（为每一个被综合的数据库分配一个编号，但是因为有些部分文档共用一个Virtools环境等，就需要将这些编号转换成共用的）  
使用此表是为了保证网页URL基于编号的命名方式可以在三个表中正常通行，而不会出现在这个表中的编号在另一个表里指示的是另一个Virtools文件。  
因此在查询其他数据库前需要先读取此数据库获得对应各个数据库的内部编号，进而在剩下的数据库中以内部编号继续查询。

表只有一个：composition。

## decorated.db 格式

decorated.db是`Decorator`输出的脚本综合的数据库，包含了所有脚本的连线和图形位置。

表列表：

- block：behavior的图形结构
- cell：类似local parameter的结构
- graph：脚本schematic图形
- link：脚本内的所有连线
- param：脚本里的parameter的数据

### block

|字段|类型|含义|
|:---|:---|:---|
|parent_graph|INTEGER||
|thisobj|INTEGER||
|name|TEXT||
|assist_text|TEXT||
|pin-ptarget|TEXT||
|pin-pin|TEXT||
|pin-pout|TEXT||
|pin-bin|TEXT||
|pin-bout|TEXT||
|x|REAL||
|y|REAL||
|width|REAL||
|height|REAL||
|expandable|INTEGER||

### cell

|字段|类型|含义|
|:---|:---|:---|
|parent_graph|INTEGER||
|thisobj|INTEGER||
|name|TEXT||
|assist_text|TEXT||
|x|REAL||
|y|REAL||
|type|INTEGER||

### graph

|字段|类型|含义|
|:---|:---|:---|
|graph|INTEGER||
|graph_name|TEXT||
|width|INTEGER||
|height|INTEGER||
|index|INTEGER||
|parent|TEXT||

### param

param为原来的info

|字段|类型|含义|
|:---|:---|:---|
|target|INTEGER||
|attach_bb|INTEGER||
|is_setting|INTEGER||
|name|TEXT||
|field|TEXT||
|data|TEXT||

### link

|字段|类型|含义|
|:---|:---|:---|
|parent_graph|INTEGER||
|delay|INTEGER||
|start_interface|INTEGER||
|end_interface|INTEGER||
|startobj|INTEGER||
|endobj|INTEGER||
|start_type|INTEGER||
|end_type|INTEGER||
|start_index|INTEGER||
|end_index|INTEGER||
|x1|REAL||
|y1|REAL||
|x2|REAL||
|y2|REAL||

## sheet.db 格式

sheet.db是`Decorator`输出的文档数据综合的数据库，包含了所有脚本的连线和图形位置。

表列表：

- data：同下
- header：同下
- body：同下
- param：与data.db几乎一致

## query.db 格式

query.db是`Decorator`输出的数据查询综合的数据库，包含了所有脚本内的数据部分，还有Virtools环境的可查询数据。

表列表：
- msg：从data.db - msg综合
- attr：同下
- op：同下
- param：同下
- plugin：同下
- variable：从env.db综合
