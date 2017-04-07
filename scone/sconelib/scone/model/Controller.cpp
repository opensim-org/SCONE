#include "Controller.h"

namespace scone
{
	Controller::Controller( const PropNode& props, ParamSet& par, Model& model, const Area& target_area ) :
		HasSignature( props ),
		m_TerminationRequest( false )
	{
	}

	Controller::~Controller()
	{
	}
}
