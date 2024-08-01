#include "Database.hpp"
#include <stdexcept>

namespace VSW::Materializer::Database {

#pragma region Help Macros

#define BEGIN_SAFE_SQL_EXEC { int errcode;
#define SAFE_SQL_EXEC(sql) errcode = sqlite3_exec(this->GetDb(), sql, nullptr, nullptr, nullptr); \
if (errcode != SQLITE_OK) return false;
#define END_SAFE_SQL_EXEC }

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

		// run extra stuff
		if (!PostOpen()) goto failed_after_open;

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

		// run extra stuff
		if (!PreClose()) goto failed;

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
		AbstractDatabase(file) {}

	ScriptDatabase::~ScriptDatabase() {}

	bool ScriptDatabase::PostOpen() {
		// initialize table
		BEGIN_SAFE_SQL_EXEC;
		SAFE_SQL_EXEC("begin;");

		SAFE_SQL_EXEC("CREATE TABLE [script] ([thisobj] INTEGER, [name] TEXT, [index] INTEGER, [behavior] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [behavior] ([thisobj] INTEGER, [name] TEXT, [type] INTEGER, [proto_name] TEXT, [proto_guid] TEXT, [flags] INTEGER, [priority] INTEGER, [version] INTEGER, [pin_count] TEXT, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pTarget] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER, [direct_source] INTEGER, [shard_source] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pIn] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER, [direct_source] INTEGER, [shared_source] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pOut] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [bIn] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [bOut] ([thisobj] INTEGER, [index] INTEGER, [name] TEXT, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [bLink] ([input] INTEGER, [output] INTEGER, [delay] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_index] INETEGR, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pLocal] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT, [is_setting] INTEGER, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pLink] ([input] INTEGER, [output] INTEGER, [input_obj] INTEGER, [input_type] INTEGER, [input_is_bb] INTEGER, [input_index] INETEGR, [output_obj] INTEGER, [output_type] INTEGER, [output_is_bb] INTEGER, [output_index] INETEGR, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pOper] ([thisobj] INTEGER, [op] TEXT, [op_guid] TEXT, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [eLink] ([export_obj] INTEGER, [internal_obj] INTEGER, [is_in] INTEGER, [index] INTEGER, [parent] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [pAttr] ([thisobj] INTEGER, [name] TEXT, [type] TEXT, [type_guid] TEXT);");

		//SAFE_SQL_EXEC("CREATE TABLE [msg] ([index] INTEGER, [name] TEXT);");
		//SAFE_SQL_EXEC("CREATE TABLE [obj] ([id] INTEGER, [name] TEXT, [classid] INTEGER, [classtype] TEXT);");

		SAFE_SQL_EXEC("CREATE TABLE [data] ([field] TEXT, [data] TEXT, [parent] INTEGER);");

		SAFE_SQL_EXEC("commit;");
		END_SAFE_SQL_EXEC;

		return true;
	}

	bool ScriptDatabase::PreClose() {
		// create index for quick select in following process
		BEGIN_SAFE_SQL_EXEC;
		SAFE_SQL_EXEC("begin;");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where1] ON [behavior] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where2] ON [pOper] ([parent], [thisobj])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where3] ON [pTarget] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where4] ON [bIn] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where5] ON [bOut] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where6] ON [pIn] ([parent], [thisobj])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where7] ON [pOut] ([parent], [thisobj])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where8] ON [pLocal] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where9] ON [pLink] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where10] ON [bLink] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where11] ON [elink] ([parent])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where12] ON [pAttr] ([thisobj])");
		SAFE_SQL_EXEC("CREATE INDEX [quick_where13] ON [data] ([parent])");
		SAFE_SQL_EXEC("commit;");
		END_SAFE_SQL_EXEC;

		return true;
	}

#pragma endregion

#pragma region Context Database

	ContextDatabase::ContextDatabase(const YYCC::yycc_u8string_view& file) :
		AbstractDatabase(file) {}

	ContextDatabase::~ContextDatabase() {}

	bool ContextDatabase::PostOpen() {
		// initialize table
		BEGIN_SAFE_SQL_EXEC;
		SAFE_SQL_EXEC("begin;");

		SAFE_SQL_EXEC("CREATE TABLE [msg] ([index] INTEGER, [name] TEXT);");
		SAFE_SQL_EXEC("CREATE TABLE [obj] ([id] INTEGER, [name] TEXT, [classid] INTEGER, [classtype] TEXT);");

		SAFE_SQL_EXEC("commit;");
		END_SAFE_SQL_EXEC;

		return true;
	}

	bool ContextDatabase::PreClose() {
		// do nothing
		return true;
	}

#pragma endregion

#pragma region Environment Database

	EnvironmentDatabase::EnvironmentDatabase(const YYCC::yycc_u8string_view& file) :
		AbstractDatabase(file) {}

	EnvironmentDatabase::~EnvironmentDatabase() {}

	bool EnvironmentDatabase::PostOpen() {
		// initialize table
		BEGIN_SAFE_SQL_EXEC;
		SAFE_SQL_EXEC("begin;");

		SAFE_SQL_EXEC("CREATE TABLE [op] ([funcptr] INTEGER, [in1_guid] TEXT, [in2_guid] TEXT, [out_guid] TEXT, [op_guid] TEXT, [op_name] TEXT, [op_code] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [param] ([index] INTEGER, [guid] TEXT, [derived_from] TEXT, [type_name] TEXT, [default_size] INTEGER, [func_CreateDefault] INTEGER, [func_Delete] INTEGER, [func_SaveLoad] INTEGER, [func_Check] INTEGER, [func_Copy] INTEGER, [func_String] INTEGER, [func_UICreator] INTEGER, [creator_dll_index] INTEGER, [creator_plugin_index] INTEGER, [dw_param] INTEGER, [dw_flags] INTEGER, [cid] INTEGER, [saver_manager] TEXT);");
		SAFE_SQL_EXEC("CREATE TABLE [attr] ([index] INTEGER, [name] TEXT, [category_index] INTEGER, [category_name] TEXT, [flags] INTEGER, [param_index] INTEGER, [compatible_classid] INTEGER, [default_value] TEXT);");
		SAFE_SQL_EXEC("CREATE TABLE [plugin] ([dll_index] INTEGER, [dll_name] TEXT, [plugin_index] INTEGER, [category] TEXT, [active] INTEGER, [guid] TEXT, [desc] TEXT, [author] TEXT, [summary] TEXT, [version] INTEGER, [func_init] INTEGER, [func_exit] INTEGER);");
		SAFE_SQL_EXEC("CREATE TABLE [variable] ([name] TEXT, [description] TEXT, [flags] INTEGER, [type] INTEGER, [representation] TEXT, [data] TEXT);");

		SAFE_SQL_EXEC("commit;");
		END_SAFE_SQL_EXEC;

		return true;
	}

	bool EnvironmentDatabase::PreClose() {
		// do nothing
		return true;
	}

#pragma endregion

}
