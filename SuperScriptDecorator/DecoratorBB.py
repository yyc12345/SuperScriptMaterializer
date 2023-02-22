import sqlite3, collections
import DecoratorData, DecoratorConst

class IntermediateTreeNode(object):
    def __init__(self):
        self.m_FirstVisit: bool = True
        self.m_LoactedLayer: int = DecoratorData.TreeLayout.NO_REFERENCE_LAYER
        self.m_LocatedPos: int = DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER
        self.m_Collection: collections.deque[int] = collections.deque()

class IntermediateOperDownwardSearch(object):
    def __init__(self):
        self.m_Collection: collections.deque[int] = collections.deque()

class BlocksFactory(object):
    def __init__(self, db: sqlite3.Connection, graph: DecoratorData.GraphResult):
        # assign members
        self.m_Graph: DecoratorData.GraphResult = graph
        self.m_Db: sqlite3.Connection = db

        self.__AllBB: set[int] = set()
        self.__AllOper: set[int] = set()

        # fill data first
        self.__FillDataFromDb()

        # proc each data
        self.__ProcActiveBB()
        self.__ProcPassiveBB()
        self.__ProcPassiveOper()


    def __GetOneFromSet(self, vals: set[int]) -> int:
        return next(iter(vals))

    def __FillDataFromDb(self):
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        cursor.execute('SELECT * FROM [script_behavior] WHERE parent == ?', (self.m_Graph.m_GraphCKID, ))
        for sqldata in cursor.fetchall():
            payload: DecoratorData.BBDataPayload = DecoratorData.BBDataPayload(sqldata)
            self.m_Graph.m_BlockDict[payload.m_CKID] = DecoratorData.BBTreeNode(payload)
            self.__AllBB.add(payload.m_CKID)

        cursor.execute('SELECT * FROM [script_pOper] WHERE parent == ?', (self.m_Graph.m_GraphCKID, ))
        for sqldata in cursor.fetchall():
            payload: DecoratorData.OperDataPayload = DecoratorData.OperDataPayload(sqldata)
            self.m_Graph.m_BlockDict[payload.m_CKID] = DecoratorData.OperTreeNode(payload)
            self.__AllOper.add(payload.m_CKID)

        cursor.close()

    def __FindBBBottomOperRoot(self, start_pOut: tuple[int]) -> tuple[int]:
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        cursor_outget: sqlite3.Cursor = self.m_Db.cursor()

        # prepare something
        stack: collections.deque[int] = collections.deque()
        procedOut: set[int] = set()
        result: set[int] = set()
        # prepare start symbol
        stack.extend(start_pOut)

        # analyze
        while len(stack) != 0:
            # peek again
            pout: int = stack.pop()

            # search this pOut in pLink
            cursor.execute('SELECT [output], [output_obj], [output_type], [output_is_bb] FROM [script_pLink] WHERE ([input] == ? AND NOT (([output_type] == ? OR [output_type] == ?) AND [output_is_bb] == ?) AND [parent] == ?);',
                (pout, 
                DecoratorConst.Export_pLink_InputOutputType.PIN, DecoratorConst.Export_pLink_InputOutputType.PTARGET, DecoratorConst.Export_Boolean.TRUE,  # filter all BB's pIn
                self.m_Graph.m_GraphCKID)
            )

            # read datas
            for (output, output_obj, output_type, output_is_bb, ) in cursor.fetchall():
                # check dup
                if output in procedOut:
                    continue
                procedOut.add(output)

                if output_type == DecoratorConst.Export_pLink_InputOutputType.PIN and output_is_bb == DecoratorConst.Export_Boolean.FALSE:
                    # this is a oper. find its pOut and add into stack
                    cursor_outget.execute('SELECT [thisobj] FROM [script_pOut] WHERE [parent] == ?;',
                        (output_obj, )
                    )
                    for (thisobj, ) in cursor_outget.fetchall():
                        stack.append(thisobj)
                else:
                    # this oper point to non-pIn, should be add into result
                    result.add(output_obj)
                    
        # free cursor and return
        cursor.close()
        cursor_outget.close()
        return tuple(i for i in result)

    def __BuildOper(self, tree: DecoratorData.TreeLayout[DecoratorData.OperTreeNode], start_oper: int):
        # prepare stack and start condition
        stack: collections.deque[IntermediateTreeNode] = collections.deque()
        
        # check whether this BB has been processed
        if start_oper not in self.__AllOper:
            return
        self.__AllOper.remove(start_oper)

        # create cursor
        cursor: sqlite3.Cursor = self.m_Db.cursor()

        # create new layer and insert it
        tree.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
        tree.NewItem(self.m_Graph.m_BlockDict[start_oper])

        # prepare stack data
        start_oper_data: IntermediateTreeNode = IntermediateTreeNode()
        start_oper_data.m_LoactedLayer = tree.GetCurrentLayerIndex()
        start_oper_data.m_LocatedPos = tree.GetCurrentItemIndex()
        cursor.execute("SELECT [input_obj] FROM [script_pLink] WHERE ([output_obj] == ? AND [output_type] == ? AND [input_type] == ? AND [input_is_bb] == ? AND [parent] = ?);",
            (start_oper, DecoratorConst.Export_pLink_InputOutputType.PIN, # order pOper's pIn
            DecoratorConst.Export_pLink_InputOutputType.POUT, DecoratorConst.Export_Boolean.FALSE,  # order pOper's pOut
            self.m_Graph.m_GraphCKID, )
        )
        for (input_obj, ) in cursor.fetchall():
            start_oper_data.m_Collection.append(input_obj)
        stack.append(start_oper_data)

        # start analyze
        while len(stack) != 0:
            # pick stack and decide whether pop this
            stack_entry: IntermediateTreeNode = stack[-1]
            if len(stack_entry.m_Collection) == 0:
                stack.pop()
                continue

            # pick a new Oper from collection.
            # check whether it need process
            oper_id: int = stack_entry.m_Collection[-1]
            if oper_id not in self.__AllOper:
                stack_entry.m_Collection.pop()
                continue
            self.__AllOper.remove(oper_id)

            # get corresponding tree node
            tree_node: DecoratorData.BBTreeNode = self.m_Graph.m_BlockDict[oper_id]

            # start proc Oper
            # create new stack entry
            oper_data: IntermediateTreeNode = IntermediateTreeNode()
            # push tree node into tree layout and get corresponding index.
            # create new layer according to whether first visit
            if stack_entry.m_FirstVisit:
                tree.NewItem(tree_node)

                oper_data.m_LoactedLayer = stack_entry.m_LoactedLayer
                oper_data.m_LocatedPos = stack_entry.m_LocatedPos + 1
                # reset first visit
                stack_entry.m_FirstVisit = False
            else:
                tree.NewLayer(stack_entry.m_LoactedLayer, stack_entry.m_LocatedPos)
                tree.NewItem(tree_node)

                oper_data.m_LoactedLayer = tree.GetCurrentLayerIndex()
                oper_data.m_LocatedPos = tree.GetCurrentItemIndex()
            # query and fill stack entry data
            cursor.execute("SELECT [input_obj] FROM [script_pLink] WHERE ([output_obj] == ? AND [output_type] == ? AND [input_type] == ? AND [input_is_bb] == ? AND [parent] = ?);",
                (oper_id, DecoratorConst.Export_pLink_InputOutputType.PIN, # order pOper's pIn
                DecoratorConst.Export_pLink_InputOutputType.POUT, DecoratorConst.Export_Boolean.FALSE,  # order pOper's pOut
                self.m_Graph.m_GraphCKID, )
            )
            for (output_obj, ) in cursor.fetchall():
                oper_data.m_Collection.append(output_obj)
            stack.append(oper_data)

        cursor.close()

    def __BuildBB(self, tree: DecoratorData.TreeLayout[DecoratorData.BBTreeNode], start_bb: int):
        # prepare stack and start condition
        stack: collections.deque[IntermediateTreeNode] = collections.deque()
        
        # check whether this BB has been processed
        if start_bb not in self.__AllBB:
            return
        self.__AllBB.remove(start_bb)

        # create cursor
        cursor: sqlite3.Cursor = self.m_Db.cursor()

        # create new layer and insert it
        tree.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
        tree.NewItem(self.m_Graph.m_BlockDict[start_bb])

        # prepare stack data
        start_bb_data: IntermediateTreeNode = IntermediateTreeNode()
        start_bb_data.m_LoactedLayer = tree.GetCurrentLayerIndex()
        start_bb_data.m_LocatedPos = tree.GetCurrentItemIndex()
        cursor.execute("SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [parent] = ?) ORDER BY [input_index] ASC;",
            (start_bb, DecoratorConst.Export_bLink_InputOutputType.OUTPUT, self.m_Graph.m_GraphCKID, )
        )
        for (output_obj, ) in cursor.fetchall():
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
            tree_node: DecoratorData.BBTreeNode = self.m_Graph.m_BlockDict[bb_id]

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
            cursor.execute("SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
                (bb_id, DecoratorConst.Export_bLink_InputOutputType.OUTPUT, self.m_Graph.m_GraphCKID, )
            )
            for (output_obj, ) in cursor.fetchall():
                bb_data.m_Collection.append(output_obj)
            stack.append(bb_data)

            # todo: executing oper finder for current bb

        cursor.close()

    def __ProcActiveBB(self):
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        cursor.execute("SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
            (self.m_Graph.m_GraphCKID, DecoratorConst.Export_bLink_InputOutputType.INPUT, self.m_Graph.m_GraphCKID, )
        )
        for (output_obj, ) in cursor.fetchall():
            self.__BuildBB(self.m_Graph.m_ActiveBB, output_obj)

        cursor.close()

    def __ProcPassiveBB(self):
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        while len(self.__AllBB) != 0:
            self.__BuildBB(self.m_PassiveBBLayer, self.__GetOneFromSet(self.__AllBB))

        cursor.close()

    def __ProcPassiveOper(self):
        pass
