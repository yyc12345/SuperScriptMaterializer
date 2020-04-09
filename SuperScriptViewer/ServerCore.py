from flask import Flask
from flask import g
from flask import render_template
from flask import url_for
from flask import request

import sqlite3
import ServerStruct as ss

app = Flask(__name__)

def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = sqlite3.connect('decorate.db')
    return db

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        db.close()

@app.route('/', methods=['GET'])
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

@app.route('/<path:scriptPath>', methods=['GET'])
def scriptHandle(scriptPath):
    # comput hamburger
    pathSlice = scriptPath.split('/')
    cur = get_db().cursor()
    cur.execute("SELECT [graph], [graph_name] FROM graph WHERE [graph] IN ({});".format(','.join(pathSlice)))
    hamburgerMap = {}
    hamburger = []
    for i in cur.fetchall():
        hamburgerMap[i[0]] = i[1]
    for i in pathSlice:
        hamburger.append(hamburgerMap[int(i)])
    currentHamburger = hamburger.pop()

    # get blocks
    cur.execute('SELECT * FROM block WHERE [belong_to_graph] = ?', (pathSlice[-1],))
    dbBlocks = cur.fetchall()

    # todo:xxxxx

    # render
    return render_template('viewer.html',
                    hamburgerHistory = hamburger,
                    static_css = url_for('static', filename='site.css'),
                    static_js = url_for('static', filename='site.js'),
                    hamburgerCurrent = currentHamburger,
                    blocks = dbBlocks)

def run():
    app.run()
    

