import xml.dom.minidom as minidom
import xml.dom
import os
import sys

# ======================== const define

build_type_standalone = "standalone"
build_type_plugin = "plugin"
valid_build_type = (
    build_type_standalone,
    build_type_plugin
)

virtools_attached_lib_plugin_dict = {
    "21": "",
    "25": "",
    "35": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib",
    "40": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib",
    "50": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib" 
}
virtools_attached_lib_standalone_dict = {
    "21": "VxMath.lib;CK2.lib",
    "25": "VxMath.lib;CK2.lib",
    "35": "vxmath.lib;ck2.lib",
    "40": "vxmath.lib;ck2.lib",
    "50": "vxmath.lib;ck2.lib"
}

virtools_std_macro_plugin_dict = {
    "21": "",
    "25": "",
    "35": "_CRT_SECURE_NO_WARNINGS",
    "40": "_CRT_SECURE_NO_WARNINGS",
    "50": "" 
}
virtools_std_macro_standalone_dict = {
    "21": "_DEBUG",
    "25": "_CRT_SECURE_NO_WARNINGS;_DEBUG",
    "35": "_CRT_SECURE_NO_WARNINGS;_DEBUG",
    "40": "_CRT_SECURE_NO_WARNINGS;_DEBUG",
    "50": "VIRTOOLS_USER_SDK;_DEBUG" 
}

executable_virtools = {
    "21": "Dev.exe",
    "25": "Dev.exe",
    "35": "devr.exe",
    "40": "devr.exe",
    "50": "devr.exe" 
}

valid_virtools_standalone_ver = (
    "21",
    "25",
    "35",
    "40",
    "50"
)
valid_virtools_plugin_ver = (
    "35",
    "40",
    "50"
)

valid_vt21_reverse_work_type = (
    'gamepiaynmo',
    'doyagu'
)

# ======================== assist func
def write_macro(dom, node_property_group, node_item_group, macro_upcase, data):
    node = dom.createElement(macro_upcase)
    if data != '':
        node.appendChild(dom.createTextNode(data))
    node_property_group.appendChild(node)

    node = dom.createElement("BuildMacro")
    node.setAttribute('Include', macro_upcase)
    inter_node = dom.createElement('Value')
    inter_node.appendChild(dom.createTextNode("$({})".format(macro_upcase)))
    node.appendChild(inter_node)
    node_item_group.appendChild(node)

def write_cl(dom, node_include, node_compile, filename):
    if filename.endswith('.h'):
        cl = dom.createElement('ClInclude')
        node = node_include
    elif filename.endswith('.cpp'):
        cl = dom.createElement('ClCompile')
        node = node_compile
    else:
        return
    cl.setAttribute('Include', filename)
    node.appendChild(cl)

def get_executable_virtools(vt_ver):
    if vt_ver == '21':
        return 'Dev.exe'
    elif vt_ver == '25':
        return 'Dev.exe'
    elif vt_ver == '35':
        return 'devr.exe'
    elif vt_ver == '40':
        return 'devr.exe'
    elif vt_ver == '50':
        return 'devr.exe'

# ======================== requirement get

# get basic cfg, such as build type, and vt version
while True:
    input_build_type = input('Choose build type(plugin, standalone): ')
    if input_build_type not in valid_build_type:
        print("Invalid build type!")
    else:
        break

valid_vtver_for_this_type = valid_virtools_plugin_ver if input_build_type == build_type_plugin else valid_virtools_standalone_ver
while True:
    input_virtools_version = input('Choose virtools version({}): '.format(', '.join(valid_vtver_for_this_type)))
    if input_virtools_version not in valid_vtver_for_this_type:
        print("Invalid virtools version!")
    else:
        break

# collect sqlite library data
while True:
    input_sqlite_header_path = input('SQLite header folder path: ')
    if not os.path.isdir(input_sqlite_header_path):
        print("Invalid SQLite header folder!")
    else:
        break

while True:
    input_sqlite_lib_path = input('SQLite lib file path: ')
    if not os.path.isfile(input_sqlite_lib_path):
        print("Invalid SQLite lib file!")
    else:
        break

# collect virtools sdk data
if input_virtools_version != '21':
    # if we do not use virtools 21, we order get original virtools SDK
    while True:
        input_virtools_root_path = input('Virtools root path: ')
        if not os.path.isdir(input_virtools_root_path):
            print("Invalid virtools root path!")
        else:
            break
else:
    # if we are in virtools 21 environment, we have 2 choose aboud used virtools sdk
    # one is gamepiaynmo and another one is doyagu
    # allow user choose a proper one from them and input their corresponding path about cloned repository.
    # also order a proper runtime environment for debug
    while True:
        input_vt21_reverse_work_type = input('Choose Virtools 2.1 reverse work source(gamepiaynmo, doyagu): ')
        if input_vt21_reverse_work_type not in valid_vt21_reverse_work_type:
            print("Invalid Virtools 2.1 reverse work source!")
        else:
            break
    while True:
        input_vt21_reverse_work_path =  input('Virtools 2.1 reverse work root path: ')
        if not os.path.isdir(input_vt21_reverse_work_path):
            print("Invalid Virtools 2.1 reverse work root path!")
        else:
            break
    while True:
        input_vt21_runtime_path =  input('Virtools 2.1 runtime path: ')
        if not os.path.isdir(input_vt21_runtime_path):
            print("Invalid Virtools 2.1 runtime path!")
        else:
            break

# ======================== construct some path
# .......todo

# build sqlite related data
sqlite_header_path = input_sqlite_header_path
(sqlite_lib_path, sqlite_lib_filename) = os.path.split(input_sqlite_lib_path)

# virtools version macro
virtools_ver = 'VIRTOOLS_' + input_virtools_version

# build type macro, and some essential build macros, linked lib
if input_build_type == build_type_plugin:
    virtools_build_type = 'VIRTOOLS_PLUGIN'
    virtools_build_suffix = 'dll'
    virtools_module_define = 'SuperScriptMaterializer.def'
    virtools_attached_lib = virtools_attached_lib_plugin_dict[input_virtools_version]
    virtools_std_macro = virtools_std_macro_plugin_dict[input_virtools_version]
elif input_build_type == build_type_standalone:
    virtools_build_type = 'VIRTOOLS_STANDALONE'
    virtools_build_suffix = 'exe'
    virtools_module_define = ''
    virtools_attached_lib = virtools_attached_lib_standalone_dict[input_virtools_version]
    virtools_std_macro = virtools_std_macro_standalone_dict[input_virtools_version]

# debug configuration and output path
if input_virtools_version == '21':
    # virtools 21 onlt allow standalone build type
    # we copy it and specific some field
    virtools_debug_root = iinput_vt21_runtime_path
    virtools_debug_commandline = 'test.nmo test_script.db test_env.db'
    virtools_debug_target = os.path.join(input_vt21_runtime_path, 'SuperScriptMaterializer.exe')
    virtools_output_path = input_vt21_runtime_path
else:
    # in original virtools sdk environment
    # output file according to build type
    virtools_debug_root = input_virtools_root_path
    if input_build_type == build_type_plugin:
        virtools_debug_commandline = ''
        virtools_debug_target = os.path.join(input_virtools_root_path, executable_virtools[input_virtools_version])
        virtools_output_path = os.path.join(input_virtools_root_path, 'InterfacePlugins')
    else:
        virtools_debug_commandline = 'test.nmo test_script.db test_env.db'
        virtools_debug_target = os.path.join(input_virtools_root_path, 'SuperScriptMaterializer.exe')
        virtools_output_path = input_virtools_root_path

# make sure the last char of output_path is slash
if virtools_output_path[-1] != '\\' or virtools_output_path[-1] != '/':
    virtools_output_path = virtools_output_path + '\\'

# virtools compile and link options
# we need do different strategy for virtools 2.1 and anything else virtools version
if input_virtools_version == '21':
    # the reverse work of doyagu and gamepiaynmo is different, so we need to 
    # use them differently
    if input_vt21_reverse_work_type == 'doyagu':
        # doyagu do not need any extra macro
        virtools_extra_macro = ''
        virtools_header_path = os.path.join(input_vt21_reverse_work_path, 'Include')
        virtools_lib_path = os.path.join(input_vt21_reverse_work_path, 'Lib')
    else:
        # gamepiaynmo need a special macro but his proj do not need any link,
        # instead, we need compile it fully which will be implemented in following code
        bml_special_macro = 'BML_EXPORT='
        virtools_header_path = os.path.join(input_vt21_reverse_work_path, 'virtools')
        virtools_lib_path = ''
else:
    virtools_extra_macro = ''
    if input_virtools_version == '25':
        virtools_header_path = os.path.join(input_virtools_root_path, 'Virtools_SDK/Includes')
        virtools_lib_path = os.path.join(input_virtools_root_path, 'Virtools_SDK/Lib')
    else:
        virtools_header_path = os.path.join(input_virtools_root_path, 'Sdk/Includes')
        virtools_lib_path = os.path.join(input_virtools_root_path, 'Sdk/Lib/Win32/Release')
    

# ======================== create document

# create header
dom = minidom.getDOMImplementation().createDocument(None, 'Project', None)
root = dom.documentElement
root.setAttribute('ToolsVersion', '4.0')
root.setAttribute('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')

cache = dom.createElement('ImportGroup')
cache.setAttribute('Label', 'PropertySheets')
root.appendChild(cache)

# ======================== write build type
# due to build chain v100 shit design, this configuration onlt can be modified in .vcxproj
# so these code were annotated
'''
for bt in ('Debug', 'Release'):
    node_build_type = dom.createElement('PropertyGroup')
    node_build_type.setAttribute('Label', 'Configuration')
    node_build_type.setAttribute('Condition', "'$(Configuration)|$(Platform)'=='{}|Win32'".format(bt))
    node_configuration_type = dom.createElement('ConfigurationType')
    if input_build_type == build_type_standalone:
        node_configuration_type.appendChild(dom.createTextNode('Application'))
    elif input_build_type == build_type_plugin:
        node_configuration_type.appendChild(dom.createTextNode('DynamicLibrary'))
    node_build_type.appendChild(node_configuration_type)
    root.appendChild(node_build_type)
'''

# ======================== write subsystem
for bt in ('Debug', 'Release'):
    node_item_def = dom.createElement('ItemDefinitionGroup')
    node_item_def.setAttribute('Condition', "'$(Configuration)|$(Platform)'=='{}|Win32'".format(bt))
    
    node_item_def_link = dom.createElement('Link')
    node_sub_system = dom.createElement('SubSystem')

    if input_build_type == build_type_standalone:
        node_sub_system.appendChild(dom.createTextNode('Console'))
    elif input_build_type == build_type_plugin:
        node_sub_system.appendChild(dom.createTextNode('Windows'))
    
    node_item_def_link.appendChild(node_sub_system)
    node_item_def.appendChild(node_item_def_link)
    root.appendChild(node_item_def)

# ======================== write macro and misc
node_property_group = dom.createElement('PropertyGroup')
node_property_group.setAttribute('Label', 'UserMacros')
root.appendChild(node_property_group)

cache = dom.createElement('PropertyGroup')
root.appendChild(cache)

cache = dom.createElement('ItemDefinitionGroup')
root.appendChild(cache)

node_item_group = dom.createElement('ItemGroup')
root.appendChild(node_item_group)

# build type distinguish macro
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_VER', virtools_ver)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_BUILD_TYPE', virtools_build_type)
# header and libs
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_HEADER_PATH', virtools_header_path)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_LIB_PATH', virtools_lib_path)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_LIB_FILENAME', virtools_attached_lib)
write_macro(dom, node_property_group, node_item_group, 'SQLITE_HEADER_PATH', sqlite_header_path)
write_macro(dom, node_property_group, node_item_group, 'SQLITE_LIB_PATH', sqlite_lib_path)
write_macro(dom, node_property_group, node_item_group, 'SQLITE_LIB_FILENAME', sqlite_lib_filename)
# output and debug
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_OUTPUT_PATH', virtools_output_path)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_DEBUG_TARGET', virtools_debug_target)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_DEBUG_ROOT', virtools_debug_root)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_DEBUG_COMMANDLINE', virtools_debug_commandline)
# essential build macro
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_STD_MACRO', virtools_std_macro)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_EXTRA_MACRO', virtools_extra_macro)
# misc macro
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_BUILD_SUFFIX', virtools_build_suffix)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_MODULE_DEFINE', virtools_module_define)

# ======================== write extra compile

# if we using gamepiaynmp vt21 reverse work
# we need add all his cpp file into our compile list
if input_virtools_version == '21' and input_vt21_reverse_work_type == 'gamepiaynmo':
    header_item_group = dom.createElement('ItemGroup')
    cpp_item_group = dom.createElement('ItemGroup')
    bml_virtools_path = os.path.join(input_vt21_reverse_work_path, 'virtools')
    for folderName, subfolders, filenames in os.walk(bml_virtools_path):
        for filename in filenames:
            write_cl(dom, header_item_group, cpp_item_group, os.path.join(folderName, filename))

    root.appendChild(header_item_group)
    root.appendChild(cpp_item_group)


# ======================== output
with open('./SuperScriptMaterializer/Virtools.props', 'w', encoding='utf-8') as f:
    dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')

# ======================== modify .vcxproj
# due to build chain v100 shit design, this configuration onlt can be modified in .vcxproj
if input_build_type == build_type_standalone:
    vcxproj_build_type = 'Application'
elif input_build_type == build_type_plugin:
    vcxproj_build_type = 'DynamicLibrary'

vcxproj = minidom.parse('./SuperScriptMaterializer/SuperScriptMaterializer.vcxproj')
node_project = vcxproj.documentElement
for item in node_project.getElementsByTagName('PropertyGroup'):
    attr_label = item.getAttribute('Label')
    attr_condition = item.getAttribute('Condition')
    if attr_label == 'Configuration' and (attr_condition == "'$(Configuration)|$(Platform)'=='Debug|Win32'" or attr_condition == "'$(Configuration)|$(Platform)'=='Release|Win32'"):
        # valid node
        node_cfg_type = item.getElementsByTagName('ConfigurationType')
        if len(node_cfg_type) != 0:
            # have node, change it
            node_cfg_type[0].childNodes[0].nodeValue = vcxproj_build_type
        else:
            # don't have node, add one
            node_cfg_type = vcxproj.createElement('ConfigurationType')
            node_cfg_type.appendChild(vcxproj.createTextNode(vcxproj_build_type))
            item.appendChild(node_cfg_type)

with open('./SuperScriptMaterializer/SuperScriptMaterializer.vcxproj', 'w', encoding='utf-8') as f:
    vcxproj.writexml(f, encoding='utf-8')

print("OK!")
