import sqlite3
import DecoratorData

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

    def __BuildBB(self, tree: DecoratorData.TreeLayout, start_io: tuple[int]):
        pass

