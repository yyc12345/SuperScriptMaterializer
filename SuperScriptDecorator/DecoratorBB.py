import sqlite3, collections
import DecoratorData, DecoratorConst

class IntermediateTreeNode(object):
    def __init__(self):
        self.m_FirstVisit: bool = True
        self.m_LoactedLayer: int = DecoratorData.TreeLayout.NO_REFERENCE_LAYER
        self.m_LocatedPos: int = DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER
        self.m_Collection: collections.deque[int] = collections.deque()

class BlocksFactory(object):
    def __init__(self, db: sqlite3.Connection, graph: int):
        # assign members
        self.m_GraphCKID: int = graph
        self.m_Db: sqlite3.Connection = db
        self.__Cursor: sqlite3.Cursor = db.cursor()

        self.m_BBDict: dict[int, DecoratorData.BBTreeNode] = {}
        self.m_OperDict: dict[int, DecoratorData.OperTreeNode] = {}
        self.__AllBB: set[int] = set()
        self.__AllOper: set[int] = set()

        self.m_PassiveOperLayer: DecoratorData.TreeLayout = DecoratorData.TreeLayout()
        self.m_ActiveBBLayer: DecoratorData.TreeLayout = DecoratorData.TreeLayout()
        self.m_PassiveBBLayer: DecoratorData.TreeLayout = DecoratorData.TreeLayout()

        # fill data first
        self.__FillDataFromDb()

        # proc each data
        self.__ProcActiveBB()
        self.__ProcPassiveBB()
        self.__ProcPassiveOper()


    def __GetOneFromSet(self, vals: set[int]) -> int:
        return next(iter(vals))

    def __FillDataFromDb(self):
        self.__Cursor.execute('SELECT * FROM [script_behavior] WHERE parent == ?', (self.m_GraphCKID, ))
        for sqldata in self.__Cursor.fetchall():
            payload: DecoratorData.BBDataPayload = DecoratorData.BBDataPayload(sqldata)
            self.m_BBDict[payload.m_CKID] = DecoratorData.BBTreeNode(payload)
            self.__AllBB.add(payload.m_CKID)

        self.__Cursor.execute('SELECT * FROM [script_pOper] WHERE parent == ?', (self.m_GraphCKID, ))
        for sqldata in self.__Cursor.fetchall():
            payload: DecoratorData.OperDataPayload = DecoratorData.OperDataPayload(sqldata)
            self.m_OperDict[payload.m_CKID] = DecoratorData.OperTreeNode(payload)
            self.__AllOper.add(payload.m_CKID)

    def __FindBBBottomOperRoot(self, start_pOut: int) -> tuple[int]:
        pass

    def __BuildOper(self, tree: DecoratorData.TreeLayout, start_oper: int):
        pass

    def __BuildBB(self, tree: DecoratorData.TreeLayout, start_bb: int):
        # prepare stack and start condition
        stack: collections.deque[IntermediateTreeNode] = collections.deque()
        
        # check whether this BB has been processed
        if start_bb not in self.__AllBB:
            return
        self.__AllBB.remove(start_bb)

        # create new layer and insert it
        tree.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
        tree.NewItem(self.m_BBDict[start_bb])

        # prepare stack data
        start_bb_data: IntermediateTreeNode = IntermediateTreeNode()
        start_bb_data.m_LoactedLayer = tree.GetCurrentLayerIndex()
        start_bb_data.m_LocatedPos = tree.GetCurrentItemIndex()
        self.__Cursor.execute("SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
            (start_bb, DecoratorConst.Database_bLink_InputOutputType.OUTPUT, self.m_GraphCKID, )
        )
        for (output_obj, ) in self.__Cursor.fetchall():
            start_bb_data.m_Collection.append(output_obj)
        stack.append(start_bb_data)

        # start analyze
        while len(stack) != 0:
            # pick stack and decide whether pop this
            stack_entry: IntermediateTreeNode = stack[-1]
            if len(stack_entry.m_Collection) == 0:
                stack.pop()
                continue

            # pick a new BB from collection.
            # check whether it need process
            bb_id: int = stack_entry.m_Collection[-1]
            if bb_id not in self.__AllBB:
                stack_entry.m_Collection.pop()
                continue
            self.__AllBB.remove(bb_id)

            # get corresponding tree node
            tree_node = self.m_BBDict[bb_id]

            # start proc BB
            # create new stack entry
            bb_data: IntermediateTreeNode = IntermediateTreeNode()
            # push tree node into tree layout and get corresponding index.
            # create new layer according to whether first visit
            if stack_entry.m_FirstVisit:
                tree.NewItem(tree_node)

                bb_data.m_LoactedLayer = stack_entry.m_LoactedLayer
                bb_data.m_LocatedPos = stack_entry.m_LocatedPos + 1
                # reset first visit
                stack_entry.m_FirstVisit = False
            else:
                tree.NewLayer(stack_entry.m_LoactedLayer, stack_entry.m_LocatedPos)
                tree.NewItem(tree_node)

                bb_data.m_LoactedLayer = tree.GetCurrentLayerIndex()
                bb_data.m_LocatedPos = tree.GetCurrentItemIndex()
            # query and fill stack entry data
            self.__Cursor.execute("SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
                (bb_id, DecoratorConst.Database_bLink_InputOutputType.OUTPUT, self.m_GraphCKID, )
            )
            for (output_obj, ) in self.__Cursor.fetchall():
                bb_data.m_Collection.append(output_obj)
            stack.append(bb_data)

            # todo: executing oper finder for current bb

    def __ProcActiveBB(self):
        self.__Cursor.execute("SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
            (self.m_GraphCKID, DecoratorConst.Database_bLink_InputOutputType.INPUT, self.m_GraphCKID, )
        )
        for (output_obj, ) in self.__Cursor.fetchall():
            self.__BuildBB(self.m_ActiveBBLayer, output_obj)

    def __ProcPassiveBB(self):
        while len(self.__AllBB) != 0:
            self.__BuildBB(self.m_PassiveBBLayer, self.__GetOneFromSet(self.__AllBB))

    def __ProcPassiveOper(self):
        pass
