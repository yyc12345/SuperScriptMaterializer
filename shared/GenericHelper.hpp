#pragma once
#include <YYCCommonplace.hpp>
#include <cinttypes>

namespace VSW {

	class Reporter {
	public:
		Reporter();
		~Reporter();

	protected:
		void PrePrint(const YYCC::yycc_char8_t* strl);

	public:
		void Err(const YYCC::yycc_char8_t* strl);
		void ErrF(const YYCC::yycc_char8_t* fmt, ...);
		void Warn(const YYCC::yycc_char8_t* strl);
		void WarnF(const YYCC::yycc_char8_t* fmt, ...);
		void Info(const YYCC::yycc_char8_t* strl);
		void InfoF(const YYCC::yycc_char8_t* fmt, ...);
		void Debug(const YYCC::yycc_char8_t* strl);
		void DebugF(const YYCC::yycc_char8_t* fmt, ...);
	};

	namespace DataTypes {

		enum class BehaviorLinkIOType : int {
			Input,
			Output
		};

		enum class ParameterLinkIOType : int {
			ParameterIn,
			ParameterOut,
			/// @brief When using this, ignore [index] and [input_is_bb], set [input_index] to -1
			ParameterLocal,
			/// @brief When using this, ignore [index] and [input_is_bb], set [input_index] to -1
			ParameterTarget,
			/// @brief When using this, ignore [index], and [input_is_bb] will become [input_is_dataarray]
			pParameterAttribute
		};

	}

}
