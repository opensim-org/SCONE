#pragma once

#include "Function.h"
#include "PropNode.h"
#include "scone/optimization/Params.h"

namespace scone
{
	/// Raised cosine bell shape curve.
	///
	/// Case function:
	///
	///  f(x) = offset + amplitude / 2 (1 + cos((x - mean) π / half_width),
	///     for mean - half_width <= x <= mean + half_width
	///
	///  f(x) = 0, otherwise
	class SCONE_API RaisedCosine : public Function
	{
	public:
		RaisedCosine( const PropNode& pn, Params& par );
		virtual ~RaisedCosine() = default;

		/// Amplitude of the bell-shaped curve; default = 1.
		Real amplitude;

		/// Mean at which the bell-shaped curved is centered; default = 0.
		Real mean;

		/// The half width of the curve; default = 1.
		Real half_width;

		/// Offset of the curve; default = 0.
		Real offset;

		virtual Real GetValue( Real x ) override;
		virtual Real GetDerivativeValue( Real x ) override;
		virtual String GetSignature() override { return "R"; }
	};
}
