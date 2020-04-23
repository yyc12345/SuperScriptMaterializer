#if !defined(_YYCDLL_DATABASE_H__IMPORTED_)
#define _YYCDLL_DATABASE_H__IMPORTED_

#include <sqlite3.h>
#include "stdafx.h"

#pragma region data struct define

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

typedef struct {
	EXPAND_CK_ID thisobj;
	char name[1024];
	CK_BEHAVIOR_TYPE type;
	char proto_name[1024];
	CKDWORD proto_guid[2];
	CK_BEHAVIOR_FLAGS flags;
	int priority;
	CKDWORD version;
	//pTarget, pIn, pOut, bIn, bOut
	char pin_count[128];
	EXPAND_CK_ID parent;
}dbCKBehavior;

typedef struct {
	EXPAND_CK_ID thisobj;
	char host_name[1024];
	int index;
	EXPAND_CK_ID behavior;
}dbCKScript;

typedef struct {
	EXPAND_CK_ID thisobj;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
}db_pTarget;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
}db_pIn;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	EXPAND_CK_ID belong_to;
}db_pOut;

typedef struct {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
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
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	BOOL is_setting;
	EXPAND_CK_ID belong_to;
}db_pLocal;

typedef struct {
	char field[1024];
	char data[1024];
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
	char op[1024];
	CKDWORD op_guid[2];
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
	CKDWORD in1_guid[2];
	CKDWORD in2_guid[2];
	CKDWORD out_guid[2];
	CKDWORD op_guid[2];
	char op_name[1024];
	CKOperationType op_code;
}db_envOp;

typedef struct {
	CKParameterType index;
	CKDWORD guid[2];
	CKDWORD derived_from[2];
	char type_name[1024];
	int default_size;
	CK_PARAMETERCREATEDEFAULTFUNCTION func_CreateDefault;
	CK_PARAMETERDELETEFUNCTION func_Delete;
	CK_PARAMETERSAVELOADFUNCTION func_SaveLoad;
	CK_PARAMETERCHECKFUNCTION func_Check;
	CK_PARAMETERCOPYFUNCTION func_Copy;
	CK_PARAMETERSTRINGFUNCTION func_String;
	CK_PARAMETERUICREATORFUNCTION func_UICreator;
	int creator_plugin_id;
	CKDWORD dw_param;
	CKDWORD dw_flags;
	CKDWORD cid;
	CKDWORD saver_manager[2];
}db_envParam;

#pragma endregion

class dbScriptDataStructHelper {
	public:
	void init(CKParameterManager* paramManager);
	void dispose();

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

	db_envOp* _db_envOp;
	db_envParam* _db_envParam;
};


class database {
	public:
	void open(const char* file);
	void close();

	protected:
	virtual BOOL init() { return TRUE; }
	virtual BOOL finalJob() { return TRUE; }

	sqlite3* db;
	char* errmsg;
	char* commandStr;
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

	protected:
	BOOL init();
	BOOL finalJob();
};

#endif