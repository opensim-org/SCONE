#pragma once

#include "scone/sim/Model.h"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( OsgModel );

	class OsgModel
	{
	public:
		OsgModel( const sim::Model& m );
		virtual ~OsgModel() {}

		void Update();
		
	private:
		const sim::Model& model;
	};
}
