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

typedef struct dbCKBehavior {
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
};

typedef struct dbCKScript {
	EXPAND_CK_ID thisobj;
	char host_name[1024];
	int index;
	EXPAND_CK_ID behavior;
};

typedef struct db_pTarget {
	EXPAND_CK_ID thisobj;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
};

typedef struct db_pIn {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
};

typedef struct db_pOut {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	EXPAND_CK_ID belong_to;
};

typedef struct db_bIO {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	EXPAND_CK_ID belong_to;
};
typedef db_bIO db_bIn;
typedef db_bIO db_bOut;

typedef struct db_bLink {
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
};

typedef struct db_pLocal {
	EXPAND_CK_ID thisobj;
	char name[1024];
	char type[1024];
	CKDWORD type_guid[2];
	BOOL is_setting;
	EXPAND_CK_ID belong_to;
};

typedef struct db_pLocalData {
	char field[1024];
	char data[1024];
	EXPAND_CK_ID belong_to;
};

typedef struct db_pLink {
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
};

typedef struct db_pOper {
	EXPAND_CK_ID thisobj;
	char op[1024];
	CKDWORD op_guid[2];
	EXPAND_CK_ID belong_to;
};

#pragma endregion

class dbDataStructHelper {
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
};

class database {
	public:
	void open(const char* file);
	void close();

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

	private:
	sqlite3* db;
	char* errmsg;
	char* commandStr;
};

#endif