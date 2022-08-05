import CustomConfig
import Database

from flask import Flask
from flask import g
from flask import render_template
from flask import url_for
from flask import request
from flask import abort
from flask import redirect

app = Flask(__name__)

# =============================================database
def get_db():
    db = getattr(g, '_database', None)
    if db is None:
        db = g._database = Database.CreateDatabase()
    return db

@app.teardown_appcontext
def close_connection(exception):
    db = getattr(g, '_database', None)
    if db is not None:
        Database.CloseDatabase(db)

# =============================================route

# =========== default

@app.route('/', methods=['GET'])
def handle_nospec():
    return redirect(url_for('handle_index'))

# =========== basic pages

@app.route('/index', methods=['GET'])
def handle_index():
    return app.send_static_file("html/index.html")

@app.route('/script', methods=['GET'])
def handle_script():
    return redirect(url_for('handle_index'))

@app.route('/array', methods=['GET'])
def handle_array():
    return redirect(url_for('handle_index'))
    
@app.route('/environment', methods=['GET'])
def handle_environment():
    return redirect(url_for('handle_index'))

@app.route('/help', methods=['GET'])
def handle_help():
    return app.send_static_file("html/help.html")

@app.route('/about', methods=['GET'])
def handle_about():
    return app.send_static_file("html/about.html")

# =========== viewer
# script and array should have at least 2 items splitted by slash(/)
# the first one is document id and the second one is the real CK_ID of viewing object.
# however, environment do not have this, environment only allow one item, the id of environment.

@app.route('/script/<path:script_path>', methods=['GET'])
def handle_script_viewer(script_path):
    # check invalid url
    if len(script_path.split('/')) < 2:
        return redirect(url_for('handle_index'))
    return app.send_static_file("html/viewer_script.html")

@app.route('/array/<path:array_path>', methods=['GET'])
def handle_array_viewer(array_path):
    # check invalid url
    if len(script_path.split('/')) < 2:
        return redirect(url_for('handle_index'))
    return app.send_static_file("html/viewer_array.html")

@app.route('/environment/<path:environment_path>', methods=['GET'])
def handle_environment_viewer(environment_path):
    # check invalid url
    if len(script_path.split('/')) > 1:
        return redirect(url_for('handle_index'))
    return app.send_static_file("html/viewer_environment.html")

# =========== viewer

@app.route('/api/index/getList', methods=['POST'])
def handle_api_index_getList(scriptPath):
    return {
        "document": [],
        "script": [],
        "array": [],
        "environment": []
    }

# =========== startup     
def run():
    app.run()
    