#pragma once

#include "Function.h"

namespace scone
{
	class SCONE_API PieceWiseFunction : public Function
	{
	public:
		PieceWiseFunction() { };
		virtual ~PieceWiseFunction() { };

		virtual void AddPoint( Real x, Real y ) = 0;
		virtual Real GetX( size_t index ) = 0;
		virtual Real GetY( size_t index ) = 0;
	};
}
