class ScriptItem(object):
    def __init__(self, name, id):
        self.name = name
        self.id = id

class HamburgerItem(object):
    def __init__(self, name, path):
        self.name = name
        self.path = path

envDatabaseList = (
    {"name": "Attribute",
     "queryKey": "attr",
     "data": (("index", "0", "(Integer) Attribute index"),
              ("name", "Floor", "(String) Attribute name"),
              ("category_index", "0", "(Integer) The category index of this attribute"),
              ("category_name", "3DXML", "(String) The category name of this attribute"),
              ("flags", "44", "(Integer) Attribute flags"),
              ("param_index", "85", "(Integer) Corresponding parameter index"),
              ("compatible_classid", "41", "(Integer) Compatible class id"),
              ("default_id", "1;NULL", "(String) Default value"))},
    {"name": "Message",
     "queryKey": "msg",
     "data": (("index", "0", "(Integer) Message index"),
              ("name", "OnClick", "(String) Message name"))},
    {"name": "Operation",
     "queryKey": "op",
     "data": (("funcptr", "615841344", "(Integer) Operation function memory location"),
              ("in1_guid", "1910468930,-1003023558", "(String) Input parameter 1 guid"),
              ("in2_guid", "-1411304621,-1568456412", "(String) Input parameter 2 guid"),
              ("out_guid", "450177678,1584666912", "(String) Output parameter guid"),
              ("op_guid", "869034825,898181163", "(String) Operation guid"),
              ("op_name", "Addition", "(String) Operation name"),
              ("op_code", "51", "(Integer) Operation code"))},
    {"name": "Parameter",
     "queryKey": "param",
     "data": (("index", "0", "(Integer) Parameter index"),
              ("guid", "481363808,1100959941", "(String) Parameter guid"),
              ("derived_from", "0,0", "(String) The parameter guid deriving this parameter"),
              ("type_name", "None", "(String) Parameter name"),
              ("default_size", "4", "(Integer) Default size"),
              ("func_CreateDefault", "604002966", "(Integer) CreateDefault function memory location"),
              ("func_Delete", "604003366", "(Integer) Delete function memory location"),
              ("func_SaveLoad", "603996047", "(Integer) SaveLoad function memory location"),
              ("func_Check", "0", "(Integer) Check function memory location"),
              ("func_Copy", "604002468", "(Integer) Copy function memory location"),
              ("func_String", "0", "(Integer) String function memory location"),
              ("func_UICreator", "619055248", "(Integer) UICreator function memory location"),
              ("creator_dll_index", "-1", "(Integer) The id of the dll defining this parameter"),
              ("creator_plugin_index", "-1", "(Integer) The id of the plugin defining this parameter"),
              ("dw_param", "0", "(Integer) An application reserved DWORD for placing parameter type specific data"),
              ("dw_flags", "133", "(Integer) Flags specifying special settings for this parameter type"),
              ("cid", "0", "(Integer) Special case for parameter types that refer to CKObjects => corresponding class ID of the object"),
              ("saver_manager", "1181355948,0", "(String) Int Manager guid"))},
    {"name": "Plugin",
     "queryKey": "plugin",
     "data": (("dll_index", "18", "(Integer) Dll index"),
              ("dll_name", "E:\\Virtools\\Plugins\\ImageReader.dll", "(String) Dll name"),
              ("plugin_index", "2", "(Integer) Plugin index"),
              ("category", "Bitmap Readers", "(String) Plugin category"),
              ("active", "1", "(Integer) For manager and Render engines TRUE if a manager was created, for other plugins this value is not used"),
              ("needed_by_file", "0", "(Integer) When saving a file TRUE if at least one object needs this plugin"),
              ("guid", "1632248895,1132147523", "(String) Plugin guid"),
              ("desc", "Windows Bitmap", "(String) Plugin description"),
              ("author", "Virtools", "(String) Plugin author"),
              ("summary", "Windows Bitmap", "(String) Plugin summary"),
              ("version", "1", "(Integer) Plugin version"),
              ("func_init", "103354496", "(Integer) Init function memory location"),
              ("func_exit", "624432336", "(Integer) Exit function memory location"))},
    {"name": "Variable",
     "queryKey": "variable",
     "data": (("name", "3D XML/ExportVersion", "(String) Variable name"),
              ("description", "Version of exported 3DXML", "(String) Variable description"),
              ("flags", "4", "(Integer) Variable flags"),
              ("type", "1", "(Integer) Variable type"),
              ("representation", "enum:0= 3DXML 3.0; 1= 3DXML 4.0", "(String) The representation (ie the input format) of a variable type"),
              ("data", " 3DXML 3.0", "(String) Variable data"))}
)

legalEnvQueryKey = list(map(lambda x: x['queryKey'], envDatabaseList))


