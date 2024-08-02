#pragma once
#include <cinttypes>

namespace VSW::DataTypes {

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
