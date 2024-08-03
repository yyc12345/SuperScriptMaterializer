#pragma once
#if defined(MATERIALIZER_PLUGIN)

#include "stdafx.hpp"
#include "afxdialogex.h"
#include "resource.h"

namespace VSW::Materializer {

	// ExportDialog dialog

	class ExportDialog : public CDialogEx {
		DECLARE_DYNAMIC(ExportDialog)

	public:
		ExportDialog(CWnd* pParent = nullptr);   // standard constructor
		virtual ~ExportDialog();

		// Dialog Data
#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_EXPORT_DIALOG };
#endif

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		virtual BOOL OnInitDialog();
		afx_msg void OnBtnOkClicked();
		afx_msg void OnBtnCancelClicked();
		afx_msg void OnBtnBrowseClicked();
		afx_msg void OnBtnEncodingClicked();

		// ===== Init Settings =====
	public:
		enum class InitialDatabaseType {
			Script, Document, Environment
		};
		void SetInitialDatabaseType(InitialDatabaseType db_type);
	protected:
		InitialDatabaseType m_InitialDatabaseType;

		// ===== Result =====
	public:
		const YYCC::yycc_u8string& GetDatabaseFileResult();
		UINT GetEncodingResult();
	private:
		YYCC::yycc_u8string m_DatabaseFileResult;
		UINT m_EncodingResult;

	protected:
		// ===== Controls =====
		CEdit m_DatabaseFile;
		CButton m_EncodingSystem;
		CButton m_EncodingCustom;
		CEdit m_EncodingCustom_Value;
		// ===== Caches =====
		YYCC::yycc_u8string m_DatabaseFileCache;
		// ===== Pull & Push =====
		const YYCC::yycc_u8string& PullDatabaseFile();
		void PushDatabaseFile(const YYCC::yycc_u8string& data);
		UINT PullEncoding();
		void PushEncoding(UINT data);
		// ===== Update Functions =====
		void UpdateLayouts();
	};

}

#endif