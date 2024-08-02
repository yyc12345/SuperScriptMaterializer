#pragma once
#include "stdafx.hpp"

namespace YYCC::Materializer {

	namespace ExportScript {
		void Export(CKContext* ctx, YYCC::yycc_u8string_view& db_path);
	}

	namespace ExportDocument {
		void Export(CKContext* ctx, YYCC::yycc_u8string_view& db_path);
	}

	namespace ExportEnvironment {
		void Export(CKContext* ctx, YYCC::yycc_u8string_view& db_path);
	}

}
