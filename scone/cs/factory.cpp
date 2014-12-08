#include "stdafx.h"
#include "factory.h"
#include "FeedForwardController.h"
#include "JumpingMeasure.h"
#include "EnergyMeasure.h"

namespace scone
{
	namespace cs
	{
		namespace factory
		{
			template<>
			sim::Simulation* Create( PropNode& props )
			{
				sim::Simulation* simptr;
			
				String type = props.GetStr( "type" );
				if ( type == "Simbody" )
					simptr = new sim::Simulation_Simbody();
				else SCONE_THROW( "Unknown simulation type: " + type );
			
				simptr->ProcessPropNode( props );

				return simptr;
			}

			template<>
			cs::ParameterizableController* Create( PropNode& props )
			{
				cs::ParameterizableController* con;

				String type = props.GetStr( "type" );
				if ( type == "FeedForwardController" )
					con = new cs::FeedForwardController();
				else SCONE_THROW( "Unknown controller type: " + type );

				con->ProcessPropNode( props );

				return con;
			}

			template<>
			Measure* Create( PropNode& props )
			{
				Measure* m;

				String type = props.GetStr( "type" );
				if ( type == "JumpingMeasure" )
					m = new JumpingMeasure();
				if ( type == "EnergyMeasure" )
					m = new EnergyMeasure();
				else SCONE_THROW( "Unknown measure type: " + type );

				m->ProcessPropNode( props );

				return m;
			}
		}
	}
}
