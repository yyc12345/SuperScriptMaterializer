import typing, collections, sqlite3

class Vector(object):
    def __init__(self):
        self.X: float = 0.0
        self.Y: float = 0.0
    def __init__(self, x: float, y: float):
        self.X: float = x
        self.Y: float = y

    @property
    def LenLeavesDir(self):
        return self.X
    @LenLeavesDir.setter
    def LenLeavesDir(self, v):
        self.X = v
    @property
    def LenRootsDir(self):
        return self.Y
    @LenRootsDir.setter
    def LenRootsDir(self, v):
        self.Y = v

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

class Margin(object):
    def __init__(self):
        self.m_TopSize: Vector = Vector()
        self.m_BodySize: Vector = Vector()
        self.m_BottomSize: Vector = Vector()

class ICanComputeSize(object):
    '''
    This class is served for TreeLayout class.

    All classes inherit this class will have ability to calculate the size of self,
    and report to TreeLayout. TreeLayout will use these data to distribute position.

    The functions declared in this class have unique name meaning. 
    The reason is that the vertical and horizonal direction between BB and Oper is opposited. 
    So we use Leaves and Root to give theme an uniformed concept.
    '''
    def GetPos() -> Vector:
        '''
        Get current node's start position.
        '''
        raise NotImplementedError()
    def GetSize() -> Vector:
        '''
        Get current node's size.
        '''
        raise NotImplementedError()
    def ComputeSize() -> Vector:
        '''
        Get current node's start position
        '''
        raise NotImplementedError()

TNode = typing.TypeVar('TNode', bound=ICanComputeSize)

class TreeLayoutLayer(typing.Generic[TNode]):
    def __init__(self, ref_layer: int, start_pos_of_ref_layer: int):
        self.m_Container: collections.deque[TNode] = collections.deque()
        self.m_RefLayer: int = ref_layer
        self.m_RefLayerIndex: int = start_pos_of_ref_layer

class TreeLayout(typing.Generic[TNode]):
    NO_REFERENCE_LAYER: int = -1
    NO_START_POS_OF_REF_LAYER: int = -1

    def __init__(self):
        self.m_Layers: collections.deque[TreeLayoutLayer[TNode]] = collections.deque()
        self.__CurrentLayer: TreeLayoutLayer[TNode] = None

    def GetCurrentLayerIndex(self) -> int:
        if self.__CurrentLayer is None: raise Exception("No layer!")
        return len(self.m_Layers) - 1

    def GetCurrentItemIndex(self) -> int:
        if self.__CurrentLayer is None: raise Exception("No layer!")
        result = len(self.__CurrentLayer)
        if result == 0: raise Exception("No item!")
        return result - 1

    def NewLayer(self, ref_layer: int, start_pos_of_ref_layer: int):
        self.__CurrentLayer = TreeLayoutLayer(ref_layer, start_pos_of_ref_layer)
        self.m_Layers.append(self.__CurrentLayer)

    def NewItem(self, node: TNode):
        if self.__CurrentLayer is None: raise Exception("No layer to append data!")
        self.__CurrentLayer.m_Container.append(node)


class BBDataPayload(object):
    SqlTableProto = collections.namedtuple('Behavior', 'thisobj, name, type, proto_name, proto_guid, flags, priority, version, pin_count, parent')

    def __init__(self, sql_data: tuple):
        v = BBDataPayload.SqlTableProto._make(sql_data)

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

class OperDataPayload(object):
    SqlTableProto = collections.namedtuple('Oper', 'thisobj, name, op_guid, parent')

    def __init__(self, sql_data: tuple):
        v = OperDataPayload.SqlTableProto._make(sql_data)
        self.m_CKID: int = v.thisobj
        self.m_Name: str = v.name
        self.m_OpGuid: str = v.op_guid
        self.m_Parent: int = v.parent

class OperTreeNode(ICanComputeSize):
    def __init__(self, payload: OperDataPayload):
        self.m_Payload: OperDataPayload = payload

class BBTreeNode(ICanComputeSize):
    def __init__(self, payload: BBDataPayload):
        self.m_UpperOper: TreeLayout[OperTreeNode] = TreeLayout()
        self.m_LowerOper: TreeLayout[OperTreeNode] = TreeLayout()
        self.m_Upperval: collections.deque = collections.deque()
        self.m_LowerVal: collections.deque = collections.deque()
        self.m_Payload: BBDataPayload = payload


class GraphWork(ICanComputeSize):
    def __init__(self):
        self.m_PassiveVal: collections.deque = collections.deque()
        self.m_PassiveOper: TreeLayout[OperTreeNode] = TreeLayout()
        self.m_ActiveBB: TreeLayout[BBTreeNode] = TreeLayout()
        self.m_PassiveBB: TreeLayout[BBTreeNode] = TreeLayout()


