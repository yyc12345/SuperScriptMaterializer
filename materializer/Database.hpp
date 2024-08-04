#pragma once
#include "stdafx.hpp"
#include "DataTypes.hpp"
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

	public:
		void Write(const DataTypes::Script::Table_script& data);
		void Write(const DataTypes::Script::Table_behavior& data);
		void Write(const DataTypes::Script::Table_bIn& data);
		void Write(const DataTypes::Script::Table_bOut& data);
		void Write(const DataTypes::Script::Table_pIn& data);
		void Write(const DataTypes::Script::Table_pOut& data);
		void Write(const DataTypes::Script::Table_bLink& data);
		void Write(const DataTypes::Script::Table_pLocal& data);
		void Write(const DataTypes::Script::Table_pAttr& data);
		void Write(const DataTypes::Script::Table_pLink& data);
		void Write(const DataTypes::Script::Table_pOper& data);
		void Write(const DataTypes::Script::Table_eLink& data);
		void Write(const DataTypes::Script::Table_pTarget& data);
		void Write(const DataTypes::Script::Table_data& data);
	};

	class DocumentDatabase : public AbstractDatabase {
	public:
		DocumentDatabase(const YYCC::yycc_u8string_view& file);
		virtual ~DocumentDatabase();
		DocumentDatabase(const DocumentDatabase&) = delete;
		DocumentDatabase& operator=(const DocumentDatabase&) = delete;
		DocumentDatabase(DocumentDatabase&&) = delete;
		DocumentDatabase& operator=(DocumentDatabase&&) = delete;

	public:
		void Write(const DataTypes::Document::Table_msg& data);
		void Write(const DataTypes::Document::Table_obj& data);
	};

	class EnvironmentDatabase : public AbstractDatabase {
	public:
		EnvironmentDatabase(const YYCC::yycc_u8string_view& file);
		virtual ~EnvironmentDatabase();
		EnvironmentDatabase(const EnvironmentDatabase&) = delete;
		EnvironmentDatabase& operator=(const EnvironmentDatabase&) = delete;
		EnvironmentDatabase(EnvironmentDatabase&&) = delete;
		EnvironmentDatabase& operator=(EnvironmentDatabase&&) = delete;

	public:
		void Write(const DataTypes::Environment::Table_op& data);
		void Write(const DataTypes::Environment::Table_param& data);
		void Write(const DataTypes::Environment::Table_attr& data);
		void Write(const DataTypes::Environment::Table_plugin& data);
		void Write(const DataTypes::Environment::Table_variable& data);
	};

}
