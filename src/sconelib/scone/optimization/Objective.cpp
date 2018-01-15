#include "Objective.h"
#include "scone/core/Exception.h"

namespace scone
{
	Objective::Objective( const PropNode& props ) :
	HasSignature( props )
	{}

	Objective::~Objective()
	{}
}
