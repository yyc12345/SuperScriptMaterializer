#if !defined(_YYCDLL_VT_MENU_H__IMPORTED_)
#define _YYCDLL_VT_MENU_H__IMPORTED_

#include "stdafx.h"
#include "database.h"

void PluginCallback(PluginInfo::CALLBACK_REASON reason, PluginInterface* plugininterface);

void InitMenu();
void RemoveMenu();
void UpdateMenu();
void PluginMenuCallback(int commandID);
void IterateBehavior(CKBehavior* bhv, database* db, dbDataStructHelper* helper, EXPAND_CK_ID parents);
void IterateScript(CKContext* ctx, database* db, dbDataStructHelper* helper);

#endif