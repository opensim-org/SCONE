/*
** Polynomial.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Function.h"
#include "scone/core/string_tools.h"
#include "PropNode.h"
#include "scone/optimization/Params.h"

namespace scone
{
	/// Parameterizable polynomial function. For each coefficient, the parameter can be defined
	/// through coefficient0, coefficient1, etc.
	class SCONE_API Polynomial : public Function
	{
	public:
		Polynomial( size_t degree );
		Polynomial( const PropNode& props, Params& par );
		virtual ~Polynomial();

		/// Degree of the polynomial, coefficients are parameterized through coefficient0, coefficient1, etc.
		size_t degree;

		virtual Real GetValue( Real x ) override;
		virtual void SetCoefficient( size_t idx, Real value );
		size_t GetCoefficientCount();

		// a signature describing the function
		virtual String GetSignature() override { return stringf( "P%d", m_Coeffs.size() - 1 ); }

	protected:
		std::vector< Real > m_Coeffs;
	};
}
