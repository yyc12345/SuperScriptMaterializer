#include "StandaloneMain.hpp"
#include "ExportCore.hpp"
#include "GenericHelper.hpp"

namespace VSW::Materializer::StandaloneMain {

#ifdef MATERIALIZER_STANDALONE

#pragma region Program Command Line Parser

	struct CmdArguments {
		CmdArguments() :
			m_InputFilePath(),
			m_ScriptDbPath(), m_DocumentDbPath(), m_EnvironmentDbPath(),
			m_CodePage(CP_ACP) {}
		YYCC::yycc_u8string m_InputFilePath;
		YYCC::yycc_u8string m_ScriptDbPath, m_DocumentDbPath, m_EnvironmentDbPath;
		UINT m_CodePage;
	};
	static YYCC::Constraints::Constraint<UINT> GetCodePageConstraint() {
		return YYCC::Constraints::Constraint<UINT> {
			[](const UINT& val) -> bool {
				return YYCC::WinFctHelper::IsValidCodePage(val);
			}
		};
	}
	class CmdParser {
	public:
		CmdParser() :
			m_InputFilePath(YYCC_U8("input"), YYCC_U8_CHAR('i'), YYCC_U8("The path to Virtools file for exporting."), YYCC_U8("example.cmo"), true),
			m_ScriptDbPath(YYCC_U8("script"), YYCC_U8_CHAR('s'), YYCC_U8("The path to exported script database. Input file is required."), YYCC_U8("script.db"), true),
			m_DocumentDbPath(YYCC_U8("document"), YYCC_U8_CHAR('d'), YYCC_U8("The path to exported document database. Input file is required."), YYCC_U8("doc.db"), true),
			m_EnvironmentDbPath(YYCC_U8("environment"), YYCC_U8_CHAR('e'), YYCC_U8("The path to exported environment database. Input file is not required."), YYCC_U8("env.db"), true),
			m_CodePage(YYCC_U8("code-page"), YYCC_U8_CHAR('c'), YYCC_U8("The code page used when reading Virtools document."), YYCC_U8("1252"), true, GetCodePageConstraint()),
			m_Version(YYCC_U8("version"), YYCC_U8_CHAR('v'), YYCC_U8("Print version infomation about this program and exit.")),
			m_Help(YYCC_U8("help"), YYCC_U8_CHAR('h'), YYCC_U8("Print this help page and exit.")),
			m_OptionContext(YYCC_U8("Virtools Schematic Weaver - Materializer"), YYCC_U8("The exporter of Virtools Schematic Weaver"), {
				&m_ScriptDbPath, &m_DocumentDbPath, &m_EnvironmentDbPath, &m_CodePage,
				&m_Version, &m_Help
			}) {}
		~CmdParser() {}
		YYCC_DEL_CLS_COPY_MOVE(CmdParser);

	public:
		YYCC::ArgParser::StringArgument m_InputFilePath;
		YYCC::ArgParser::StringArgument m_ScriptDbPath;
		YYCC::ArgParser::StringArgument m_DocumentDbPath;
		YYCC::ArgParser::StringArgument m_EnvironmentDbPath;
		YYCC::ArgParser::NumberArgument<UINT> m_CodePage;

		YYCC::ArgParser::SwitchArgument m_Version;
		YYCC::ArgParser::SwitchArgument m_Help;

		YYCC::ArgParser::OptionContext m_OptionContext;
	};
	static bool ParseCmd(int argc, char* argv[], CmdArguments& captured) {
		// fetch argument list
#if YYCC_OS == YYCC_OS_WINDOWS
		auto al = YYCC::ArgParser::ArgumentList::CreateFromWin32();
#else
		auto al = YYCC::ArgParser::ArgumentList::CreateFromStd(argc, argv);
#endif

		// do parse
		CmdParser parser;
		if (!parser.m_OptionContext.Parse(al)) {
			YYCC::ConsoleHelper::WriteLine(YYCC_U8(YYCC_COLOR_LIGHT_RED("Invalid command line.")));
			parser.m_OptionContext.Help();
			return false;
		}

		// check help command
		if (parser.m_Help.IsCaptured()) {
			parser.m_OptionContext.Help();
			return false;
		}
		// check version command
		if (parser.m_Version.IsCaptured()) {
			YYCC::ConsoleHelper::WriteLine(YYCC_U8("Virtools Schematic Weaver - Materializer v2.0.0"));
			return false;
		}

		// check command relation
		// if specify script or document export, user must provide input file
		if (parser.m_ScriptDbPath.IsCaptured() || parser.m_DocumentDbPath.IsCaptured()) {
			if (!parser.m_InputFilePath.IsCaptured()) {
				YYCC::ConsoleHelper::WriteLine(YYCC_U8(YYCC_COLOR_LIGHT_RED("You must specify one input file if you want to export script or document.")));
				parser.m_OptionContext.Help();
				return false;
			}
		}

		// setup argument
		if (parser.m_InputFilePath.IsCaptured()) captured.m_InputFilePath = parser.m_InputFilePath.Get();
		else captured.m_InputFilePath.clear();
		if (parser.m_ScriptDbPath.IsCaptured()) captured.m_ScriptDbPath = parser.m_ScriptDbPath.Get();
		else captured.m_ScriptDbPath.clear();
		if (parser.m_DocumentDbPath.IsCaptured()) captured.m_DocumentDbPath = parser.m_DocumentDbPath.Get();
		else captured.m_DocumentDbPath.clear();
		if (parser.m_EnvironmentDbPath.IsCaptured()) captured.m_EnvironmentDbPath = parser.m_EnvironmentDbPath.Get();
		else captured.m_EnvironmentDbPath.clear();
		if (parser.m_CodePage.IsCaptured()) captured.m_CodePage = parser.m_CodePage.Get();
		else captured.m_CodePage = CP_ACP;
		// okey, return
		return true;
	}

#pragma endregion

#pragma region Assistant Functions

	static void CustomAssert(VSW::Reporter& reporter, bool condition, const YYCC::yycc_char8_t* msg) {
		if (!condition) {
			if (msg != nullptr)
				reporter.Err(msg);
			std::abort();
		}
	}
#define CUSTOM_ASSERT(condition, msg) CustomAssert(reporter, (condition), YYCC_U8(msg))

#pragma endregion

	void PlayerMain(int argc, char* argv[]) {
		// ========== Check Passed Arguments ==========
		CmdArguments captured;
		if (!ParseCmd(argc, argv, captured)) {
			return;
		}

		// ========== Print Header ==========
		YYCC::ConsoleHelper::WriteLine(YYCC_U8("Virtools Schematic Weaver - Materializer"));
		YYCC::ConsoleHelper::WriteLine(YYCC_U8("Homepage: https://github.com/yyc12345/SuperScriptMaterializer"));
		YYCC::ConsoleHelper::WriteLine(YYCC_U8("Report bug: https://github.com/yyc12345/SuperScriptMaterializer/issues"));

		// ========== Init CK2 Engine ==========
		// Prepare reporter
		VSW::Reporter reporter;

		CUSTOM_ASSERT(CKStartUp() == CK_OK, "CKStartUp() Error");
		CKPluginManager* plugin_mgr = CKGetPluginManager();
		CUSTOM_ASSERT(plugin_mgr != nullptr, "CKPluginManager is nullptr");
		CUSTOM_ASSERT(plugin_mgr->ParsePlugins("RenderEngines") > 0, "Error loading RenderEngines");
		CUSTOM_ASSERT(plugin_mgr->ParsePlugins("Managers") > 0, "Error loading Managers");
		CUSTOM_ASSERT(plugin_mgr->ParsePlugins("BuildingBlocks") > 0, "Error loading BuildingBlocks");
		CUSTOM_ASSERT(plugin_mgr->ParsePlugins("Plugins") > 0, "Error loading Plugins");
		
		// ========== Create CKContext and Load File ==========
		CKContext* ctx = nullptr;
#if defined(VIRTOOLS_21)
		CUSTOM_ASSERT(CKCreateContext(&ctx, NULL, 0, 0) == CK_OK, "CKCreateContext Error");
#else
		CUSTOM_ASSERT(CKCreateContext(&ctx, NULL) == CK_OK, "CKCreateContext Error");
#endif

		CKObjectArray* array = CreateCKObjectArray();
		CUSTOM_ASSERT(!ctx->Load((char*)virtools_composition, array), "CKContext::Load() Error");

		printf("Parsing %s...\n", virtools_composition);
		
		// ========== Export Data ==========
		if (!captured.m_ScriptDbPath.empty()) {
			reporter.Info(YYCC_U8("Exporting script database..."));
			ExportScript::Export(ctx, captured.m_ScriptDbPath, captured.m_CodePage);
		}
		if (!captured.m_DocumentDbPath.empty()) {
			reporter.Info(YYCC_U8("Exporting document database..."));
			ExportDocument::Export(ctx, captured.m_DocumentDbPath, captured.m_CodePage);
		}
		if (!captured.m_EnvironmentDbPath.empty()) {
			reporter.Info(YYCC_U8("Exporting environment database..."));
			ExportEnvironment::Export(ctx, captured.m_EnvironmentDbPath, captured.m_CodePage);
		}

		// ====================== free resources and shutdown engine
		DeleteCKObjectArray(array);
		ctx->Reset();
		ctx->ClearAll();
		
		// ========== Destroy CKContext ==========
		// todo: Virtools 4.0 standalone version throw exception in there, but i don't know why
		// but it doesn't affect SSMaterializerDatabase export, perhaps
		CUSTOM_ASSERT(CKCloseContext(ctx) == CK_OK, "CKCloseContext Error.");

		// ========== Shutdown CK2 Engine ==========
		CUSTOM_ASSERT(CKShutdown() == CK_OK, "CKShutdown() Error.");

		// todo: Virtools 2.5 standalone version throw exception in there, but i don't know why
		// but it doesn't affect SSMaterializerDatabase export, perhaps
	}

#endif

}
