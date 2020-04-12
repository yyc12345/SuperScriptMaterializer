import sqlite3
import DecoratorConstValue as dcv
import json

def run():
    exportDb = sqlite3.connect('export.db')
    decorateDb = sqlite3.connect('decorate.db')

    # init table
    print('Init decorate.dll')
    initDecorateDb(decorateDb)
    decorateDb.commit()

    # decorate graph
    graphList = []
    decorateGraph(exportDb, decorateDb, graphList)

    # decorate each graph
    currentGraphBlockCell = {}
    for i in graphList:
        currentGraphBlockCell.clear()
        buildBlock(exportDb, decorateDb, i, currentGraphBlockCell)
        buildCell(exportDb, decorateDb, i, currentGraphBlockCell)
        

    # give up all change of eexport.db (because no change)
    exportDb.close()
    decorateDb.commit()
    decorateDb.close()

def initDecorateDb(db):
    cur = db.cursor()
    cur.execute("CREATE TABLE graph([graph] INTEGER, [graph_name] TEXT, [width] INTEGER, [height] INTEGER, [index] INTEGER, [belong_to] TEXT);")
    cur.execute("CREATE TABLE info([target] INTEGER, [field] TEXT, [data] TEXT);")

    cur.execute("CREATE TABLE block([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [pin-pin] TEXT, [pin-pout] TEXT, [pin-bin] TEXT, [pin-bout] TEXT, [x] REAL, [y] REAL, [width] REAL, [height] REAL, [expandable] INTEGER);")
    cur.execute("CREATE TABLE cell([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [x] REAL, [y] REAL, [type] INTEGER);")
    cur.execute("CREATE TABLE link([belong_to_graph] INETGER, [thisobj] INTEGER, [delay] INTEGER, [startobj] INTEGER, [endobj] INTEGER, [start_index] INTEGER, [end_index] INTEGER, [x1] REAL, [y1] REAL, [x2] REAL, [y2] REAL);")

def decorateGraph(exDb, deDb, graph):
    exCur = exDb.cursor()
    deCur = deDb.cursor()
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

def buildBlock(exDb, deDb, target, currentGraphBlockCell):
    exCur = exDb.cursor()
    deCur = deDb.cursor()

    # sort inner bb
    # use current graph input as the start point
    treeRoot = dcv.BBTreeNode(target, -1)
    processedBB = set()
    # layer start from 2, 0 is occupied for pLocal, 1 is occupied for pOper
    arrangedLayer = recursiveBuildBBTree(treeRoot, exCur, processedBB, 2, 0, target)
    
    # get no linked bb and place them.  linked bb position will be computed
    # following
    # calc each bb's x postion, as for y, calc later
    # flat bb tree
    arrangedLayer+=1
    singleBB = set()
    bbResult = {}
    bb_layer_map = {}
    baseX = dcv.GRAPH_CONTENTOFFSET_X
    exCur.execute('SELECT [thisobj], [name], [type], [proto_name], [pin_count] FROM behavior WHERE parent == ?', (target,))
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

    # calc poper
    allBB = processedBB | singleBB
    processedOper = set()
    pluggedOper = {}
    occupiedLayerCountForSpecificBB = {}
    exCur.execute('SELECT [thisobj] FROM pOper WHERE [belong_to] == ?', (target,))
    newCur = exDb.cursor()
    newCur2 = exDb.cursor()
    for i in exCur.fetchall():
        if i[0] in processedOper:
            continue

        # check current bout, plugin into the first bb
        newCur.execute("SELECT [output_obj] FROM pLink WHERE ([input_obj] == ? AND [output_type] == ? AND [output_is_bb] == 1)", (i[0], dcv.dbPLinkInputOutputType.PIN))
        for j in newCur.fetchall():
            if j[0] in allBB:
                # can be plugin
                # try get tree
                if j[0] not in pluggedOper.keys():
                    pluggedOper[j[0]] = {}
                recursiveBuildOperTree(i[0], bb_layer_map, processedOper, occupiedLayerCountForSpecificBB, newCur2, 1, j[0], target, pluggedOper[j[0]])
                # exit for due to have found a proper host bb
                break
            

    # calc layer position
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
    for i in occupiedLayerCountForSpecificBB.keys():   # add oper occipation
        layer_height[i] += occupiedLayerCountForSpecificBB[i] * dcv.GRAPH_SPAN_BB_POPER

    # calc bb Y
    baseY = dcv.GRAPH_CONTENTOFFSET_Y
    for i in range(arrangedLayer + 1):
        baseY += layer_height[i] + dcv.GRAPH_LAYER_SPAN
        layer_y[i] = baseY
    for i in bbResult.keys():
        cache = bbResult[i]
        layer = bb_layer_map[i]
        cache.y = layer_y[layer] - layer_height[layer]

    # calc oper position
    # flat oper tree
    operResult = {}
    exCur.execute('SELECT [thisobj], [op] FROM pOper WHERE [belong_to] == ?', (target,))
    homelessOperCurrentX = dcv.GRAPH_CONTENTOFFSET_X
    for i in exCur.fetchall():
        if i[0] not in processedOper:
            # homeless oper
            cache2 = dcv.OperResult(i[1])
            cache2.computSize()
            cache2.x = homelessOperCurrentX
            cache2.y = layer_y[1] - cache2.height
            homelessOperCurrentX += cache2.width + dcv.GRAPH_BB_SPAN
            operResult[i[0]] = cache2

    for i in pluggedOper.keys():   # plugged oper
        cache = bbResult[i]
        for j in pluggedOper[i]:
            jCache = pluggedOper[i][j]
            baseX = cache.x
            for q in jCache:
                exCur.execute("SELECT [op] FROM pOper WHERE [thisobj] == ?", (q,))
                cache2 = dcv.OperResult(exCur.fetchone()[0])
                cache2.computSize()
                cache2.x = baseX
                baseX += cache2.width + dcv.GRAPH_BB_SPAN
                cache2.y = cache.y - j * dcv.GRAPH_SPAN_BB_POPER
                operResult[q] = cache2

    # query bb pin's data
    listCache = []
    listItemCache = None
    for i in allBB:
        cache = bbResult[i]
        exCur.execute("SELECT [thisobj], [name], [type] FROM pTarget WHERE [belong_to] == ?;", (i,))
        temp = exCur.fetchone()
        if temp == None:
            cache.ptargetData = ''
        else:
            cache.ptargetData = json.dumps(dcv.PinInformation(temp[0], temp[1], temp[2]), cls = dcv.JsonCustomEncoder)

        listCache.clear()
        exCur.execute("SELECT [thisobj], [name] FROM bIn WHERE [belong_to] == ? ORDER BY [index];", (i,))
        for j in exCur.fetchall():
            listItemCache = dcv.PinInformation(j[0], j[1], '')
            listCache.append(listItemCache)
        cache.binData = json.dumps(listCache, cls = dcv.JsonCustomEncoder)

        listCache.clear()
        exCur.execute("SELECT [thisobj], [name] FROM bOut WHERE [belong_to] == ? ORDER BY [index];", (i,))
        for j in exCur.fetchall():
            listItemCache = dcv.PinInformation(j[0], j[1], '')
            listCache.append(listItemCache)
        cache.boutData = json.dumps(listCache, cls = dcv.JsonCustomEncoder)

        listCache.clear()
        exCur.execute("SELECT [thisobj], [name], [type] FROM pIn WHERE [belong_to] == ? ORDER BY [index];", (i,))
        for j in exCur.fetchall():
            listItemCache = dcv.PinInformation(j[0], j[1], j[2])
            listCache.append(listItemCache)
        cache.pinData = json.dumps(listCache, cls = dcv.JsonCustomEncoder)

        listCache.clear()
        exCur.execute("SELECT [thisobj], [name], [type] FROM pOut WHERE [belong_to] == ? ORDER BY [index];", (i,))
        for j in exCur.fetchall():
            listItemCache = dcv.PinInformation(j[0], j[1], j[2])
            listCache.append(listItemCache)
        cache.poutData = json.dumps(listCache, cls = dcv.JsonCustomEncoder)

    # query oper pin's data
    for i in operResult.keys():
        cache = operResult[i]

        listCache.clear()
        exCur.execute("SELECT [thisobj], [name], [type] FROM pIn WHERE [belong_to] == ? ORDER BY [index];", (i,))
        for j in exCur.fetchall():
            listItemCache = dcv.PinInformation(j[0], j[1], j[2])
            listCache.append(listItemCache)
        cache.pinData = json.dumps(listCache, cls = dcv.JsonCustomEncoder)

        listCache.clear()
        exCur.execute("SELECT [thisobj], [name], [type] FROM pOut WHERE [belong_to] == ? ORDER BY [index];", (i,))
        for j in exCur.fetchall():
            listItemCache = dcv.PinInformation(j[0], j[1], j[2])
            listCache.append(listItemCache)
        cache.poutData = json.dumps(listCache, cls = dcv.JsonCustomEncoder)

    # write to database and return
    for i in bbResult.keys():
        cache = bbResult[i]
        currentGraphBlockCell[i] = dcv.BlockCellItem(cache.x, cache.y, cache.width, cache.height)
        deCur.execute('INSERT INTO block VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)',
                      (target, i, cache.name, cache.assistName, cache.pinData, cache.poutData, cache.binData, cache.boutData, cache.x, cache.y, cache.width, cache.height, cache.expandable))
    for i in operResult.keys():
        cache = operResult[i]
        currentGraphBlockCell[i] = dcv.BlockCellItem(cache.x, cache.y, cache.width, cache.height)
        deCur.execute("INSERT INTO block VALUES (?, ?, ?, '', ?, ?, '[]', '[]', ?, ?, ?, ?, -1)",
                      (target, i, cache.name, cache.pinData, cache.poutData, cache.x, cache.y, cache.width, cache.height))

def recursiveBuildBBTree(node, exCur, processedBB, layer, depth, graphId):
    realLinkedBB = set()
    # find links
    exCur.execute("SELECT [output_obj] FROM bLink WHERE ([input_obj] == ? AND [input_type] == ? AND [belong_to] = ?) ORDER BY [input_index] ASC;",
                  (node.bb, (dcv.dbBLinkInputOutputType.INPUT if depth == 0 else dcv.dbBLinkInputOutputType.OUTPUT), graphId))
    for i in exCur.fetchall():
        if i[0] != graphId: # omit self
            realLinkedBB.add(i[0])

    if (len(realLinkedBB) == 0):
        return layer

    # ignore duplicated bb
    # calc need processed bb first
    # and register all gotten bb.  for preventing infinity resursive func and
    # keep bb tree structure
    realLinkedBB = realLinkedBB - processedBB
    processedBB.update(realLinkedBB)

    # iterate each bb
    for i in realLinkedBB:
        # recursive execute this method
        newNode = dcv.BBTreeNode(i, layer)
        layer = recursiveBuildBBTree(newNode, exCur, processedBB, layer, depth + 1, graphId)
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

def recursiveBuildOperTree(oper, bb_layer_map, processedOper, occupiedLayerMap, exCur, sublayer, bb, graphId, subLayerColumnMap):
    if oper in processedOper:
        return

    # for avoid fucking export parameter feature.  check whether self is
    # current graph's memeber
    exCur.execute("SELECT [belong_to] FROM pOper WHERE [thisobj] == ?;", (oper,))
    if (exCur.fetchone()[0] != graphId):
        # fuck export param, exit
        return

    # make sure sub layer column map is ok
    if sublayer not in subLayerColumnMap.keys():
        subLayerColumnMap[sublayer] = []

    # register self
    # mark processed
    processedOper.add(oper)
    subLayerColumnMap[sublayer].append(oper)

    # record layer occupation
    layer = bb_layer_map[bb]
    occupiedLayerMap[layer] = max(occupiedLayerMap.get(layer, -1), sublayer)

    # iterate sub item
    exCur.execute("SELECT [input_obj] FROM pLink WHERE ([output_obj] == ? AND [input_type] == ? AND [input_is_bb] == 0) ORDER BY [output_index];", (oper, dcv.dbPLinkInputOutputType.POUT))
    res = []
    for i in exCur.fetchall():
        res.append(i[0])

    for i in res:
        recursiveBuildOperTree(i, bb_layer_map, processedOper, occupiedLayerMap, exCur, sublayer + 1, bb, graphId, subLayerColumnMap)

def buildCell(exDb, deDb, target, currentGraphBlockCell):
    exCur = exDb.cursor()
    deCur = deDb.cursor()
    # prepare block set
    blockSet = set()
    for i in currentGraphBlockCell.keys():
        blockSet.add(i)

    # find current graph's pio bio
    boutx = set()
    pouty = set()
    graphPIO = set()

    # bOut.x and pOut.y data is not confirmed, when graph size was confirmed,
    # update it
    exCur.execute("SELECT [thisobj], [name], [index] FROM bIn WHERE [belong_to] == ?", (target,))
    for i in exCur.fetchall():
        x = 0
        y = dcv.GRAPH_BOFFSET + i[2] * dcv.GRAPH_BSPAN
        currentGraphBlockCell[i[0]] = dcv.BlockCellItem(x, y, dcv.BB_PBSIZE, dcv.BB_PBSIZE)
        deCur.execute("INSERT INTO cell VALUES (?, ?, ?, '', ?, ?, ?)", (target, i[0], i[1], x, y, dcv.CellType.BIO))
    exCur.execute("SELECT [thisobj], [name], [index] FROM bOut WHERE [belong_to] == ?", (target,))
    for i in exCur.fetchall():
        x = 0
        y = dcv.GRAPH_BOFFSET + i[2] * dcv.GRAPH_BSPAN
        currentGraphBlockCell[i[0]] = dcv.BlockCellItem(x, y, dcv.BB_PBSIZE, dcv.BB_PBSIZE)
        deCur.execute("INSERT INTO cell VALUES (?, ?, ?, '', ?, ?, ?)", (target, i[0], i[1], x, y, dcv.CellType.BIO))
        boutx.add(i[0])

    exCur.execute("SELECT [thisobj], [name], [index], [type] FROM pIn WHERE [belong_to] == ?", (target,))
    for i in exCur.fetchall():
        x = dcv.GRAPH_POFFSET + i[2] * dcv.GRAPH_PSPAN
        y = 0
        currentGraphBlockCell[i[0]] = dcv.BlockCellItem(x, y, dcv.BB_PBSIZE, dcv.BB_PBSIZE)
        deCur.execute("INSERT INTO cell VALUES (?, ?, ?, ?, ?, ?, ?)", (target, i[0], i[1], i[3], x, y, dcv.CellType.PIO))
        graphPIO.add(i[0])
    exCur.execute("SELECT [thisobj], [name], [index], [type] FROM pOut WHERE [belong_to] == ?", (target,))
    for i in exCur.fetchall():
        x = dcv.GRAPH_POFFSET + i[2] * dcv.GRAPH_PSPAN
        y = 0
        currentGraphBlockCell[i[0]] = dcv.BlockCellItem(x, y, dcv.BB_PBSIZE, dcv.BB_PBSIZE)
        deCur.execute("INSERT INTO cell VALUES (?, ?, ?, ?, ?, ?, ?)", (target, i[0], i[1], i[3], x, y, dcv.CellType.PIO))
        graphPIO.add(i[0])
        pouty.add(i[0])
    exCur.execute("SELECT [thisobj], [name], [type] FROM pTarget WHERE [belong_to] == ?", (target,))
    cache = exCur.fetchone()
    if cache != None:
        currentGraphBlockCell[cache[0]] = dcv.BlockCellItem(0, 0, dcv.BB_PBSIZE, dcv.BB_PBSIZE)
        deCur.execute("INSERT INTO cell VALUES (?, ?, ?, ?, 0, 0, ?)", (target, i[0], i[1], i[2], dcv.CellType.PTARGET))
        graphPIO.add(cache[0])

    # query all plocal
    allLocal = set()
    localUsageCounter = {}
    exCur.execute("SELECT [thisobj], [name], [type] FROM pLocal WHERE [belong_to] == ?;", (target,))
    for i in exCur.fetchall():
        allLocal.add(i[0])
        localUsageCounter[i[0]] = dcv.LocalUsageItem(0, False, dcv.LocalUsageType.PLOCAL)

    # query all links(don't need to consider export pIO, due to it will not add
    # any shortcut)
    exCur.execute("SELECT * FROM pLink WHERE [belong_to] == ?", (target,))
    for i in exCur.fetchall():
        # check export pIO.
        if ((i[2] == target) and (i[0] in graphPIO) or (i[6] == target) and (i[1] in graphPIO)):
            # fuck export param
            continue

        # analyse 5 chancee one by one
        if (i[7] == dcv.dbPLinkInputOutputType.PTARGET or i[7] == dcv.dbPLinkInputOutputType.PIN):
            if (i[3] == dcv.dbPLinkInputOutputType.PLOCAL):
                if i[2] in allLocal:
                    cache = localUsageCounter[i[2]]
                else:
                    cache = dcv.LocalUsageItem(0, True, dcv.LocalUsageType.PLOCAL)
                    localUsageCounter[i[2]] = cache

                cache.count += 1
                cache.lastUse = i[6]
                cache.lastDirection = 0
                cache.lastIndex = i[9]

            elif (i[3] == dcv.dbPLinkInputOutputType.PIN):
                if i[2] not in blockSet:
                    cache = dcv.LocalUsageItem(0, True, dcv.LocalUsageType.PIN)
                    localUsageCounter[i[0]] = cache

                    cache.count+=1
                    cache.lastUse = i[6]
                    cache.lastDirection = 0
                    cache.lastIndex = i[9]
            else:
                if i[2] not in blockSet:
                    cache = dcv.LocalUsageItem(0, True, dcv.LocalUsageType.POUT)
                    localUsageCounter[i[0]] = cache

                    cache.count+=1
                    cache.lastUse = i[6]
                    cache.lastDirection = 1
                    cache.lastIndex = i[9]
        else:
            if (i[7] == dcv.dbPLinkInputOutputType.PLOCAL):
                if i[6] in allLocal:
                    cache = localUsageCounter[i[6]]
                else:
                    cache = dcv.LocalUsageItem(0, True, dcv.LocalUsageType.PLOCAL)
                    localUsageCounter[i[6]] = cache

                cache.count += 1
                cache.lastUse = i[2]
                cache.lastDirection = 1
                cache.lastIndex = i[5]
            else:
                if i[6] not in blockSet:
                    cache = dcv.LocalUsageItem(0, True, dcv.LocalUsageType.POUT)
                    localUsageCounter[i[1]] = cache

                    cache.count += 1
                    cache.lastUse = i[2]
                    cache.lastDirection = 1
                    cache.lastIndex = i[5]

    # apply all cells
    defaultCellIndex = 0
    for i in localUsageCounter.keys():
        cache = localUsageCounter[i]
        # comput x,y
        if (cache.count == 1):
            # attachable
            attachTarget = currentGraphBlockCell[cache.lastUse]
            (x, y) = computCellPosition(attachTarget.x, attachTarget.y, attachTarget.h, cache.lastDirection, cache.lastIndex)

        else:
            # place it in default area
            y = dcv.GRAPH_CONTENTOFFSET_Y
            x = dcv.GRAPH_CONTENTOFFSET_X + defaultCellIndex * (dcv.CELL_WIDTH + dcv.GRAPH_BB_SPAN)
            defaultCellIndex += 1
        # get information
        if (cache.internal_type == dcv.LocalUsageType.PIN):
            tableName = 'pIn'
        elif (cache.internal_type == dcv.LocalUsageType.POUT):
            tableName = 'pOut'
        else:
            tableName = 'pLocal'
        exCur.execute("SELECT [name], [type] FROM {} WHERE [thisobj] == ?".format(tableName), (i,))
        temp = exCur.fetchone()

        # submit to database and map
        currentGraphBlockCell[i] = dcv.BlockCellItem(x, y, dcv.CELL_WIDTH, dcv.CELL_HEIGHT)
        deCur.execute("INSERT INTO cell VALUES (?, ?, ?, ?, ?, ?, ?)",
                      (target, i, temp[0], temp[1], x, y, (dcv.CellType.SHORTCUT if cache.isshortcut else dcv.CellType.PLOCAL)))

    # comput size and update database
    graphX = 0
    graphY = 0
    for key, values in currentGraphBlockCell.items():
        graphX = max(graphX, values.x + values.w)
        graphY = max(graphY, values.y + values.h)
    graphX += dcv.GRAPH_POFFSET
    graphY += dcv.GRAPH_BOFFSET

    deCur.execute("UPDATE graph SET [width] = ?, [height] = ? WHERE [graph] == ?", (graphX, graphY, target))

    # update bOut.x and pOut.y data
    for i in boutx:
        deCur.execute("UPDATE cell SET [x] = ? WHERE ([thisobj] == ? AND [belong_to_graph] == ?)", (graphX - dcv.BB_PBSIZE, i, target))
    for i in pouty:
        deCur.execute("UPDATE cell SET [y] = ? WHERE ([thisobj] == ? AND [belong_to_graph] == ?)", (graphY - dcv.BB_PBSIZE, i, target))

def computCellPosition(baseX, baseY, height, direction, index):
    if (index == -1):
        return (0, 0)

    if (direction == 0):
        return (baseX + dcv.BB_POFFSET + index * (dcv.BB_PBSIZE + dcv.BB_PSPAN), baseY - dcv.GRAPH_SPAN_BB_PLOCAL)
    else:
        return (baseX + dcv.BB_POFFSET + index * (dcv.BB_PBSIZE + dcv.BB_PSPAN), baseY + height + dcv.GRAPH_SPAN_BB_PLOCAL)

def buildLink(exDb, deDb, target, currentGraphBlockCell):
    pass