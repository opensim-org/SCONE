#pragma once

#include "Function.h"

namespace scone
{
	class Polynomial : public Function
	{
	public:
		Polynomial( size_t degree );
		virtual ~Polynomial();

		virtual Real GetValue( Real x ) override;

		void SetCoefficient( size_t idx, Real value );
		size_t GetCoefficientCount();
	private:
		std::vector< Real > m_Coeffs;
	};
}
