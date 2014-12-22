#include "stdafx.h"
#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Muscle.h"

namespace scone
{
	namespace sim
	{
		Model::Model()
		{
		}
		
		Model::~Model()
		{
		}

		void Model::AddController( ControllerSP controller )
		{
			m_Controllers.push_back( controller );
		}

		void Model::ProcessProperties( const PropNode& props )
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}
