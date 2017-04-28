#include "Controller.h"

namespace scone
{
	Controller::Controller( const PropNode& props, ParamSet& par, Model& model, const Locality& target_area ) :
		HasSignature( props ),
		m_TerminationRequest( false )
	{
	}

	Controller::~Controller()
	{
	}
}
