#pragma once

#include "Function.h"
#include "PropNode.h"
#include "scone/optimization/Params.h"

namespace scone
{
	/// Parameterizable sine-wave or sinusoid function
	/// f(t) = amplitude * sin( 2 * Pi * frequency * t + phase ) + offset
	class SCONE_API SineWave : public Function
	{
	public:
		SineWave( const PropNode& pn, Params& par );
		virtual ~SineWave() = default;

		/// Amplitude of the sine-wave; default = 0.5
		Real amplitude_;

		/// Frequency [Hz] of the sine-wave; default = 1.0
		Real frequency_;

		/// Phase [Rad] of the sine-wave; default = 0
		Real phase_;

		/// Offset of the sine-wave; default = 0.5
		Real offset_;

		// value
		virtual Real GetValue( Real x ) override;
		virtual Real GetDerivativeValue( Real x ) override;

		// a signature describing the function
		virtual String GetSignature() override { return "S"; }
	};
}
