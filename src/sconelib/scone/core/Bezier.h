/*
** Bezier.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Function.h"
#include "PropNode.h"
#include "scone/core/string_tools.h"
#include "scone/optimization/Params.h"

namespace scone
{
	/// One-dimensional Bezier polynomial (page 138 [1]).
	///
	/// \verbatim
	///   Bezier {
	///     control_points = 5
	///     control_point_y = 0.1~0.1<0,1>
	///   }
	/// \endverbatim
	///
	/// or
	///
	/// \verbatim
	///   Bezier {
	///     control_points = 3
	///     Y0 = 0  // if omitted will be assumed zero
	///     Y1 = 1
	///     Y2 = 0
	///   }
	/// \endverbatim
	///
	/// If control_point_y is not defined, then we look for the value of the
	/// control points Y_i. If Y_i is omitted then it is assumed 0.
	///
	/// [1]
	/// http://web.eecs.umich.edu/faculty/grizzle/papers/Westervelt_biped_control_book_15_May_2007.pdf
	class SCONE_API Bezier : public Function
	{
	public:
		/// The number of control points.
		int control_points;

		Bezier( const std::vector< Real >& controlPoints );
		Bezier( const PropNode& props, Params& par );
		virtual ~Bezier( );
		virtual Real GetValue( Real x ) override;
		virtual Real GetDerivativeValue( Real x ) override;
		virtual String GetSignature( ) override;

	protected:
		std::vector< Real > m_ControlPoints;
	};
} // namespace scone
