#include "database.h"

#pragma region data struct helper

DbDataStructHelper_Doc::DbDataStructHelper_Doc(CKParameterManager* paramManager) :
	script(), script_behavior(), script_pOper(),
	script_bIn(), script_bOut(), script_bLink(),
	script_pIn(), script_pOut(), script_pLink(), script_eLink(),
	script_pLocal(), script_pTarget(), script_pAttr(),
	msg(),
	_array(), array_header(), array_cell(),
	data() {
	param_manager = paramManager;
}

DbDataStructHelper_Doc::~DbDataStructHelper_Doc() {
	;
}

DbDataStructHelper_Env::DbDataStructHelper_Env() :
	op(), param(), attr(), plugin(), variable() {
	;
}

DbDataStructHelper_Env::~DbDataStructHelper_Env() {
	;
}

#pragma endregion

#pragma region universal sqlite database init and free

SSMaterializerDatabase::SSMaterializerDatabase(const char* file) :
	mStmtCache(), mDb(NULL) {
	// open mDb
	int result;
	result = sqlite3_open(file, &mDb);
	if (result != SQLITE_OK) goto fail;

	// disable synchronous
	result = sqlite3_exec(mDb, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
	if (result != SQLITE_OK) goto fail;

	// do some custom Init
	if (!Init()) goto fail;

	//start job
	sqlite3_exec(mDb, "begin;", NULL, NULL, NULL);

	return;
fail:
	mDb = NULL;
}

SSMaterializerDatabase::~SSMaterializerDatabase() {
	if (mDb == NULL) return;

	//free all cached stmts and commit job
	for (auto it = mStmtCache.begin(); it != mStmtCache.end(); it++) {
		if (*it != NULL)
			sqlite3_finalize(*it);
	}
	sqlite3_exec(mDb, "commit;", NULL, NULL, NULL);

	// do some custom job
	Finalize();

	//release res
	sqlite3_close(mDb);
	mDb = NULL;
}

sqlite3_stmt* SSMaterializerDatabase::CreateStmt(const char* str_stmt) {
	int result;
	sqlite3_stmt* stmt = NULL;
	result = sqlite3_prepare_v2(mDb, str_stmt, -1, &stmt, NULL);
	if (result != SQLITE_OK) return NULL;

	// append new one
	mStmtCache.push_back(stmt);
}

#pragma endregion

#pragma region sub-database constructor and deconstructor

DocumentDatabase::DocumentDatabase(const char* file) :
	SSMaterializerDatabase(file), m_pAttrUniqueEnsurance() {
	;
}

DocumentDatabase::~DocumentDatabase() {
	;
}

EnvironmentDatabase::EnvironmentDatabase(const char* file) :
	SSMaterializerDatabase(file) {
	;
}

EnvironmentDatabase::~EnvironmentDatabase() {
	;
}

#pragma endregion

#pragma region table, index creation functions

#define SafeSqlExec(sql) result = sqlite3_exec(mDb, sql, NULL, NULL, NULL); \
if (result != SQLITE_OK) { return FALSE; }

BOOL DocumentDatabase::Init() {
	int result;

	//Init table
	SafeSqlExec("begin;");

	SafeSqlExec("CREATE TABLE [script] ([thisobj] INTEGER, [name] TEXT, [index] INTEGER, [behavior] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_behavior] ([thisobj] INTEGER, [name] TEXT, [type] INTEGER, [proto_name] TEXT, [proto_guid] TEXT, [flags] INTEGER, [priority] INTEGER, [version] INTEGER, [pin_count] TEXT, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pTarget] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pIn] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER, [direct_source] INTEGER, [shared_source] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pOut] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_bIn] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_bOut] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_bLink] ([input] INTEGER, [output] INTEGER, [delay] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_index] INETEGR, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pLocal] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [is_setting] INTEGER, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pLink] ([input] INTEGER, [output] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_is_bb] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_is_bb] INTEGER, [output_index] INETEGR, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pOper] ([thisobj] INTEGER, [op] TEXT, [op_guid] TEXT, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_eLink] ([export_obj] INTEGER, [internal_obj] INTEGER, [is_in] INTEGER, [index] INTEGER, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [script_pAttr] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT);");

	SafeSqlExec("CREATE TABLE [msg] ([index] INTEGER, [name] TEXT);");

	SafeSqlExec("CREATE TABLE [array] ([thisobj] INTEGER, [name] TEXT, [rows] INTEGER, [columns] INTEGER);");
	SafeSqlExec("CREATE TABLE [array_header] ([index] INTEGER, [name] TEXT, [type] INTEGER, [param_type] TEXT, [param_type_guid] TEXT, [parent] INTEGER);");
	SafeSqlExec("CREATE TABLE [array_cell] ([row] INTEGER, [column] INTEGER, [showcase] TEXT, [inner_param] INTEGER, [parent] INTEGER);");

	SafeSqlExec("CREATE TABLE [data] ([field] TEXT, [data] TEXT, [parent] INTEGER);");

	SafeSqlExec("commit;");

	return TRUE;
}

BOOL DocumentDatabase::Finalize() {
	//create index for quick select in SuperScriptDecorator
	int result;

	SafeSqlExec("begin;");
	SafeSqlExec("CREATE INDEX [quick_where1] ON [script_behavior] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where2] ON [script_pOper] ([parent], [thisobj])");
	SafeSqlExec("CREATE INDEX [quick_where3] ON [script_pTarget] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where4] ON [script_bIn] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where5] ON [script_bOut] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where6] ON [script_pIn] ([parent], [thisobj])");
	SafeSqlExec("CREATE INDEX [quick_where7] ON [script_pOut] ([parent], [thisobj])");
	SafeSqlExec("CREATE INDEX [quick_where8] ON [script_pLocal] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where9] ON [script_pLink] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where10] ON [script_bLink] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where11] ON [script_elink] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where12] ON [script_pAttr] ([thisobj])");
	SafeSqlExec("CREATE INDEX [quick_where13] ON [array_cell] ([parent])");
	SafeSqlExec("CREATE INDEX [quick_where14] ON [data] ([parent])");
	SafeSqlExec("commit;");

	return TRUE;
}

BOOL EnvironmentDatabase::Init() {
	int result;

	//init table
	SafeSqlExec("begin;");

	SafeSqlExec("CREATE TABLE [op] ([funcptr] INTEGER, [in1_guid] TEXT, [in2_guid] TEXT, [out_guid] TEXT, [op_guid] TEXT, [op_name] TEXT, [op_code] INTEGER);");
	SafeSqlExec("CREATE TABLE [param] ([index] INTEGER, [guid] TEXT, [derived_from] TEXT, [type_name] TEXT, [default_size] INTEGER, [func_CreateDefault] INTEGER, [func_Delete] INTEGER, [func_SaveLoad] INTEGER, [func_Check] INTEGER, [func_Copy] INTEGER, [func_String] INTEGER, [func_UICreator] INTEGER, [creator_dll_index] INTEGER, [creator_plugin_index] INTEGER, [dw_param] INTEGER, [dw_flags] INTEGER, [cid] INTEGER, [saver_manager] TEXT);");
	SafeSqlExec("CREATE TABLE [attr] ([index] INTEGER, [name] TEXT, [category_index] INTEGER, [category_name] TEXT, [flags] INTEGER, [param_index] INTEGER, [compatible_classid] INTEGER, [default_value] TEXT);");
	SafeSqlExec("CREATE TABLE [plugin] ([dll_index] INTEGER, [dll_name] TEXT, [plugin_index] INTEGER, [category] TEXT, [active] INTEGER, [guid] TEXT, [desc] TEXT, [author] TEXT, [summary] TEXT, [version] INTEGER, [func_init] INTEGER, [func_exit] INTEGER);");
	SafeSqlExec("CREATE TABLE [variable] ([name] TEXT, [description] TEXT, [flags] INTEGER, [type] INTEGER, [representation] TEXT, [data] TEXT);");

	SafeSqlExec("commit;");

	return TRUE;
}

BOOL EnvironmentDatabase::Finalize() {
	return TRUE;
}

#undef SafeSqlExec

#pragma endregion

#define TryGetStmtCache(str_sql) static sqlite3_stmt* stmt = NULL; \
if (stmt == NULL) { \
	stmt = CreateStmt(str_sql); \
	if (stmt == NULL) return; \
}

#pragma region document database

void DocumentDatabase::write_script(dbdoc_script& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script] VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.host_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.index);
	sqlite3_bind_int(stmt, 4, data.behavior);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_behavior(dbdoc_script_behavior& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_behavior] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.type);
	sqlite3_bind_text(stmt, 4, data.proto_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data.proto_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data.flags);
	sqlite3_bind_int(stmt, 7, data.priority);
	sqlite3_bind_int(stmt, 8, data.version);
	sqlite3_bind_text(stmt, 9, data.pin_count.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 10, data.parent);
	sqlite3_step(stmt);
}


void DocumentDatabase::write_script_pTarget(dbdoc_script_pTarget& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pTarget] VALUES (?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data.parent);
	sqlite3_bind_int(stmt, 6, data.direct_source);
	sqlite3_bind_int(stmt, 7, data.shared_source);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_pIn(dbdoc_script_pIn& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pIn] VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_int(stmt, 2, data.index);
	sqlite3_bind_text(stmt, 3, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data.type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data.parent);
	sqlite3_bind_int(stmt, 7, data.direct_source);
	sqlite3_bind_int(stmt, 8, data.shared_source);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_pOut(dbdoc_script_pOut& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pOut] VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_int(stmt, 2, data.index);
	sqlite3_bind_text(stmt, 3, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data.type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_bIn(dbdoc_script_bIn& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_bIn] VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_int(stmt, 2, data.index);
	sqlite3_bind_text(stmt, 3, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_bOut(dbdoc_script_bOut& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_bOut] VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_int(stmt, 2, data.index);
	sqlite3_bind_text(stmt, 3, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_bLink(dbdoc_script_bLink& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_bLink] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.input);
	sqlite3_bind_int(stmt, 2, data.output);
	sqlite3_bind_int(stmt, 3, data.delay);
	sqlite3_bind_int(stmt, 4, data.input_obj);
	sqlite3_bind_int(stmt, 5, data.input_type);
	sqlite3_bind_int(stmt, 6, data.input_index);
	sqlite3_bind_int(stmt, 7, data.output_obj);
	sqlite3_bind_int(stmt, 8, data.output_type);
	sqlite3_bind_int(stmt, 9, data.output_index);
	sqlite3_bind_int(stmt, 10, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_pLocal(dbdoc_script_pLocal& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pLocal] VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data.is_setting);
	sqlite3_bind_int(stmt, 6, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_pLink(dbdoc_script_pLink& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pLink] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.input);
	sqlite3_bind_int(stmt, 2, data.output);
	sqlite3_bind_int(stmt, 3, data.input_obj);
	sqlite3_bind_int(stmt, 4, data.input_type);
	sqlite3_bind_int(stmt, 5, data.input_is_bb);
	sqlite3_bind_int(stmt, 6, data.input_index);
	sqlite3_bind_int(stmt, 7, data.output_obj);
	sqlite3_bind_int(stmt, 8, data.output_type);
	sqlite3_bind_int(stmt, 9, data.output_is_bb);
	sqlite3_bind_int(stmt, 10, data.output_index);
	sqlite3_bind_int(stmt, 11, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_pOper(dbdoc_script_pOper& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pOper] VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.op.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.op_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_eLink(dbdoc_script_eLink& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_eLink] VALUES (?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.export_obj);
	sqlite3_bind_int(stmt, 2, data.internal_obj);
	sqlite3_bind_int(stmt, 3, data.is_in);
	sqlite3_bind_int(stmt, 4, data.index);
	sqlite3_bind_int(stmt, 5, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_script_pAttr(dbdoc_script_pAttr& data, BOOL* already_exist) {
	// check duplication first
	if (m_pAttrUniqueEnsurance.find(data.thisobj) != m_pAttrUniqueEnsurance.end()) {
		//existing item. skip it to make sure unique
		*already_exist = TRUE;
		return;
	} else {
		//add this item
		m_pAttrUniqueEnsurance.insert(data.thisobj);
		*already_exist = FALSE;
	}

	// then check database validation
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [script_pAttr] VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);

	return;
}

void DocumentDatabase::write_msg(dbdoc_msg& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [msg] VALUES (?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.index);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_array(dbdoc_array& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [array] VALUES (?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.thisobj);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.rows);
	sqlite3_bind_int(stmt, 4, data.columns);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_array_header(dbdoc_array_header& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [array_header] VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.index);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.type);
	sqlite3_bind_text(stmt, 4, data.param_type.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data.param_type_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 6, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_array_cell(dbdoc_array_cell& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [array_cell] VALUES (?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.row);
	sqlite3_bind_int(stmt, 2, data.column);
	sqlite3_bind_text(stmt, 3, data.showcase.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 4, data.inner_param);
	sqlite3_bind_int(stmt, 5, data.parent);
	sqlite3_step(stmt);
}

void DocumentDatabase::write_data(dbdoc_data& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [data] VALUES (?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_text(stmt, 1, data.field.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, data.data.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.parent);
	sqlite3_step(stmt);
}

#pragma endregion

#pragma region environment database

void EnvironmentDatabase::write_op(dbenv_op& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [op] VALUES (?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, (int)data.funcPtr);
	sqlite3_bind_text(stmt, 2, data.in1_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.in2_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.out_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 5, data.op_guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 6, data.op_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 7, data.op_code);
	sqlite3_step(stmt);
}

void EnvironmentDatabase::write_param(dbenv_param& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [param] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.index);
	sqlite3_bind_text(stmt, 2, data.guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, data.derived_from.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, data.type_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data.default_size);
	sqlite3_bind_int(stmt, 6, (int)data.func_CreateDefault);
	sqlite3_bind_int(stmt, 7, (int)data.func_Delete);
	sqlite3_bind_int(stmt, 8, (int)data.func_SaveLoad);
	sqlite3_bind_int(stmt, 9, (int)data.func_Check);
	sqlite3_bind_int(stmt, 10, (int)data.func_Copy);
	sqlite3_bind_int(stmt, 11, (int)data.func_String);
	sqlite3_bind_int(stmt, 12, (int)data.func_UICreator);
	sqlite3_bind_int(stmt, 13, data.creator_dll_index);
	sqlite3_bind_int(stmt, 14, data.creator_plugin_index);
	sqlite3_bind_int(stmt, 15, data.dw_param);
	sqlite3_bind_int(stmt, 16, data.dw_flags);
	sqlite3_bind_int(stmt, 17, data.cid);
	sqlite3_bind_text(stmt, 18, data.saver_manager.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
}

void EnvironmentDatabase::write_attr(dbenv_attr& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [attr] VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.index);
	sqlite3_bind_text(stmt, 2, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.category_index);
	sqlite3_bind_text(stmt, 4, data.category_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data.flags);
	sqlite3_bind_int(stmt, 6, data.param_index);
	sqlite3_bind_int(stmt, 7, data.compatible_classid);
	sqlite3_bind_text(stmt, 8, data.default_value.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
}

void EnvironmentDatabase::write_plugin(dbenv_plugin& data) {
	if (mDb == NULL) return;

	TryGetStmtCache("INSERT INTO [plugin] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_int(stmt, 1, data.dll_index);
	sqlite3_bind_text(stmt, 2, data.dll_name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.plugin_index);
	sqlite3_bind_text(stmt, 4, data.category.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, data.active);
	sqlite3_bind_text(stmt, 6, data.guid.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 7, data.desc.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 8, data.author.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 9, data.summary.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 10, data.version);
	sqlite3_bind_int(stmt, 11, (int)data.func_init);
	sqlite3_bind_int(stmt, 12, (int)data.func_exit);
	sqlite3_step(stmt);
}

void EnvironmentDatabase::write_variable(dbenv_variable& data) {
	if (mDb == NULL) return;

#if !defined(VIRTOOLS_21)
	TryGetStmtCache("INSERT INTO [variable] VALUES (?, ?, ?, ?, ?, ?)");
	sqlite3_reset(stmt);

	sqlite3_bind_text(stmt, 1, data.name.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, data.desciption.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 3, data.flags);
	sqlite3_bind_int(stmt, 4, data.type);
	sqlite3_bind_text(stmt, 5, data.representation.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 6, data.data.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_step(stmt);
#endif
}

#pragma endregion

#undef TryGetStmtCache

