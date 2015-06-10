#include "stdafx.h"
#include "Body.h"

namespace scone
{
	namespace sim
	{
		Body::Body()
		{
		}
		
		Body::~Body()
		{
		}

		const StringIndexMap& Body::GetSensorNames() const 
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

		scone::Real Body::GetSensorValue( Index idx ) const 
		{
			throw std::logic_error("The method or operation is not implemented.");
		}

	}
}
