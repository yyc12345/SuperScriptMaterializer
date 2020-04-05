import sqlite3
import DecoratorConstValue as dcv
import queue

def run():
    exportDb = sqlite3.connect('export.db')
    decorateDb = sqlite3.connect('decorate.db')
    exportCur = exportDb.cursor()
    decorateCur = decorateDb.cursor()

    # init table
    print('Init decorate.dll')
    initDecorateDb(decorateCur)
    decorateDb.commit()

    # decorate graph
    graphList = []
    decorateGraph(exportCur, decorateCur, graphList)

    # decorate each graph
    for i in graphList:
        buildBBTree(exportCur, decorateCur, i)

    # give up all change of eexport.db (because no change)
    exportDb.close()
    decorateDb.commit()
    decorateDb.close()

def initDecorateDb(cur):
    cur.execute("CREATE TABLE graph([graph] INTEGER, [width] INTEGER, [height] INTEGER, [index] INTEGER, [belong_to] TEXT);")
    cur.execute("CREATE TABLE info([target] INTEGER, [field] TEXT, [data] TEXT);")

    cur.execute("CREATE TABLE block([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [pin-pin] INTEGER, [pin-pout] INTEGER, [pin-bin] INTEGER, [pin-bout] INTEGER, [x] INTEGER, [y] INTEGER, [width] INTEGER, [height] INTEGER, [expandable] INTEGER);")
    cur.execute("CREATE TABLE cell([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [x] INTEGER, [y] INTEGER, [type] INTEGER);")
    cur.execute("CREATE TABLE link([belong_to_graph] INETGER, [thisobj] INTEGER, [delay] INTEGER, [startobj] INTEGER, [endobj] INTEGER, [start_index] INTEGER, [end_index] INTEGER, [x1] INTEGER, [y1] INTEGER, [x2] INTEGER, [y2] INTEGER);")

def decorateGraph(exCur, deCur, graph):
    scriptMap = {}

    exCur.execute("SELECT [behavior], [index], [name] FROM script;")
    while True:
        lines = exCur.fetchone()
        if lines == None:
            break
        scriptMap[lines[0]] = (lines[1], lines[2])

    exCur.execute("SELECT [thisobj], [type] FROM behavior WHERE [type] != 0;")
    while True:
        lines = exCur.fetchone()
        if lines == None:
            break

        # add into global graph list
        graph.append(lines[0])

        # width and height will be computed by following method and use update
        # statement to change it
        if lines[1] == 1:
            # script
            deCur.execute("INSERT INTO graph VALUES(?, 0, 0, ?, ?)", (lines[0], scriptMap[lines[0]][0], scriptMap[lines[0]][1]))
        else:
            # sub bb
            deCur.execute("INSERT INTO graph VALUES(?, 0, 0, -1, '')", (lines[0],))

def buildBBTree(exCur, deCur, target):
    # sort inner bb
    # use current graph input as the start point
    treeRoot = dcv.BBTreeNode(target, -1)
    processedBB = set()
    bb_layer_map = {}
    # layer start from 1, 0 is occupied for pLocal
    arrangedLayer = recursiveBuildBBTree(treeRoot, exCur, deCur, processedBB, 1, 0, target)
    
    # get no linked bb and place them. linked bb position will be computed following
    # calc each bb's x postion, as for y, we need wait for oOper to confirm each layer's height
    arrangedLayer+=1
    singleBB = set()
    bbResult = {}
    baseX = dcv.GRAPH_CONTENTOFFSET_X
    exCur.execute('SELECT [thisobj], [name], [type], [proto_name], [pin_count] FROM behavior WHERE parent == ?', (target, ))
    for i in exCur.fetchall():
        pinSplit = i[4].split(',')
        bbCache = dcv.BBResult(i[1], i[3], pinSplit[0], pinSplit[1], pinSplit[2], pinSplit[3], (i[0] if i[2] != 0 else -1))
        bbCache.computSize()
        if i[0] not in processedBB:
            # single bb, process it
            singleBB.add(i[0])
            bbCache.x = baseX
            baseX += bbCache.width + dcv.GRAPH_BB_SPAN

        bbResult[i[0]] = bbCache

    recursiveCalcBBX(treeRoot, dcv.GRAPH_CONTENTOFFSET_X, bbResult)
    pass

def recursiveBuildBBTree(node, exCur, deCur, processedBB, layer, depth, graphId):
    cache = []
    if depth == 0:
        # find bIn
        exCur.execute('SELECT [thisobj] FROM bIn WHERE [belong_to] == ?;', (node.bb,))
    else:
        # find bOut
        exCur.execute('SELECT [thisobj] FROM bOut WHERE [belong_to] == ?;', (node.bb,))

    for i in exCur.fetchall():
        cache.append(i[0])

    if (len(cache) == 0):
        return layer

    # find links
    exCur.execute("SELECT [output] FROM bLink WHERE ([belong_to] == ? AND ([input] IN ({})));".format(','.join(map(lambda x:str(x), cache))), (graphId,))
    cache.clear()
    for i in exCur.fetchall():
        cache.append(i[0])

    if (len(cache) == 0):
        return layer

    #find bIn (find in bIn list to omit the line linked to current bb's output)
    exCur.execute("SELECT [belong_to] FROM bIn WHERE [thisobj] IN ({});".format(','.join(map(lambda x:str(x), cache))))
    cache.clear()
    for i in exCur.fetchall():
        cache.append(i[0])

    if (len(cache) == 0):
        return layer

    # ignore duplicated bb
    realLinkedBB = set(cache)
    # calc need processed bb first
    # and register all gotten bb. for preventing infinity resursive func and keep bb tree structure
    realLinkedBB = realLinkedBB - processedBB
    processedBB.update(realLinkedBB)

    # iterate each bb
    for i in realLinkedBB:
        # recursive execute this method
        newNode = dcv.BBTreeNode(i, layer)
        layer = recursiveBuildBBTree(newNode, exCur, deCur, processedBB, layer, depth + 1, graphId)
        # add new node into list and ++layer
        layer+=1
        node.nodes.append(newNode)

    # minus extra ++ due to for
    if (len(realLinkedBB) != 0):
        layer-=1
    
    return layer


def recursiveCalcBBX(node, baseX, resultList):
    maxExpand = 0
    for i in node.nodes:
        resultList[i.bb].x = baseX
        maxExpand = max(maxExpand, resultList[i.bb].width)

    for i in node.nodes:
        recursiveCalcBBX(i, baseX + maxExpand + dcv.GRAPH_BB_SPAN, resultList)