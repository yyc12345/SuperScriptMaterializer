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
    node.appendChild(dom.createTextNode(data))
    node_property_group.appendChild(node)

    node = dom.createElement("BuildMacro")
    node.setAttribute('Include', macro_upcase)
    inter_node = dom.createElement('Value')
    inter_node.appendChild(dom.createTextNode("$({})".format(macro_upcase)))
    node.appendChild(inter_node)
    node_item_group.appendChild(node)

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
    print("Example: python3 mk_materializer_cfg.py [plugin|standalone] [21|25|35|40|50] [virtools_root_path] [sqlite_header] [sqlite_lib] [sqlite attach ref] [bml path]")
    sys.exit(1)

build_type = sys.argv[1]
if build_type not in valid_build_type:
    print("Invalid build_type!")
    sys.exit(1)

virtools_version = sys.argv[2]
if build_type == build_type_standalone and virtools_version not in valid_virtools_standalone_ver:
    print("Invalid virtools_version!")
    sys.exit(1)
elif build_type == build_type_plugin and virtools_version not in valid_virtools_plugin_ver:
    print("Invalid virtools_version!")
    sys.exit(1)

virtools_root_path = sys.argv[3]
sqlite_header_path = sys.argv[4]
sqlite_lib_path = sys.argv[5]
sqlite_attach_ref = sys.argv[6]
bml_path = sys.argv[7]

# ======================== construct some path
# .......todo

virtools_header_path = os.path.join(virtools_root_path, 'Sdk/Includes')
virtools_lib_path = os.path.join(virtools_root_path, 'Sdk/Lib/Win32/Release')
virtools_ver = 'VIRTOOLS_' + virtools_version
virtools_debug_root = virtools_root_path
if build_type == build_type_plugin:
    virtools_build_type = 'VIRTOOLS_PLUGIN'
    virtools_build_suffix = 'dll'
    virtools_debug_target = os.path.join(virtools_root_path, get_executable_virtools(virtools_version))
    virtools_output_path = os.path.join(virtools_root_path, 'InterfacePlugins')
elif build_type == build_type_standalone:
    virtools_build_type = 'VIRTOOLS_STANDALONE'
    virtools_build_suffix = 'exe'
    virtools_debug_target = os.path.join(virtools_root_path, 'SuperScriptMaterializer.exe')
    virtools_output_path = virtools_root_path

if virtools_output_path[-1] != '\\' or virtools_output_path[-1] != '/':
    virtools_output_path = virtools_output_path + '\\'

if virtools_version == '21':
    bml_special_macro = 'BML_EXPORT='
else:
    bml_special_macro = ''

# ======================== create document

dom = minidom.getDOMImplementation().createDocument(None, 'Project', None)
root = dom.documentElement
root.setAttribute('ToolsVersion', '4.0')
root.setAttribute('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')

cache = dom.createElement('ImportGroup')
cache.setAttribute('Label', 'PropertySheets')
root.appendChild(cache)

node_property_group = dom.createElement('PropertyGroup')
node_property_group.setAttribute('Label', 'UserMacros')
root.appendChild(node_property_group)

cache = dom.createElement('PropertyGroup')
root.appendChild(cache)

cache = dom.createElement('ItemDefinitionGroup')
root.appendChild(cache)

node_item_group = dom.createElement('ItemGroup')
root.appendChild(node_item_group)

# ======================== write macro

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
write_macro(dom, node_property_group, node_item_group, 'VIRTOOLS_ATTACH_REF', "vxmath.lib;DllEditor.lib;ck2.lib;InterfaceControls.lib;CKControls.lib")
write_macro(dom, node_property_group, node_item_group, 'SQLITE_ATTACH_REF', sqlite_attach_ref)


# ======================== output
with open('./SuperScriptMaterializer/Virtools.props', 'w', encoding='utf-8') as f:
    dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')

print("OK!")
