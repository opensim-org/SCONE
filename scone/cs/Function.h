#pragma once

#include "../core/core.h"
#include "../opt/ParamSet.h"
#include "../core/Named.h"

namespace scone
{
	/// Parameterizable function
	class Function : public opt::Parameterizable, public Named
	{
	public:
		Function( const String& name = "" ) : m_Name( name ) { };
		virtual ~Function() { };

		virtual Real GetValue( Real x ) = 0;
		virtual void ProcessParameters( opt::ParamSet& par ) override { }

		virtual const String& GetName() const override { return m_Name; }
		virtual void SetName( const String& name) override { m_Name = name; }

	private:
		String m_Name;
	};
}
