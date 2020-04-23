#include "database.h"

#pragma region helper

void dbScriptDataStructHelper::init(CKParameterManager* paramManager) {
	_dbCKBehavior = new dbCKBehavior();
	_dbCKScript = new dbCKScript();
	_db_pTarget = new db_pTarget();
	_db_pIn = new db_pIn();
	_db_pOut = new db_pOut();
	_db_bIn = new db_bIn();
	_db_bOut = new db_bOut();
	_db_bLink = new db_bLink();
	_db_pLocal = new db_pLocal();
	_db_pLink = new db_pLink();
	_db_pLocalData = new db_pLocalData();
	_db_pOper = new db_pOper();
	_db_eLink = new db_eLink();
	_parameterManager = paramManager;
}

void dbScriptDataStructHelper::dispose() {
	delete _dbCKBehavior;
	delete _dbCKScript;
	delete _db_pTarget;
	delete _db_pIn;
	delete _db_pOut;
	delete _db_bIn;
	delete _db_bOut;
	delete _db_bLink;
	delete _db_pLocalData;
	delete _db_pLink;
	delete _db_pLocal;
	delete _db_pOper;
	delete _db_eLink;
	_parameterManager = NULL;
}

void dbEnvDataStructHelper::init() {
	_db_envOp = new db_envOp;
	_db_envParam = new db_envParam;
}

void dbEnvDataStructHelper::dispose() {
	delete _db_envOp;
	delete _db_envParam;
}

#pragma endregion


#pragma region general func

void database::open(const char* file) {
	db = NULL;
	errmsg = (char*)calloc(256, sizeof(char));
	commandStr = (char*)calloc(2048, sizeof(char));

	//open db
	int result;
	result = sqlite3_open(file, &db);
	if (result != SQLITE_OK) goto fail;

	if (!init()) goto fail;

	return;
fail:
	db = NULL;
}

void database::close() {
	if (db == NULL) return;

	finalJob();

	//release res
	sqlite3_close(db);
	db = NULL;

	free(errmsg);
	free(commandStr);
}

BOOL scriptDatabase::init() {
	int result;
	result = sqlite3_exec(db, "PRAGMA synchronous = OFF;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	//init table
	result = sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	result = sqlite3_exec(db,
		"CREATE TABLE script([thisobj] INTEGER, [name] TEXT, [index] INTEGER, [behavior] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE behavior([thisobj] INTEGER, [name] TEXT, [type] INTEGER, [proto_name] TEXT, [proto_guid] TEXT, [flags] INTEGER, [priority] INTEGER, [version] INTEGER, [pin_count] TEXT, [parent] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pTarget([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [belong_to] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pIn([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [belong_to] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pOut([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE bIn([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE bOut([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE bLink([input] INTEGER, [output] INTEGER, [delay] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_index] INETEGR, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pLocal([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [is_setting] INTEGER, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pLocalData([field] TEXT, [data] TEXT, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pLink([input] INTEGER, [output] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_is_bb] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_is_bb] INTEGER, [output_index] INETEGR, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE pOper([thisobj] INTEGER, [op] TEXT, [op_guid] TEXT, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE eLink([export_obj] INTEGER, [internal_obj] INTEGER, [is_in] INTEGER, [index] INTEGER, [belong_to] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	result = sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	//start job
	sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);

	return TRUE;
}

BOOL scriptDatabase::finalJob() {
	//stop job
	sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);

	//create index for quick select in following app
	/*sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX [quick_where1] ON bIn (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX [quick_where2] ON bOut (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX [quick_where3] ON pIn (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX [quick_where4] ON pOut (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);*/

	return TRUE;
}

BOOL envDatabase::init() {
	int result;
	result = sqlite3_exec(db, "PRAGMA synchronous = OFF;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	//init table
	result = sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	result = sqlite3_exec(db,
		"CREATE TABLE op([funcptr] INTEGER, [in1_guid] TEXT, [in2_guid] TEXT, [out_guid] TEXT, [op_guid] INTEGER, [op_name] TEXT, [op_code] INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;
	result = sqlite3_exec(db,
		"CREATE TABLE param([index] INTEGER, [guid] TEXT, [derived_from] TEXT, [type_name] TEXT, [default_size] INTEGER, [func_CreateDefault] INTEGER, [func_Delete] INTEGER, [func_SaveLoad] INTEGER, [func_Check] INTEGER, [func_Copy] INTEGER, [func_String] INTEGER, [func_UICreator] INTEGER, [creator_plugin_id] INTEGER, [dw_param] INTEGER, [dw_flags] INTEGER, [cid] INTEGER, [saver_manager] TEXT);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	result = sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) return FALSE;

	//start job
	sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);

	return TRUE;
}

BOOL envDatabase::finalJob() {
	//stop job
	sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);

	return TRUE;
}

#pragma endregion

#pragma region write func

void scriptDatabase::write_CKBehavior(dbCKBehavior* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO behavior VALUES (%d, '%s', %d, '%s', '%d, %d', %d, %d, %d, '%s', %d);",
		data->thisobj,
		data->name,
		data->type,
		data->proto_name,
		data->proto_guid[0],
		data->proto_guid[1],
		data->flags,
		data->priority,
		data->version,
		data->pin_count,
		data->parent);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_CKScript(dbCKScript* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO script VALUES (%d, '%s', %d, %d);",
		data->thisobj,
		data->host_name,
		data->index,
		data->behavior);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pTarget(db_pTarget* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pTarget VALUES (%d, '%s', '%s', '%d, %d', %d, %d, %d);",
		data->thisobj,
		data->name,
		data->type,
		data->type_guid[0],
		data->type_guid[1],
		data->belong_to,
		data->direct_source,
		data->shared_source);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pIn(db_pIn* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pIn VALUES (%d, %d, '%s', '%s', '%d, %d', %d, %d, %d);",
		data->thisobj,
		data->index,
		data->name,
		data->type,
		data->type_guid[0],
		data->type_guid[1],
		data->belong_to,
		data->direct_source,
		data->shared_source);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pOut(db_pOut* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pOut VALUES (%d, %d, '%s', '%s', '%d, %d', %d);",
		data->thisobj,
		data->index,
		data->name,
		data->type,
		data->type_guid[0],
		data->type_guid[1],
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_bIn(db_bIn* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO bIn VALUES (%d, %d, '%s', %d);",
		data->thisobj,
		data->index,
		data->name,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_bOut(db_bOut* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO bOut VALUES (%d, %d, '%s', %d);",
		data->thisobj,
		data->index,
		data->name,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_bLink(db_bLink* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO bLink VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d);",
		data->input,
		data->output,
		data->delay,
		data->input_obj,
		data->input_type,
		data->input_index,
		data->output_obj,
		data->output_type,
		data->output_index,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pLocal(db_pLocal* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pLocal VALUES (%d, '%s', '%s', '%d, %d', %d, %d);",
		data->thisobj,
		data->name,
		data->type,
		data->type_guid[0],
		data->type_guid[1],
		data->is_setting,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pLink(db_pLink* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pLink VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);",
		data->input,
		data->output,
		data->input_obj,
		data->input_type,
		data->input_is_bb,
		data->input_index,
		data->output_obj,
		data->output_type,
		data->output_is_bb,
		data->output_index,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pLocalData(db_pLocalData* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pLocalData VALUES ('%s', '%s', %d);",
		data->field,
		data->data,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_pOper(db_pOper* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pOper VALUES (%d, '%s', '%d, %d', %d);",
		data->thisobj,
		data->op,
		data->op_guid[0],
		data->op_guid[1],
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void scriptDatabase::write_eLink(db_eLink* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO eLink VALUES (%d, %d, %d, %d, %d);",
		data->export_obj,
		data->internal_obj,
		data->is_in,
		data->index,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void envDatabase::write_envOp(db_envOp* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO op VALUES (%d, '%d,%d', '%d,%d', '%d,%d', '%d,%d', '%s', %d);",
		data->funcPtr,
		data->in1_guid[0],
		data->in1_guid[1],
		data->in2_guid[0],
		data->in2_guid[1],
		data->out_guid[0],
		data->out_guid[1],
		data->op_guid[0],
		data->op_guid[1],
		data->op_name,
		data->op_code);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void envDatabase::write_envParam(db_envParam* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO param VALUES (%d, '%d,%d', '%d,%d', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%d,%d');",
		data->index,
		data->guid[0],
		data->guid[1],
		data->derived_from[0],
		data->derived_from[1],
		data->type_name,
		data->default_size,
		data->func_CreateDefault,
		data->func_Delete,
		data->func_SaveLoad,
		data->func_Check,
		data->func_Copy,
		data->func_String,
		data->func_UICreator,
		data->creator_plugin_id,
		data->dw_param,
		data->dw_flags,
		data->cid,
		data->saver_manager[0],
		data->saver_manager[1]);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}


#pragma endregion
