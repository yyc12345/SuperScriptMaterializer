#if defined(MATERIALIZER_PLUGIN)

#include "ExportDialog.hpp"
#include <stdexcept>

namespace VSW::Materializer {

	class ExportDialogSetting {
	public:
		static ExportDialogSetting& GetSingleton();
	private:
		static YYCC::yycc_u8string GetConfigFilePath();
		ExportDialogSetting() :
			m_LastFilePath(YYCC_U8("last-file-path"), YYCC_U8("")),
			m_Encoding(YYCC_U8("encoding"), CP_ACP),
			m_Mgr(ExportDialogSetting::GetConfigFilePath(), UINT64_C(0), {
				&m_LastFilePath, &m_Encoding
			}) {
			m_Mgr.Load();
		}
		~ExportDialogSetting() {
			m_Mgr.Save();
		}

	public:
		YYCC::ConfigManager::StringSetting m_LastFilePath;
		YYCC::ConfigManager::NumberSetting<UINT> m_Encoding;
		YYCC::ConfigManager::CoreManager m_Mgr;
	};
	ExportDialogSetting& ExportDialogSetting::GetSingleton() {
		static ExportDialogSetting g_Singleton;
		return g_Singleton;
	}
	YYCC::yycc_u8string ExportDialogSetting::GetConfigFilePath() {
		// get path to executable virtools
		YYCC::yycc_u8string u8_virtools_path;
		if (!YYCC::WinFctHelper::GetModuleFileName(NULL, u8_virtools_path))
			u8_virtools_path.clear();
		// get its parent folder and append with cfg file name
		std::filesystem::path virtools_path(YYCC::FsPathPatch::FromUTF8Path(u8_virtools_path.c_str()));
		return YYCC::FsPathPatch::ToUTF8Path(virtools_path.parent_path() / YYCC::FsPathPatch::FromUTF8Path(YYCC_U8("vsw_materializer.cfg")));
	}

	namespace ExportDialogHelper {

#define RADIOBTN_GETCHECK(instance) (instance.GetCheck() == BST_CHECKED)
#define RADIOBTN_SETCHECK(instance, stmt) (instance.SetCheck((stmt) ? BST_CHECKED : BST_UNCHECKED))

		static bool ValidateCodePage(UINT code_page) {
			CPINFOEXW cpinfo;
			return GetCPInfoExW(code_page, 0, &cpinfo);
		}

		static void SetCWndText(CWnd* ctl, const YYCC::yycc_u8string_view& val) {
			ctl->SetWindowTextA(YYCC::EncodingHelper::UTF8ToChar(val, CP_ACP).c_str());
		}

		YYCC::yycc_u8string GetCWndText(CWnd* ctl) {
			// Ref: https://learn.microsoft.com/zh-cn/cpp/mfc/reference/cwnd-class?view=msvc-170#getwindowtext
			CString recv;
			ctl->GetWindowTextA(recv);
			return YYCC::EncodingHelper::CharToUTF8((LPCSTR)recv, CP_ACP);
		}

	}

	// ExportDialog dialog

	IMPLEMENT_DYNAMIC(ExportDialog, CDialogEx)

		ExportDialog::ExportDialog(CWnd* pParent /*=nullptr*/)
		: CDialogEx(IDD_EXPORT_DIALOG, pParent),
		m_InitialDatabaseType(InitialDatabaseType::Script),
		m_DatabaseFileCache(),
		m_DatabaseFileResult(), m_EncodingResult(CP_ACP) {}

	ExportDialog::~ExportDialog() {}

	void ExportDialog::DoDataExchange(CDataExchange* pDX) {
		CDialogEx::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_EDIT1, m_DatabaseFile);
		DDX_Control(pDX, IDC_RADIO1, m_EncodingSystem);
		DDX_Control(pDX, IDC_RADIO2, m_EncodingCustom);
		DDX_Control(pDX, IDC_EDIT2, m_EncodingCustom_Value);
	}


	BEGIN_MESSAGE_MAP(ExportDialog, CDialogEx)
		// ===== Buttons =====
		ON_BN_CLICKED(IDOK, &ExportDialog::OnBtnOkClicked)
		ON_BN_CLICKED(IDCANCEL, &ExportDialog::OnBtnCancelClicked)
		ON_BN_CLICKED(IDC_BUTTON1, &ExportDialog::OnBtnBrowseClicked)
		// ===== Radio Buttons =====
		ON_BN_CLICKED(IDC_RADIO1, &ExportDialog::OnBtnEncodingClicked)
		ON_BN_CLICKED(IDC_RADIO2, &ExportDialog::OnBtnEncodingClicked)
	END_MESSAGE_MAP()

#pragma region Initial Settings

	void ExportDialog::SetInitialDatabaseType(InitialDatabaseType db_type) {
		m_InitialDatabaseType = db_type;
	}

#pragma endregion

#pragma region Result Getter

	const YYCC::yycc_u8string& ExportDialog::GetDatabaseFileResult() {
		return m_DatabaseFileResult;
	}

	UINT ExportDialog::GetEncodingResult() {
		return m_EncodingResult;
	}

#pragma endregion

	// ExportDialog message handlers

#pragma region Event Handler

	BOOL ExportDialog::OnInitDialog() {
		CDialogEx::OnInitDialog();

		// Read settings from config manager
		auto& config_manager = ExportDialogSetting::GetSingleton();
		auto u8_last_path = config_manager.m_LastFilePath.Get();
		if (!u8_last_path.empty()) {
			auto last_path = YYCC::FsPathPatch::FromUTF8Path(u8_last_path.c_str());
			switch (m_InitialDatabaseType) {
				case InitialDatabaseType::Script:
					last_path.replace_filename(YYCC::FsPathPatch::FromUTF8Path(YYCC_U8("script.db")));
					break;
				case InitialDatabaseType::Document:
					last_path.replace_filename(YYCC::FsPathPatch::FromUTF8Path(YYCC_U8("doc.db")));
					break;
				case InitialDatabaseType::Environment:
					last_path.replace_filename(YYCC::FsPathPatch::FromUTF8Path(YYCC_U8("env.db")));
					break;
				default:
					throw std::runtime_error("invalid initial database type");
			}
			u8_last_path = YYCC::FsPathPatch::ToUTF8Path(last_path);
		}
		PushDatabaseFile(u8_last_path);
		PushEncoding(config_manager.m_Encoding.Get());

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void ExportDialog::OnBtnOkClicked() {
		// ===== Collect input settings =====
		m_DatabaseFileResult = PullDatabaseFile();
		m_EncodingResult = PullEncoding();
		if (m_DatabaseFileResult.empty()) {
			MessageBoxW(m_hWnd, L"Exported database file should not be empty!", L"Setting Error", MB_OK + MB_ICONERROR);
			return;
		}
		if (!ExportDialogHelper::ValidateCodePage(m_EncodingResult)) {
			MessageBoxW(m_hWnd, L"Invalid encoding!", L"Setting Error", MB_OK + MB_ICONERROR);
			return;
		}

		// check done. sync settings to config manager
		auto& config_manager = ExportDialogSetting::GetSingleton();
		config_manager.m_LastFilePath.Set(m_DatabaseFileResult);
		config_manager.m_Encoding.Set(m_EncodingResult);

		CDialogEx::OnOK();
	}

	void ExportDialog::OnBtnCancelClicked() {
		CDialogEx::OnCancel();
	}

	void ExportDialog::OnBtnBrowseClicked() {
		// configure dialog properties
		YYCC::DialogHelper::FileDialog dialog_settings;
		auto& dialog_filter = dialog_settings.ConfigreFileTypes();
		dialog_filter.Add(YYCC_U8("Database File (*.db)"), { YYCC_U8("*.db") });
		dialog_settings.SetOwner(this->m_hWnd);
		switch (m_InitialDatabaseType) {
			case InitialDatabaseType::Script:
				dialog_settings.SetInitFileName(YYCC_U8("script.db"));
				break;
			case InitialDatabaseType::Document:
				dialog_settings.SetInitFileName(YYCC_U8("doc.db"));
				break;
			case InitialDatabaseType::Environment:
				dialog_settings.SetInitFileName(YYCC_U8("env.db"));
				break;
			default:
				throw std::runtime_error("invalid initial database type");
		}

		// open directory picker and assign it if user click ok.
		YYCC::yycc_u8string picked_file;
		if (YYCC::DialogHelper::SaveFileDialog(dialog_settings, picked_file)) {
			PushDatabaseFile(picked_file);
		}
	}

	void ExportDialog::OnBtnEncodingClicked() {
		UpdateLayouts();
	}

#pragma endregion

#pragma region Assist Functions

	const YYCC::yycc_u8string& ExportDialog::PullDatabaseFile() {
		return m_DatabaseFileCache;
	}

	void ExportDialog::PushDatabaseFile(const YYCC::yycc_u8string& data) {
		m_DatabaseFileCache = data;
		ExportDialogHelper::SetCWndText(&m_DatabaseFile, data);
	}

	UINT ExportDialog::PullEncoding() {
		if (RADIOBTN_GETCHECK(m_EncodingSystem)) return CP_ACP;
		else if (RADIOBTN_GETCHECK(m_EncodingCustom)) {
			UINT result;
			if (!YYCC::ParserHelper::TryParse(ExportDialogHelper::GetCWndText(&m_EncodingCustom_Value), result))
				result = static_cast<UINT>(-1);
			return result;
		} else {
			throw std::runtime_error("invalid encoding selection");
		}
	}

	void ExportDialog::PushEncoding(UINT data) {
		RADIOBTN_SETCHECK(m_EncodingSystem, false);
		RADIOBTN_SETCHECK(m_EncodingCustom, false);

		if (data == CP_ACP) RADIOBTN_SETCHECK(m_EncodingSystem, true);
		else {
			RADIOBTN_SETCHECK(m_EncodingCustom, true);
			ExportDialogHelper::SetCWndText(&m_EncodingCustom_Value, YYCC::ParserHelper::ToString(data));
		}

		UpdateLayouts();
	}

	void ExportDialog::UpdateLayouts() {
		// enable custom encoding input box according to selection.
		m_EncodingCustom_Value.EnableWindow(RADIOBTN_GETCHECK(m_EncodingCustom));
	}

#pragma endregion

}

#endif