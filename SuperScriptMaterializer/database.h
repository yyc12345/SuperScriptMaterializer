#if !defined(_YYCDLL_DATABASE_H__IMPORTED_)
#define _YYCDLL_DATABASE_H__IMPORTED_

#include <sqlite3.h>
#include "stdafx.h"
#include <string>
#include <vector>
#include <set>

typedef long EXPAND_CK_ID;
enum bLinkInputOutputType {
	bLinkInputOutputType_INPUT,
	bLinkInputOutputType_OUTPUT
};
enum pLinkInputOutputType {
	pLinkInputOutputType_PIN,
	pLinkInputOutputType_POUT,
	pLinkInputOutputType_PLOCAL, //when using pLocal, omit [index] and [input_is_bb], [input_index] set -1
	pLinkInputOutputType_PTARGET, //when using pTarget, omit [index] and [input_is_bb], [input_index] set -1
	pLinkInputOutputType_PATTR //when using pAttr, omit [index] and [input_is_bb], [input_index] set -1
};

#pragma region data struct define

// =================== doc mDb

struct dbdoc_script {
	EXPAND_CK_ID thisobj;
	std::string host_name;
	int index;
	EXPAND_CK_ID behavior;
};

struct dbdoc_script_behavior {
	EXPAND_CK_ID thisobj;
	std::string name;
	CK_BEHAVIOR_TYPE type;
	std::string proto_name;
	std::string proto_guid;
	CK_BEHAVIOR_FLAGS flags;
	int priority;
	CKDWORD version;
	//pTarget, pIn, pOut, bIn, bOut
	std::string pin_count;
	EXPAND_CK_ID parent;
};

struct dbdoc_script_bIO {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	EXPAND_CK_ID parent;
};
typedef dbdoc_script_bIO dbdoc_script_bIn;
typedef dbdoc_script_bIO dbdoc_script_bOut;

struct dbdoc_script_pTarget {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID parent;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
};

struct dbdoc_script_pIn {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID parent;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
};

struct dbdoc_script_pOut {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID parent;
};

struct dbdoc_script_bLink {
	EXPAND_CK_ID input;
	EXPAND_CK_ID output;
	int delay;
	EXPAND_CK_ID parent;

	//additional field
	EXPAND_CK_ID input_obj;
	bLinkInputOutputType input_type;
	int input_index;
	EXPAND_CK_ID output_obj;
	bLinkInputOutputType output_type;
	int output_index;
};

struct dbdoc_script_pLocal {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
	BOOL is_setting;
	EXPAND_CK_ID parent;
};

struct dbdoc_script_pAttr {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
};

struct dbdoc_script_pLink {
	EXPAND_CK_ID input;
	EXPAND_CK_ID output;
	EXPAND_CK_ID parent;

	//additional field
	EXPAND_CK_ID input_obj;
	pLinkInputOutputType input_type;
	BOOL input_is_bb;
	int input_index;
	EXPAND_CK_ID output_obj;
	pLinkInputOutputType output_type;
	BOOL output_is_bb;
	int output_index;
};

struct dbdoc_script_pOper {
	EXPAND_CK_ID thisobj;
	std::string op;
	std::string op_guid;
	EXPAND_CK_ID parent;
};

struct dbdoc_script_eLink {
	EXPAND_CK_ID export_obj;
	EXPAND_CK_ID internal_obj;
	BOOL is_in;
	int index;
	EXPAND_CK_ID parent;
};

struct dbdoc_msg {
	CKMessageType index;
	std::string name;
};

struct dbdoc_array {
	EXPAND_CK_ID thisobj;
	std::string name;
	int rows;
	int columns;
};

struct dbdoc_array_header {
	int index;
	std::string name;
	CK_ARRAYTYPE type;
	std::string param_type;
	std::string param_type_guid;
	EXPAND_CK_ID parent;
};

struct dbdoc_array_cell {
	int row;
	int column;
	std::string showcase;
	EXPAND_CK_ID inner_param;
	EXPAND_CK_ID parent;
};

struct dbdoc_data {
	std::string field;
	std::string data;
	EXPAND_CK_ID parent;
};

// =================== env mDb

struct dbenv_op {
	CK_PARAMETEROPERATION funcPtr;
	std::string in1_guid;
	std::string in2_guid;
	std::string out_guid;
	std::string op_guid;
	std::string op_name;
	CKOperationType op_code;
};

struct dbenv_param {
	CKParameterType index;
	std::string guid;
	std::string derived_from;
	std::string type_name;
	int default_size;
	CK_PARAMETERCREATEDEFAULTFUNCTION func_CreateDefault;
	CK_PARAMETERDELETEFUNCTION func_Delete;
	CK_PARAMETERSAVELOADFUNCTION func_SaveLoad;
	CK_PARAMETERCHECKFUNCTION func_Check;
	CK_PARAMETERCOPYFUNCTION func_Copy;
	CK_PARAMETERSTRINGFUNCTION func_String;
	CK_PARAMETERUICREATORFUNCTION func_UICreator;
	int creator_dll_index;
	int creator_plugin_index;
	CKDWORD dw_param;
	CKDWORD dw_flags;
	CKDWORD cid;
	std::string saver_manager;
};

struct dbenv_attr {
	CKAttributeType index;
	std::string name;
	CKAttributeCategory category_index;
	std::string category_name;
	CK_ATTRIBUT_FLAGS flags;
	CKParameterType param_index;
	CK_CLASSID compatible_classid;
	std::string default_value;
};

struct dbenv_plugin {
	int dll_index;
	std::string dll_name;
	int plugin_index;
	std::string category;
	CKBOOL active;
	std::string guid;
	std::string desc;
	std::string author;
	std::string summary;
	DWORD version;
	CK_INITINSTANCEFCT func_init;
	CK_EXITINSTANCEFCT func_exit;
};

struct dbenv_variable {
	std::string name;
	std::string desciption;
	XWORD flags;
	UNIVERSAL_VAR_TYPE type;
	std::string representation;
	std::string data;
};

#pragma endregion

class DbDataStructHelper_Doc {
public:
	DbDataStructHelper_Doc(CKParameterManager* paramManager);
	~DbDataStructHelper_Doc();

	CKParameterManager* param_manager;

	dbdoc_script script;
	dbdoc_script_behavior script_behavior;
	dbdoc_script_bIn script_bIn;
	dbdoc_script_bOut script_bOut;
	dbdoc_script_pIn script_pIn;
	dbdoc_script_pOut script_pOut;
	dbdoc_script_bLink script_bLink;
	dbdoc_script_pLocal script_pLocal;
	dbdoc_script_pAttr script_pAttr;
	dbdoc_script_pLink script_pLink;
	dbdoc_script_pOper script_pOper;
	dbdoc_script_eLink script_eLink;
	dbdoc_script_pTarget script_pTarget;

	dbdoc_msg msg;

	dbdoc_array _array;
	dbdoc_array_header array_header;
	dbdoc_array_cell array_cell;

	dbdoc_data data;
};

class DbDataStructHelper_Env {
public:
	DbDataStructHelper_Env();
	~DbDataStructHelper_Env();

	dbenv_op op;
	dbenv_param param;
	dbenv_attr attr;
	dbenv_plugin plugin;
	dbenv_variable variable;
};


class SSMaterializerDatabase {
public:
	SSMaterializerDatabase(const char* file);
	virtual ~SSMaterializerDatabase();

protected:
	sqlite3_stmt* CreateStmt(const char* stmt);
	virtual BOOL Init() = 0;
	virtual BOOL Finalize() = 0;

	sqlite3* mDb;
	std::vector<sqlite3_stmt*> mStmtCache;
};

class DocumentDatabase : public SSMaterializerDatabase {
public:
	DocumentDatabase(const char* file);
	virtual ~DocumentDatabase();

	void write_script(dbdoc_script& data);
	void write_script_behavior(dbdoc_script_behavior& data);
	void write_script_pTarget(dbdoc_script_pTarget& data);
	void write_script_pIn(dbdoc_script_pIn& data);
	void write_script_pOut(dbdoc_script_pOut& data);
	void write_script_bIn(dbdoc_script_bIn& data);
	void write_script_bOut(dbdoc_script_bOut& data);
	void write_script_bLink(dbdoc_script_bLink& data);
	void write_script_pLocal(dbdoc_script_pLocal& data);
	void write_script_pLink(dbdoc_script_pLink& data);
	void write_script_pOper(dbdoc_script_pOper& data);
	void write_script_eLink(dbdoc_script_eLink& data);
	BOOL write_script_pAttr(dbdoc_script_pAttr& data);

	void write_msg(dbdoc_msg& data);

	void write_array(dbdoc_array& data);
	void write_array_header(dbdoc_array_header& data);
	void write_array_cell(dbdoc_array_cell& data);

	void write_data(dbdoc_data& data);

protected:
	BOOL Init() override;
	BOOL Finalize() override;

	std::set<EXPAND_CK_ID> m_pAttrUniqueEnsurance;
};

class EnvironmentDatabase : public SSMaterializerDatabase {
public:
	EnvironmentDatabase(const char* file);
	virtual ~EnvironmentDatabase();

	void write_op(dbenv_op& data);
	void write_param(dbenv_param& data);
	void write_attr(dbenv_attr& data);
	void write_plugin(dbenv_plugin& data);
	void write_variable(dbenv_variable& data);

protected:
	BOOL Init() override;
	BOOL Finalize() override;
};

#endif