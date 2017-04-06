#include "Controller.h"

namespace scone
{
	namespace sim
	{
		Controller::Controller( const PropNode& props, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		HasSignature( props ),
		m_TerminationRequest( false )
		{
		}

		Controller::~Controller()
		{
		}
	}
}
