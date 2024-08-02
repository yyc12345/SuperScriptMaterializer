#pragma once
#include "stdafx.hpp"
#include "GenericDataTypes.hpp"

namespace VSW::Materializer::DataTypes {

	namespace Script {

		struct Table_script {
			CK_ID thisobj;
			std::string host_name;
			int index;
			CK_ID behavior;
		};

		struct Table_behavior {
			CK_ID thisobj;
			std::string name;
			CK_BEHAVIOR_TYPE type;
			std::string proto_name;
			std::string proto_guid;
			CK_BEHAVIOR_FLAGS flags;
			int priority;
			CKDWORD version;
			//pTarget, pIn, pOut, bIn, bOut
			std::string pin_count;
			CK_ID parent;
		};

		struct Table_bIO {
			CK_ID thisobj;
			int index;
			std::string name;
			CK_ID parent;
		};
		struct Table_bIn : public Table_bIO {};
		struct Table_bOut : public Table_bIO {};

		struct Table_pTarget {
			CK_ID thisobj;
			std::string name;
			std::string type;
			std::string type_guid;
			CK_ID parent;
			CK_ID direct_source;
			CK_ID shared_source;
		};

		struct Table_pIn {
			CK_ID thisobj;
			int index;
			std::string name;
			std::string type;
			std::string type_guid;
			CK_ID parent;
			CK_ID direct_source;
			CK_ID shared_source;
		};

		struct Table_pOut {
			CK_ID thisobj;
			int index;
			std::string name;
			std::string type;
			std::string type_guid;
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
			std::string name;
			std::string type;
			std::string type_guid;
			BOOL is_setting;
			CK_ID parent;
		};

		struct Table_pAttr {
			CK_ID thisobj;
			std::string name;
			std::string type;
			std::string type_guid;
		};

		struct Table_pLink {
			CK_ID input;
			CK_ID output;
			CK_ID parent;

			//additional field
			CK_ID input_obj;
			VSW::DataTypes::ParameterLinkIOType input_type;
			BOOL input_is_bb;
			int input_index;
			CK_ID output_obj;
			VSW::DataTypes::ParameterLinkIOType output_type;
			BOOL output_is_bb;
			int output_index;
		};

		struct Table_pOper {
			CK_ID thisobj;
			std::string op;
			std::string op_guid;
			CK_ID parent;
		};

		struct Table_eLink {
			CK_ID export_obj;
			CK_ID internal_obj;
			BOOL is_in;
			int index;
			CK_ID parent;
		};

		struct Table_data {
			std::string field;
			std::string data;
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
			std::string name;
		};

		struct Table_obj {
			CK_ID id;
			std::string name;
			CK_CLASSID classid;
			std::string classtype;
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
			std::string func_ptr;
			int64_t in1_guid;
			int64_t in2_guid;
			int64_t out_guid;
			int64_t op_guid;
			std::string op_name;
			CKOperationType op_code;
		};

		struct Table_param {
			CKParameterType index;
			int64_t guid;
			int64_t derived_from;
			std::string type_name;
			int default_size;
			std::string func_CreateDefault;
			std::string func_Delete;
			std::string func_SaveLoad;
			std::string func_Check;
			std::string func_Copy;
			std::string func_String;
			std::string func_UICreator;
			int creator_dll_index;
			int creator_plugin_index;
			CKDWORD dw_param;
			CKDWORD dw_flags;
			CKDWORD cid;
			int64_t saver_manager;
		};

		struct Table_attr {
			CKAttributeType index;
			std::string name;
			CKAttributeCategory category_index;
			std::string category_name;
			CK_ATTRIBUT_FLAGS flags;
			CKParameterType param_index;
			CK_CLASSID compatible_classid;
			std::string default_value;
		};

		struct Table_plugin {
			int dll_index;
			std::string dll_name;
			int plugin_index;
			std::string category;
			CKBOOL active;
			int64_t guid;
			std::string desc;
			std::string author;
			std::string summary;
			DWORD version;
			std::string func_init;
			std::string func_exit;
		};

#if defined(VIRTOOLS_21)
		using GenericVarType_t = int;
#else
		using GenericVarType_t = CKVariableManager::Variable::Type;
#endif
		struct Table_variable {
			std::string name;
			std::string desciption;
			XWORD flags;
			GenericVarType_t type;
			std::string representation;
			std::string data;
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
