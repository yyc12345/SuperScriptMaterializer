import vs_props_writer, vs_vcxproj_modifier
import os, enum, sys, argparse

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

VT_HEADER_PATH: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: 'Include',
    VirtoolsVersion.V25: 'Virtools_SDK\\Includes',
    VirtoolsVersion.V30: 'Sdk\\Includes',
    VirtoolsVersion.V35: 'Sdk\\Includes',
    VirtoolsVersion.V40: 'Sdk\\Includes',
    VirtoolsVersion.V50: 'Sdk\\Includes'
}

VT_LIB_PATH: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V21: 'Lib',
    VirtoolsVersion.V25: 'Virtools_SDK\\Lib',
    VirtoolsVersion.V30: 'Sdk\\Lib',
    VirtoolsVersion.V35: 'Sdk\\Lib\\Win32\\Release',
    VirtoolsVersion.V40: 'Sdk\\Lib\\Win32\\Release',
    VirtoolsVersion.V50: 'Sdk\\Lib\\Win32\\Release'
}

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

VT_PLUGIN_SUPPORTED_VER: set[VirtoolsVersion] = set((
    VirtoolsVersion.V30,
    VirtoolsVersion.V35,
    VirtoolsVersion.V40,
    VirtoolsVersion.V50,
))
VT_STANDALONE_SUPPORTED_VER: set[VirtoolsVersion] = set((
    VirtoolsVersion.V21,
    VirtoolsVersion.V25,
    VirtoolsVersion.V30,
    VirtoolsVersion.V35,
    VirtoolsVersion.V40,
    VirtoolsVersion.V50,
))

VT_EXECUTABLE_DEV: dict[VirtoolsVersion, str] = {
    VirtoolsVersion.V25: "Dev.exe",
    VirtoolsVersion.V30: "devr.exe",
    VirtoolsVersion.V35: "devr.exe",
    VirtoolsVersion.V40: "devr.exe",
    VirtoolsVersion.V50: "devr.exe"
}

#endregion

#region Assist Functions

def get_project_root() -> str:
    # build project root path and return
    return os.path.dirname(os.path.dirname(__file__))

def check_project_root() -> None:
    """
    Check whether this script is executed in correct folder.

    If check failed, this function will exit script, otherwise, return directly.
    """
    project_root: str = get_project_root()
    # check whether have readme file
    if not os.path.isfile(os.path.join(project_root, 'README.md')):
        print('Fail to get project root folder. This script may be placed at wrong location.')
        sys.exit(1)

def validate_combination(build_type: BuildType, vt_version: VirtoolsVersion) -> None:
    """
    Check whether the given combination of build type and Virtools version is OK.

    Some combination of build type and Virtools version is invalid.
    For example, because Virtools 2.5 do not have UI register so plugin build type is not available in Virtools 2.1

    If validation failed, this function will exit script, otherwise, return directly.
    """
    # fetch result
    is_okey: bool
    match(build_type):
        case BuildType.Standalone:
            is_okey = vt_version in VT_STANDALONE_SUPPORTED_VER
        case BuildType.Plugin:
            is_okey = vt_version in VT_PLUGIN_SUPPORTED_VER
    # output result
    if not is_okey:
        print(f'The combination of "{build_type.value}" and "{vt_version.value}" is not compatible currently.')
        sys.exit(1)

def ensure_trailing_slash(given_path: str) -> str:
    """
    Ensure given path is end with slash or backslash.

    @param given_path The path for checking.
    @return The path with trailing slash or backslash,
    or the given path self if it already has.
    """
    # make sure return value is end with slash or backslash
    if given_path[-1] != '\\' or given_path[-1] != '/':
        return given_path + '\\'
    else:
        return given_path

def get_header_path(vt_version: VirtoolsVersion, vt_root: str) -> str:
    return os.path.join(vt_root, VT_HEADER_PATH[vt_version])

def get_lib_path(vt_version: VirtoolsVersion, vt_root: str) -> str:
    return os.path.join(vt_root, VT_LIB_PATH[vt_version])

def get_attached_libs(build_type: BuildType, vt_version: VirtoolsVersion) -> str:
    match(build_type):
        case BuildType.Standalone:
            return VT_STANDALONE_ATTACHED_LIBS[vt_version]
        case BuildType.Plugin:
            return VT_PLUGIN_ATTACHED_LIBS[vt_version]

def get_macros(build_type: BuildType, vt_version: VirtoolsVersion) -> str:
    match(build_type):
        case BuildType.Standalone:
            return VT_STANDALONE_MACROS[vt_version]
        case BuildType.Plugin:
            return VT_PLUGIN_MACROS[vt_version]

def get_executable_dev(vt_version: VirtoolsVersion, vt_root: str) -> str:
    """
    Return the path to executable Virtools Dev according to given Virtools version.

    Usually it is `Dev.exe` or `devr.exe`.

    @param vt_version The version of Virtools.
    @param vt_root The path to Virtools root folder.
    @return The path to executable Virtools Dev.
    """
    return os.path.join(vt_root, VT_EXECUTABLE_DEV[vt_version])

def get_output_path(build_type: BuildType, vt_root: str) -> str:
    """
    Get the output path for plugin.

    For plugin build type, script will try output binary into `InterfacePlugins` folder in Virtools environment.
    For standalone build type, script will try output binary in the root path of Virtools.

    @param vt_version The version of Virtools.
    @param vt_root The path to Virtools root folder.
    @return The path where the compiled project binary will be placed.
    """
    # fetch output path by build type
    ret: str
    match(build_type):
        case BuildType.Standalone:
            ret = vt_root
        case BuildType.Plugin:
            ret = os.path.join(vt_root, 'InterfacePlugins')
    # make sure return value is end with slash or backslash
    return ensure_trailing_slash(ret)

def get_binary_suffix(build_type: BuildType) -> str:
    match(build_type):
        case BuildType.Standalone:
            return 'exe'
        case BuildType.Plugin:
            return 'dll'

def get_module_define(build_type: BuildType) -> str:
    match(build_type):
        case BuildType.Standalone:
            return ''
        case BuildType.Plugin:
            return 'SuperScriptMaterializer.def'

#endregion

def main() -> None:
    # ========== Check Environment ==========
    check_project_root()

    # ========== Accept User Input ==========
    # build args parser
    parser = argparse.ArgumentParser(description='Project Configuration Maker')
    parser.add_argument(
        '-b', '--build-type', required=True, action='store', dest='build_type', choices=tuple(item.value for item in BuildType),
        help='The build type of project.'
    )
    parser.add_argument(
        '-t', '--virtools-version', required=True, action='store', dest='virtools_version', choices=tuple(item.value for item in VirtoolsVersion),
        help='The Virtools version you picked.'
    )
    parser.add_argument(
        '-p', '--virtools-path', required=True, action='store', dest='virtools_path',
        help='''
        The path to the root folder of you picked Virtools version where you can find "Dev.exe".
        If you select Virtools 2.1, this path should be the root folder of GitHub project "doyaGu/Virtools-SDK-2.1".
        '''
    )
    parser.add_argument(
        '-a', '--sqlite-amalgamation-path', required=True, action='store', dest='sqlite_amalgamation_path',
        help='The path to downloaded sqlite amalgamation folder where you can find "sqlite3.h"'
    )
    parser.add_argument(
        '-d', '--sqlite-dll-path', required=True, action='store', dest='sqlite_dll_path',
        help='The path to downloaded sqlite dll folder where you can find "sqlite3.dll" and "sqlite3.lib" you just built a few minutes ago.'
    )
    # parse arguments
    args = parser.parse_args()

    # ========== Analyse Arguments ==========
    # extract arguments
    arg_virtools_version: VirtoolsVersion = VirtoolsVersion(args.virtools_version)
    arg_build_type: BuildType = BuildType(args.build_type)
    arg_virtools_path: str = args.virtools_path
    arg_sqlite_amalgamation_path: str = args.sqlite_amalgamation_path
    arg_sqlite_dll_path: str = args.sqlite_dll_path

    # validate the combination
    validate_combination(arg_build_type, arg_virtools_version)

    # build macros values
    # virtools version and build type macro
    macro_virtools_ver: str = f'VIRTOOLS_{arg_virtools_version.value}'
    macro_virtools_build_type: str = f'VIRTOOLS_{arg_build_type.value.upper()}'
    # virtools header and lib
    macro_virtools_header_path: str = get_header_path(arg_virtools_version, arg_virtools_path)
    macro_virtools_lib_path: str = get_lib_path(arg_virtools_version, arg_virtools_path)
    macro_virtools_attcahed_libs: str = get_attached_libs(arg_build_type, arg_virtools_version)
    # sqlite header and lib
    macro_sqlite_header_path: str = arg_sqlite_amalgamation_path
    macro_sqlite_lib_path: str = arg_sqlite_dll_path
    # output path macro
    macro_virtools_output_path: str = get_output_path(arg_build_type, arg_virtools_path)
    # virtools used macros
    macro_virtools_macros: str = get_macros(arg_build_type, arg_virtools_version)
    # misc
    macro_virtools_binary_suffix: str = get_binary_suffix(arg_build_type)
    macro_virtools_module_define: str = get_module_define(arg_build_type)

    # ========== Create Property File ==========

    props = vs_props_writer.VsPropsWriter()

    # write subsystem
    match(arg_build_type):
        case BuildType.Standalone:
            props.SetSubSystem(vs_props_writer.VsSubSystem.Console)
        case BuildType.Plugin:
            props.SetSubSystem(vs_props_writer.VsSubSystem.Windows)

    # write macro and misc
    # build type distinguish macro
    props.AddMacro('VIRTOOLS_VER', macro_virtools_ver)
    props.AddMacro('VIRTOOLS_BUILD_TYPE', macro_virtools_build_type)
    # header and libs
    props.AddMacro('VIRTOOLS_HEADER_PATH', macro_virtools_header_path)
    props.AddMacro('VIRTOOLS_LIB_PATH', macro_virtools_lib_path)
    props.AddMacro('VIRTOOLS_ATTACHED_LIBS', macro_virtools_attcahed_libs)
    props.AddMacro('SQLITE_HEADER_PATH', macro_sqlite_header_path)
    props.AddMacro('SQLITE_LIB_PATH', macro_sqlite_lib_path)
    # output
    props.AddMacro('VIRTOOLS_OUTPUT_PATH', macro_virtools_output_path)
    # essential build macro
    props.AddMacro('VIRTOOLS_MACROS', macro_virtools_macros)
    # misc macro
    props.AddMacro('VIRTOOLS_BINARY_SUFFIX', macro_virtools_binary_suffix)
    props.AddMacro('VIRTOOLS_MODULE_DEFINE', macro_virtools_module_define)

    # output
    props.Generate(os.path.join(get_project_root(), 'Virtools.props'))

    # ========== Modify ==========
    vcxproj = vs_vcxproj_modifier.VsVcxprojModifier(
        os.path.join(get_project_root(), 'SuperScriptMaterializer/SuperScriptMaterializer.vcxproj')
    )

    # write build type
    match(arg_build_type):
        case BuildType.Standalone:
            vcxproj.SetBuildType(vcxproj.BUILDTYPE_EXE)
        case BuildType.Plugin:
            vcxproj.SetBuildType(vcxproj.BUILDTYPE_DLL)

    # output
    vcxproj.Modify()

    # ========== Done ==========
    print("Configuration OK!")

if __name__ == '__main__':
    main()
