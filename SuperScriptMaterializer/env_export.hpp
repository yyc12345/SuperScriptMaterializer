#pragma once

#include "stdafx.h"
#include "database.hpp"

namespace SSMaterializer {
	namespace EnvironmentExporter {

		void IterateParameterOperation(CKParameterManager* parameterManager, Database::EnvironmentDatabase* mDb);
		void IterateParameter(CKParameterManager* parameterManager, Database::EnvironmentDatabase* mDb);
		void IterateAttribute(CKAttributeManager* attrManager, Database::EnvironmentDatabase* mDb);
		void IteratePlugin(CKPluginManager* plgManager, Database::EnvironmentDatabase* mDb);

#if !defined(VIRTOOLS_21)
		void IterateVariable(CKVariableManager* varManager, Database::EnvironmentDatabase* mDb);
#endif

	}
}