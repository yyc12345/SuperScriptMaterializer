import xml.dom.minidom as minidom
import xml.dom
import os
import sys

class VsVcxprojModifier():
    
    BUILDTYPE_EXE: str = 'Application'
    BUILDTYPE_DLL: str = 'DynamicLibrary'

    def __init__(self, vcfile: str):
        self.__Dom = minidom.parse(vcfile)
        self.__FileName: str = vcfile
        self.__BuildType: str = None

    def SetBuildType(self, bt: str):
        self.__BuildType = bt

    def Modify(self):
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

