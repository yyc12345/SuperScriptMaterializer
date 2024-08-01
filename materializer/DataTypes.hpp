#pragma once
#include "stdafx.hpp"

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
			bLinkInputOutputType input_type;
			int input_index;
			CK_ID output_obj;
			bLinkInputOutputType output_type;
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
			pLinkInputOutputType input_type;
			BOOL input_is_bb;
			int input_index;
			CK_ID output_obj;
			pLinkInputOutputType output_type;
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

	}

	namespace Context {

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

	}

	namespace Environment {
		
		struct Table_op {
			CK_PARAMETEROPERATION funcPtr;
			std::string in1_guid;
			std::string in2_guid;
			std::string out_guid;
			std::string op_guid;
			std::string op_name;
			CKOperationType op_code;
		};

		struct Table_param {
			CKParameterType index;
			std::string guid;
			std::string derived_from;
			std::string type_name;
			int default_size;
			CK_PARAMETERCREATEDEFAULTFUNCTION func_CreateDefault;
			CK_PARAMETERDELETEFUNCTION func_Delete;
			CK_PARAMETERSAVELOADFUNCTION func_SaveLoad;
			CK_PARAMETERCHECKFUNCTION func_Check;
			CK_PARAMETERCOPYFUNCTION func_Copy;
			CK_PARAMETERSTRINGFUNCTION func_String;
			CK_PARAMETERUICREATORFUNCTION func_UICreator;
			int creator_dll_index;
			int creator_plugin_index;
			CKDWORD dw_param;
			CKDWORD dw_flags;
			CKDWORD cid;
			std::string saver_manager;
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
			std::string guid;
			std::string desc;
			std::string author;
			std::string summary;
			DWORD version;
			CK_INITINSTANCEFCT func_init;
			CK_EXITINSTANCEFCT func_exit;
		};

		struct Table_variable {
			std::string name;
			std::string desciption;
			XWORD flags;
			UNIVERSAL_VAR_TYPE type;
			std::string representation;
			std::string data;
		};

	}

}
