import json

FONT_SIZE = 12

GRAPH_POFFSET = 40          # 主体bb的p距离左边框距离
GRAPH_BOFFSET = 40          # 主体bb的b距离上边框距离
GRAPH_PSPAN = 20            # 主体bb的每个p之间的水平间距
GRAPH_BSPAN = 20            # 主体bb的每个b之间的垂直间距
GRAPH_LAYER_SPAN = 50       # 绘图中各个bb层之间的间距
GRAPH_BB_SPAN = 25          # 绘图中每个bb之间的距离（扩展到Oper和不可插入的local之间的距离）
GRAPH_SPAN_BB_POPER = 60    # 每个bb上面挂载的oper和被挂载bb之间的垂直距离 和 每个bb上面挂载的oper的各层之间的垂直距离
GRAPH_SPAN_BB_PLOCAL = 10   # 每个bb上面挂载的plocal和被挂载bb之间的垂直距离
GRAPH_CONTENTOFFSET_X = 40  # 绘图内容原点到左边框的距离
GRAPH_CONTENTOFFSET_Y = 40  # 绘图内容原点到顶边框的距离
BB_POFFSET = 20 # bb框中的p距离左边框距离
BB_BOFFSET = 10 # bb框中的b距离上边框距离
BB_PSPAN = 20   # bb框每个p之间的水平间距
BB_BSPAN = 20   # bb框每个b之间的垂直间距
BB_PBSIZE = 6   # bb框中o和b的正方形符号的边长（扩展到graph中的p/b大小）
CELL_WIDTH = 15
CELL_HEIGHT = 5

class dbPLinkInputOutputType(object):
    PIN = 0
    POUT = 1
    PLOCAL = 2
    PTARGET = 3
    PATTR = 4

class dbBLinkInputOutputType(object):
    INPUT = 0
    OUTPUT = 1

class CellType(object):
    PLOCAL = 0
    SHORTCUT = 1
    PIO = 2
    BIO = 3
    PTARGET = 4

class LocalUsageType(object):
    PIN = 0
    POUT = 1
    PLOCAL = 2
    PATTR = 3

class JsonCustomEncoder(json.JSONEncoder):
    def default(self, field): 
        if isinstance(field, PinInformation):
            return {'id': field.id, 'name': field.name, 'type': field.type}
        else: 
            return json.JSONEncoder.default(self, field)

class BlockCellItem(object):
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h

class BBTreeNode(object):
    def __init__(self, ckid, layer):
        self.bb = ckid
        self.layer = layer
        self.nodes = []

class BBResult(object):
    def __init__(self, name, assistName, pin, pout, bin, bout, expandable):
        self.name = name
        self.assistName = assistName
        self.ptargetData = None
        self.pin = int(pin)
        self.pinData = None
        self.pout = int(pout)
        self.poutData = None
        self.bin = int(bin)
        self.binData = None
        self.bout = int(bout)
        self.boutData = None
        self.x = 0.0
        self.y = 0.0
        self.width = 0.0
        self.height = 0.0
        self.expandable = expandable

    def computSize(self):
        wText = max(len(self.name), len(self.assistName)) * FONT_SIZE / 3 * 2
        hText = FONT_SIZE * 3

        wp = max(self.pin, self.pout) * (BB_PBSIZE + BB_PSPAN)
        hb = max(self.bin, self.bout) * (BB_PBSIZE + BB_BSPAN)

        self.width = 2 * BB_POFFSET + max(wp, wText)
        self.height = 2 * BB_BOFFSET + max(hb, hText)

class OperResult(object):
    def __init__(self, name):
        self.name = name
        self.x = 0.0
        self.y = 0.0
        self.pinData = None
        self.poutData = None
        self.height = 0.0
        self.width = 0.0

    def computSize(self):
        wText = len(self.name) * FONT_SIZE / 3 * 2
        hText = FONT_SIZE * 3

        wp = 2 * BB_POFFSET + 2 * (BB_PBSIZE + BB_PSPAN)
        hb = 2 * BB_BOFFSET + 0 * (BB_PBSIZE + BB_BSPAN)

        self.width = max(wp, wText)
        self.height = max(hb, hText)

class PinInformation(object):
    def __init__(self, id, name, type):
        self.id = id
        self.name = name
        self.type = type

class LocalUsageItem(object):
    def __init__(self, count, isshortcut, internal_type):
        self.count = count
        self.lastUse = -1
        self.lastDirection = 0 # 0 for pIn, 1 for pOut
        self.lastIndex = -1    # -1 for pTarget, otherwise the real index
        self.isshortcut = isshortcut
        self.internal_type = internal_type  # 0 pIn, 1 pOut, 2 pLocal. for convenient query match data