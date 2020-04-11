FONT_SIZE = 12

GRAPH_POFFSET = 40          # 主体bb的p距离左边框距离
GRAPH_BOFFSET = 40          # 主体bb的b距离上边框距离
GRAPH_PSPAN = 20            # 主体bb的每个p之间的水平间距
GRAPH_BSPAN = 20            # 主体bb的每个b之间的垂直间距
GRAPH_LAYER_SPAN = 50       # 绘图中各个bb层之间的间距
GRAPH_BB_SPAN = 25          # 绘图中每个bb之间的距离
GRAPH_SPAN_BB_POPER = 60    # 每个bb上面挂载的oper和被挂载bb之间的垂直距离 和 每个bb上面挂载的oper的各层之间的垂直距离
GRAPH_CONTENTOFFSET_X = 40  # 绘图内容原点到左边框的距离
GRAPH_CONTENTOFFSET_Y = 40  # 绘图内容原点到顶边框的距离

BB_POFFSET = 20 # bb框中的p距离左边框距离
BB_BOFFSET = 10 # bb框中的b距离上边框距离
BB_PSPAN = 20   # bb框每个p之间的水平间距
BB_BSPAN = 20   # bb框每个b之间的垂直间距
BB_PBSIZE = 6   # bb框中o和b的正方形符号的边长

CELL_WIDTH = 15
CELL_HEIGHT = 5

class dbPLinkInputOutputType(object):
    PIN = 0
    POUT = 1
    PLOCAL = 2
    PTARGET = 3

class dbBLinkInputOutputType(object):
    INPUT = 0
    OUTPUT = 1

class LinkType(object):
    PLOCAL = 0
    SHORTCUR = 1
    PIO = 2
    BIO = 3

class BBTreeNode(object):
    def __init__(self, ckid, layer):
        self.bb = ckid
        self.layer = layer
        self.nodes = []

class BBResult(object):
    def __init__(self, name, assistName, pin, pout, bin, bout, expandable):
        self.name = name
        self.assistName = assistName
        self.pin = int(pin)
        self.pinData = []
        self.pout = int(pout)
        self.poutData = []
        self.bin = int(bin)
        self.binData = []
        self.bout = int(bout)
        self.boutData = []
        self.x = 0.0
        self.y = 0.0
        self.width = 0.0
        self.height = 0.0
        self.expandable = expandable

    def computSize(self):
        wText = max(len(self.name), len(self.assistName)) * FONT_SIZE / 2
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
        self.pinData = []
        self.poutData = []
        self.height = 0.0
        self.width = 0.0

    def computSize(self):
        wText = len(self.name) * FONT_SIZE / 2
        hText = FONT_SIZE * 3

        wp = 2 * BB_POFFSET + 2 * (BB_PBSIZE + BB_PSPAN)
        hb = 2 * BB_BOFFSET + 0 * (BB_PBSIZE + BB_BSPAN)

        self.width = max(wp, wText)
        self.height = max(hb, hText)
