#pragma once
#include "stdafx.hpp"

namespace VSW::Materializer {

	namespace ExportScript {
		void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page);
	}

	namespace ExportDocument {
		void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page);
	}

	namespace ExportEnvironment {
		void Export(CKContext* ctx, const YYCC::yycc_u8string_view& db_path, UINT code_page);
	}

}
