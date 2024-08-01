#pragma once
#include <cinttypes>

namespace VSW {

	enum class BehaviorLinkIOType : uint32_t {
		Input,
		Output
	};

	enum class ParameterLinkIOType : uint32_t {
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
