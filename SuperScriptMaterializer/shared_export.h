#if !defined(_YYCDLL_SHARED_EXPORT_H__IMPORTED_)
#define _YYCDLL_SHARED_EXPORT_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

class combined_database {
public:
	combined_database();
	db_shared_dictData* get_struct();
	char* get_string_cache();
	void write_dict(db_shared_dictData* data);

	scriptDatabase* db_script;
	dbScriptDataStructHelper* helper_script;
	dataDatabase* db_data;
	dbDataDataStructHelper* helper_data;
};

#define copyGuid(guid,str) sprintf(helper->_stringCache,"%d,%d",guid.d1,guid.d2);str=helper->_stringCache;
#define safeStringCopy(storage,str) storage=(str)?(str):"";

void DigParameterData(CKParameter* p, combined_database* helper, EXPAND_CK_ID parents);
void DigParameterData(CKParameter* p, scriptDatabase* db, dbScriptDataStructHelper* helper, EXPAND_CK_ID parents);
void DigParameterData(CKParameter* p, dataDatabase* db, dbDataDataStructHelper* helper, EXPAND_CK_ID parents);

void helper_FillStruct(const char* field, long data, db_shared_dictData* helper);
void helper_FillStruct(const char* field, float data, db_shared_dictData* helper);
void helper_FillStruct(const char* field, const char* data, db_shared_dictData* helper);

#endif