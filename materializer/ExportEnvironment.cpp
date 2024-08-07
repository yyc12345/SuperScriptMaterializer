#include "ExportCore.hpp"
#include "Database.hpp"
#include "DataTypes.hpp"
#include "Utilities.hpp"
#include <vector>

namespace VSW::Materializer::ExportEnvironment {

	struct ExportContext {
		ExportContext(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) :
			db(db_path), cache(), reporter(ctx), cp(code_page) {}
		Database::EnvironmentDatabase db;
		DataTypes::Environment::DataCache cache;
		Utilities::EnhancedReporter reporter;
		UINT cp;
	};

	static void IterateParameterOperation(ExportContext& expctx, CKParameterManager* param_mgr) {
		// prepare variables
		std::vector<CKOperationDesc> op_list;
		CKGUID guid;

		// get the count of all supported paramter operations and iterate it.
		int count = param_mgr->GetParameterOperationCount();
		int listCount = 0, cacheListCount = 0;
		for (int i = 0; i < count; i++) {
			// fill the shared data part.
			expctx.cache.op.op_code = i;
			guid = param_mgr->OperationCodeToGuid(i);
			CP_GUID(expctx.cache.op.op_guid, guid);
			CP_CKSTR(expctx.cache.op.op_name, param_mgr->OperationCodeToName(i));

			// get all sub-operation of this parameter operation.
			// each sub-operation can have different in out parameter type
			// and they are just grouped into the same parameter operation name.
			int op_list_count = param_mgr->GetAvailableOperationsDesc(guid, nullptr, nullptr, nullptr, nullptr);
			op_list.resize(static_cast<size_t>(op_list_count));
			param_mgr->GetAvailableOperationsDesc(guid, nullptr, nullptr, nullptr, op_list.data());
			for (const auto& op : op_list) {
				CP_GUID(expctx.cache.op.in1_guid, op.P1Guid);
				CP_GUID(expctx.cache.op.in2_guid, op.P2Guid);
				CP_GUID(expctx.cache.op.out_guid, op.ResGuid);
				CP_ADDR(expctx.cache.op.func_ptr, op.Fct);

				expctx.db.Write(expctx.cache.op);
			}
		}
	}

	static void IterateParameter(ExportContext& expctx, CKParameterManager* param_mgr, CKPluginManager* plugin_mgr) {
		// prepare variables
		CKParameterTypeDesc* desc = nullptr;

		// get the count of all paramter types and iterate it.
		int count = param_mgr->GetParameterTypesCount();
		for (int i = 0; i < count; i++) {
			desc = param_mgr->GetParameterTypeDescription(i);

			// Parameter basic infos
			expctx.cache.param.index = desc->Index;
			CP_GUID(expctx.cache.param.guid, desc->Guid);
			CP_GUID(expctx.cache.param.derived_from, desc->DerivedFrom);
			CP_CKSTR(expctx.cache.param.name, desc->TypeName.CStr());
			expctx.cache.param.default_size = desc->DefaultSize;
			CP_ADDR(expctx.cache.param.func_CreateDefault, desc->CreateDefaultFunction);
			CP_ADDR(expctx.cache.param.func_Delete, desc->DeleteFunction);
			CP_ADDR(expctx.cache.param.func_SaveLoad, desc->SaveLoadFunction);
			CP_ADDR(expctx.cache.param.func_Check, desc->CheckFunction);
			CP_ADDR(expctx.cache.param.func_Copy, desc->CopyFunction);
			CP_ADDR(expctx.cache.param.func_String, desc->StringFunction);
			CP_ADDR(expctx.cache.param.func_UICreator, desc->UICreatorFunction);

			// Creator dll infos
			// This is different with plugin.
			// Because some parameters are provided by Virtools self.
			CKPluginEntry* plugin_entry = desc->CreatorDll;
			CKPluginDll* plugin_dll = plugin_mgr->GetPluginDllInfo(plugin_entry->m_PluginDllIndex);
			if (plugin_entry != nullptr) {
				CP_CKSTR(expctx.cache.param.dll_name, plugin_dll->m_DllFileName.CStr());
				expctx.cache.param.dll_index = plugin_entry->m_PluginDllIndex;
				expctx.cache.param.position_in_dll = plugin_entry->m_PositionInDll;
			} else {
				expctx.cache.param.dll_name = YYCC_U8("<embedded>");
				expctx.cache.param.dll_index = -1;
				expctx.cache.param.position_in_dll = -1;
			}

			// Misc
			expctx.cache.param.flags = desc->dwFlags;
			expctx.cache.param.dw_param = desc->dwParam;
			expctx.cache.param.cid = desc->Cid;
			CP_GUID(expctx.cache.param.saver_manager, desc->Saver_Manager);

			expctx.db.Write(expctx.cache.param);
		}
	}

	static void IterateAttribute(ExportContext& expctx, CKAttributeManager* attr_mgr) {
		// get the count of all attributes and iterate it.
		int count = attr_mgr->GetAttributeCount();
		for (int i = 0; i < count; i++) {
			expctx.cache.attr.index = i;
			CP_CKSTR(expctx.cache.attr.name, attr_mgr->GetAttributeNameByType(i));
			expctx.cache.attr.category_index = attr_mgr->GetAttributeCategoryIndex(i);
			CP_CKSTR(expctx.cache.attr.category_name, attr_mgr->GetAttributeCategory(i));
			expctx.cache.attr.flags = attr_mgr->GetAttributeFlags(i);
			expctx.cache.attr.param_index = attr_mgr->GetAttributeParameterType(i);
			CP_GUID(expctx.cache.attr.param_guid, attr_mgr->GetAttributeParameterGUID(i));
			expctx.cache.attr.compatible_classid = attr_mgr->GetAttributeCompatibleClassId(i);
			CP_CKSTR(expctx.cache.attr.default_value, attr_mgr->GetAttributeDefaultValue(i));

			expctx.db.Write(expctx.cache.attr);
		}
	}

	static void IteratePlugin(ExportContext& expctx, CKPluginManager* plugin_mgr) {
		// prepare variables
		CK_PLUGIN_TYPE plugin_type;

		// get category count and iterate them
		int category_count = plugin_mgr->GetCategoryCount();
		for (int i = 0; i < category_count; ++i) {
			// category name and its index
			CP_CKSTR(expctx.cache.plugin.category_name, plugin_mgr->GetCategoryName(i));
			expctx.cache.plugin.category_index = i;

			// iterate plugin within this category
			int plugin_count = plugin_mgr->GetPluginCount(i);
			for (int j = 0; j < plugin_count; j++) {
				CKPluginEntry* plugin_entry = plugin_mgr->GetPluginInfo(i, j);
				CKPluginInfo* plugin_info = &(plugin_entry->m_PluginInfo);
				CKPluginDll* plugin_dll = plugin_mgr->GetPluginDllInfo(plugin_entry->m_PluginDllIndex);

				// dll infomation (name + index + position in dll)
				CP_CKSTR(expctx.cache.plugin.dll_name, plugin_dll->m_DllFileName.CStr());
				expctx.cache.plugin.dll_index = plugin_entry->m_PluginDllIndex;
				expctx.cache.plugin.position_in_dll = plugin_entry->m_PositionInDll;

				// plugin info
				expctx.cache.plugin.index = j;
				CP_GUID(expctx.cache.plugin.guid, plugin_info->m_GUID);
				CP_CKSTR(expctx.cache.plugin.desc, plugin_info->m_Description.CStr());
				CP_CKSTR(expctx.cache.plugin.author, plugin_info->m_Author.CStr());
				CP_CKSTR(expctx.cache.plugin.summary, plugin_info->m_Summary.CStr());
				expctx.cache.plugin.version = plugin_info->m_Version;
				plugin_type = plugin_info->m_Type;
				expctx.cache.plugin.type = plugin_type;
				CP_ADDR(expctx.cache.plugin.func_init, plugin_info->m_InitInstanceFct);
				CP_ADDR(expctx.cache.plugin.func_exit, plugin_info->m_ExitInstanceFct);

				// extra fields according to plugin type
				// first reset these specific fields
				expctx.cache.plugin.reader_fct.clear();
				expctx.cache.plugin.reader_opt_count = 0;
				expctx.cache.plugin.reader_flags = static_cast<CK_DATAREADER_FLAGS>(0);
				expctx.cache.plugin.reader_setting_param_guid = INT64_C(0);
				expctx.cache.plugin.reader_file_ext.clear();
				expctx.cache.plugin.behavior_guids.clear();
				expctx.cache.plugin.manager_active = FALSE;
				// then try to fetch these specific fields
				switch (plugin_type) {
					case CKPLUGIN_BITMAP_READER:
					case CKPLUGIN_SOUND_READER:
					case CKPLUGIN_MODEL_READER:
					case CKPLUGIN_MOVIE_READER:
					{
						// Reader specific
						CP_ADDR(expctx.cache.plugin.reader_fct, plugin_entry->m_ReadersInfo->m_GetReaderFct);
						expctx.cache.plugin.reader_opt_count = plugin_entry->m_ReadersInfo->m_OptionCount;
						expctx.cache.plugin.reader_flags = plugin_entry->m_ReadersInfo->m_ReaderFlags;
						CP_GUID(expctx.cache.plugin.reader_setting_param_guid, plugin_entry->m_ReadersInfo->m_SettingsParameterGuid);
						CP_CKSTR(expctx.cache.plugin.reader_file_ext, static_cast<const char*>(plugin_info->m_Extension), YYCC_U8(""));
						break;
					}
					case CKPLUGIN_BEHAVIOR_DLL:
					{
						// Behavior specific
						std::vector<YYCC::yycc_u8string> guids;
						YYCC::yycc_u8string guid_cache;
						for (int i = 0; i < plugin_entry->m_BehaviorsInfo->m_BehaviorsGUID.Size(); ++i) {
							CP_STR_GUID(guid_cache, plugin_entry->m_BehaviorsInfo->m_BehaviorsGUID[i]);
							guids.emplace_back(guid_cache);
						}
						expctx.cache.plugin.behavior_guids = YYCC::StringHelper::Join(guids, YYCC_U8(", "));
						break;
					}
					case CKPLUGIN_MANAGER_DLL:
					case CKPLUGIN_RENDERENGINE_DLL:
					{
						// Render engine and manager specific
						// active info
						expctx.cache.plugin.manager_active = plugin_entry->m_Active;
						break;
					}
				}

				expctx.db.Write(expctx.cache.plugin);
			}
		}
	}

#if !defined(VIRTOOLS_21)
	static void IterateVariable(ExportContext& expctx, CKVariableManager* var_mgr) {
		// prepare variables
		CKVariableManager::Variable* varobj = nullptr;
		const char* var_name;
		CKVariableManager::Variable::Type var_type;
		const char* var_representation;
		int int_cache;
		float float_cache;
		XString xstring_cache;

		for (CKVariableManager::Iterator it = var_mgr->GetVariableIterator(); !it.End(); it++) {
			// get variable
			varobj = it.GetVariable();
			// variable name
			var_name = it.GetName();
			CP_CKSTR(expctx.cache.variable.name, var_name);
			// variable description
			CP_CKSTR(expctx.cache.variable.desciption, varobj->GetDescription());
			// variable flags
			expctx.cache.variable.flags = varobj->GetFlags();
			// variable type
			var_type = varobj->GetType();
			expctx.cache.variable.type = var_type;
			// variable representation
			// Representation need a special treatment.
			// Because it is not a name.
			// So we should record it as empty string if it is nullptr, instead of default <unamed>
			var_representation = varobj->GetRepresentation();
			CP_CKSTR(expctx.cache.variable.representation, var_representation, YYCC_U8(""));
			// We output variable stored value in different way
			// according to its type.
			switch (var_type) {
				case CKVariableManager::Variable::Type::INT:
					var_mgr->GetValue(var_name, &int_cache);
					expctx.cache.variable.data = YYCC::ParserHelper::ToString(int_cache);
					break;
				case CKVariableManager::Variable::Type::FLOAT:
					var_mgr->GetValue(var_name, &float_cache);
					expctx.cache.variable.data = YYCC::ParserHelper::ToString(float_cache);
					break;
				case CKVariableManager::Variable::Type::STRING:
					var_mgr->GetValue(var_name, xstring_cache);
					CP_CKSTR(expctx.cache.variable.data, xstring_cache.CStr());
					break;
				default:
					throw std::runtime_error("invalid variable type!");
			}

			expctx.db.Write(expctx.cache.variable);
		}
	}
#endif

	void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) {
		// create database and data cache in context
		ExportContext expctx(ctx, db_path, code_page);
		if (!expctx.db.IsValid()) {
			expctx.reporter.Err(YYCC_U8("Fail to open database. Export process aborted."));
			return;
		}

		// export environment one by one
		IterateParameterOperation(expctx, ctx->GetParameterManager());
		IterateParameter(expctx, ctx->GetParameterManager(), CKGetPluginManager());
		IterateAttribute(expctx, ctx->GetAttributeManager());
		IteratePlugin(expctx, CKGetPluginManager());
#if !defined(VIRTOOLS_21)
		IterateVariable(expctx, ctx->GetVariableManager());
#endif
	}

}
