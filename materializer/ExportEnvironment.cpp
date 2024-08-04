#include "ExportCore.hpp"
#include "Database.hpp"
#include "DataTypes.hpp"
#include "Utilities.hpp"
#include <vector>

namespace VSW::Materializer::ExportEnvironment {

	struct ExportContext {
		ExportContext(CKContext* ctx, const YYCC::yycc_u8string_view& db_path) :
			db(db_path), cache(), reporter(ctx) {}
		Database::EnvironmentDatabase db;
		DataTypes::Environment::DataCache cache;
		Utilities::EnhancedReporter reporter;
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
			Utilities::CopyGuid(expctx.cache.op.op_guid, guid);
			expctx.cache.op.op_name = param_mgr->OperationCodeToName(i);

			// get all sub-operation of this parameter operation.
			// each sub-operation can have different in out parameter type
			// and they are just grouped into the same parameter operation name.
			int op_list_count = param_mgr->GetAvailableOperationsDesc(guid, nullptr, nullptr, nullptr, nullptr);
			op_list.resize(static_cast<size_t>(op_list_count));
			param_mgr->GetAvailableOperationsDesc(guid, nullptr, nullptr, nullptr, op_list.data());
			for (const auto& op : op_list) {
				Utilities::CopyGuid(expctx.cache.op.in1_guid, op.P1Guid);
				Utilities::CopyGuid(expctx.cache.op.in2_guid, op.P2Guid);
				Utilities::CopyGuid(expctx.cache.op.out_guid, op.ResGuid);
				expctx.cache.op.func_ptr = Utilities::RelativeAddress(op.Fct);

				expctx.db.Write(expctx.cache.op);
			}
		}
	}

	static void IterateParameter(ExportContext& expctx, CKParameterManager* param_mgr) {
		// prepare variables
		CKParameterTypeDesc* desc = nullptr;

		// get the count of all paramter types and iterate it.
		int count = param_mgr->GetParameterTypesCount();
		for (int i = 0; i < count; i++) {
			desc = param_mgr->GetParameterTypeDescription(i);

			expctx.cache.param.index = desc->Index;
			Utilities::CopyGuid(expctx.cache.param.guid, desc->Guid);
			Utilities::CopyGuid(expctx.cache.param.derived_from, desc->DerivedFrom);
			expctx.cache.param.type_name = desc->TypeName.CStr();
			expctx.cache.param.default_size = desc->DefaultSize;
			expctx.cache.param.func_CreateDefault = Utilities::RelativeAddress(desc->CreateDefaultFunction);
			expctx.cache.param.func_Delete = Utilities::RelativeAddress(desc->DeleteFunction);
			expctx.cache.param.func_SaveLoad = Utilities::RelativeAddress(desc->SaveLoadFunction);
			expctx.cache.param.func_Check = Utilities::RelativeAddress(desc->CheckFunction);
			expctx.cache.param.func_Copy = Utilities::RelativeAddress(desc->CopyFunction);
			expctx.cache.param.func_String = Utilities::RelativeAddress(desc->StringFunction);
			expctx.cache.param.func_UICreator = Utilities::RelativeAddress(desc->UICreatorFunction);
			CKPluginEntry* plugin_entry = desc->CreatorDll;
			if (plugin_entry != nullptr) {
				expctx.cache.param.creator_dll_index = plugin_entry->m_PluginDllIndex;
				expctx.cache.param.creator_plugin_index = plugin_entry->m_PositionInDll;
			} else {
				expctx.cache.param.creator_dll_index = -1;
				expctx.cache.param.creator_plugin_index = -1;
			}
			expctx.cache.param.dw_param = desc->dwParam;
			expctx.cache.param.dw_flags = desc->dwFlags;
			expctx.cache.param.cid = desc->Cid;
			Utilities::CopyGuid(expctx.cache.param.saver_manager, desc->Saver_Manager);

			expctx.db.Write(expctx.cache.param);
		}
	}

	static void IterateAttribute(ExportContext& expctx, CKAttributeManager* attr_mgr) {
		// get the count of all attributes and iterate it.
		int count = attr_mgr->GetAttributeCount();
		for (int i = 0; i < count; i++) {
			expctx.cache.attr.index = i;
			expctx.cache.attr.name = attr_mgr->GetAttributeNameByType(i);
			expctx.cache.attr.category_index = attr_mgr->GetAttributeCategoryIndex(i);
			Utilities::CopyCKString(expctx.cache.attr.category_name, attr_mgr->GetAttributeCategory(i));
			expctx.cache.attr.flags = attr_mgr->GetAttributeFlags(i);
			expctx.cache.attr.param_index = attr_mgr->GetAttributeParameterType(i);
			expctx.cache.attr.compatible_classid = attr_mgr->GetAttributeCompatibleClassId(i);
			Utilities::CopyCKString(expctx.cache.attr.default_value, attr_mgr->GetAttributeDefaultValue(i));

			expctx.db.Write(expctx.cache.attr);
		}
	}

	static void IteratePlugin(ExportContext& expctx, CKPluginManager* plugin_mgr) {
		// get category count and iterate them
		int category_count = plugin_mgr->GetCategoryCount();
		for (int i = 0; i < category_count; ++i) {
			// get category name and set it
			expctx.cache.plugin.category = plugin_mgr->GetCategoryName(i);

			// iterate plugin within this category
			int plugin_count = plugin_mgr->GetPluginCount(i);
			for (int j = 0; j < plugin_count; j++) {
				CKPluginEntry* plugin_entry = plugin_mgr->GetPluginInfo(i, j);
				CKPluginInfo* plugin_info = &(plugin_entry->m_PluginInfo);
				CKPluginDll* plugin_dll = plugin_mgr->GetPluginDllInfo(plugin_entry->m_PluginDllIndex);

				expctx.cache.plugin.dll_index = plugin_entry->m_PluginDllIndex;

				expctx.cache.plugin.dll_name = plugin_dll->m_DllFileName.CStr();

				expctx.cache.plugin.plugin_index = plugin_entry->m_PositionInDll;
				expctx.cache.plugin.active = plugin_entry->m_Active;

				Utilities::CopyGuid(expctx.cache.plugin.guid, plugin_info->m_GUID);
				expctx.cache.plugin.desc = plugin_info->m_Description.CStr();
				expctx.cache.plugin.author = plugin_info->m_Author.CStr();
				expctx.cache.plugin.summary = plugin_info->m_Summary.CStr();
				expctx.cache.plugin.version = plugin_info->m_Version;
				expctx.cache.plugin.func_init = Utilities::RelativeAddress(plugin_info->m_InitInstanceFct);
				expctx.cache.plugin.func_exit = Utilities::RelativeAddress(plugin_info->m_ExitInstanceFct);

				expctx.db.Write(expctx.cache.plugin);
			}
		}
	}

#if !defined(VIRTOOLS_21)
	static void IterateVariable(ExportContext& expctx, CKVariableManager* var_mgr) {
		// prepare variables
		CKVariableManager::Variable* varobj = nullptr;
		XString xstring_cache;

		for (CKVariableManager::Iterator it = var_mgr->GetVariableIterator(); !it.End(); it++) {
			varobj = it.GetVariable();
			expctx.cache.variable.name = it.GetName();
			Utilities::CopyCKString(expctx.cache.variable.desciption, varobj->GetDescription());
			expctx.cache.variable.flags = varobj->GetFlags();
			expctx.cache.variable.type = varobj->GetType();
			Utilities::CopyCKString(expctx.cache.variable.representation, varobj->GetRepresentation());
			varobj->GetStringValue(xstring_cache);
			expctx.cache.variable.data = xstring_cache.CStr();

			expctx.db.Write(expctx.cache.variable);
		}
	}
#endif

	void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) {
		// create database and data cache in context
		ExportContext expctx(ctx, db_path);

		// export environment one by one
		IterateParameterOperation(expctx, ctx->GetParameterManager());
		IterateParameter(expctx, ctx->GetParameterManager());
		IterateAttribute(expctx, ctx->GetAttributeManager());
		IteratePlugin(expctx, CKGetPluginManager());
#if !defined(VIRTOOLS_21)
		IterateVariable(expctx, ctx->GetVariableManager());
#endif
	}

}
