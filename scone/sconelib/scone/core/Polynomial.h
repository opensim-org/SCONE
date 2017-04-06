#pragma once

#include "Function.h"
#include "scone/core/string_tools.h"

namespace scone
{
	class SCONE_API Polynomial : public Function
	{
	public:
		Polynomial( size_t degree );
		Polynomial( const PropNode& props, opt::ParamSet& par );
		virtual ~Polynomial();

		virtual Real GetValue( Real x ) override;
		virtual void SetCoefficient( size_t idx, Real value );
		size_t GetCoefficientCount();

		// a signature describing the function
		virtual String GetSignature() override { return stringf( "P%d", m_Coeffs.size() - 1 ); }

	protected:
		std::vector< Real > m_Coeffs;
	};
}
