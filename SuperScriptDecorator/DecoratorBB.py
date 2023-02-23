import sqlite3, collections
import DecoratorData, DecoratorConst

class BuildBBEnvironment(object):
    def __init__(self, 
        cursor: sqlite3.Cursor, tree: DecoratorData.TreeLayout[DecoratorData.BBTreeNodeWrapper], 
        depth: int, query_bb: int):

        self.m_Cursor: sqlite3.Cursor = cursor
        self.m_Tree: DecoratorData.TreeLayout[DecoratorData.BBTreeNodeWrapper] = tree
        self.m_Depth: int = depth
        self.m_QueryBB: int = query_bb

class BuildOperEnvironment(object):
    def __init__(self, 
        cursor: sqlite3.Cursor, tree: DecoratorData.TreeLayout[DecoratorData.OperTreeNodeWrapper], 
        depth: int, query_oper: int):

        self.m_Cursor: sqlite3.Cursor = cursor
        self.m_Tree: DecoratorData.TreeLayout[DecoratorData.OperTreeNodeWrapper] = tree
        self.m_Depth: int = depth
        self.m_QueryOper: int = query_oper

class FindOperRootEnvironment(object):
    def __init__(self, 
        cursor: sqlite3.Cursor, walked_oper: set[int], 
        depth: int, query_oper: int):

        self.m_Cursor: sqlite3.Cursor = cursor
        self.m_WalkedOper: set[int] = walked_oper
        self.m_Depth: int = depth
        self.m_QueryOper: int = query_oper

class BlocksFactory(object):
    def __init__(self, db: sqlite3.Connection, graph: DecoratorData.GraphResult):
        # assign members
        self.m_Graph: DecoratorData.GraphResult = DecoratorData.GraphResult()
        self.m_Graph.m_GraphCKID = graph
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
        cursor.execute('SELECT * FROM [script_behavior] WHERE parent == ?;', (self.m_Graph.m_GraphCKID, ))
        for sqldata in cursor.fetchall():
            payload: DecoratorData.BBDataPayload = DecoratorData.BBDataPayload(sqldata)
            self.m_Graph.m_BBDict[payload.m_CKID] = DecoratorData.BBTreeNodeWrapper(payload)
            self.__AllBB.add(payload.m_CKID)

        cursor.execute('SELECT * FROM [script_pOper] WHERE parent == ?;', (self.m_Graph.m_GraphCKID, ))
        for sqldata in cursor.fetchall():
            payload: DecoratorData.OperDataPayload = DecoratorData.OperDataPayload(sqldata)
            self.m_Graph.m_OperDict[payload.m_CKID] = DecoratorData.BBTreeNodeWrapper(payload)
            self.__AllOper.add(payload.m_CKID)

        cursor.close()

    def __RecursiveFindOperRoot(self, envir: FindOperRootEnvironment) -> tuple[int]:
        envir.m_Cursor.execute('SELECT [output_obj], [output_type], [output_is_bb] FROM [script_pLink] WHERE ([input_obj] == ? AND [input_is_bb] == ? AND NOT (([output_type] == ? OR [output_type] == ?) AND [output_is_bb] == ?) AND [parent] == ?);',
            (envir.m_QueryOper, (DecoratorConst.Export_Boolean.TRUE if envir.m_Depth == 0 else DecoratorConst.Export_Boolean.FALSE), 
            DecoratorConst.Export_pLink_InputOutputType.PIN, DecoratorConst.Export_pLink_InputOutputType.PTARGET, DecoratorConst.Export_Boolean.TRUE,  # filter all BB's pIn
            self.m_Graph.m_GraphCKID)
        )

        opers: set[int] = set()
        results: set[int] = set()

        # read datas
        for (output_obj, output_type, output_is_bb, ) in envir.m_Cursor.fetchall():
            # check dup
            if output_obj in envir.m_WalkedOper:
                continue
            envir.m_WalkedOper.add(output_obj)

            if output_type == DecoratorConst.Export_pLink_InputOutputType.PIN:
                # this is a oper. find its pOut and add into stack
                opers.add(output_obj)
            else:
                # this oper point to non-pIn, should be add into result
                if envir.m_Depth != 0:  # omit BB
                    results.add(envir.m_QueryOper)

        # recursive calling opers to get root
        for oper in opers:
            results.update(self.__RecursiveFindOperRoot(FindOperRootEnvironment(
                envir.m_Cursor, envir.m_WalkedOper, envir.m_Depth + 1, oper
            )))
            
        return tuple(i for i in results)

    def __RecursiveBuildOper(self, envir: BuildOperEnvironment):
        envir.m_Cursor.execute("SELECT [input_obj] FROM [script_pLink] WHERE ([output_obj] == ? AND [output_type] == ? AND [input_type] == ? AND [input_is_bb] == ? AND [parent] = ?);",
            (envir.m_QueryOper, DecoratorConst.Export_pLink_InputOutputType.PIN, # order pOper's pIn
            DecoratorConst.Export_pLink_InputOutputType.POUT, DecoratorConst.Export_Boolean.FALSE,  # order pOper's pOut
            self.m_Graph.m_GraphCKID, )
        )

        gottenOper: list[int] = []
        for (input_obj, ) in envir.m_Cursor.fetchall():
            if input_obj in self.__AllOper:
                self.__AllOper.remove(input_obj)
                gottenOper.append(input_obj)

        if len(gottenOper) == 0:
            return

        if envir.m_Depth == 0:
            # if depth == 0, all item should start from a new layer
            for item in gottenOper:
                envir.m_Tree.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
                envir.m_Tree.NewItem(self.m_Graph.m_OperDict[item])
                self.__RecursiveBuildOper(BuildOperEnvironment(
                    envir.m_Cursor, envir.m_Tree, envir.m_Depth + 1, item
                ))
        else:
            # otherwise, only non-first node need new layer
            # proc first node
            first: int = gottenOper[0]
            envir.m_Tree.NewItem(self.m_Graph.m_OperDict[first])
            self.__RecursiveBuildOper(BuildOperEnvironment(
                envir.m_Cursor, envir.m_Tree, envir.m_Depth + 1, first
            ))

            # get layer and pos
            cur_layer = envir.m_Tree.GetCurrentLayerIndex()
            cur_idx = envir.m_Tree.GetCurrentItemIndex()

            # proc other node
            for other in gottenOper[1:]:
                envir.m_Tree.NewLayer(cur_layer, cur_idx)
                envir.m_Tree.NewItem(self.m_Graph.m_OperDict[other])
                self.__RecursiveBuildOper(BuildOperEnvironment(
                    envir.m_Cursor, envir.m_Tree, envir.m_Depth + 1, other
                ))

    def __RecursiveBuildBB(self, envir: BuildBBEnvironment):
        # query all link for next bb
        envir.m_Cursor.execute('SELECT [output_obj] FROM [script_bLink] WHERE ([input_obj] == ? AND [input_type] == ? AND [parent] = ?) ORDER BY [input_index] ASC;',
            (envir.m_QueryBB,
            (DecoratorConst.Export_bLink_InputOutputType.INPUT if envir.m_Depth == 0 else DecoratorConst.Export_bLink_InputOutputType.OUTPUT),
            self.m_Graph.m_GraphCKID)
        )

        # filter useless bb
        gottenBB: list[int] = []
        for (output_obj, ) in envir.m_Cursor.fetchall():
            if output_obj in self.__AllBB:
                self.__AllBB.remove(output_obj)
                gottenBB.append(output_obj)

        if len(gottenBB) == 0:
            return

        # we need process this BB related opers now
        # for every item
        for bbid in gottenBB:
            bb: DecoratorData.BBTreeNodeWrapper = self.m_Graph.m_BBDict[bbid]
            # upper opers
            self.__RecursiveBuildOper(BuildOperEnvironment(
                envir.m_Cursor, bb.m_UpperOper, 0, bbid
            ))
            # lower opers
            lower_root: tuple[int] = self.__RecursiveFindOperRoot(FindOperRootEnvironment(
                envir.m_Cursor, set(), 0, bbid
            ))
            for operid in lower_root:
                # we need add it manually
                self.__AllOper.remove(operid)
                bb.m_LowerOper.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
                bb.m_LowerOper.NewItem(self.m_Graph.m_OperDict[operid])

                # gotten "root" is oper's CKID, so use depth 1 to cheat this function
                self.__RecursiveBuildOper(BuildOperEnvironment(
                    envir.m_Cursor, bb.m_LowerOper, 1, operid
                ))

        # now we place BB in tree layout
        if envir.m_Depth == 0:
            # if depth == 0, all item should start from a new layer
            for item in gottenBB:
                envir.m_Tree.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
                envir.m_Tree.NewItem(self.m_Graph.m_BBDict[item])
                self.__RecursiveBuildBB(BuildBBEnvironment(
                    envir.m_Cursor, envir.m_Tree, envir.m_Depth + 1, item
                ))
        else:
            # otherwise, only non-first node need new layer
            # proc first node
            first: int = gottenBB[0]
            envir.m_Tree.NewItem(self.m_Graph.m_BBDict[first])
            self.__RecursiveBuildBB(BuildBBEnvironment(
                envir.m_Cursor, envir.m_Tree, envir.m_Depth + 1, first
            ))

            # get layer and pos
            cur_layer = envir.m_Tree.GetCurrentLayerIndex()
            cur_idx = envir.m_Tree.GetCurrentItemIndex()

            # proc other node
            for other in gottenBB[1:]:
                envir.m_Tree.NewLayer(cur_layer, cur_idx)
                envir.m_Tree.NewItem(self.m_Graph.m_BBDict[other])
                self.__RecursiveBuildBB(BuildBBEnvironment(
                    envir.m_Cursor, envir.m_Tree, envir.m_Depth + 1, other
                ))

        
    def __ProcActiveBB(self):
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        self.__RecursiveBuildBB(BuildBBEnvironment(
            cursor, self.m_Graph.m_ActivePassiveBB, 0, self.m_Graph.m_GraphCKID
        ))
        cursor.close()

    def __ProcPassiveBB(self):
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        while len(self.__AllBB) != 0:
            # we manually add first
            bbid = self.__GetOneFromSet(self.__AllBB)
            
            self.__AllBB.remove(bbid)
            self.m_Graph.m_ActivePassiveBB.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
            self.m_Graph.m_ActivePassiveBB.NewItem(self.m_Graph.m_BBDict[bbid])

            # use depth = 1 to cheat this function for avoiding error bIO type
            self.__RecursiveBuildBB(BuildBBEnvironment(
                cursor, self.m_Graph.m_ActivePassiveBB, 1, bbid
            ))
        cursor.close()

    def __ProcPassiveOper(self):
        cursor: sqlite3.Cursor = self.m_Db.cursor()
        while len(self.__AllOper) != 0:
            # peek one randomly and try finding root
            peek: int = self.__GetOneFromSet(self.__AllOper)
            roots: tuple[int] = self.__RecursiveFindOperRoot(FindOperRootEnvironment(
                cursor, set(), 1, peek    # use 1 to cheat this function
            ))

            # if no root found, add self
            roots = (peek, )

            # generate tree
            for operid in roots:
                # we need add it manually
                self.__AllOper.remove(operid)
                self.m_Graph.m_PassiveOper.NewLayer(DecoratorData.TreeLayout.NO_REFERENCE_LAYER, DecoratorData.TreeLayout.NO_START_POS_OF_REF_LAYER)
                self.m_Graph.m_PassiveOper.NewItem(self.m_Graph.m_OperDict[operid])

                # gotten "roots" is oper's CKID, so use depth 1 to cheat this function
                self.__RecursiveBuildOper(BuildOperEnvironment(
                    cursor, self.m_Graph.m_PassiveOper, 1, operid
                ))

        cursor.close()

