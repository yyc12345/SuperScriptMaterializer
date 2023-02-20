import typing, collections, sqlite3

class Vector(object):
    def __init__(self, x: float, y: float):
        self.X: float = x
        self.Y: float = y
    def __add__(self, other):
        if not isinstance(other, Vector): return NotImplemented
        return Vector(self.X + other.X, self.Y + other.Y)
    def __sub__(self, other):
        if not isinstance(other, Vector): return NotImplemented
        return Vector(self.X - other.X, self.Y - other.Y)

    def __radd__(self, other):
        if not isinstance(other, Vector): return NotImplemented
        return Vector(self.X + other.X, self.Y + other.Y)
    def __rsub__(self, other):
        if not isinstance(other, Vector): return NotImplemented
        return Vector(self.X - other.X, self.Y - other.Y)

    def __iadd__(self, other):
        if not isinstance(other, Vector): return NotImplemented
        self.X += other.X
        self.Y += other.Y
        return self
    def __isub__(self, other):
        if not isinstance(other, Vector): return NotImplemented
        self.X -= other.X
        self.Y -= other.Y
        return self

    def __eq__(self, other) -> bool:
        if not isinstance(other, Vector): return NotImplemented
        return (self.X == other.X and self.Y == other.Y)
    def __ne__(self, other) -> bool:
        if not isinstance(other, Vector): return NotImplemented
        return (self.X != other.X or self.Y != other.Y)

class ICanComputeSize(object):
    '''
    This class is served for TreeLayout class.

    All classes inherit this class will have ability to calculate the size of self,
    and report to TreeLayout. TreeLayout will use these data to distribute position.

    The functions declared in this class have unique name meaning. 
    The reason is that the vertical and horizonal direction between BB and Oper is opposited. 
    So we use Leaves and Root to give theme an uniformed concept.
    '''
    def GetLeavesDirLength() -> float:
        '''
        Get the length of the direction where leaves grow up.
        '''
        raise NotImplementedError()
    def GetRootDirLength() -> float:
        '''
        Get the length of the direction where the tree planted.
        
        '''
        raise NotImplementedError()

TNode = typing.TypeVar('TNode')
class TreeLayout(typing.Generic[TNode]):
    pass

SqlTableProto_Behavior = collections.namedtuple('Behavior', 'thisobj, name, type, proto_name, proto_guid, flags, priority, version, pin_count, parent')

class BBDataPayload(object):
    def __init__(self, sql_data: tuple):
        v = SqlTableProto_Behavior._make(sql_data)

        self.m_CKID: int = v.thisobj
        self.m_Name: str = v.name
        self.m_Type: int = v.type
        self.m_ProtoName: str = v.proto_name
        self.m_ProtoGUID: str = v.proto_guid
        self.m_Flags: int = v.flags
        self.m_Priority: int = v.priority
        self.m_Version: int = v.version
        self.m_Parent: int = v.parent

        div_pin_count = v.pin_count.split(',')
        self.m_pTargetExisting: bool = int(div_pin_count[0] == 1)
        self.m_pInCount: int = int(div_pin_count[1])
        self.m_pOutCount: int = int(div_pin_count[2])
        self.m_bInCount: int = int(div_pin_count[3])
        self.m_bOutCount: int = int(div_pin_count[4])

class OperTreeNode(ICanComputeSize):
    def __init__(self):
        pass

class BBTreeNode(ICanComputeSize):
    def __init__(self):
        self.m_UpperOper: TreeLayout[OperTreeNode] = TreeLayout()
        self.m_LowerOper: TreeLayout[OperTreeNode] = TreeLayout()
        self.m_Upperval: collections.deque = collections.deque()
        self.m_LowerVal: collections.deque = collections.deque()
        self.m_Payload: BBDataPayload = None


class GraphWork(ICanComputeSize):
    def __init__(self):
        self.m_PassiveVal: collections.deque = collections.deque()
        self.m_PassiveOper: TreeLayout[OperTreeNode] = TreeLayout()
        self.m_ActiveBB: TreeLayout[BBTreeNode] = TreeLayout()
        self.m_PassiveBB: TreeLayout[BBTreeNode] = TreeLayout()


