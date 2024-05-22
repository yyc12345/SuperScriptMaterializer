import vs_props_writer, vs_vcxproj_modifier
import os, enum, sys

#region Constant Declarations

class BuildType(enum.Enum):
    Standalone: str = "standalone"
    Plugin: str = "plugin"

class VirtoolsVersion(enum.Enum):
    V21 = '21'
    V25 = '25'
    V30 = '30'
    V35 = '35'
    V40 = '40'
    V50 = '50'

VT_STANDALONE_ATTACHED_LIBS: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: "VxMath.lib;CK2.lib",
    VirtoolsVersion.V25: "VxMath.lib;CK2.lib",
    VirtoolsVersion.V30: "VxMath.lib;CK2.lib",
    VirtoolsVersion.V35: "VxMath.lib;CK2.lib",
    VirtoolsVersion.V40: "VxMath.lib;CK2.lib",
    VirtoolsVersion.V50: "VxMath.lib;CK2.lib"
}
VT_PLUGIN_ATTACHED_LIBS: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: "",
    VirtoolsVersion.V25: "",
    VirtoolsVersion.V30: "VxMath.lib;DllEditor.lib;CK2.lib;InterfaceControls.lib;CKControls.lib",
    VirtoolsVersion.V35: "VxMath.lib;DllEditor.lib;CK2.lib;InterfaceControls.lib;CKControls.lib",
    VirtoolsVersion.V40: "VxMath.lib;DllEditor.lib;CK2.lib;InterfaceControls.lib;CKControls.lib",
    VirtoolsVersion.V50: "VxMath.lib;DllEditor.lib;CK2.lib;InterfaceControls.lib;CKControls.lib" 
}

VT_PLUGIN_MACROS: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    VirtoolsVersion.V25: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    VirtoolsVersion.V30: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    VirtoolsVersion.V35: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    VirtoolsVersion.V40: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    VirtoolsVersion.V50: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE" 
}
VT_STANDALONE_MACROS: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    VirtoolsVersion.V25: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    VirtoolsVersion.V30: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    VirtoolsVersion.V35: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    VirtoolsVersion.V40: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    VirtoolsVersion.V50: "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG;VIRTOOLS_USER_SDK" 
}

VT_EXECUTABLE_DEV: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: "Dev.exe",
    VirtoolsVersion.V25: "Dev.exe",
    VirtoolsVersion.V30: "devr.exe",
    VirtoolsVersion.V35: "devr.exe",
    VirtoolsVersion.V40: "devr.exe",
    VirtoolsVersion.V50: "devr.exe"
}

#endregion

#region Assist Functions

def get_project_root() -> str:
    # build project root path
    ret: str = os.path.dirname(os.path.dirname(__file__))
    # check whether have readme file
    if not os.path.isfile(os.path.join(ret, 'README.md')):
        print('Fail to get project root folder. This script may be placed at wrong location.')
        sys.exit(1)
    # return value
    return ret

def get_attached_libs(build_type: BuildType, vt_version: VirtoolsVersion) -> str:
    match(build_type):
        case BuildType.Standalone:
            return VT_STANDALONE_ATTACHED_LIBS[vt_version]
        case BuildType.Plugin:
            return VT_PLUGIN_ATTACHED_LIBS[vt_version]
        case _:
            raise Exception('invalid build type')

def get_macros(build_type: BuildType, vt_version: VirtoolsVersion) -> str:
    match(build_type):
        case BuildType.Standalone:
            return VT_STANDALONE_MACROS[vt_version]
        case BuildType.Plugin:
            return VT_PLUGIN_MACROS[vt_version]
        case _:
            raise Exception('invalid build type')

def get_executable_dev(vt_version: VirtoolsVersion) -> str:
    """
    Return the path to executable Virtools Dev according to given Virtools version.

    Usually it is `Dev.exe` or `devr.exe`.

    :param vt_version The version of Virtools.
    """
    pass

def get_output_path(build_type: BuildType, vt_root: str) -> str:
    # fetch output path by build type
    ret: str
    match(build_type):
        case BuildType.Standalone:
            ret = vt_root
        case BuildType.Plugin:
            ret = os.path.join(vt_root, 'InterfacePlugins')
        case _:
            raise Exception('invalid build type')
    # make sure return value is end with slash or backslash
    if ret[-1] != '\\' or ret[-1] != '/':
        ret += '\\'
    # return value
    return ret

#endregion



# =========== check work dir ===========

if not os.path.isfile(os.path.join(os.getcwd(), 'README.md')):
    print('Error! Please run this script at the root of this repository.')
    sys.exit(1)

# =========== const define ===========

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
virtools_gp_static_proj = 'GPVirtoolsStatic'

virtools_std_macro_plugin_dict = {
    "21": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    "25": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    "35": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    "40": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE",
    "50": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE" 
}
virtools_std_macro_standalone_dict = {
    "21": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    "25": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    "35": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    "40": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;_DEBUG",
    "50": "_CRT_SECURE_NO_WARNINGS;_CRT_NONSTDC_NO_DEPRECATE;VIRTOOLS_USER_SDK;_DEBUG" 
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

# =========== assist func ===========
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

# =========== requirement get ===========

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

# =========== construct some path ===========

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
    virtools_std_macro = virtools_std_macro_plugin_dict[input_virtools_version]
    virtools_attached_lib = virtools_attached_lib_plugin_dict[input_virtools_version]
elif input_build_type == build_type_standalone:
    virtools_build_type = 'VIRTOOLS_STANDALONE'
    virtools_build_suffix = 'exe'
    virtools_module_define = ''
    virtools_std_macro = virtools_std_macro_standalone_dict[input_virtools_version]
    # gamepiaynmo linked lib need special lib name
    if input_virtools_version == '21' and input_vt21_reverse_work_type == 'gamepiaynmo':
        virtools_attached_lib = virtools_gp_static_proj + '.lib'
    else:
        virtools_attached_lib = virtools_attached_lib_standalone_dict[input_virtools_version]

# debug configuration and output path
if input_virtools_version == '21':
    # virtools 21 onlt allow standalone build type
    # we copy it and specific some field
    virtools_debug_root = input_vt21_runtime_path
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
        virtools_extra_macro = 'BML_EXPORT='
        virtools_header_path = os.path.join(input_vt21_reverse_work_path, 'virtools')
        virtools_lib_path = '$(SolutionDir)out\\$(Platform)\\$(Configuration)\\' + virtools_gp_static_proj
else:
    virtools_extra_macro = ''
    if input_virtools_version == '25':
        virtools_header_path = os.path.join(input_virtools_root_path, 'Virtools_SDK/Includes')
        virtools_lib_path = os.path.join(input_virtools_root_path, 'Virtools_SDK/Lib')
    else:
        virtools_header_path = os.path.join(input_virtools_root_path, 'Sdk/Includes')
        virtools_lib_path = os.path.join(input_virtools_root_path, 'Sdk/Lib/Win32/Release')


# =========== create props ===========

props = vs_props_writer.VsPropsWriter()
vcxproj = vs_vcxproj_modifier.VsVcxprojModifier(
    './SuperScriptMaterializer/SuperScriptMaterializer.vcxproj'
)

# write build type
if input_build_type == build_type_standalone:
    vcxproj.SetBuildType(vcxproj.BUILDTYPE_EXE)
elif input_build_type == build_type_plugin:
    vcxproj.SetBuildType(vcxproj.BUILDTYPE_DLL)

# write subsystem
if input_build_type == build_type_standalone:
    props.SetSubSystem(vs_props_writer.VsSubSystem.Console)
elif input_build_type == build_type_plugin:
    props.SetSubSystem(vs_props_writer.VsSubSystem.Windows)

# write macro and misc
# build type distinguish macro
props.AddMacro('VIRTOOLS_VER', virtools_ver)
props.AddMacro('VIRTOOLS_BUILD_TYPE', virtools_build_type)
# header and libs
props.AddMacro('VIRTOOLS_HEADER_PATH', virtools_header_path)
props.AddMacro('VIRTOOLS_LIB_PATH', virtools_lib_path)
props.AddMacro('VIRTOOLS_LIB_FILENAME', virtools_attached_lib)
props.AddMacro('SQLITE_HEADER_PATH', sqlite_header_path)
props.AddMacro('SQLITE_LIB_PATH', sqlite_lib_path)
props.AddMacro('SQLITE_LIB_FILENAME', sqlite_lib_filename)
# output and debug
props.AddMacro('VIRTOOLS_OUTPUT_PATH', virtools_output_path)
props.AddMacro('VIRTOOLS_DEBUG_TARGET', virtools_debug_target)
props.AddMacro('VIRTOOLS_DEBUG_ROOT', virtools_debug_root)
props.AddMacro('VIRTOOLS_DEBUG_COMMANDLINE', virtools_debug_commandline)
# essential build macro
props.AddMacro('VIRTOOLS_STD_MACRO', virtools_std_macro)
props.AddMacro('VIRTOOLS_EXTRA_MACRO', virtools_extra_macro)
# misc macro
props.AddMacro('VIRTOOLS_BUILD_SUFFIX', virtools_build_suffix)
props.AddMacro('VIRTOOLS_MODULE_DEFINE', virtools_module_define)

# output
props.Write2File('./SuperScriptMaterializer/Virtools.props')
vcxproj.Write2File()

# =========== create vt21 props ===========

# if we are using virtools 2.1. and we use gamepiaynmo as our
# reverse library. we need enable project GPVirtoolsStatic and 
# add some macro for it
if input_virtools_version == '21' and input_vt21_reverse_work_type == 'gamepiaynmo':
    gp_props = VSProp.VSPropWriter()
    gp_props.AddMacro('BML_REPOSITORY', input_vt21_reverse_work_path)
    gp_props.Write2File('./GPVirtoolsStatic/Virtools.props')

print("OK!")

if __name__ == '__main__':
    pass

