#include "Factories.h"

#include "flut/factory.hpp"
#include "scone/model/Sensors.h"

namespace scone
{
	namespace sim
	{
		static ModelFactory g_ModelFactory;
		SCONE_API ModelFactory& GetModelFactory() { return g_ModelFactory; }
		SCONE_API ModelUP CreateModel( const PropNode& props, opt::ParamSet& par ) {
			return ModelUP( GetModelFactory().Create( props )( props, par ) );
		}

		//static ControllerFactory g_ControllerFactory;
		//SCONE_API ControllerFactory& GetControllerFactory() { return g_ControllerFactory; }
		//SCONE_API ControllerUP CreateController( const PropNode& props, opt::ParamSet& par, Model& model, const Area& target_area ) {
		//	return ControllerUP( GetControllerFactory().Create( props )( props, par, model, target_area ) );
		//}

		//static SensorFactory g_SensorFactory;
		//SCONE_API SensorFactory& GetSensorFactory() { return g_SensorFactory; }
		//SCONE_API SensorUP CreateSensor( const PropNode& props, opt::ParamSet& par, Model& model, const Area& target_area ) {
		//	return SensorUP( GetSensorFactory().Create( props )( props, par, model, target_area ) );
		//}

		//SCONE_API ModelUP CreateModel( const PropNode&, opt::ParamSet& )
		//{
		//}

		SCONE_API SensorUP CreateSensor( const PropNode& props, opt::ParamSet& par, Model& m, const Area& a )
		{
			static flut::factory< Sensor, const PropNode&, opt::ParamSet&, Model&, const Area& > g_SensorFactory;
			if ( g_SensorFactory.empty() )
			{
				g_SensorFactory.register_class< MuscleForceSensor >();
				g_SensorFactory.register_class< MuscleLengthSensor >();
				g_SensorFactory.register_class< MuscleVelocitySensor >();
				g_SensorFactory.register_class< MuscleSpindleSensor >();

				g_SensorFactory.register_class< DofPositionSensor >();
				g_SensorFactory.register_class< DofVelocitySensor >();
			}
			return g_SensorFactory[ props.get< String >( "type" ) ]( props, par, m, a );
		}

	}
}
