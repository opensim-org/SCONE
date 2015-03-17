#include "stdafx.h"
#include "Factories.h"

namespace scone
{
	namespace sim
	{
		static ModelFactory g_ModelFactory;
		SCONE_SIM_API ModelFactory& GetModelFactory() { return g_ModelFactory; }
		SCONE_SIM_API ModelUP CreateModel( const PropNode& props, opt::ParamSet& par ) {
			return ModelUP( GetModelFactory().Create( props )( props, par ) );
		}

		static ControllerFactory g_ControllerFactory;
		SCONE_SIM_API ControllerFactory& GetControllerFactory() { return g_ControllerFactory; }
		SCONE_SIM_API ControllerUP CreateController( const PropNode& props, opt::ParamSet& par, Model& model, const Area& target_area ) {
			return ControllerUP( GetControllerFactory().Create( props )( props, par, model, target_area ) );
		}
	}
}
