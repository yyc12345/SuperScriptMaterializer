#include "Database.hpp"
#include <stdexcept>
#include <type_traits>

namespace VSW::Materializer::Database {

#pragma region Help Macros

#define BEGIN_CTOR int errcode;
#define CTOR_SQL_EXEC(sql) errcode = sqlite3_exec(this->GetDb(), sql, nullptr, nullptr, nullptr); \
if (errcode != SQLITE_OK) goto failed;
#define END_CTOR return; \
failed: throw std::runtime_error("fail to initialize database.");
#define BEGIN_DTOR int errcode;
#define DTOR_SQL_EXEC(sql) errcode = sqlite3_exec(this->GetDb(), sql, nullptr, nullptr, nullptr); \
if (errcode != SQLITE_OK) return;
#define END_DTOR ;

	/// @brief 
	/// Check database connection first. 
	/// Create index variables for sqlite prepared statement binding use. 
	/// Fetch and reset prepared statement
#define BEGIN_WRITER(stmt_str) if (!this->IsValid()) throw std::runtime_error("write on disconnected database."); \
int argument_index = 1; \
sqlite3_stmt* stmt = GetStmt(stmt_str); \
sqlite3_reset(stmt);
	/// @brief Wrapper of binding function calling.
#define WRITER_BIND(eval) if ((eval) != SQLITE_OK) goto failed;
	/// @brief Get auto incresement index during binding.
#define WRITER_INDEX (argument_index++)
	/// @brief Get prepared statement during binding.
#define WRITER_STMT (stmt)
	/// @brief 
	/// Sumbit binded prepared statement.
	/// Binding error process.
#define END_WRITER if (sqlite3_step(stmt) == SQLITE_DONE) return; \
failed: throw std::runtime_error("fail to bind value for prepared statement.");

#define REVEAL_ENUM(enum_val) static_cast<std::underlying_type_t<decltype(enum_val)>>(enum_val)
#define REVEAL_U8STR(u8_str) YYCC::EncodingHelper::ToOrdinary(u8_str.c_str())

#pragma endregion

#pragma region Abstract Database

	AbstractDatabase::AbstractDatabase(const YYCC::yycc_u8string_view& file) :
		m_Db(nullptr), m_StmtCache() {
		// connect to database (the argument of sqlite3 open function is UTF8)
		int errcode;
		if (file.empty()) goto failed;
		errcode = sqlite3_open(YYCC::EncodingHelper::ToOrdinary(file.data()), &m_Db);
		if (errcode != SQLITE_OK) goto failed;

		// disable synchronous to accelerate speed.
		errcode = sqlite3_exec(m_Db, "PRAGMA synchronous = OFF;", nullptr, nullptr, nullptr);
		if (errcode != SQLITE_OK) goto failed_after_open;

		// start transaction
		errcode = sqlite3_exec(m_Db, "begin;", nullptr, nullptr, nullptr);
		if (errcode != SQLITE_OK) goto failed_after_open;

		return;

	failed_after_open:
		sqlite3_close(m_Db);
	failed:
		m_Db = nullptr;
	}

	AbstractDatabase::~AbstractDatabase() {
		if (m_Db == nullptr) return;
		int errcode;

		// free all cached stmts
		for (const auto& pair : m_StmtCache) {
			if (pair.second != nullptr) {
				errcode = sqlite3_finalize(pair.second);
				if (errcode != SQLITE_OK) goto failed;
			}
		}

		// commit transaction
		errcode = sqlite3_exec(m_Db, "commit;", nullptr, nullptr, nullptr);
		if (errcode != SQLITE_OK) goto failed;

	failed:
		// directly release resources
		sqlite3_close(m_Db);
		m_Db = nullptr;
	}

	bool AbstractDatabase::IsValid() const {
		return m_Db != nullptr;
	}

	sqlite3_stmt* AbstractDatabase::GetStmt(const char* stmt_str) {
		// try to find matched cache
		auto finder = m_StmtCache.find(stmt_str);
		if (finder != m_StmtCache.end()) {
			return finder->second;
		}

		// no found. create one
		sqlite3_stmt* stmt = nullptr;
		int errcode = sqlite3_prepare_v2(m_Db, stmt_str, -1, &stmt, nullptr);
		if (errcode != SQLITE_OK) throw std::runtime_error("fail to create sqlite3 stmt!");

		// append new one
		m_StmtCache.emplace(stmt_str, stmt);
		return stmt;
	}

	sqlite3* AbstractDatabase::GetDb() {
		return m_Db;
	}

#pragma endregion

#pragma region Script Database

	ScriptDatabase::ScriptDatabase(const YYCC::yycc_u8string_view& file) :
		AbstractDatabase(file) {
		// initialize table
		BEGIN_CTOR;
		CTOR_SQL_EXEC("CREATE TABLE [script] ([thisobj] INTEGER, [name] TEXT, [index] INTEGER, [behavior] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [behavior] ([thisobj] INTEGER, [name] TEXT, [type] INTEGER, [proto_name] TEXT, [proto_guid] TEXT, [flags] INTEGER, [priority] INTEGER, [version] INTEGER, [pin_count] TEXT, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pTarget] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pIn] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER, [direct_source] INTEGER, [shared_source] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pOut] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [bIn] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [bOut] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [bLink] ([input] INTEGER, [output] INTEGER, [delay] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_index] INETEGR, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pLocal] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [is_setting] INTEGER, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pLink] ([input] INTEGER, [output] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_is_bb] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_is_bb] INTEGER, [output_index] INETEGR, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pOper] ([thisobj] INTEGER, [op] TEXT, [op_guid] TEXT, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [eLink] ([export_obj] INTEGER, [internal_obj] INTEGER, [is_in] INTEGER, [index] INTEGER, [parent] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [pAttr] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT);");
		CTOR_SQL_EXEC("CREATE TABLE [data] ([field] TEXT, [data] TEXT, [parent] INTEGER);");
		END_CTOR;
	}

	ScriptDatabase::~ScriptDatabase() {
		// create index for quick select in following process
		BEGIN_DTOR;
		DTOR_SQL_EXEC("begin;");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where1] ON [behavior] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where2] ON [pOper] ([parent], [thisobj])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where3] ON [pTarget] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where4] ON [bIn] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where5] ON [bOut] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where6] ON [pIn] ([parent], [thisobj])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where7] ON [pOut] ([parent], [thisobj])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where8] ON [pLocal] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where9] ON [pLink] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where10] ON [bLink] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where11] ON [elink] ([parent])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where12] ON [pAttr] ([thisobj])");
		DTOR_SQL_EXEC("CREATE INDEX [quick_where13] ON [data] ([parent])");
		DTOR_SQL_EXEC("commit;");
		END_DTOR;
	}

	void ScriptDatabase::Write(const DataTypes::Script::Table_script& data) {
		BEGIN_WRITER("INSERT INTO [script] VALUES (?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.host_name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.behavior));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_behavior& data) {
		BEGIN_WRITER("INSERT INTO [script_behavior] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.type));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.proto_name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.proto_guid.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.flags));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.priority));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.version));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.pin_count.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_bIn& data) {
		BEGIN_WRITER("INSERT INTO [script_bIn] VALUES (?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_bOut& data) {
		BEGIN_WRITER("INSERT INTO [script_bOut] VALUES (?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pIn& data) {
		BEGIN_WRITER("INSERT INTO [script_pIn] VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type_guid.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.direct_source));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.shared_source));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pOut& data) {
		BEGIN_WRITER("INSERT INTO [script_pOut] VALUES (?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type_guid.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_bLink& data) {
		BEGIN_WRITER("INSERT INTO [script_bLink] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.delay));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input_obj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, REVEAL_ENUM(data.input_type)));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input_index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output_obj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, REVEAL_ENUM(data.output_type)));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output_index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pLocal& data) {
		BEGIN_WRITER("INSERT INTO [script_pLocal] VALUES (?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type_guid.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.is_setting));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pAttr& data) {
		BEGIN_WRITER("INSERT INTO [script_pAttr] VALUES (?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type_guid.c_str(), -1, SQLITE_TRANSIENT));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pLink& data) {
		BEGIN_WRITER("INSERT INTO [script_pLink] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input_obj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, REVEAL_ENUM(data.input_type)));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input_is_bb));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.input_index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output_obj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, REVEAL_ENUM(data.output_type)));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output_is_bb));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.output_index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pOper& data) {
		BEGIN_WRITER("INSERT INTO [script_pOper] VALUES (?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.op.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.op_guid.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_eLink& data) {
		BEGIN_WRITER("INSERT INTO [script_eLink] VALUES (?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.export_obj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.internal_obj));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.is_in));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_pTarget& data) {
		BEGIN_WRITER("INSERT INTO [script_pTarget] VALUES (?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.thisobj));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.type_guid.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.direct_source));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.shared_source));
		END_WRITER;
	}
	void ScriptDatabase::Write(const DataTypes::Script::Table_data& data) {
		BEGIN_WRITER("INSERT INTO [data] VALUES (?, ?, ?);");
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.field.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.data.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.parent));
		END_WRITER;
	}

#pragma endregion

#pragma region Document Database

	DocumentDatabase::DocumentDatabase(const YYCC::yycc_u8string_view& file) :
		AbstractDatabase(file) {
		// initialize table
		BEGIN_CTOR;
		CTOR_SQL_EXEC("CREATE TABLE [msg] ([index] INTEGER, [name] TEXT);");
		CTOR_SQL_EXEC("CREATE TABLE [obj] ([id] INTEGER, [name] TEXT, [classid] INTEGER, [classtype] TEXT);");
		END_CTOR;
	}

	DocumentDatabase::~DocumentDatabase() {
		// do nothing
	}

	void DocumentDatabase::Write(const DataTypes::Document::Table_msg& data) {
		BEGIN_WRITER("INSERT INTO [msg] VALUES (?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		END_WRITER;
	}
	void DocumentDatabase::Write(const DataTypes::Document::Table_obj& data) {
		BEGIN_WRITER("INSERT INTO [obj] VALUES (?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.id));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.name.c_str(), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.classid));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, data.classtype.c_str(), -1, SQLITE_TRANSIENT));
		END_WRITER;
	}

#pragma endregion

#pragma region Environment Database

	EnvironmentDatabase::EnvironmentDatabase(const YYCC::yycc_u8string_view& file) :
		AbstractDatabase(file) {
		// initialize table
		BEGIN_CTOR;
		CTOR_SQL_EXEC("CREATE TABLE [op] ([func_ptr] TEXT, [in1_guid] INTEGER, [in2_guid] INTEGER, [out_guid] INTEGER, [op_guid] INTEGER, [op_name] TEXT, [op_code] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [param] ([index] INTEGER, [guid] INTEGER, [derived_from] INTEGER, [name] TEXT, [default_size] INTEGER, [func_CreateDefault] TEXT, [func_Delete] TEXT, [func_SaveLoad] TEXT, [func_Check] TEXT, [func_Copy] TEXT, [func_String] TEXT, [func_UICreator] TEXT, [dll_name] TEXT, [dll_index] INTEGER, [position_in_dll] INTEGER, [flags] INTEGER, [dw_param] INTEGER, [cid] INTEGER, [saver_manager] INTEGER);");
		CTOR_SQL_EXEC("CREATE TABLE [attr] ([index] INTEGER, [name] TEXT, [category_index] INTEGER, [category_name] TEXT, [flags] INTEGER, [param_index] INTEGER, [param_guid] INTEGER, [compatible_classid] INTEGER, [default_value] TEXT);");
		CTOR_SQL_EXEC("CREATE TABLE [plugin] ([category_name] TEXT, [category_index] INTEGER, [dll_name] TEXT, [dll_index] INTEGER, [position_in_dll] INTEGER, [index] INTEGER, [guid] INTEGER, [desc] TEXT, [author] TEXT, [summary] TEXT, [version] INTEGER, [type] INTEGER, [func_init] TEXT, [func_exit] TEXT, [reader_fct] TEXT, [reader_opt_count] INTEGER, [reader_flags] INTEGER, [reader_setting_param_guid] INTEGER, [reader_file_ext] TEXT, [manager_active] INTEGER, [behavior_guids] TEXT);");
		CTOR_SQL_EXEC("CREATE TABLE [variable] ([name] TEXT, [desciption] TEXT, [flags] INTEGER, [type] INTEGER, [representation] TEXT, [data] TEXT);");
		END_CTOR;
	}

	EnvironmentDatabase::~EnvironmentDatabase() {
		// do nothing
	}

	void EnvironmentDatabase::Write(const DataTypes::Environment::Table_op& data) {
		BEGIN_WRITER("INSERT INTO [op] VALUES (?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_ptr), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.in1_guid));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.in2_guid));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.out_guid));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.op_guid));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.op_name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.op_code));
		END_WRITER;
	}
	void EnvironmentDatabase::Write(const DataTypes::Environment::Table_param& data) {
		BEGIN_WRITER("INSERT INTO [param] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.guid));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.derived_from));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.default_size));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_CreateDefault), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_Delete), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_SaveLoad), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_Check), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_Copy), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_String), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_UICreator), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.dll_name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.dll_index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.position_in_dll));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.flags));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.dw_param));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.cid));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.saver_manager));
		END_WRITER;
	}
	void EnvironmentDatabase::Write(const DataTypes::Environment::Table_attr& data) {
		BEGIN_WRITER("INSERT INTO [attr] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.category_index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.category_name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.flags));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.param_index));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.param_guid));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.compatible_classid));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.default_value), -1, SQLITE_TRANSIENT));
		END_WRITER;
	}
	void EnvironmentDatabase::Write(const DataTypes::Environment::Table_plugin& data) {
		BEGIN_WRITER("INSERT INTO [plugin] VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.category_name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.category_index));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.dll_name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.dll_index));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.position_in_dll));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.index));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.guid));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.desc), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.author), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.summary), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.version));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.type));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_init), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.func_exit), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.reader_fct), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.reader_opt_count));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.reader_flags));
		WRITER_BIND(sqlite3_bind_int64(WRITER_STMT, WRITER_INDEX, data.reader_setting_param_guid));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.reader_file_ext), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.manager_active));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.behavior_guids), -1, SQLITE_TRANSIENT));
		END_WRITER;
	}
	void EnvironmentDatabase::Write(const DataTypes::Environment::Table_variable& data) {
		BEGIN_WRITER("INSERT INTO [variable] VALUES (?, ?, ?, ?, ?, ?);");
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.name), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.desciption), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.flags));
		WRITER_BIND(sqlite3_bind_int(WRITER_STMT, WRITER_INDEX, data.type));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.representation), -1, SQLITE_TRANSIENT));
		WRITER_BIND(sqlite3_bind_text(WRITER_STMT, WRITER_INDEX, REVEAL_U8STR(data.data), -1, SQLITE_TRANSIENT));
		END_WRITER;
	}

#pragma endregion

}
