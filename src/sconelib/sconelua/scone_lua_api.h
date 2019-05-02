#pragma once

#include <string>
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/model/Muscle.h"
#include "xo/geometry/vec3_type.h"

namespace sol { class state; }

namespace scone
{
	struct lua_vec : public Vec3d
	{
		using Vec3::vec3_;
	};

	struct lua_actuator
	{
		lua_actuator( Actuator& a ) : act_( a ) {}

		const char* name() { return act_.GetName().c_str(); }
		void add_input( double value ) { act_.AddInput( value ); }

		Actuator& act_;
	};

	struct lua_muscle
	{
		lua_muscle( Muscle& m ) : mus_( m ) {}

		const char* name() { return mus_.GetName().c_str(); }
		double activation() { return mus_.GetActivation(); }
		double fiber_length() { return mus_.GetFiberLength(); }
		double force() { return mus_.GetFiberForce(); }
		double contraction_velocity() { return mus_.GetNormalizedFiberVelocity(); }

		Muscle& mus_;
	};

	struct lua_body
	{
		lua_body( Body& b ) : bod_( b ) {}

		const char* name() { return bod_.GetName().c_str(); }
		lua_vec com_pos() { return bod_.GetComPos(); }
		lua_vec com_vel() { return bod_.GetComVel(); }

		Body& bod_;
	};

	struct lua_model
	{
		lua_model( Model& m ) : mod_( m ) {}

		double time() { return mod_.GetTime(); }

		lua_actuator actuator( int index ) { return *mod_.GetActuators()[ index - 1 ]; }
		size_t actuator_count() { return mod_.GetActuators().size(); }

		lua_body find_body( const std::string name ) { return *FindByName( mod_.GetBodies(), name ); }
		lua_body body( int index ) { return *mod_.GetBodies()[ index - 1 ]; }
		size_t body_count() { return mod_.GetBodies().size(); }

		Model& mod_;
	};

	struct lua_params
	{
		lua_params( Params& p ) : par_( p ) {}

		double get( const std::string& name, double mean, double stdev, double minval, double maxval ) {
			return par_.get( name, mean, stdev, minval, maxval );
		}
		Params& par_;
	};

	void register_lua_wrappers( sol::state& lua );
}
