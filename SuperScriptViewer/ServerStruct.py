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
              ("name", "OnClick", "(String) Message name"))}
)
