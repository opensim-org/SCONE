#pragma once

#include "scone/model/Model.h"

namespace scone
{
	struct model_wrapper
	{
		model_wrapper( Model& m ) : model_( m ) {}

		double get_time() { return model_.GetTime(); }
		Actuator* get_actuator( int index ) { return model_.GetActuators()[ index - 1 ]; }
		Body* get_body( int index ) { return model_.GetBodies()[ index - 1 ].get(); }

		Model& model_;
	};
}
