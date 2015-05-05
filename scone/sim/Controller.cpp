#include "stdafx.h"
#include "Controller.h"

namespace scone
{
	namespace sim
	{
		Controller::Controller( const PropNode& props, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Signature( props ),
		m_TerminationRequest( false )
		{
		}

		Controller::~Controller()
		{
		}
	}
}
