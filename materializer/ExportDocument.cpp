#include "ExportCore.hpp"
#include "Database.hpp"
#include "DataTypes.hpp"
#include "Utilities.hpp"
#include <vector>

namespace VSW::Materializer::ExportDocument {

	struct ExportContext {
		ExportContext(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) :
			db(db_path), cache(), reporter(ctx), cp(code_page) {}
		Database::DocumentDatabase db;
		DataTypes::Document::DataCache cache;
		Utilities::EnhancedReporter reporter;
		UINT cp;
	};


	static void IterateMessage(ExportContext& expctx, CKMessageManager* msg_mgr) {
		int count = msg_mgr->GetMessageTypeCount();
		for (int i = 0; i < count; i++) {
			expctx.cache.msg.index = i;
			CP_CKSTR(expctx.cache.msg.name, msg_mgr->GetMessageTypeName(i));

			expctx.db.Write(expctx.cache.msg);
		}
	}

	/// @brief The common parents list which exported objects belong to.
	static const std::vector<CK_CLASSID> g_CommonClassIds {
		// Export CKSceneObject will export almost objects in scene
		// That's enough
		CKCID_SCENEOBJECT
	};
	static void IterateObject(ExportContext& expctx, CKContext* ctx) {
		// Get CKParameterManager to resolve Class ID name
		CKParameterManager* param_mgr = ctx->GetParameterManager();
		// prepare variables
		CK_CLASSID classid_cache;

		for (const auto& cid : g_CommonClassIds) {
			XObjectPointerArray obj_array = ctx->GetObjectListByType(cid, TRUE);
			int obj_count = obj_array.Size();
			for (int i = 0; i < obj_count; i++) {
				CKSceneObject* scene_obj = static_cast<CKSceneObject*>(obj_array.GetObjectA(i));

				// basic infos
				expctx.cache.obj.id = scene_obj->GetID();
				CP_CKSTR(expctx.cache.obj.name, scene_obj->GetName());
				classid_cache = scene_obj->GetClassID();
				expctx.cache.obj.classid = classid_cache;
				// resolve class id name
				CP_CKSTR(expctx.cache.obj.classid_name, param_mgr->ParameterGuidToName(param_mgr->ClassIDToGuid(classid_cache)));

				expctx.db.Write(expctx.cache.obj);
			}
		}

	}

	void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page) {
		// create export context
		ExportContext expctx(ctx, db_path, code_page);
		if (!expctx.db.IsValid()) {
			expctx.reporter.Err(YYCC_U8("Fail to open database. Export process aborted."));
			return;
		}

		// export document
		IterateMessage(expctx, ctx->GetMessageManager());
		IterateObject(expctx, ctx);
	}

}
