import CustomConfig

from flask import Flask
from flask import g
from flask import render_template
from flask import url_for
from flask import request
from flask import abort
from flask import redirect

from functools import reduce
import sqlite3
import json
import ServerStruct as ss

app = Flask(__name__)

# =============================================database
def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect(CustomConfig.decorated_db)
    return db

def get_env():
    env = getattr(g, '_envDatabase', None)
    if env is None:
        env = g._envDatabase = sqlite3.connect(CustomConfig.env_db)
        env.text_factory = lambda x: x.decode(CustomConfig.database_encoding, errors="ignore")
    return env

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()

# =============================================template func
@app.template_global(name = 'pinDecoder')
def block_pin_decoder(target):
    return json.loads(target)

@app.template_global(name = 'pinDecoder2')
def block_pin_decoder2(target):
    vab = json.loads(target)
    return [vab['name'], vab['type']]

# =============================================route

# =========== default

@app.route('/', methods=['GET'])
def nospecHandle():
    return redirect(url_for('indexHandle'))

# =========== misc page

@app.route('/help', methods=['GET'])
def helpMainHandle():
    return render_template("help.html")

@app.route('/about', methods=['GET'])
def aboutHandle():
    return render_template("about.html", static_icon = url_for('static', filename='icon.png'))

# =========== help page

@app.route('/help/<path:scriptPath>', methods=['GET'])
def helpHandle(scriptPath):
    if scriptPath == 'converter':
        return render_template("help/converter.html",
                        tabcontrol_css = url_for('static', filename='tabcontrol.css'),
                        tabcontrol_js = url_for('static', filename='tabcontrol.js'),
                        converter_js = url_for('static', filename='converter.js'))
    elif scriptPath == 'env':
        return render_template("help/env.html",
                        tabcontrol_css = url_for('static', filename='tabcontrol.css'),
                        tabcontrol_js = url_for('static', filename='tabcontrol.js'),
                        env_js = url_for('static', filename='env.js'),
                        env_css = url_for('static', filename='env.css'),
                        database_data = ss.envDatabaseList)
    elif scriptPath == 'legend':
        return render_template("help/legend.html")
    else:
        abort(404)

@app.route('/help/env', methods=['POST'])
def envQueryHandle():
    basicReturn = {
        "status": False,
        "overflow": False,
        "data": []
    }

    # check tag
    queryTag = request.form['tag'];
    if queryTag not in ss.legalEnvQueryKey:
        return basicReturn

    cur = get_env().cursor()
    #try:
    readyData = json.loads(request.form['data'])
    fieldLength = len(readyData.keys())
    if fieldLength == 0:
        cur.execute("SELECT * FROM {}".format(queryTag))
    else:
        whereStatement = 'AND'.join(map(lambda x: "([" + x + "] = ?)", readyData.keys()))
        cur.execute("SELECT * FROM {} WHERE ({})".format(queryTag, whereStatement), list(readyData.values()))
        
    # iterate
    counter = 0
    for i in cur.fetchall():
        if counter == 100:
            basicReturn['overflow'] = True
            break
        basicReturn['data'].append(i)
        counter+=1

    basicReturn['status'] = True
    #except Exception as ex:
    #    basicReturn['status'] = False
    #    basicReturn['overflow'] = False
    #    basicReturn['data'] = []

    return basicReturn

# =========== index

@app.route('/index', methods=['GET'])
def indexHandle():
    cur = get_db().cursor()
    cur.execute("SELECT [graph], [graph_name], [belong_to] FROM graph WHERE [index] != -1 ORDER BY [belong_to], [index] ASC;")
    data = {}
    for i in cur.fetchall():
        if i[2] not in data.keys():
            data[i[2]] = [ss.ScriptItem(i[1], i[0])]
        else:
            data[i[2]].append(ss.ScriptItem(i[1], i[0]))

    return render_template('index.html', scripts = data)

# =========== viewer

@app.route('/viewer/<path:scriptPath>', methods=['GET'])
def viewerHandle(scriptPath):

    # comput hamburger
    pathSlice = scriptPath.split('/')
    cur = get_db().cursor()
    cur.execute("SELECT [graph], [graph_name] FROM graph WHERE [graph] IN ({});".format(','.join(pathSlice)))
    hamburgerMap = {}
    hamburger = []
    for i in cur.fetchall():
        hamburgerMap[str(i[0])] = i[1]
    currentHamburger = hamburgerMap[pathSlice[-1]]
    
    for i in range(len(pathSlice) - 1):
        hamburger.append(ss.HamburgerItem(hamburgerMap[pathSlice[i]], reduce(lambda x,y: x + '/' + y, pathSlice[0:i + 1], '')))

    # gei w/h
    cur.execute('SELECT [width], [height] FROM graph WHERE [graph] == ?', (pathSlice[-1], ))
    cache = cur.fetchone()
    width = cache[0]
    height = cache[1]

    # get blocks
    cur.execute('SELECT * FROM block WHERE [belong_to_graph] == ?', (pathSlice[-1], ))
    dbBlocks = cur.fetchall()

    # get cells
    cur.execute("SELECT * FROM cell WHERE [belong_to_graph] == ?", (pathSlice[-1], ))
    dbCells = cur.fetchall()

    # get links
    cur.execute("SELECT * FROM link WHERE [belong_to_graph] == ?", (pathSlice[-1], ))
    dbLinks = cur.fetchall()

    # render
    return render_template('viewer.html',
                    currentPath = scriptPath,
                    gWidth = width,
                    gHeight = height,
                    hamburgerHistory = hamburger,
                    viewer_css = url_for('static', filename='viewer.css'),
                    tabcontrol_css = url_for('static', filename='tabcontrol.css'),
                    viewer_js = url_for('static', filename='viewer.js'),
                    tabcontrol_js = url_for('static', filename='tabcontrol.js'),
                    hamburgerCurrent = currentHamburger,
                    blocks = dbBlocks,
                    cells = dbCells,
                    links = dbLinks)

@app.route('/viewer/<path:scriptPath>', methods=['POST'])
def actionHandle(scriptPath):
    cache = request.form['operation']
    if cache == "info":
        return infoHandle(request.form['target'], request.form['tag'])
    elif cache == "move":
        return moveHandle(request.form['target'])
    else:
        abort(400)

def infoHandle(target, tag):
    cur = get_db().cursor()

    data = {}
    existedSet = set()
    if tag == '0':
        # call from cell
        cur.execute("SELECT * FROM info WHERE [target] == ?", (target, ))
    elif tag == '1':
        # call from bb
        cur.execute("SELECT * FROM info WHERE [attach_bb] == ?", (target, ))
    else:
        return {}
    # get data
    for i in cur.fetchall():
        if i[0] in existedSet:
            data[i[0]]['data'].append((i[4], i[5]))
        else:
            existedSet.add(i[0])
            data[i[0]] = {
                'name': i[3],
                'is_setting': True if i[2] != 0 else False,
                'data': [(i[4], i[5])]
            }

    return data

def moveHandle(target):
    return {}

def run():
    app.run()
    