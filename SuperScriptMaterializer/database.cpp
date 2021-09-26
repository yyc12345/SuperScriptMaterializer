#include "database.h"

#pragma region helper

void dbScriptDataStructHelper::init(CKParameterManager* paramManager) {
	_db_behavior = new db_script_behavior();
	_db_script = new db_script_script();
	_db_pTarget = new db_script_pTarget();
	_db_pIn = new db_script_pIn();
	_db_pOut = new db_script_pOut();
	_db_bIn = new db_script_bIn();
	_db_bOut = new db_script_bOut();
	_db_bLink = new db_script_bLink();
	_db_pLocal = new db_script_pLocal();
	_db_pAttr = new db_script_pAttr();
	_db_pLink = new db_script_pLink();
	_db_pData = new db_script_pData();
	_db_pOper = new db_script_pOper();
	_db_eLink = new db_script_eLink();

	_parameterManager = paramManager;
	_stringCache = (char*)malloc(STRINGCACHE_SIZE * sizeof(char));
	if (_stringCache == NULL)
		throw new std::bad_alloc();
}

void dbScriptDataStructHelper::dispose() {
	delete _db_behavior;
	delete _db_script;
	delete _db_pTarget;
	delete _db_pIn;
	delete _db_pOut;
	delete _db_bIn;
	delete _db_bOut;
	delete _db_bLink;
	delete _db_pData;
	delete _db_pLink;
	delete _db_pLocal;
	delete _db_pAttr;
	delete _db_pOper;
	delete _db_eLink;

	_parameterManager = NULL;
	free(_stringCache);
}

void dbDataDataStructHelper::init(CKParameterManager* paramManager) {
	_db_obj = new db_data_obj();
	_db_objHeader = new db_data_objHeader();
	_db_objBody = new db_data_objBody();
	_db_objParam = new db_data_objParam();
	_db_msg = new db_data_msg();

	_parameterManager = paramManager;
	_stringCache = (char*)malloc(STRINGCACHE_SIZE * sizeof(char));
	if (_stringCache == NULL)
		throw new std::bad_alloc();
}

void dbDataDataStructHelper::dispose() {
	delete _db_obj;
	delete _db_objHeader;
	delete _db_objBody;
	delete _db_objParam;
	delete _db_msg;

	_parameterManager = NULL;
	free(_stringCache);
}

void dbEnvDataStructHelper::init() {
	_db_op = new db_env_op();
	_db_param = new db_env_param();
	_db_attr = new db_env_attr();
	_db_plugin = new db_env_plugin();
	_db_variable = new db_env_variable();

	_stringCache = (char*)malloc(STRINGCACHE_SIZE * sizeof(char));
	if (_stringCache == NULL)
		throw new std::bad_alloc();
}

void dbEnvDataStructHelper::dispose() {
	delete _db_op;
	delete _db_param;
	delete _db_attr;
	delete _db_plugin;
	delete _db_variable;

	free(_stringCache);
}

#pragma endregion


#pragma region general func

void database::open(const char* file) {
	db = NULL;
	stmtCache = new std::vector<sqlite3_stmt*>();

	//open db
	int result;
	result = sqlite3_open(file, &db);
	if (result != SQLITE_OK) goto fail;

	// disable synchronous
	result = sqlite3_exec(db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
	if (result != SQLITE_OK) goto fail;

	// do some custom init
	if (!init()) goto fail;

	//start job
	sqlite3_exec(db, "begin;", NULL, NULL, NULL);

	return;
fail:
	db = NULL;
}

void database::close() {
	if (db == NULL) return;

	//stop job
	for (auto it = stmtCache->begin(); it != stmtCache->end(); it++) {
		if (*it != NULL)
			sqlite3_finalize(*it);
	}
	sqlite3_exec(db, "commit;", NULL, NULL, NULL);

	// do some custom job
	finalJob();

	//release res
	sqlite3_close(db);
	db = NULL;
	delete stmtCache;
}

sqlite3_stmt* database::safeStmt(size_t index) {
	if (index >= stmtCache->size()) {
		// need resize
		stmtCache->resize(index + 1, NULL);
	}
}

#define createTable(sql) result=sqlite3_exec(db,sql,NULL,NULL,NULL);if(result!=SQLITE_OK)return FALSE;

BOOL scriptDatabase::init() {
	pAttrUniqueEnsurance = new std::set<EXPAND_CK_ID>();

	int result;

	//init table
	result = sqlite3_exec(db, "begin;", NULL, NULL, NULL);
	if (result != SQLITE_OK) return FALSE;

	createTable("CREATE TABLE script([thisobj] INTEGER, [name] TEXT, [index] INTEGER, [behavior] INTEGER);");
	createTable("CREATE TABLE behavior([thisobj] INTEGER, [name] TEXT, [type] INTEGER, [proto_name] TEXT, [proto_guid] TEXT, [flags] INTEGER, [priority] INTEGER, [version] INTEGER, [pin_count] TEXT, [parent] INTEGER);");
	createTable("CREATE TABLE pTarget([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [belong_to] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);");
	createTable("CREATE TABLE pIn([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [belong_to] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);");
	createTable("CREATE TABLE pOut([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE bIn([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE bOut([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE bLink([input] INTEGER, [output] INTEGER, [delay] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_index] INETEGR, [belong_to] INTEGER);");
	createTable("CREATE TABLE pLocal([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [is_setting] INTEGER, [belong_to] INTEGER);");
	createTable("CREATE TABLE pData([field] TEXT, [data] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE pLink([input] INTEGER, [output] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_is_bb] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_is_bb] INTEGER, [output_index] INETEGR, [belong_to] INTEGER);");
	createTable("CREATE TABLE pOper([thisobj] INTEGER, [op] TEXT, [op_guid] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE eLink([export_obj] INTEGER, [internal_obj] INTEGER, [is_in] INTEGER, [index] INTEGER, [belong_to] INTEGER);");
	createTable("CREATE TABLE pAttr([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT);");

	result = sqlite3_exec(db, "commit;", NULL, NULL, NULL);
	if (result != SQLITE_OK) return FALSE;

	return TRUE;
}

BOOL scriptDatabase::finalJob() {
	//create index for quick select in following app
	sqlite3_exec(db, "begin;", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where1] ON behavior ([parent])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where2] ON pOper ([belong_to], [thisobj])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where3] ON pTarget ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where4] ON bIn ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where5] ON bOut ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where6] ON pIn ([belong_to], [thisobj])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where7] ON pOut ([belong_to], [thisobj])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where8] ON pLocal ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where9] ON pLink ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where10] ON bLink ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where11] ON elink ([belong_to])", NULL, NULL, NULL);
	sqlite3_exec(db, "CREATE INDEX [quick_where12] ON pAttr ([thisobj])", NULL, NULL, NULL);
	sqlite3_exec(db, "commit;", NULL, NULL, NULL);

	delete pAttrUniqueEnsurance;
	return TRUE;
}

BOOL dataDatabase::init() {

	int result;

	//init table
	result = sqlite3_exec(db, "begin;", NULL, NULL, NULL);
	if (result != SQLITE_OK) return FALSE;

	createTable("CREATE TABLE obj([thisObj] INTEGER, [name] TEXT, [type_classid] INTEGER, [type_guid] TEXT, [type_name] TEXT, [rows] INTEGER, [columns] INTEGER);");
	createTable("CREATE TABLE objHeader([index] INTEGER, [name] TEXT, [type] INTEGER, [param_type] TEXT, [param_type_guid] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE objBody([row] INTEGER, [column] INTEGER, [showcase] TEXT, [inner_object] INTEGER, [inner_param] INTEGER, [belong_to] INTEGER);");
	createTable("CREATE TABLE objParam([field] TEXT, [data] TEXT, [belong_to] INTEGER);");
	createTable("CREATE TABLE msg([index] INTEGER, [name] TEXT);");

	result = sqlite3_exec(db, "commit;", NULL, NULL, NULL);
	if (result != SQLITE_OK) return FALSE;

	return TRUE;
}

BOOL dataDatabase::finalJob() {
	return TRUE;
}

BOOL envDatabase::init() {

	int result;

	//init table
	result = sqlite3_exec(db, "begin;", NULL, NULL, NULL);
	if (result != SQLITE_OK) return FALSE;

	createTable("CREATE TABLE op([funcptr] INTEGER, [in1_guid] TEXT, [in2_guid] TEXT, [out_guid] TEXT, [op_guid] TEXT, [op_name] TEXT, [op_code] INTEGER);");
	createTable("CREATE TABLE param([index] INTEGER, [guid] TEXT, [derived_from] TEXT, [type_name] TEXT, [default_size] INTEGER, [func_CreateDefault] INTEGER, [func_Delete] INTEGER, [func_SaveLoad] INTEGER, [func_Check] INTEGER, [func_Copy] INTEGER, [func_String] INTEGER, [func_UICreator] INTEGER, [creator_dll_index] INTEGER, [creator_plugin_index] INTEGER, [dw_param] INTEGER, [dw_flags] INTEGER, [cid] INTEGER, [saver_manager] TEXT);");
	createTable("CREATE TABLE attr([index] INTEGER, [name] TEXT, [category_index] INTEGER, [category_name] TEXT, [flags] INTEGER, [param_index] INTEGER, [compatible_classid] INTEGER, [default_value] TEXT);");
	createTable("CREATE TABLE plugin([dll_index] INTEGER, [dll_name] TEXT, [plugin_index] INTEGER, [category] TEXT, [active] INTEGER, [needed_by_file] INTEGER, [guid] TEXT, [desc] TEXT, [author] TEXT, [summary] TEXT, [version] INTEGER, [func_init] INTEGER, [func_exit] INTEGER);");
	createTable("CREATE TABLE [variable] ([name] TEXT, [description] TEXT, [flags] INTEGER, [type] INTEGER, [representation] TEXT, [data] TEXT);");

	result = sqlite3_exec(db, "commit;", NULL, NULL, NULL);
	if (result != SQLITE_OK) return FALSE;

	return TRUE;
}

BOOL envDatabase::finalJob() {
	return TRUE;
}

#undef createTable

#pragma endregion

#pragma region write func

#define tryGetStmt(index,defaultStr) safeStmt(index);stmt=(*stmtCache)[index];if(stmt==NULL){sqlite3_prepare_v2(db,defaultStr,-1,&stmt,NULL);(*stmtCache)[index]=stmt;}

void scriptDatabase::write_behavior(db_script_behavior* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(0, "INSERT INTO behavior VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->type);
	sqlite3_bind_text(stmt, 4, data->proto_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data->proto_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data->flags);
	sqlite3_bind_int(stmt, 7, data->priority);
	sqlite3_bind_int(stmt, 8, data->version);
	sqlite3_bind_text(stmt, 9, data->pin_count.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 10, data->parent);
	sqlite3_step(stmt);
}

void scriptDatabase::write_script(db_script_script* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(1, "INSERT INTO script VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->host_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->index);
	sqlite3_bind_int(stmt, 4, data->behavior);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pTarget(db_script_pTarget* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(2, "INSERT INTO pTarget VALUES (?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data->type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data->belong_to);
	sqlite3_bind_int(stmt, 6, data->direct_source);
	sqlite3_bind_int(stmt, 7, data->shared_source);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pIn(db_script_pIn* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(3, "INSERT INTO pIn VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_int(stmt, 2, data->index);
	sqlite3_bind_text(stmt, 3, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data->type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data->belong_to);
	sqlite3_bind_int(stmt, 7, data->direct_source);
	sqlite3_bind_int(stmt, 8, data->shared_source);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pOut(db_script_pOut* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(4, "INSERT INTO pOut VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_int(stmt, 2, data->index);
	sqlite3_bind_text(stmt, 3, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data->type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_bIn(db_script_bIn* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(5, "INSERT INTO bIn VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_int(stmt, 2, data->index);
	sqlite3_bind_text(stmt, 3, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_bOut(db_script_bOut* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(6, "INSERT INTO bOut VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_int(stmt, 2, data->index);
	sqlite3_bind_text(stmt, 3, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_bLink(db_script_bLink* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(7, "INSERT INTO bLink VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->input);
	sqlite3_bind_int(stmt, 2, data->output);
	sqlite3_bind_int(stmt, 3, data->delay);
	sqlite3_bind_int(stmt, 4, data->input_obj);
	sqlite3_bind_int(stmt, 5, data->input_type);
	sqlite3_bind_int(stmt, 6, data->input_index);
	sqlite3_bind_int(stmt, 7, data->output_obj);
	sqlite3_bind_int(stmt, 8, data->output_type);
	sqlite3_bind_int(stmt, 9, data->output_index);
	sqlite3_bind_int(stmt, 10, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pLocal(db_script_pLocal* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(8, "INSERT INTO pLocal VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data->type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data->is_setting);
	sqlite3_bind_int(stmt, 6, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pLink(db_script_pLink* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(9, "INSERT INTO pLink VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->input);
	sqlite3_bind_int(stmt, 2, data->output);
	sqlite3_bind_int(stmt, 3, data->input_obj);
	sqlite3_bind_int(stmt, 4, data->input_type);
	sqlite3_bind_int(stmt, 5, data->input_is_bb);
	sqlite3_bind_int(stmt, 6, data->input_index);
	sqlite3_bind_int(stmt, 7, data->output_obj);
	sqlite3_bind_int(stmt, 8, data->output_type);
	sqlite3_bind_int(stmt, 9, data->output_is_bb);
	sqlite3_bind_int(stmt, 10, data->output_index);
	sqlite3_bind_int(stmt, 11, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pData(db_script_pData* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(10, "INSERT INTO pData VALUES (?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_text(stmt, 1, data->field.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, data->data.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_pOper(db_script_pOper* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(11, "INSERT INTO pOper VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->op.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data->op_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data->belong_to);
	sqlite3_step(stmt);
}

void scriptDatabase::write_eLink(db_script_eLink* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(12, "INSERT INTO eLink VALUES (?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->export_obj);
	sqlite3_bind_int(stmt, 2, data->internal_obj);
	sqlite3_bind_int(stmt, 3, data->is_in);
	sqlite3_bind_int(stmt, 4, data->index);
	sqlite3_bind_int(stmt, 5, data->belong_to);
	sqlite3_step(stmt);
}

BOOL scriptDatabase::write_pAttr(db_script_pAttr* data) {
	if (db == NULL) return TRUE;

	if (pAttrUniqueEnsurance->find(data->thisobj) != pAttrUniqueEnsurance->end())
		return FALSE;	//existing item. skip it to make sure unique
	pAttrUniqueEnsurance->insert(data->thisobj);	//add this item

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(13, "INSERT INTO pAttr VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data->type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);

	return TRUE;
}


void dataDatabase::write_obj(db_data_obj* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(0, "INSERT INTO objParam VALUES (?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->thisobj);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->type_classid);
	sqlite3_bind_text(stmt, 4, data->type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data->type_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data->rows);
	sqlite3_bind_int(stmt, 7, data->columns);
	sqlite3_step(stmt);
}

void dataDatabase::write_objHeader(db_data_objHeader* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(1, "INSERT INTO objParam VALUES (?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->index);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->type);
	sqlite3_bind_text(stmt, 4, data->param_type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data->param_type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data->belong_to);
	sqlite3_step(stmt);
}

void dataDatabase::write_objBody(db_data_objBody* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(2, "INSERT INTO objParam VALUES (?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->row);
	sqlite3_bind_int(stmt, 2, data->column);
	sqlite3_bind_text(stmt, 3, data->showcase.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data->inner_object);
	sqlite3_bind_int(stmt, 5, data->inner_param);
	sqlite3_bind_int(stmt, 6, data->belong_to);
	sqlite3_step(stmt);
}

void dataDatabase::write_objParam(db_data_objParam* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(3, "INSERT INTO objParam VALUES (?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_text(stmt, 1, data->field.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, data->data.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->belong_to);
	sqlite3_step(stmt);
}

void dataDatabase::write_msg(db_data_msg* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(4, "INSERT INTO msg VALUES (?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->index);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
}


void envDatabase::write_op(db_env_op* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(0, "INSERT INTO op VALUES (?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, (int)data->funcPtr);
	sqlite3_bind_text(stmt, 2, data->in1_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data->in2_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->out_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data->op_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 6, data->op_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 7, data->op_code);
	sqlite3_step(stmt);
}

void envDatabase::write_param(db_env_param* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(1, "INSERT INTO param VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->index);
	sqlite3_bind_text(stmt, 2, data->guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data->derived_from.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data->type_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data->default_size);
	sqlite3_bind_int(stmt, 6, (int)data->func_CreateDefault);
	sqlite3_bind_int(stmt, 7, (int)data->func_Delete);
	sqlite3_bind_int(stmt, 8, (int)data->func_SaveLoad);
	sqlite3_bind_int(stmt, 9, (int)data->func_Check);
	sqlite3_bind_int(stmt, 10, (int)data->func_Copy);
	sqlite3_bind_int(stmt, 11, (int)data->func_String);
	sqlite3_bind_int(stmt, 12, (int)data->func_UICreator);
	sqlite3_bind_int(stmt, 13, data->creator_dll_index);
	sqlite3_bind_int(stmt, 14, data->creator_plugin_index);
	sqlite3_bind_int(stmt, 15, data->dw_param);
	sqlite3_bind_int(stmt, 16, data->dw_flags);
	sqlite3_bind_int(stmt, 17, data->cid);
	sqlite3_bind_text(stmt, 18, data->saver_manager.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
}

void envDatabase::write_attr(db_env_attr* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(2, "INSERT INTO attr VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->index);
	sqlite3_bind_text(stmt, 2, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->category_index);
	sqlite3_bind_text(stmt, 4, data->category_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data->flags);
	sqlite3_bind_int(stmt, 6, data->param_index);
	sqlite3_bind_int(stmt, 7, data->compatible_classid);
	sqlite3_bind_text(stmt, 8, data->default_value.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
}

void envDatabase::write_plugin(db_env_plugin* data) {
	if (db == NULL) return;

	sqlite3_stmt* stmt = NULL;
	tryGetStmt(3, "INSERT INTO plugin VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data->dll_index);
	sqlite3_bind_text(stmt, 2, data->dll_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->plugin_index);
	sqlite3_bind_text(stmt, 4, data->category.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data->active);
	sqlite3_bind_int(stmt, 6, data->needed_by_file);
	sqlite3_bind_text(stmt, 7, data->guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 8, data->desc.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 9, data->author.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 10, data->summary.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 11, data->version);
	sqlite3_bind_int(stmt, 12, (int)data->func_init);
	sqlite3_bind_int(stmt, 13, (int)data->func_exit);
	sqlite3_step(stmt);
}

void envDatabase::write_variable(db_env_variable* data) {
	if (db == NULL) return;

#if !defined(VIRTOOLS_21)
	sqlite3_stmt* stmt = NULL;
	tryGetStmt(4, "INSERT INTO [variable] VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_text(stmt, 1, data->name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, data->desciption.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data->flags);
	sqlite3_bind_int(stmt, 4, data->type);
	sqlite3_bind_text(stmt, 5, data->representation.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 6, data->data.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
#endif
}

#undef tryGetStmt

#pragma endregion
