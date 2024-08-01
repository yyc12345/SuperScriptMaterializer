#pragma once
#include "stdafx.hpp"
#include <sqlite3.h>
#include <map>

namespace VSW::Materializer::Database {

	class AbstractDatabase {
	public:
		AbstractDatabase(const YYCC::yycc_u8string_view& file);
		virtual ~AbstractDatabase();
		AbstractDatabase(const AbstractDatabase&) = delete;
		AbstractDatabase& operator=(const AbstractDatabase&) = delete;
		AbstractDatabase(AbstractDatabase&&) = delete;
		AbstractDatabase& operator=(AbstractDatabase&&) = delete;

	public:
		/// @brief Check whether this database is ready for use.
		bool IsValid() const;

	protected:
		/**
		 * @brief User implemented function called after connect to the database before transition.
		 * @return True if success, otherwise false.
		*/
		virtual bool PostOpen() = 0;
		/**
		 * @brief User implemented function called after transition before disconnect from database.
		 * @return True if success, otherwise false.
		*/
		virtual bool PreClose() = 0;

	protected:
		/// @brief Subclass used for creating stmt with cache feature.
		sqlite3_stmt* GetStmt(const char* stmt_str);
		/// @brief Subclass used for get pointer to opened database.
		sqlite3* GetDb();

	private:
		sqlite3* m_Db;
		std::map<const void*, sqlite3_stmt*> m_StmtCache;
	};

	class ScriptDatabase : public AbstractDatabase {
	public:
		ScriptDatabase(const YYCC::yycc_u8string_view& file);
		virtual ~ScriptDatabase();
		ScriptDatabase(const ScriptDatabase&) = delete;
		ScriptDatabase& operator=(const ScriptDatabase&) = delete;
		ScriptDatabase(ScriptDatabase&&) = delete;
		ScriptDatabase& operator=(ScriptDatabase&&) = delete;

	protected:
		virtual bool PostOpen() override;
		virtual bool PreClose() override;

	public:

	};

	class ContextDatabase : public AbstractDatabase {
	public:
		ContextDatabase(const YYCC::yycc_u8string_view& file);
		virtual ~ContextDatabase();
		ContextDatabase(const ContextDatabase&) = delete;
		ContextDatabase& operator=(const ContextDatabase&) = delete;
		ContextDatabase(ContextDatabase&&) = delete;
		ContextDatabase& operator=(ContextDatabase&&) = delete;
		
	protected:
		virtual bool PostOpen() override;
		virtual bool PreClose() override;

	public:

	};

	class EnvironmentDatabase : public AbstractDatabase {
	public:
		EnvironmentDatabase(const YYCC::yycc_u8string_view& file);
		virtual ~EnvironmentDatabase();
		EnvironmentDatabase(const EnvironmentDatabase&) = delete;
		EnvironmentDatabase& operator=(const EnvironmentDatabase&) = delete;
		EnvironmentDatabase(EnvironmentDatabase&&) = delete;
		EnvironmentDatabase& operator=(EnvironmentDatabase&&) = delete;
		
	protected:
		virtual bool PostOpen() override;
		virtual bool PreClose() override;

	public:

	};

}
