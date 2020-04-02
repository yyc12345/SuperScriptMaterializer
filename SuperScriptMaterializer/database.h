#if !defined(_YYCDLL_DATABASE_H__IMPORTED_)
#define _YYCDLL_DATABASE_H__IMPORTED_

#include <sqlite3.h>
#include "stdafx.h"

#pragma region data struct define

typedef long EXPAND_CK_ID;

typedef struct dbCKBehavior {
	EXPAND_CK_ID thisobj;
	char name[1024];
	CK_BEHAVIOR_TYPE type;
	char proto_name[1024];
	CKDWORD proto_guid;
	CK_BEHAVIOR_FLAGS flags;
	int priority;
	CKDWORD version;
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
	CKDWORD type_guid;
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
};

typedef struct db_pIn {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	char type[1024];
	CKDWORD type_guid;
	EXPAND_CK_ID belong_to;
	EXPAND_CK_ID direct_source;
	EXPAND_CK_ID shared_source;
};

typedef struct db_pOut {
	EXPAND_CK_ID thisobj;
	int index;
	char name[1024];
	char type[1024];
	CKDWORD type_guid;
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

typedef struct db_bLlink {
	EXPAND_CK_ID input;
	EXPAND_CK_ID output;
	int delay;
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
	db_bLlink* _db_bLlink;


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
	void write_bLink(db_bLlink* data);

	private:
	sqlite3* db;
	char* errmsg;
	char* commandStr;
};

#endif