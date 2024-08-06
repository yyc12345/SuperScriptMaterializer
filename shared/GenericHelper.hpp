#pragma once
#include <YYCCommonplace.hpp>
#include <cinttypes>

namespace VSW {

	class Reporter {
	public:
		Reporter();
		~Reporter();

	protected:
		virtual void PrePrint(const YYCC::yycc_char8_t* strl) const;

	public:
		void Err(const YYCC::yycc_char8_t* strl) const;
		void ErrF(const YYCC::yycc_char8_t* fmt, ...) const;
		void Warn(const YYCC::yycc_char8_t* strl) const;
		void WarnF(const YYCC::yycc_char8_t* fmt, ...) const;
		void Info(const YYCC::yycc_char8_t* strl) const;
		void InfoF(const YYCC::yycc_char8_t* fmt, ...) const;
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
