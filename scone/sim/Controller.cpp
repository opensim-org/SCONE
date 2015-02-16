#include "stdafx.h"
#include "Controller.h"

namespace scone
{
	namespace sim
	{
		Controller::Controller( const PropNode& props ) : m_TerminationRequest( false )
		{
		}

		Controller::~Controller()
		{
		}

		void Controller::Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props )
		{
			m_TerminationRequest = false;
		}
	}
}

