#include "database.h"

#pragma region helper

void dbDataStructHelper::init(CKParameterManager* paramManager) {
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
	_parameterManager = paramManager;
}

void dbDataStructHelper::dispose() {
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
	_parameterManager = NULL;
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

	result = sqlite3_exec(db, "PRAGMA synchronous = OFF;", NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;

	//init table
	result = sqlite3_exec(db,
		"CREATE TABLE script('thisobj' INTEGER, 'name' TEXT, 'index' INTEGER, 'behavior' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE behavior('thisobj' INTEGER, 'name' TEXT, 'type' INTEGER, 'proto_name' TEXT, 'proto_guid' TEXT, 'flags' INTEGER, 'priority' INTEGER, 'version' INTEGER, 'pin_count' TEXT, 'parent' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pTarget('thisobj' INTEGER, 'name' TEXT, 'type' TEXT, 'type_guid' TEXT, 'belong_to' INTEGER, 'direct_source' INTEGER, 'shard_source' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pIn('thisobj' INTEGER, 'index' INTEGER, 'name' TEXT, 'type' TEXT, 'type_guid' TEXT, 'belong_to' INTEGER, 'direct_source' INTEGER, 'shard_source' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pOut('thisobj' INTEGER, 'index' INTEGER, 'name' TEXT, 'type' TEXT, 'type_guid' TEXT, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE bIn('thisobj' INTEGER, 'index' INTEGER, 'name' TEXT, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE bOut('thisobj' INTEGER, 'index' INTEGER, 'name' TEXT, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE bLink('input' INTEGER, 'output' INTEGER, 'delay' INTEGER, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pLocal('thisobj' INTEGER, 'name' TEXT, 'type' TEXT, 'type_guid' TEXT, 'is_setting' INTEGER, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pLocalData('field' TEXT, 'data' TEXT, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pLink('input' INTEGER, 'output' INTEGER, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;
	result = sqlite3_exec(db,
		"CREATE TABLE pOper('thisobj' INTEGER, 'op' TEXT, 'op_guid' TEXT, 'belong_to' INTEGER);",
		NULL, NULL, &errmsg);
	if (result != SQLITE_OK) goto fail;


	//start job
	sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);

	//return
	return;
fail:
	db = NULL;
}

void database::close() {
	if (db == NULL) return;

	//stop job
	sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);

	//create index for quick select in following app
	sqlite3_exec(db, "begin;", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX 'quick_where1' ON bIn (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX 'quick_where2' ON bOut (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX 'quick_where3' ON pIn (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "CREATE INDEX 'quick_where4' ON pOut (thisobj)", NULL, NULL, &errmsg);
	sqlite3_exec(db, "commit;", NULL, NULL, &errmsg);

	//release res
	sqlite3_close(db);
	db = NULL;

	free(errmsg);
	free(commandStr);
}

#pragma endregion

#pragma region write func

void database::write_CKBehavior(dbCKBehavior* data) {
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

void database::write_CKScript(dbCKScript* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO script VALUES (%d, '%s', %d, %d);",
		data->thisobj,
		data->host_name,
		data->index,
		data->behavior);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void database::write_pTarget(db_pTarget* data) {
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

void database::write_pIn(db_pIn* data) {
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

void database::write_pOut(db_pOut* data) {
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

void database::write_bIn(db_bIn* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO bIn VALUES (%d, %d, '%s', %d);",
		data->thisobj,
		data->index,
		data->name,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void database::write_bOut(db_bOut* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO bOut VALUES (%d, %d, '%s', %d);",
		data->thisobj,
		data->index,
		data->name,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void database::write_bLink(db_bLink* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO bLink VALUES (%d, %d, %d, %d);",
		data->input,
		data->output,
		data->delay,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void database::write_pLocal(db_pLocal* data) {
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

void database::write_pLink(db_pLink* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pLink VALUES (%d, %d, %d);",
		data->input,
		data->output,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void database::write_pLocalData(db_pLocalData* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pLocalData VALUES ('%s', '%s', %d);",
		data->field,
		data->data,
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}

void database::write_pOper(db_pOper* data) {
	if (db == NULL) return;

	sprintf(commandStr, "INSERT INTO pOper VALUES (%d, '%s', '%d, %d', %d);",
		data->thisobj,
		data->op,
		data->op_guid[0],
		data->op_guid[1],
		data->belong_to);

	sqlite3_exec(db, commandStr, NULL, NULL, &errmsg);
}


#pragma endregion
