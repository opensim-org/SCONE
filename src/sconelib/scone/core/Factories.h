/*
** Factories.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "xo/utility/factory.h"

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/core/FactoryProps.h"

#include "scone/optimization/Params.h"

#include "scone/controllers/Reflex.h"
#include "scone/model/Model.h"
#include "scone/model/Location.h"

namespace scone
{
	using ReflexFactory = xo::factory< Reflex, const PropNode&, Params&, Model&, const Location& >;
	SCONE_API ReflexFactory& GetReflexFactory();
	SCONE_API ReflexUP CreateReflex( const FactoryProps& fp, Params& par, Model& model, const Location& target_area );

	using FunctionFactory = xo::factory< Function, const PropNode&, Params& >;
	SCONE_API FunctionFactory& GetFunctionFactory();
	SCONE_API FunctionUP CreateFunction( const FactoryProps& fp, Params& par );

	using ControllerFactory = xo::factory< Controller, const PropNode&, Params&, Model&, const Location& >;
	SCONE_API ControllerFactory& GetControllerFactory();
	SCONE_API ControllerUP CreateController( const FactoryProps& fp, Params& par, Model& model, const Location& target_area );
	SCONE_API ControllerUP CreateController( const PropNode& pn, Params& par, Model& model, const Location& target_area );

	using MeasureFactory = xo::factory< Measure, const PropNode&, Params&, const Model&, const Location& >;
	SCONE_API MeasureFactory& GetMeasureFactory();
	SCONE_API MeasureUP CreateMeasure( const FactoryProps& fp, Params& par, const Model& model, const Location& target_area );
	SCONE_API MeasureUP CreateMeasure( const PropNode& pn, Params& par, const Model& model, const Location& target_area );

	using OptimizerFactory = xo::factory< Optimizer, const PropNode&, const PropNode&, const path& >;
	SCONE_API OptimizerFactory& GetOptimizerFactory();
	SCONE_API OptimizerUP CreateOptimizer( const PropNode& props, const path& scenario_dir );

	using ModelFactory = xo::factory< Model, const PropNode&, Params& >;
	SCONE_API ModelFactory& GetModelFactory();
	SCONE_API ModelUP CreateModel( const FactoryProps& fp, Params& par, const path& scenario_dir );

	using ObjectiveFactory = xo::factory< Objective, const PropNode&, const path& >;
	SCONE_API ObjectiveFactory& GetObjectiveFactory();
	SCONE_API ObjectiveUP CreateObjective( const FactoryProps& fp, const path& find_file_folder );

	template< typename F >
	FactoryProps MakeFactoryProps( const F& fac, const PropNode::pair_t& p, const String& fac_name ) {
		if ( p.first == fac_name )
			return FactoryProps{ p.second.get< String >( "type" ), &p.second };
		else if ( fac.has_type( p.first ) )
			return { p.first, &p.second };
		else return {};
	}

	// return a type / PropNode pair for a given factory
	// type should either be the key name or a special 'type' child
	template< typename F >
	FactoryProps TryFindFactoryProps( const F& fac, const PropNode& pn, const String& fac_name ) {
		for ( auto& item : pn ) {
			if ( auto fp = MakeFactoryProps( fac, item, fac_name ) )
				return fp;
		}
		return {};
	}

	template< typename F >
	FactoryProps FindFactoryProps( const F& fac, const PropNode& pn, const String& fac_name ) {
		if ( auto r = TryFindFactoryProps( fac, pn, fac_name ) )
			return r;
		SCONE_THROW( "Could not find " + fac_name );
	}
}
