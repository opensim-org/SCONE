#pragma once

#include <string>
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "xo/geometry/vec3_type.h"
#include "xo/string/string_cast.h"
#include "scone/core/Log.h"
#include "xo/geometry/quat_type.h"
#include "scone/core/Storage.h"

namespace sol { class state; }

namespace scone
{
	using LuaString = const char*;

	template< typename T > T& GetByLuaIndex( std::vector<T>& vec, int index ) {
		SCONE_ERROR_IF( index < 1 || index > vec.size(), "Index must be between 1 and " + xo::to_str( vec.size() ) );
		return vec[ index - 1 ];
	}

	template< typename T > T& GetByLuaName( std::vector<T>& vec, const std::string name ) {
		auto it = std::find_if( vec.begin(), vec.end(), [&]( const T& item ) { return item->GetName() == name; } );
		SCONE_ERROR_IF( it == vec.end(), "Could not find \"" + name + "\"" );
		return *it;
	}

	/// Access to scone logging in script
	struct LuaScone
	{
		/// display trace message
		static void trace( LuaString msg ) { log::trace( msg ); }
		/// display debug message
		static void debug( LuaString msg ) { log::debug( msg ); }
		/// display info message
		static void info( LuaString msg ) { log::info( msg ); }
		/// display warning message
		static void warning( LuaString msg ) { log::warning( msg ); }
		/// display error message
		static void error( LuaString msg ) { log::error( msg ); }
	};

	/// 3d vector type with components x, y, z
	struct LuaVec3 : public Vec3d
	{
		using Vec3::vec3_;
	};

	/// Access to writing data for scone Analysis window
	struct LuaFrame
	{
		LuaFrame( Storage<Real>::Frame& f ) : frame_( f ) {}

		/// set a numeric value for channel named key
		void set_value( LuaString key, double value ) { frame_[ key ] = value; }
		/// set a boolean (true or false) value for channel named key
		void set_bool( LuaString key, bool b ) { frame_[ key ] = b ? 1.0 : 0.0; }
		/// get time of current frame
		double time() { return frame_.GetTime(); }

		Storage<Real>::Frame& frame_;
	};

	/// Actuator type for use in lua scripting.
	/// See ScriptController and ScriptMeasure for details on scripting.
	struct LuaActuator
	{
		LuaActuator( Actuator& a ) : act_( a ) {}

		/// get the name of the actuator
		LuaString name() { return act_.GetName().c_str(); }
		/// add a value to the normalized actuator input
		void add_input( double value ) { act_.AddInput( value ); }
		/// get the current actuator input
		double input() { return act_.GetInput(); }

		Actuator& act_;
	};

	/// Dof (degree-of-freedom) type for use in lua scripting.
	/// See ScriptController and ScriptMeasure for details on scripting.
	struct LuaDof
	{
		LuaDof( Dof& d ) : dof_( d ) {}

		/// get the name of the muscle
		LuaString name() { return dof_.GetName().c_str(); }
		/// get the current value (position) of the dof
		double position() { return dof_.GetPos(); }
		/// get the current velocity of the dof [/s]
		double velocity() { return dof_.GetVel(); }

		Dof& dof_;
	};

	/// Muscle type for use in lua scripting.
	/// See ScriptController and ScriptMeasure for details on scripting.
	struct LuaMuscle
	{
		LuaMuscle( Muscle& m ) : mus_( m ) {}

		/// get the name of the muscle
		LuaString name() { return mus_.GetName().c_str(); }
		/// add a value to the normalized actuator input
		void add_input( double value ) { mus_.AddInput( value ); }
		/// get the current actuator input
		double input() { return mus_.GetInput(); }
		/// get the normalized excitation level [0..1] of the muscle
		double excitation() { return mus_.GetExcitation(); }
		/// get the normalized activation level [0..1] of the muscle
		double activation() { return mus_.GetActivation(); }
		/// get the fiber length [m] of the contractile element
		double fiber_length() { return mus_.GetFiberLength(); }
		/// get the normalized fiber length of the contractile element
		double normalized_fiber_length() { return mus_.GetNormalizedFiberLength(); }
		/// get the optimal fiber length [m]
		double optimal_fiber_length() { return mus_.GetOptimalFiberLength(); }
		/// get the current muscle force [N]
		double fiber_force() { return mus_.GetFiberForce(); }
		/// get the normalized muscle force [0..1]
		double normalized_fiber_force() { return mus_.GetNormalizedFiberForce(); }
		/// get the maximum isometric force [N]
		double max_isometric_force() { return mus_.GetMaxIsometricForce(); }
		/// get the contraction velocity [m/s]
		double contraction_velocity() { return mus_.GetFiberVelocity(); }
		/// get the contraction velocity [m/s]
		double normalized_contraction_velocity() { return mus_.GetNormalizedFiberVelocity(); }

		Muscle& mus_;
	};

	/// Body type for use in lua scripting.
	/// See ScriptController and ScriptMeasure for details on scripting.
	struct LuaBody
	{
		LuaBody( Body& b ) : bod_( b ) {}

		/// get the name of the body
		LuaString name() { return bod_.GetName().c_str(); }
		/// get the current com position [m]
		LuaVec3 com_pos() { return bod_.GetComPos(); }
		/// get the current com velocity [m/s]
		LuaVec3 com_vel() { return bod_.GetComVel(); }
		/// get the global position [m] of a local point p on the body
		LuaVec3 point_pos( const LuaVec3& p ) { return bod_.GetPosOfPointOnBody( p ); }
		/// get the global linear velocity [m/s] of a local point p on the body
		LuaVec3 point_vel( const LuaVec3& p ) { return bod_.GetLinVelOfPointOnBody( p ); }
		/// get the body orientation as a 3d rotation vector [rad]
		LuaVec3 ang_pos() { return rotation_vector_from_quat( bod_.GetOrientation() ); }
		/// get the angular velocity [rad/s] of the body
		LuaVec3 ang_vel() { return bod_.GetAngVel(); }
		/// add external moment [Nm] to body
		void add_external_moment( double x, double y, double z ) { bod_.AddExternalMoment( Vec3d( x, y, z ) ); }
		/// add external force [N] to body com
		void add_external_force( double x, double y, double z ) { bod_.AddExternalForce( Vec3d( x, y, z ) ); }

		Body& bod_;
	};

	/// Model type for use in lua scripting.
	/// See ScriptController and ScriptMeasure for details on scripting.
	struct LuaModel
	{
		LuaModel( Model& m ) : mod_( m ) {}

		/// get the current simulation time [s]
		double time() { return mod_.GetTime(); }
		/// get the current simulation time [s]
		double delta_time() { return mod_.GetDeltaTime(); }
		/// get the current com position [m]
		LuaVec3 com_pos() { return mod_.GetComPos(); }
		/// get the current com velocity [m/s]
		LuaVec3 com_vel() { return mod_.GetComVel(); }

		/// get the actuator at index (starting at 1)
		LuaActuator actuator( int index ) { return *GetByLuaIndex( mod_.GetActuators(), index ); }
		/// find an actuator with a specific name
		LuaActuator find_actuator( LuaString name ) { return *GetByLuaName( mod_.GetActuators(), name ); }
		/// number of actuators
		size_t actuator_count() { return mod_.GetActuators().size(); }

		/// get the muscle at index (starting at 1)
		LuaDof dof( int index ) { return *GetByLuaIndex( mod_.GetDofs(), index ); }
		/// find a muscle with a specific name
		LuaDof find_dof( LuaString name ) { return *GetByLuaName( mod_.GetDofs(), name ); }
		/// number of bodies
		size_t dof_count() { return mod_.GetDofs().size(); }

		/// get the muscle at index (starting at 1)
		LuaMuscle muscle( int index ) { return *GetByLuaIndex( mod_.GetMuscles(), index ); }
		/// find a muscle with a specific name
		LuaMuscle find_muscle( LuaString name ) { return *GetByLuaName( mod_.GetMuscles(), name ); }
		/// number of bodies
		size_t muscle_count() { return mod_.GetMuscles().size(); }

		/// get the body at index (starting at 1)
		LuaBody body( int index ) { return *GetByLuaIndex( mod_.GetBodies(), index ); }
		/// find a body with a specific name
		LuaBody find_body( LuaString name ) { return *GetByLuaName( mod_.GetBodies(), name ); }
		/// number of bodies
		size_t body_count() { return mod_.GetBodies().size(); }

		Model& mod_;
	};

	/// parameter access for use in lua scripting.
	/// See ScriptController and ScriptMeasure for details on scripting.
	struct LuaParams
	{
		LuaParams( Params& p ) : par_( p ) {}

		/// get or create an optimization parameter with a specific name, mean, stdev, minval and maxval
		double create_from_mean_std( LuaString name, double mean, double stdev, double minval, double maxval ) {
			return par_.get( name, mean, stdev, minval, maxval );
		}
		/// get or create an optimization parameter from a string
		double create_from_string( LuaString name, const std::string& value ) {
			return par_.get( name, xo::to_prop_node( value ) );
		}

		Params& par_;
	};

	void register_lua_wrappers( sol::state& lua );
}
