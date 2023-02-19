import Scripts.VSProp as VSProp
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

# ======================== assist func
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


# ======================== create props

props = VSProp.VSPropWriter()
vcxproj = VSProp.VSVcxprojModifier('./SuperScriptMaterializer/SuperScriptMaterializer.vcxproj')

# write build type
if input_build_type == build_type_standalone:
    vcxproj.SetBuildType(vcxproj.BUILDTYPE_EXE)
elif input_build_type == build_type_plugin:
    vcxproj.SetBuildType(vcxproj.BUILDTYPE_DLL)

# write subsystem
if input_build_type == build_type_standalone:
    props.SetSubSystem(props.SUBSYSTEM_CON);
elif input_build_type == build_type_plugin:
    props.SetSubSystem(props.SUBSYSTEM_WIN)

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

# ======================== create vt21 props

# if we are using virtools 2.1. and we use gamepiaynmo as our
# reverse library. we need enable project GPVirtoolsStatic and 
# add some macro for it
if input_virtools_version == '21' and input_vt21_reverse_work_type == 'gamepiaynmo':
    gp_props = VSProp.VSPropWriter()
    gp_props.AddMacro('BML_REPOSITORY', input_vt21_reverse_work_path)
    gp_props.Write2File('./GPVirtoolsStatic/Virtools.props')

print("OK!")
