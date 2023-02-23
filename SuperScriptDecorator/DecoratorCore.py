import sqlite3, json, collections
import CustomConfig, Progressbar
import DecoratorBB, DecoratorConst

class CompositionIngredients(object):
    def __init__(self, name: str, export_id: int, env_id: int):
        self.m_CompositionName: str = name
        self.m_ExportIndex = export_id
        self.m_EnvIndex = env_id

def __InitDecoratedDb(db: sqlite3.Connection):
    cur = db.cursor()
    cur.execute("CREATE TABLE [compositions] ([id] INTEGER, [name] TEXT, [export_id] INTEGER, [env_id] INTEGER);")

    cur.execute("CREATE TABLE [graph_graph] ([parent_export] INTEGER, [thisobj] INTEGER, [name] TEXT, [hierarchy] TEXT, [height] REAL, [width] REAL, [bind_behavior] TEXT, [bind_beh_ord] INTEGER);")
    #cur.execute("CREATE TABLE info([target] INTEGER, [attach_bb] INTEGER, [is_setting] INTEGER, [name] TEXT, [field] TEXT, [data] TEXT);")

    cur.execute("CREATE TABLE [graph_block] ([parent_graph] INETGER, [parent_export] INTEGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [x] REAL, [y] REAL, [width] REAL, [height] REAL, [expand_id] INTEGER);")
    cur.execute("CREATE TABLE [graph_cell] ([parent_graph] INETGER, [parent_export] INTEGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [x] REAL, [y] REAL, [type] INTEGER);")
    cur.execute("CREATE TABLE [graph_particle] ([parent_graph] INETGER, [parent_export] INTEGER, [thisobj] INTEGER, [name] TEXT, [x] REAL, [y] REAL, [type] INTEGER, [relation_block] INTEGER);")
    cur.execute("CREATE TABLE [graph_link] ([parent_graph] INETGER, [parent_export] INTEGER, [delay] INTEGER, [start_particle] INTEGER, [end_particle] INTEGER, [start_block] INTEGER, [end_block] INTEGER, [line_type] INTEGER, [x1] REAL, [y1] REAL, [x2] REAL, [y2] REAL);")

    db.commit()
    cur.close()

def __GenerateCompositions(cfg: CustomConfig.CustomConfig) -> tuple[tuple[CompositionIngredients], tuple[str], tuple[str]]:
    compositions: list[CompositionIngredients] = []
    exportdb: collections.OrderedDict = collections.OrderedDict()
    envdb: collections.OrderedDict = collections.OrderedDict()

    for entry in cfg.m_InputEntries:
        # check 2 database
        export_id = exportdb.get(entry.m_ExportDb, None)
        if export_id is None:
            export_id = len(exportdb)
            exportdb[entry.m_ExportDb] = export_id

        env_id = envdb.get(entry.m_EnvDb, None)
        if env_id is None:
            env_id = len(envdb)
            envdb[entry.m_EnvDb] = env_id

        # create record
        compositions.append(CompositionIngredients(entry.m_Name, export_id, env_id))

    return (
        tuple(compositions), 
        tuple(exportdb.keys()),
        tuple(envdb.keys())
    )

def __UploadComposition(db: sqlite3.Connection, compositions: list[CompositionIngredients]):
    cur = db.cursor()
    cur.executemany('INSERT INTO [compositions] VALUES(?, ?, ?, ?);',
        ((idx, ingredient.m_CompositionName, ingredient.m_ExportIndex, ingredient.m_EnvIndex) for idx, ingredient in enumerate(compositions))
    )

    db.commit()
    cur.close()

def __ProcessGraph(cfg: CustomConfig.CustomConfig, export_id: int, export_filename: str):
    # create database and apply encoding
    db: sqlite3.Connection = sqlite3.connect(export_filename)
    db.text_factory = lambda x: x.decode(cfg.m_DatabaseEncoding, errors="ignore")

    # create a cursor to get graph
    cur: sqlite3.Cursor = db.cursor()
    cur.execute('SELECT [thisobj] FROM [script_behavior] WHERE [type] != ?;',
        (DecoratorConst.Export_Behavior_Type.FUNCTION, )
    )

    # proc each graph
    for (thisobj, ) in cur.fetchall():
        # todo: add more proc step
        blocks: DecoratorBB.BlocksFactory = DecoratorBB.BlocksFactory(db, thisobj)
        del blocks

    # todo: recursively build [graph_graph]

    # close db
    cur.close()
    db.close()


def Run(cfg: CustomConfig.CustomConfig):
    # establish target database
    print('Opening decorated database...')
    decorateDb: sqlite3.Connection = sqlite3.connect(cfg.m_DecoratedDb)

    # init table
    print('Initializing decorated database...')
    __InitDecoratedDb(decorateDb)
    decorateDb.commit()

    # we need know which database we need analyse first
    print('Generating compositions...')
    (compositions, exportdb, envdb) = __GenerateCompositions(cfg)
    __UploadComposition(decorateDb, compositions)
    print(f'Analysation done. {len(exportdb)} Export DB and {len(envdb)} Env DB.')

    # process export
    print('Generating graphs...')
    progressbar: Progressbar.Prograssbar = Progressbar.Prograssbar(len(exportdb))
    for expid, exp in enumerate(exportdb):
        __ProcessGraph(cfg, expid, exp)
    progressbar.Finish()

    # process env
    print('Generating infos...')
    progressbar = Progressbar.Prograssbar(len(envdb))
    for envid, env in enumerate(envdb):
        pass
    progressbar.Finish()

    # close database
    print('Closing decorated database...')
    decorateDb.commit()
    decorateDb.close()