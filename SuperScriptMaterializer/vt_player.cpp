#include "vt_player.h"
#include "script_export.h"
#include "env_export.h"

#if defined(VIRTOOLS_STANDALONE)

void PlayerMain(const char* virtools_composition, const char* script_db_path, const char* env_db_path) {
	printf("Super Script Materializer\n");
	printf("Homepage: https://github.com/yyc12345/SuperScriptMaterializer\n");
	printf("Report bug: https://github.com/yyc12345/SuperScriptMaterializer/issues\n");

	// ====================== Init ck2 engine
#if defined(VIRTOOLS_21)
	CommonAssert(LoadLibrary("CK2.dll") != NULL, "Error loading CK2.dll");
#endif

	CommonAssert(!CKStartUp(), "CKStartUp Error");
	CKPluginManager* pluginManager = CKGetPluginManager();
	CommonAssert(pluginManager != NULL, "PluginManager = null");
	CommonAssert(pluginManager->ParsePlugins("RenderEngines") > 0, "Error loading RenderEngines");
	CommonAssert(pluginManager->ParsePlugins("Managers") > 0, "Error loading Managers");
	CommonAssert(pluginManager->ParsePlugins("BuildingBlocks") > 0, "Error loading BuildingBlocks");
	CommonAssert(pluginManager->ParsePlugins("Plugins") > 0, "Error loading Plugins");

	// ====================== create context and load file
	CKContext* context = NULL;
	CommonAssert(!CKCreateContext(&context, NULL), "CKCreateContext Error");

	CKObjectArray* array = CreateCKObjectArray();
	CommonAssert(!context->Load((char*)virtools_composition, array), "CKContext->Load() Error");

#if defined(_RELEASE)
	try {
#endif
		printf("Parsing %s...\n", virtools_composition);

		// ====================== do SSMaterializerDatabase export
		// define and Init
		scriptDatabase* _script_db = new scriptDatabase();
		dbScriptDataStructHelper* _script_helper = new dbScriptDataStructHelper();
		envDatabase* _env_db = new envDatabase();
		dbEnvDataStructHelper* _env_helper = new dbEnvDataStructHelper();

		DeleteFile(script_db_path);
		DeleteFile(env_db_path);
		_script_db->open(script_db_path);
		_script_helper->init(context->GetParameterManager());
		_env_db->open(env_db_path);
		_env_helper->init();

		// export
		IterateScript(context, _script_db, _script_helper);

		IterateParameterOperation(context->GetParameterManager(), _env_db, _env_helper);
		IterateParameter(context->GetParameterManager(), _env_db, _env_helper);
		IterateMessage(context->GetMessageManager(), _env_db, _env_helper);
		IterateAttribute(context->GetAttributeManager(), _env_db, _env_helper);
		IteratePlugin(CKGetPluginManager(), _env_db, _env_helper);
#if !defined(VIRTOOLS_21)
		IterateVariable(context->GetVariableManager(), _env_db, _env_helper);
#endif

		// free
		_script_helper->dispose();
		_script_db->close();
		_env_helper->dispose();
		_env_db->close();
		delete  _script_helper;
		delete  _script_db;
		delete _env_helper;
		delete _env_db;

		printf("Done!");

#if defined(_RELEASE)
	} catch (const std::exception & e) {
		std::string errstr;
		errstr = "An error occurs, application will exit. Please report to developer with this window and reproduce step.\nError message: ";
		errstr += e.what();
		printf("[ERROR] %s", errstr.c_str());
		ExitProcess(1);
	}
#endif

	// ====================== free resources and shutdown engine
	DeleteCKObjectArray(array);
	context->Reset();
	context->ClearAll();

	// todo: Virtools 4.0 standalone version throw exception in there, but i don't knwo why
	// but it doesn't affect SSMaterializerDatabase export, perhaps
	CKCloseContext(context);

	CKShutdown();

	// todo: Virtools 2.5 standalone version throw exception in there, but i don't knwo why
	// but it doesn't affect SSMaterializerDatabase export, perhaps
}

void CommonAssert(BOOL condition, const char* desc) {
	if (!condition) {
		printf("[ERROR] %s\n", desc);
		ExitProcess(1);
	}
}

#endif