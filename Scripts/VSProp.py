import xml.dom.minidom as minidom
import xml.dom
import os
import sys

class VSPropWriter(object):

    SUBSYSTEM_WIN: str = 'Windows'
    SUBSYSTEM_CON: str = 'Console'

    def __init__(self):
        self.__MacroList: dict[str, str] = {}
        self.__SubSystem: str = None
        self.__BuildType: str = None

    def AddMacro(self, key: str, value: str):
        if (key in self.__MacroList):
            raise Exception(f'Duplicated Set Macro "{key}".')
        self.__MacroList[key] = value

    def SetSubSystem(self, subsys: str):
        self.__SubSystem = subsys

    def Write2File(self, filename: str):
        # create some header
        dom = minidom.getDOMImplementation().createDocument(None, 'Project', None)
        root = dom.documentElement
        root.setAttribute('ToolsVersion', '4.0')
        root.setAttribute('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')

        cache = dom.createElement('ImportGroup')
        cache.setAttribute('Label', 'PropertySheets')
        root.appendChild(cache)
        
        # write sub system
        if self.__SubSystem is not None:
            for bt in ('Debug', 'Release'):
                node_IDG = dom.createElement('ItemDefinitionGroup')
                node_IDG.setAttribute('Condition', f"'$(Configuration)|$(Platform)'=='{bt}|Win32'")
                
                node_link = dom.createElement('Link')
                node_sub_system = dom.createElement('SubSystem')

                node_sub_system.appendChild(dom.createTextNode(self.__SubSystem))
                
                node_link.appendChild(node_sub_system)
                node_IDG.appendChild(node_link)
                root.appendChild(node_IDG)

        # write macro
        node_PG = dom.createElement('PropertyGroup')    # macro node 1
        node_PG.setAttribute('Label', 'UserMacros')
        root.appendChild(node_PG)

        cache = dom.createElement('PropertyGroup') # dummy structure
        root.appendChild(cache)
        cache = dom.createElement('ItemDefinitionGroup') #dummy structure
        root.appendChild(cache)

        node_IG = dom.createElement('ItemGroup')    # macro node 2
        root.appendChild(node_IG)
        
        for key, value in self.__MacroList.items():
            # create for PropertyGroup
            node_macro_decl = dom.createElement(key)
            if value != '': # check whether data is empty.
                node_macro_decl.appendChild(dom.createTextNode(value))
            node_PG.appendChild(node_macro_decl)

            # create for ItemGroup
            node_macro_ref = dom.createElement("BuildMacro")
            node_macro_ref.setAttribute('Include', key)
            node_inner_macro_ref = dom.createElement('Value')
            node_inner_macro_ref.appendChild(dom.createTextNode("$({})".format(key)))
            node_macro_ref.appendChild(node_inner_macro_ref)
            node_IG.appendChild(node_macro_ref)

        # write to file
        with open(filename, 'w', encoding='utf-8') as f:
            dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')

class VSVcxprojModifier(object):
    
    BUILDTYPE_EXE: str = 'Application'
    BUILDTYPE_DLL: str = 'DynamicLibrary'

    def __init__(self, vcfile: str):
        self.__Dom = minidom.parse(vcfile)
        self.__FileName: str = vcfile
        self.__BuildType: str = None

    def SetBuildType(self, bt: str):
        self.__BuildType = bt

    def Write2File(self):
        # if no spec build type, do not modify
        if self.__BuildType is None:
            return

        # get corresponding entry
        dom = self.__Dom
        node_project = dom.documentElement
        for node_PG in node_project.getElementsByTagName('PropertyGroup'):
            attr_label = node_PG.getAttribute('Label')
            attr_condition = node_PG.getAttribute('Condition')

            # skip invalid node
            if attr_label != 'Configuration':
                continue
            if attr_condition != "'$(Configuration)|$(Platform)'=='Debug|Win32'" and attr_condition != "'$(Configuration)|$(Platform)'=='Release|Win32'":
                continue

            # this is valid node, process it
            node_CT = node_PG.getElementsByTagName('ConfigurationType')
            if len(node_CT) != 0:
                # have node, change it
                node_CT[0].childNodes[0].nodeValue = self.__BuildType
            else:
                # don't have node, add one
                node_CT = dom.createElement('ConfigurationType')
                node_CT.appendChild(dom.createTextNode(self.__BuildType))
                node_PG.appendChild(node_CT)

        # write file
        with open(self.__FileName, 'w', encoding='utf-8') as f:
            dom.writexml(f, encoding='utf-8')

