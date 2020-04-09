FONT_SIZE = 12

GRAPH_POFFSET = 40
GRAPH_BOFFSET = 40
GRAPH_PSPAN = 20
GRAPH_BSPAN = 20
GRAPH_LAYER_SPAN = 50
GRAPH_BB_SPAN = 25

BB_POFFSET = 20
BB_BOFFSET = 10
BB_PSPAN = 20
BB_BSPAN = 20
BB_PBSIZE = 6

CELL_WIDTH = 15
CELL_HEIGHT = 5

GRAPH_CONTENTOFFSET_X = 40
GRAPH_CONTENTOFFSET_Y = 40


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
        self.pout = int(pout)
        self.bin = int(bin)
        self.bout = int(bout)
        self.x = 0.0
        self.y = 0.0
        self.width = 0.0
        self.height = 0.0
        self.expandable = expandable

    def computSize(self):
        wText = max(len(self.name), len(self.assistName)) * FONT_SIZE
        hText = FONT_SIZE * 4

        wp = 2 * BB_POFFSET + max(self.pin, self.pout) * (BB_PBSIZE + BB_PSPAN)
        hb = 2 * BB_BOFFSET + max(self.bin, self.bout) * (BB_PBSIZE + BB_BSPAN)

        self.width = max(wp, wText)
        self.height = max(hb, hText)

class pOperArrangement(object):
    def __init__(self, attachedBB, sublayer):
        self.attachedBB = attachedBB
        self.sublayer = sublayer

class OperResult(object):
    def __init__(self, name, x):
        self.name = name
        self.x = x
        self.y = 0.0
        self.height = 0.0
        self.width = 0.0

    def computSize(self):
        wText = len(self.name) * FONT_SIZE
        hText = FONT_SIZE * 4

        wp = 2 * BB_POFFSET + 2 * (BB_PBSIZE + BB_PSPAN)
        hb = 2 * BB_BOFFSET + 0 * (BB_PBSIZE + BB_BSPAN)

        self.width = max(wp, wText)
        self.height = max(hb, hText)
