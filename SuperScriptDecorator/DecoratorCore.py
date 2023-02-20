import sqlite3, json, collections
import CustomConfig, Progressbar

class CompositionIngredients(object):
    def __init__(self, name: str, export_id: int, env_id: int):
        self.m_CompositionName: str = name
        self.m_ExportIndex = export_id
        self.m_EnvIndex = env_id

def _InitDecoratedDb(db: sqlite3.Connection):
    cur = db.cursor()
    cur.execute("CREATE TABLE [compositions] ([id] INTEGER, [name] TEXT, [export_id] INTEGER, [env_id] INTEGER);")

    cur.execute("CREATE TABLE graph([graph_ckid] INTEGER, [graph_name] TEXT, [hierarchy_ckid] TEXT, [export_id] INTEGER);")
    #cur.execute("CREATE TABLE info([target] INTEGER, [attach_bb] INTEGER, [is_setting] INTEGER, [name] TEXT, [field] TEXT, [data] TEXT);")

    #cur.execute("CREATE TABLE block([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [pin-ptarget] TEXT, [pin-pin] TEXT, [pin-pout] TEXT, [pin-bin] TEXT, [pin-bout] TEXT, [x] REAL, [y] REAL, [width] REAL, [height] REAL, [expandable] INTEGER);")
    #cur.execute("CREATE TABLE cell([belong_to_graph] INETGER, [thisobj] INTEGER, [name] TEXT, [assist_text] TEXT, [x] REAL, [y] REAL, [type] INTEGER);")
    #cur.execute("CREATE TABLE link([belong_to_graph] INETGER, [delay] INTEGER, [start_interface] INTEGER, [end_interface] INTEGER, [startobj] INTEGER, [endobj] INTEGER, [start_type] INTEGER, [end_type] INTEGER, [start_index] INTEGER, [end_index] INTEGER, [x1] REAL, [y1] REAL, [x2] REAL, [y2] REAL);")

    db.commit()
    cur.close()

def _GenerateCompositions(cfg: CustomConfig.CustomConfig) -> tuple[tuple[CompositionIngredients], tuple[str], tuple[str]]:
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
        tuple(exportdb.values()),
        tuple(envdb.values())
    )

def _UploadComposition(db: sqlite3.Connection, compositions: list[CompositionIngredients]):
    cur = db.cursor()
    for idx, ingredient in enumerate(compositions):
        cur.execute("INSERT INTO [compositions] VALUES(?, ?, ?, ?)", 
            (idx, ingredient.m_CompositionName, ingredient.m_ExportIndex, ingredient.m_EnvIndex)
        )

    db.commit()
    cur.close()

def Run(cfg: CustomConfig.CustomConfig):
    # establish target database
    print('Opening decorated database...')
    decorateDb: sqlite3.Connection = sqlite3.connect(cfg.m_DecoratedDb)

    # init table
    print('Initializing decorated database...')
    _InitDecoratedDb(decorateDb)
    decorateDb.commit()

    # we need know which database we need analyse first
    print('Generating compositions...')
    (compositions, exportdb, envdb) = _GenerateCompositions(cfg)
    _UploadComposition(decorateDb, compositions)
    print(f'Analysation done. {len(exportdb)} Export DB and {len(envdb)} Env DB.')

    # process export
    print('Generating graphs...')
    progressbar: Progressbar.Prograssbar = Progressbar.Prograssbar(len(exportdb))
    for expid, exp in enumerate(exportdb):
        pass
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