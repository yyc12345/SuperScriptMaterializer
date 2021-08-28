import xml.dom.minidom as minidom
import xml.dom
import os
import sys

# ======================== const define

program_name = "SuperScriptMaterializer"

build_type_standalone = "standalone"
build_type_plugin = "plugin"
valid_build_type = (
    build_type_standalone,
    build_type_plugin
)

virtools_attach_ref_dict = {
    "21": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib",
    "25": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib",
    "35": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib",
    "40": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib",
    "50": "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib" 
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

# ======================== requirement check

if len(sys.argv) != 8:
    print("Error parameter!")
    print("Format: python3 mk_materializer_cfg.py [plugin|standalone] [21|25|35|40|50] [virtools_root_path] [sqlite_header] [sqlite_lib] [sqlite attach ref] [bml path]")
    print('Example: python3 .\mk_materializer_cfg.py standalone 50 "E:\Virtools\Virtools Dev 5.0" "D:\CppLib\SQLite\sqlite-amalgamation-3310100" "D:\CppLib\SQLite\sqlite-dll-win32-x86-3310100" sqlite3.lib "D:\BallanceModLoader"')
    sys.exit(1)

input_build_type = sys.argv[1]
if input_build_type not in valid_build_type:
    print("Invalid build_type!")
    sys.exit(1)

input_virtools_version = sys.argv[2]
if input_build_type == build_type_standalone and input_virtools_version not in valid_virtools_standalone_ver:
    print("Invalid virtools_version!")
    sys.exit(1)
elif input_build_type == build_type_plugin and input_virtools_version not in valid_virtools_plugin_ver:
    print("Invalid virtools_version!")
    sys.exit(1)

input_virtools_root_path = sys.argv[3]
input_sqlite_header_path = sys.argv[4]
input_sqlite_lib_path = sys.argv[5]
input_sqlite_attach_ref = sys.argv[6]
input_bml_path = sys.argv[7]

# ======================== construct some path
# .......todo

sqlite_header_path = input_sqlite_header_path
sqlite_lib_path = input_sqlite_lib_path
sqlite_attach_ref = input_sqlite_attach_ref

virtools_ver = 'VIRTOOLS_' + input_virtools_version
virtools_debug_root = input_virtools_root_path
virtools_attach_ref = virtools_attach_ref_dict[input_virtools_version]

if input_build_type == build_type_plugin:
    virtools_build_type = 'VIRTOOLS_PLUGIN'
    virtools_build_suffix = 'dll'
    virtools_debug_target = os.path.join(input_virtools_root_path, executable_virtools[input_virtools_version])
    virtools_output_path = os.path.join(input_virtools_root_path, 'InterfacePlugins')
elif input_build_type == build_type_standalone:
    virtools_build_type = 'VIRTOOLS_STANDALONE'
    virtools_build_suffix = 'exe'
    virtools_debug_target = os.path.join(input_virtools_root_path, 'SuperScriptMaterializer.exe')
    virtools_output_path = input_virtools_root_path

# make sure the last char of output_path is slash
if virtools_output_path[-1] != '\\' or virtools_output_path[-1] != '/':
    virtools_output_path = virtools_output_path + '\\'

# in virtools 2.1, we use bml, so we need add bml macro and set virtools header to bml's virtools header
# also, we don't need set lib path because all virtools file have been imported in project
if input_virtools_version == '21':
    virtools_header_path = ''#os.path.join(input_bml_path, 'virtools')
    virtools_lib_path = ''
    bml_special_macro = 'BML_EXPORT='
else:
    virtools_header_path = os.path.join(input_virtools_root_path, 'Sdk/Includes')
    virtools_lib_path = os.path.join(input_virtools_root_path, 'Sdk/Lib/Win32/Release')
    bml_special_macro = ''

# ======================== create document

dom = minidom.getDOMImplementation().createDocument(None, 'Project', None)
root = dom.documentElement
root.setAttribute('ToolsVersion', '4.0')
root.setAttribute('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')

cache = dom.createElement('ImportGroup')
cache.setAttribute('Label', 'PropertySheets')
root.appendChild(cache)

# ======================== write build type
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

write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_HEADER_PATH', virtools_header_path)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_LIB_PATH', virtools_lib_path)
write_macro(dom, node_property_group, node_item_group, 'SQLITE_HEADER_PATH', sqlite_header_path)
write_macro(dom, node_property_group, node_item_group, 'SQLITE_LIB_PATH', sqlite_lib_path)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_OUTPUT_PATH', virtools_output_path)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_BUILD_TYPE', virtools_build_type)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_DEBUG_TARGET', virtools_debug_target)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_DEBUG_ROOT', virtools_debug_root)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_BUILD_SUFFIX', virtools_build_suffix)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_VER', virtools_ver)
write_macro(dom, node_property_group, node_item_group, 'BML_SPECIAL_MACRO', bml_special_macro)
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_ATTACH_REF', virtools_attach_ref)
write_macro(dom, node_property_group, node_item_group, 'SQLITE_ATTACH_REF', sqlite_attach_ref)

# ======================== write extra compile

if input_virtools_version == '21':
    header_item_group = dom.createElement('ItemGroup')
    cpp_item_group = dom.createElement('ItemGroup')
    bml_virtools_path = os.path.join(input_bml_path, 'virtools')
    for folderName, subfolders, filenames in os.walk(bml_virtools_path):
        for filename in filenames:
            write_cl(dom, header_item_group, cpp_item_group, os.path.join(folderName, filename))

    root.appendChild(header_item_group)
    root.appendChild(cpp_item_group)


# ======================== output
with open('./SuperScriptMaterializer/Virtools.props', 'w', encoding='utf-8') as f:
    dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')

print("OK!")
