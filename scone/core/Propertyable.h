#pragma once
#include "PropNode.h"

namespace scone
{
	class CORE_API Propertyable
	{
	public:
		Propertyable();
		virtual ~Propertyable();

		virtual void ProcessPropNode( PropNode& props ) = 0;

	protected:
		template< typename T >
		void ProcessProp( PropNode& props, T& var, const String& name )
		{
			var = props.Get< T >( name );
		}
	};

#define PROCESS_PROP( _props_, _var_ ) ProcessProp( props, _var_, GetCleanVarName( #_var_ ) )
}
