#include "stdafx.h"
#include "JointLimitMeasure.h"
#include "../sim/Model.h"

namespace scone
{
	namespace cs
	{
		JointLimitMeasure::JointLimitMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			const PropNode& lp = props.GetChild( "Limits" );
			for ( auto it = lp.Begin(); it != lp.End(); ++it )
				m_Limits.push_back( Limit( it->second->Touch(), model ) );
		}

		JointLimitMeasure::~JointLimitMeasure()
		{
		}

		JointLimitMeasure::Limit::Limit( const PropNode& props, sim::Model& model ) :
		dof( FindNamed( model.GetDofs(), props.GetStr( "dof" ) ) )
		{
			CONSTRUCT_FROM_PROP( props, range );
			INIT_FROM_PROP( props, penalty, 1.0 );
		}

		void JointLimitMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		double JointLimitMeasure::GetResult( sim::Model& model )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		scone::String JointLimitMeasure::GetSignature()
		{
			return "JLM";
		}
}
}
