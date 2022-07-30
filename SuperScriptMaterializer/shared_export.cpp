#include "shared_export.h"

combined_database::combined_database() :
	db_script(NULL),
	helper_script(NULL),
	db_data(NULL),
	helper_data(NULL) {
	;
}

dbdoc_data* combined_database::get_struct() {
	if (helper_script != NULL) return helper_script->_db_pData;
	if (helper_data != NULL) return helper_data->_db_objParam;
	return NULL;
}

char* combined_database::get_string_cache() {
	if (helper_script != NULL) return helper_script->_stringCache;
	if (helper_data != NULL) return helper_data->_stringCache;
	return NULL;
}

void combined_database::write_dict(dbdoc_data* data) {
	if (db_script != NULL) db_script->write_pData(data);
	if (db_data != NULL) db_data->write_objParam(data);
}

void DigParameterData(CKParameter* p, DocumentDatabase* mDb, dbDocDataStructHelper* helper, EXPAND_CK_ID parents) {
	combined_database cdb;
	cdb.db_script = mDb;
	cdb.helper_script = helper;

	DigParameterData(p, &cdb, parents);
}

void DigParameterData(CKParameter* p, dataDatabase* mDb, dbDataDataStructHelper* helper, EXPAND_CK_ID parents) {
	combined_database cdb;
	cdb.db_data = mDb;
	cdb.helper_data = helper;

	DigParameterData(p, &cdb, parents);
}
