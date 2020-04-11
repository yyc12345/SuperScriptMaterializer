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
        (plocal_layer, bbMap, operMap) = buildBlock(exportCur, decorateCur, i)

    # give up all change of eexport.db (because no change)
    exportDb.close()
    decorateDb.commit()
    decorateDb.close()

def initDecorateDb(cur):
    cur.execute("CREATE TABLE graph([graph] INTEGER, [graph_name] TEXT, [width] INTEGER, [height] INTEGER, [index] INTEGER, [belong_to] TEXT);")
    cur.execute("CREATE TABLE info([target] INTEGER, [field] TEXT, [data] TEXT);")

    cur.execute("CREATE TABLE block([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [pin-pin] TEXT, [pin-pout] TEXT, [pin-bin] TEXT, [pin-bout] TEXT, [x] REAL, [y] REAL, [width] REAL, [height] REAL, [expandable] INTEGER);")
    cur.execute("CREATE TABLE cell([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [x] REAL, [y] REAL, [type] INTEGER);")
    cur.execute("CREATE TABLE link([belong_to_graph] INETGER, [thisobj] INTEGER, [delay] INTEGER, [startobj] INTEGER, [endobj] INTEGER, [start_index] INTEGER, [end_index] INTEGER, [x1] REAL, [y1] REAL, [x2] REAL, [y2] REAL);")

def decorateGraph(exCur, deCur, graph):
    scriptMap = {}

    exCur.execute("SELECT [behavior], [index], [name] FROM script;")
    while True:
        lines = exCur.fetchone()
        if lines == None:
            break
        scriptMap[lines[0]] = (lines[1], lines[2])

    exCur.execute("SELECT [thisobj], [type], [name] FROM behavior WHERE [type] != 0;")
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
            deCur.execute("INSERT INTO graph VALUES(?, ?, 0, 0, ?, ?)", (lines[0], lines[2], scriptMap[lines[0]][0], scriptMap[lines[0]][1]))
        else:
            # sub bb
            deCur.execute("INSERT INTO graph VALUES(?, ?, 0, 0, -1, '')", (lines[0], lines[2]))

def buildBlock(exCur, deCur, target):
    # sort inner bb
    # use current graph input as the start point
    treeRoot = dcv.BBTreeNode(target, -1)
    processedBB = set()
    # layer start from 2, 0 is occupied for pLocal, 1 is occupied for pOper
    arrangedLayer = recursiveBuildBBTree(treeRoot, exCur, processedBB, 2, 0, target)
    
    # get no linked bb and place them. linked bb position will be computed following
    # calc each bb's x postion, as for y, calc later
    arrangedLayer+=1
    singleBB = set()
    bbResult = {}
    bb_layer_map = {}
    baseX = dcv.GRAPH_CONTENTOFFSET_X
    exCur.execute('SELECT [thisobj], [name], [type], [proto_name], [pin_count] FROM behavior WHERE parent == ?', (target, ))
    for i in exCur.fetchall():
        pinSplit = i[4].split(',')
        bbCache = dcv.BBResult(i[1], i[3], pinSplit[1], pinSplit[2], pinSplit[3], pinSplit[4], (i[0] if i[2] != 0 else -1))
        bbCache.computSize()
        if i[0] not in processedBB:
            # single bb, process it
            singleBB.add(i[0])
            bbCache.x = baseX
            baseX += bbCache.width + dcv.GRAPH_BB_SPAN
            bb_layer_map[i[0]] = arrangedLayer

        bbResult[i[0]] = bbCache

    recursiveCalcBBX(treeRoot, dcv.GRAPH_CONTENTOFFSET_X, bbResult, bb_layer_map)

    # calc bb y
    layer_height = {}
    layer_y = {}
    layer_height[0] = 25
    layer_height[1] = 50
    for i in bb_layer_map.keys():
        curLayer = bb_layer_map[i]
        if curLayer not in layer_height.keys():
            layer_height[curLayer] = bbResult[i].height
        else:
            layer_height[curLayer] = max(layer_height.get(curLayer, 0), bbResult[i].height)
    layer_height[arrangedLayer] = layer_height.get(arrangedLayer, 0) # make sure misc bb height exist
    baseY = dcv.GRAPH_CONTENTOFFSET_Y
    for i in range(arrangedLayer + 1):
        baseY += layer_height[i] + dcv.GRAPH_LAYER_SPAN
        layer_y[i] = baseY
    for i in bbResult.keys():
        cache = bbResult[i]
        layer = bb_layer_map[i]
        cache.y = layer_y[layer] - layer_height[layer] + cache.height

    # calc poper
    operResult = {}
    baseX = dcv.GRAPH_CONTENTOFFSET_X
    exCur.execute('SELECT [thisobj], [op] FROM pOper WHERE [belong_to] == ?', (target, ))
    for i in exCur.fetchall():
        cache = dcv.OperResult(i[1], baseX)
        cache.computSize()
        baseX += cache.width + dcv.GRAPH_BB_SPAN
        cache.y = layer_y[1] - cache.height
        operResult[i[0]] = cache

    # write to database and return
    for i in bbResult.keys():
        cache = bbResult[i]
        deCur.execute('INSERT INTO block VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)',
                      (target, i, cache.name, cache.assistName, cache.pin, cache.pout, cache.bin, cache.bout, cache.x, cache.y, cache.width, cache.height, cache.expandable))
    for i in operResult.keys():
        cache = operResult[i]
        deCur.execute("INSERT INTO block VALUES (?, ?, ?, '', 2, 1, 0, 0, ?, ?, ?, ?, -1)",
                      (target, i, cache.name, cache.x, cache.y, cache.width, cache.height))
    
    return (layer_y[0] - dcv.CELL_HEIGHT, bbResult, operResult)

def recursiveBuildBBTree(node, exCur, processedBB, layer, depth, graphId):
    realLinkedBB = set()
    # find links
    exCur.execute("SELECT [output_obj] FROM bLink WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
                  (node.bb, (dcv.dbBLinkInputOutputType.INPUT if depth == 0 else dcv.dbBLinkInputOutputType.OUTPUT), graphId))
    for i in exCur.fetchall():
        realLinkedBB.add(i[0])

    if (len(cache) == 0):
        return layer

    # ignore duplicated bb
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

def recursiveCalcBBX(node, baseX, resultList, layerMap):
    maxExpand = 0
    for i in node.nodes:
        layerMap[i.bb] = i.layer
        resultList[i.bb].x = baseX
        maxExpand = max(maxExpand, resultList[i.bb].width)

    for i in node.nodes:
        recursiveCalcBBX(i, baseX + maxExpand + dcv.GRAPH_BB_SPAN, resultList, layerMap)
