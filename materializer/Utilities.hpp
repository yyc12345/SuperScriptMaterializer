#pragma once
#include "stdafx.hpp"
#include <GenericHelper.hpp>

namespace VSW::Materializer::Utilities {
	
	/// @brief The value representing a invalid CK_ID.
	constexpr CK_ID INVALID_CK_ID = static_cast<CK_ID>(-1);
	constexpr char NULLPTR_CKSTRING[] = "<null>";

	class EnhancedReporter : public VSW::Reporter {
	public:
		EnhancedReporter(CKContext* ctx);
		~EnhancedReporter();

	protected:
		virtual void PrePrint(const YYCC::yycc_char8_t* strl) const override;

	private:
		CKContext* m_Ctx;
	};

	/**
	 * @brief Get relative address from given absolute address
	 * @details This function is used when exporting function pointer into database.
	 * @param[out] relative_addr_str 
	 * The variable holding relative address result.
	 * The result is module based relative address like \c xxx.dll+0x00000000.
	 * @param[in] absolute_addr The absolute address
	*/
	void RelativeAddress(const EnhancedReporter& reporter, YYCC::yycc_u8string& relative_addr_str, const void* absolute_addr);
	void CopyStrGuid(const EnhancedReporter& reporter, YYCC::yycc_u8string& dst, const CKGUID& src);
	void CopyGuid(const EnhancedReporter& reporter, int64_t& dst, const CKGUID& src);
	void CopyCKString(
		const EnhancedReporter& reporter,
		YYCC::yycc_u8string& storage, 
		const char* str,
		UINT code_page,
		const YYCC::yycc_char8_t* fallback = YYCC::EncodingHelper::ToUTF8(NULLPTR_CKSTRING)
	);

#pragma region Convenient Macros

#define CP_ADDR(dst, src) ::VSW::Materializer::Utilities::RelativeAddress(expctx.reporter, (dst), (src))
#define CP_STR_GUID(dst, src) ::VSW::Materializer::Utilities::CopyStrGuid(expctx.reporter, (dst), (src))
#define CP_GUID(dst, src) ::VSW::Materializer::Utilities::CopyGuid(expctx.reporter, (dst), (src))
#define CP_CKSTR(dst, src, ...) ::VSW::Materializer::Utilities::CopyCKString(expctx.reporter, (dst), (src), expctx.cp, ##__VA_ARGS__)

#pragma endregion


}
