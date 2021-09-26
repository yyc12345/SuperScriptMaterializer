#if !defined(_YYCDLL_DATABASE_H__IMPORTED_)
#define _YYCDLL_DATABASE_H__IMPORTED_

#include <sqlite3.h>
#include "stdafx.h"
#include <string>
#include <vector>
#include <set>
#include "virtools_compatible.h"

#define STRINGCACHE_SIZE 25565

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

// =================== script db

typedef struct {
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
}db_script_behavior;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string host_name;
	int index;
	EXPAND_CK_ID behavior;
}db_script_script;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
}db_script_pTarget;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
}db_script_pIn;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID belong_to;
}db_script_pOut;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	EXPAND_CK_ID belong_to;
}db_shared_bIO;
typedef db_shared_bIO db_script_bIn;
typedef db_shared_bIO db_script_bOut;

typedef struct {
	EXPAND_CK_ID input;
	EXPAND_CK_ID output;
	int delay;
	EXPAND_CK_ID belong_to;

	//additional field
	EXPAND_CK_ID input_obj;
	bLinkInputOutputType input_type;
	int input_index;
	EXPAND_CK_ID output_obj;
	bLinkInputOutputType output_type;
	int output_index;
}db_script_bLink;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
	BOOL is_setting;
	EXPAND_CK_ID belong_to;
}db_script_pLocal;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
}db_script_pAttr;

typedef struct {
	std::string field;
	std::string data;
	EXPAND_CK_ID belong_to;
}db_shared_dictData;
typedef db_shared_dictData db_script_pData;

typedef struct {
	EXPAND_CK_ID input;
	EXPAND_CK_ID output;
	EXPAND_CK_ID belong_to;

	//additional field
	EXPAND_CK_ID input_obj;
	pLinkInputOutputType input_type;
	BOOL input_is_bb;
	int input_index;
	EXPAND_CK_ID output_obj;
	pLinkInputOutputType output_type;
	BOOL output_is_bb;
	int output_index;
}db_script_pLink;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string op;
	std::string op_guid;
	EXPAND_CK_ID belong_to;
}db_script_pOper;

typedef struct {
	EXPAND_CK_ID export_obj;
	EXPAND_CK_ID internal_obj;
	BOOL is_in;
	int index;
	EXPAND_CK_ID belong_to;
}db_script_eLink;


// =================== data db

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string name;
	CK_CLASSID type_classid;
	std::string type_guid;
	std::string type_name;
	int rows;
	int columns;
}db_data_obj;

typedef struct {
	int index;
	std::string name;
	CK_ARRAYTYPE type;
	std::string param_type;
	std::string param_type_guid;
	EXPAND_CK_ID belong_to;
}db_data_objHeader;

typedef struct {
	int row;
	int column;
	std::string showcase;
	EXPAND_CK_ID inner_object;
	EXPAND_CK_ID inner_param;
	EXPAND_CK_ID belong_to;
}db_data_objBody;

typedef db_shared_dictData db_data_objParam;

typedef struct {
	CKMessageType index;
	std::string name;
}db_data_msg;

// =================== env db

typedef struct {
	CK_PARAMETEROPERATION funcPtr;
	std::string in1_guid;
	std::string in2_guid;
	std::string out_guid;
	std::string op_guid;
	std::string op_name;
	CKOperationType op_code;
}db_env_op;

typedef struct {
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
}db_env_param;

typedef struct {
	CKAttributeType index;
	std::string name;
	CKAttributeCategory category_index;
	std::string category_name;
	CK_ATTRIBUT_FLAGS flags;
	CKParameterType param_index;
	CK_CLASSID compatible_classid;
	std::string default_value;
}db_env_attr;

typedef struct {
	int dll_index;
	std::string dll_name;
	int plugin_index;
	std::string category;
	CKBOOL active;
	CKBOOL needed_by_file;
	std::string guid;
	std::string desc;
	std::string author;
	std::string summary;
	DWORD version;
	CK_INITINSTANCEFCT func_init;
	CK_EXITINSTANCEFCT func_exit;
}db_env_plugin;

typedef struct {
	std::string name;
	std::string desciption;
	XWORD flags;
	UNIVERSAL_VAR_TYPE type;
	std::string representation;
	std::string data;
}db_env_variable;

#pragma endregion

class dbScriptDataStructHelper {
	public:
	void init(CKParameterManager* paramManager);
	void dispose();

	char* _stringCache;
	CKParameterManager* _parameterManager;
	db_script_behavior* _db_behavior;
	db_script_script* _db_script;
	db_script_pTarget* _db_pTarget;
	db_script_pIn* _db_pIn;
	db_script_pOut* _db_pOut;
	db_script_bIn* _db_bIn;
	db_script_bOut* _db_bOut;
	db_script_bLink* _db_bLink;
	db_script_pLocal* _db_pLocal;
	db_script_pAttr* _db_pAttr;
	db_script_pLink* _db_pLink;
	db_script_pData* _db_pData;
	db_script_pOper* _db_pOper;
	db_script_eLink* _db_eLink;
};

class dbDataDataStructHelper {
public:
	void init(CKParameterManager* paramManager);
	void dispose();

	char* _stringCache;
	CKParameterManager* _parameterManager;
	db_data_obj* _db_obj;
	db_data_objHeader* _db_objHeader;
	db_data_objBody* _db_objBody;
	db_data_objParam* _db_objParam;
	db_data_msg* _db_msg;
	
};

class dbEnvDataStructHelper {
	public:
	void init();
	void dispose();

	char* _stringCache;
	db_env_op* _db_op;
	db_env_param* _db_param;
	//db_data_msg* _db_envMsg;
	db_env_attr* _db_attr;
	db_env_plugin* _db_plugin;
	db_env_variable* _db_variable;
};


class database {
	public:
	void open(const char* file);
	void close();

	protected:
	sqlite3_stmt* safeStmt(size_t index);
	virtual BOOL init() { return TRUE; }
	virtual BOOL finalJob() { return TRUE; }

	sqlite3* db;
	std::vector<sqlite3_stmt*>* stmtCache;
};

class scriptDatabase : public database {
	public:
	void write_behavior(db_script_behavior* data);
	void write_script(db_script_script* data);
	void write_pTarget(db_script_pTarget* data);
	void write_pIn(db_script_pIn* data);
	void write_pOut(db_script_pOut* data);
	void write_bIn(db_script_bIn* data);
	void write_bOut(db_script_bOut* data);
	void write_bLink(db_script_bLink* data);
	void write_pLocal(db_script_pLocal* data);
	void write_pLink(db_script_pLink* data);
	void write_pData(db_script_pData* data);
	void write_pOper(db_script_pOper* data);
	void write_eLink(db_script_eLink* data);
	BOOL write_pAttr(db_script_pAttr* data);

	protected:
	BOOL init();
	BOOL finalJob();

	std::set<EXPAND_CK_ID>* pAttrUniqueEnsurance;
};

class dataDatabase : public database {
public:
	void write_obj(db_data_obj* data);
	void write_objHeader(db_data_objHeader* data);
	void write_objBody(db_data_objBody* data);
	void write_objParam(db_data_objParam* data);
	void write_msg(db_data_msg* data);

protected:
	BOOL init();
	BOOL finalJob();

};

class envDatabase : public database {
	public:
	void write_op(db_env_op* data);
	void write_param(db_env_param* data);
	//void write_msg(db_data_msg* data);
	void write_attr(db_env_attr* data);
	void write_plugin(db_env_plugin* data);
	void write_variable(db_env_variable* data);

	protected:
	BOOL init();
	BOOL finalJob();
};

#endif