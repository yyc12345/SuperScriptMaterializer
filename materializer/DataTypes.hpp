#pragma once
#include "stdafx.hpp"
#include <GenericHelper.hpp>

namespace VSW::Materializer::DataTypes {

	struct BlobDescriptor {
		const void* ptr;
		int length;
	};

	namespace Script {

		struct Table_script {
			CK_ID beobj;
			YYCC::yycc_u8string beobj_name;
			int behavior_index;
			CK_ID behavior;
		};

		struct Table_behavior {
			CK_ID thisobj;
			YYCC::yycc_u8string name;
			CK_BEHAVIOR_TYPE type;
			YYCC::yycc_u8string proto_name;
			int64_t proto_guid;
			CK_BEHAVIOR_FLAGS flags;
			int priority;
			CKDWORD version;
			int pin_count_ptarget;
			int pin_count_pin;
			int pin_count_pout;
			int pin_count_bin;
			int pin_count_bout;
			CK_ID parent;
		};

		struct Table_bIO {
			CK_ID thisobj;
			int index;
			YYCC::yycc_u8string name;
			CK_ID parent;
		};
		struct Table_bIn : public Table_bIO {};
		struct Table_bOut : public Table_bIO {};

		struct Table_pTarget {
			CK_ID thisobj;
			YYCC::yycc_u8string name;
			int64_t type;
			CK_ID parent;
			CK_ID direct_source;
			CK_ID shared_source;
		};

		struct Table_pIn {
			CK_ID thisobj;
			int index;
			YYCC::yycc_u8string name;
			int64_t type;
			CK_ID parent;
			CK_ID direct_source;
			CK_ID shared_source;
		};

		struct Table_pOut {
			CK_ID thisobj;
			int index;
			YYCC::yycc_u8string name;
			int64_t type;
			CK_ID parent;
		};

		struct Table_bLink {
			CK_ID input;
			CK_ID output;
			int delay;
			CK_ID parent;

			// additional field
			CK_ID input_obj;
			VSW::DataTypes::BehaviorLinkIOType input_type;
			int input_index;
			CK_ID output_obj;
			VSW::DataTypes::BehaviorLinkIOType output_type;
			int output_index;
		};

		struct Table_pLocal {
			CK_ID thisobj;
			YYCC::yycc_u8string name;
			int64_t type;
			bool is_setting;
			CK_ID parent;
		};

		struct Table_pAttr {
			CK_ID thisobj;
			YYCC::yycc_u8string name;
			int64_t type;
			CK_ID owner;
		};

		struct Table_pLink {
			// Basic infos
			CK_ID parent;
			// Input infos
			CK_ID input;
			CK_ID input_obj;
			VSW::DataTypes::ParameterLinkIOType input_type;
			bool input_is_bb;
			int input_index;
			// Output infos
			CK_ID output;
			CK_ID output_obj;
			VSW::DataTypes::ParameterLinkIOType output_type;
			bool output_is_bb;
			int output_index;
		};

		struct Table_pOper {
			CK_ID thisobj;
			int64_t op;
			CK_ID parent;
		};

		struct Table_eLink {
			CK_ID export_obj;
			CK_ID internal_obj;
			bool is_in;
			int index;
			CK_ID parent;
		};

		struct Table_data {
			YYCC::yycc_u8string field;
			BlobDescriptor data;
			CK_ID parent;
		};

		class DataCache {
		public:
			DataCache() :
				script(), behavior(),
				bIn(), bOut(), bLink(),
				pOper(),
				pIn(), pOut(), pLocal(), pAttr(), pTarget(), pLink(),
				eLink(),
				data() {}
			~DataCache() {}

		public:
			Table_script script;
			Table_behavior behavior;
			Table_bIn bIn;
			Table_bOut bOut;
			Table_pIn pIn;
			Table_pOut pOut;
			Table_bLink bLink;
			Table_pLocal pLocal;
			Table_pAttr pAttr;
			Table_pLink pLink;
			Table_pOper pOper;
			Table_eLink eLink;
			Table_pTarget pTarget;
			Table_data data;
		};

	}

	namespace Document {

		struct Table_msg {
			CKMessageType index;
			YYCC::yycc_u8string name;
		};

		struct Table_obj {
			CK_ID id;
			YYCC::yycc_u8string name;
			CK_CLASSID classid;
			YYCC::yycc_u8string classid_name;
		};

		class DataCache {
		public:
			DataCache() : msg(), obj() {}
			~DataCache() {}

		public:
			Table_msg msg;
			Table_obj obj;
		};

	}

	namespace Environment {

		struct Table_op {
			YYCC::yycc_u8string func_ptr;
			int64_t in1_guid;
			int64_t in2_guid;
			int64_t out_guid;
			int64_t op_guid;
			YYCC::yycc_u8string op_name;
			CKOperationType op_code;
		};

		struct Table_param {
			// Parameter infos
			CKParameterType index;
			int64_t guid;
			int64_t derived_from;
			YYCC::yycc_u8string name;
			int default_size;
			YYCC::yycc_u8string func_CreateDefault;
			YYCC::yycc_u8string func_Delete;
			YYCC::yycc_u8string func_SaveLoad;
			YYCC::yycc_u8string func_Check;
			YYCC::yycc_u8string func_Copy;
			YYCC::yycc_u8string func_String;
			YYCC::yycc_u8string func_UICreator;
			// Dll infos
			YYCC::yycc_u8string dll_name;
			int dll_index;
			int position_in_dll;
			// Misc
			CKDWORD flags;
			CKDWORD dw_param;
			CKDWORD cid;
			int64_t saver_manager;
		};

		struct Table_attr {
			CKAttributeType index;
			YYCC::yycc_u8string name;
			CKAttributeCategory category_index;
			YYCC::yycc_u8string category_name;
			CK_ATTRIBUT_FLAGS flags;
			CKParameterType param_index;
			int64_t param_guid;
			CK_CLASSID compatible_classid;
			YYCC::yycc_u8string default_value;
		};

		struct Table_plugin {
			// Category
			YYCC::yycc_u8string category_name;
			int category_index;
			// Dll infos
			YYCC::yycc_u8string dll_name;
			int dll_index;
			int position_in_dll;
			// Plugin infos
			int index;
			int64_t guid;
			YYCC::yycc_u8string desc;
			YYCC::yycc_u8string author;
			YYCC::yycc_u8string summary;
			DWORD version;
			CK_PLUGIN_TYPE type;
			YYCC::yycc_u8string func_init;
			YYCC::yycc_u8string func_exit;
			// Reader specific
			YYCC::yycc_u8string reader_fct;
			int reader_opt_count;
			CK_DATAREADER_FLAGS reader_flags;
			int64_t reader_setting_param_guid;
			YYCC::yycc_u8string reader_file_ext;
			// Manager and Render Engine specific
			CKBOOL manager_active;
			// Behavior specific
			YYCC::yycc_u8string behavior_guids;
		};

#if defined(VIRTOOLS_21)
		// Virtools 2.1 doesn't have Variable Manager
		// We use a normal type as a placeholder.
		using GenericVarType_t = int;
#else
		using GenericVarType_t = CKVariableManager::Variable::Type;
#endif
		struct Table_variable {
			YYCC::yycc_u8string name;
			YYCC::yycc_u8string desciption;
			XWORD flags;
			GenericVarType_t type;
			YYCC::yycc_u8string representation;
			YYCC::yycc_u8string data;
		};

		class DataCache {
		public:
			DataCache() :
				op(), param(), attr(), plugin(), variable() {}
			~DataCache() {}

		public:
			Table_op op;
			Table_param param;
			Table_attr attr;
			Table_plugin plugin;
			Table_variable variable;
		};

	}

}
