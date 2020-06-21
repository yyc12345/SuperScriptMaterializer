#if !defined(_YYCDLL_DATABASE_H__IMPORTED_)
#define _YYCDLL_DATABASE_H__IMPORTED_

#include <sqlite3.h>
#include "stdafx.h"
#include <string>
#include <vector>

#define STRINGCACHE_SIZE 25565

typedef long EXPAND_CK_ID;
enum bLinkInputOutputType {
	bLinkInputOutputType_INPUT,
	bLinkInputOutputType_OUTPUT
};
enum pLinkInputOutputType {
	pLinkInputOutputType_PIN,
	pLinkInputOutputType_POUT,
	pLinkInputOutputType_PLOCAL, //when using pLocal, omit [index] and [input_is_bb]
	pLinkInputOutputType_PTARGET //when using pTarget, omit [index] and [input_is_bb]
};

#pragma region data struct define

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
}dbCKBehavior;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string host_name;
	int index;
	EXPAND_CK_ID behavior;
}dbCKScript;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
}db_pTarget;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
}db_pIn;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	std::string type;
	std::string type_guid;
	EXPAND_CK_ID belong_to;
}db_pOut;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	std::string name;
	EXPAND_CK_ID belong_to;
}db_bIO;
typedef db_bIO db_bIn;
typedef db_bIO db_bOut;

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
}db_bLink;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string name;
	std::string type;
	std::string type_guid;
	BOOL is_setting;
	EXPAND_CK_ID belong_to;
}db_pLocal;

typedef struct {
	std::string field;
	std::string data;
	EXPAND_CK_ID belong_to;
}db_pLocalData;

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
}db_pLink;

typedef struct {
	EXPAND_CK_ID thisobj;
	std::string op;
	std::string op_guid;
	EXPAND_CK_ID belong_to;
}db_pOper;

typedef struct {
	EXPAND_CK_ID export_obj;
	EXPAND_CK_ID internal_obj;
	BOOL is_in;
	int index;
	EXPAND_CK_ID belong_to;
}db_eLink;



typedef struct {
	CK_PARAMETEROPERATION funcPtr;
	std::string in1_guid;
	std::string in2_guid;
	std::string out_guid;
	std::string op_guid;
	std::string op_name;
	CKOperationType op_code;
}db_envOp;

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
}db_envParam;

typedef struct {
	CKMessageType index;
	std::string name;
}db_envMsg;

typedef struct {
	CKAttributeType index;
	std::string name;
	CKAttributeCategory category_index;
	std::string category_name;
	CK_ATTRIBUT_FLAGS flags;
	CKParameterType param_index;
	CK_CLASSID compatible_classid;
	std::string default_value;
}db_envAttr;

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
}db_envPlugin;

#pragma endregion

class dbScriptDataStructHelper {
	public:
	void init(CKParameterManager* paramManager);
	void dispose();

	char* _stringCache;
	CKParameterManager* _parameterManager;
	dbCKBehavior* _dbCKBehavior;
	dbCKScript* _dbCKScript;
	db_pTarget* _db_pTarget;
	db_pIn* _db_pIn;
	db_pOut* _db_pOut;
	db_bIn* _db_bIn;
	db_bOut* _db_bOut;
	db_bLink* _db_bLink;
	db_pLocal* _db_pLocal;
	db_pLink* _db_pLink;
	db_pLocalData* _db_pLocalData;
	db_pOper* _db_pOper;
	db_eLink* _db_eLink;
};

class dbEnvDataStructHelper {
	public:
	void init();
	void dispose();

	char* _stringCache;
	db_envOp* _db_envOp;
	db_envParam* _db_envParam;
	db_envMsg* _db_envMsg;
	db_envAttr* _db_envAttr;
	db_envPlugin* _db_envPlugin;
};


class database {
	public:
	void open(const char* file);
	void close();

	protected:
	virtual BOOL init() { return TRUE; }
	virtual BOOL finalJob() { return TRUE; }

	sqlite3* db;
	std::vector<sqlite3_stmt*>* stmtCache;
};

class scriptDatabase : public database {
	public:
	void write_CKBehavior(dbCKBehavior* data);
	void write_CKScript(dbCKScript* data);
	void write_pTarget(db_pTarget* data);
	void write_pIn(db_pIn* data);
	void write_pOut(db_pOut* data);
	void write_bIn(db_bIn* data);
	void write_bOut(db_bOut* data);
	void write_bLink(db_bLink* data);
	void write_pLocal(db_pLocal* data);
	void write_pLink(db_pLink* data);
	void write_pLocalData(db_pLocalData* data);
	void write_pOper(db_pOper* data);
	void write_eLink(db_eLink* data);

	protected:
	BOOL init();
	BOOL finalJob();
};

class envDatabase : public database {
	public:
	void write_envOp(db_envOp* data);
	void write_envParam(db_envParam* data);
	void write_envMsg(db_envMsg* data);
	void write_envAttr(db_envAttr* data);
	void write_envPlugin(db_envPlugin* data);

	protected:
	BOOL init();
	BOOL finalJob();
};

#endif