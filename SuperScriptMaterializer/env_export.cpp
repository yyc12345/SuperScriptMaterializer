#include "env_export.hpp"
#include "string_helper.hpp"

//disable shit tip
#pragma warning(disable:26812)

namespace SSMaterializer {
	namespace EnvironmentExporter {

		void IterateParameterOperation(CKParameterManager* parameterManager, Database::EnvironmentDatabase* mDb) {
			int count = parameterManager->GetParameterOperationCount();
			CKOperationDesc* opList = NULL;
			CKGUID _guid;
			int listCount = 0, cacheListCount = 0;
			for (int i = 0; i < count; i++) {
				//fill basic data
				mDb->mDbHelper.op.op_code = i;
				_guid = parameterManager->OperationCodeToGuid(i);
				Utils::CopyGuid(mDb->mDbHelper.op.op_guid, _guid);
				mDb->mDbHelper.op.op_name = parameterManager->OperationCodeToName(i);

				//allocate mem
				cacheListCount = parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, NULL);
				if (cacheListCount > listCount) {
					listCount = cacheListCount;
					opList = (CKOperationDesc*)realloc(opList, listCount * sizeof(CKOperationDesc));
					if (opList == NULL) return;
				}

				parameterManager->GetAvailableOperationsDesc(_guid, NULL, NULL, NULL, opList);
				for (int j = 0; j < cacheListCount; j++) {
					Utils::CopyGuid(mDb->mDbHelper.op.in1_guid, opList[j].P1Guid);
					Utils::CopyGuid(mDb->mDbHelper.op.in2_guid, opList[j].P2Guid);
					Utils::CopyGuid(mDb->mDbHelper.op.out_guid, opList[j].ResGuid);
					mDb->mDbHelper.op.funcPtr = opList[j].Fct;

					mDb->write_op(mDb->mDbHelper.op);
				}
			}
			if (opList != NULL) free(opList);

		}

		void IterateParameter(CKParameterManager* parameterManager, Database::EnvironmentDatabase* mDb) {
			int count = parameterManager->GetParameterTypesCount();
			CKParameterTypeDesc* desc = NULL;
			for (int i = 0; i < count; i++) {
				desc = parameterManager->GetParameterTypeDescription(i);

				mDb->mDbHelper.param.index = desc->Index;
				Utils::CopyGuid(mDb->mDbHelper.param.guid, desc->Guid);
				Utils::CopyGuid(mDb->mDbHelper.param.derived_from, desc->DerivedFrom);
				mDb->mDbHelper.param.type_name = desc->TypeName.CStr();
				mDb->mDbHelper.param.default_size = desc->DefaultSize;
				mDb->mDbHelper.param.func_CreateDefault = desc->CreateDefaultFunction;
				mDb->mDbHelper.param.func_Delete = desc->DeleteFunction;
				mDb->mDbHelper.param.func_SaveLoad = desc->SaveLoadFunction;
				mDb->mDbHelper.param.func_Check = desc->CheckFunction;
				mDb->mDbHelper.param.func_Copy = desc->CopyFunction;
				mDb->mDbHelper.param.func_String = desc->StringFunction;
				mDb->mDbHelper.param.func_UICreator = desc->UICreatorFunction;
				CKPluginEntry* plgEntry = desc->CreatorDll;
				if (plgEntry != NULL) {
					mDb->mDbHelper.param.creator_dll_index = plgEntry->m_PluginDllIndex;
					mDb->mDbHelper.param.creator_plugin_index = plgEntry->m_PositionInDll;
				} else {
					mDb->mDbHelper.param.creator_dll_index = -1;
					mDb->mDbHelper.param.creator_plugin_index = -1;
				}
				mDb->mDbHelper.param.dw_param = desc->dwParam;
				mDb->mDbHelper.param.dw_flags = desc->dwFlags;
				mDb->mDbHelper.param.cid = desc->Cid;
				Utils::CopyGuid(mDb->mDbHelper.param.saver_manager, desc->Saver_Manager);

				mDb->write_param(mDb->mDbHelper.param);
			}
		}

		void IterateAttribute(CKAttributeManager* attrManager, Database::EnvironmentDatabase* mDb) {
			int count = attrManager->GetAttributeCount();
			for (int i = 0; i < count; i++) {
				mDb->mDbHelper.attr.index = i;
				mDb->mDbHelper.attr.name = attrManager->GetAttributeNameByType(i);
				mDb->mDbHelper.attr.category_index = attrManager->GetAttributeCategoryIndex(i);
				Utils::CopyCKString(mDb->mDbHelper.attr.category_name, attrManager->GetAttributeCategory(i));
				mDb->mDbHelper.attr.flags = attrManager->GetAttributeFlags(i);
				mDb->mDbHelper.attr.param_index = attrManager->GetAttributeParameterType(i);
				mDb->mDbHelper.attr.compatible_classid = attrManager->GetAttributeCompatibleClassId(i);
				Utils::CopyCKString(mDb->mDbHelper.attr.default_value, attrManager->GetAttributeDefaultValue(i));

				mDb->write_attr(mDb->mDbHelper.attr);
			}
		}

		void IteratePlugin(CKPluginManager* plgManager, Database::EnvironmentDatabase* mDb) {
			for (int i = 0; i <= 7; i++) {
				int catCount = plgManager->GetPluginCount(i);
				mDb->mDbHelper.plugin.category = plgManager->GetCategoryName(i);
				for (int j = 0; j < catCount; j++) {
					CKPluginEntry* plgEntry = plgManager->GetPluginInfo(i, j);
					CKPluginInfo* plgInfo = &(plgEntry->m_PluginInfo);
					CKPluginDll* plgDll = plgManager->GetPluginDllInfo(plgEntry->m_PluginDllIndex);

					mDb->mDbHelper.plugin.dll_index = plgEntry->m_PluginDllIndex;

					mDb->mDbHelper.plugin.dll_name = plgDll->m_DllFileName.CStr();

					mDb->mDbHelper.plugin.plugin_index = plgEntry->m_PositionInDll;
					mDb->mDbHelper.plugin.active = plgEntry->m_Active;

					Utils::CopyGuid(mDb->mDbHelper.plugin.guid, plgInfo->m_GUID);
					mDb->mDbHelper.plugin.desc = plgInfo->m_Description.CStr();
					mDb->mDbHelper.plugin.author = plgInfo->m_Author.CStr();
					mDb->mDbHelper.plugin.summary = plgInfo->m_Summary.CStr();
					mDb->mDbHelper.plugin.version = plgInfo->m_Version;
					mDb->mDbHelper.plugin.func_init = plgInfo->m_InitInstanceFct;
					mDb->mDbHelper.plugin.func_exit = plgInfo->m_ExitInstanceFct;

					mDb->write_plugin(mDb->mDbHelper.plugin);
				}
			}
		}

#if !defined(VIRTOOLS_21)
		void IterateVariable(CKVariableManager* varManager, Database::EnvironmentDatabase* mDb) {
			CKVariableManager::Iterator it = varManager->GetVariableIterator();
			CKVariableManager::Variable* varobj = NULL;
			XString dataCopyCache;
			for (; !it.End(); it++) {
				varobj = it.GetVariable();
				mDb->mDbHelper.variable.name = it.GetName();
				Utils::CopyCKString(mDb->mDbHelper.variable.desciption, varobj->GetDescription());
				mDb->mDbHelper.variable.flags = varobj->GetFlags();
				mDb->mDbHelper.variable.type = varobj->GetType();
				Utils::CopyCKString(mDb->mDbHelper.variable.representation, varobj->GetRepresentation());
				varobj->GetStringValue(dataCopyCache);
				mDb->mDbHelper.variable.data = dataCopyCache.CStr();

				mDb->write_variable(mDb->mDbHelper.variable);
			}
		}
#endif

	}
}