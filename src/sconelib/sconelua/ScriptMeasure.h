#pragma once

#include "platform.h"
#include <functional>
#include "xo/utility/pointer_types.h"
#include "scone/measures/Measure.h"
#include "scone/core/system_tools.h"

namespace scone
{
	/// Measure defined through a Lua script.
	/** Example:
	\verbatim
	# Measure based on lua script
	ScriptMeasure {
		minimize = 0 # let the optimizer know we want to maximize this measure
		target_body = "toes_r" # this parameter will be used in the script
		script_file = "data/ScriptMeasureJump.lua"
	}
	\endverbatim
	See Tutorial 6a and 6b for more examples.
	*/
	class SCONE_LUA_API ScriptMeasure : public Measure
	{
	public:
		ScriptMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );
		
		virtual double ComputeResult( const Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

		/// filename of the Lua script, path is relative to the .scone file
		path script_file;

	protected:
		virtual String GetClassSignature() const override;

	private:
		u_ptr< class lua_script > script_;
		std::function<void( struct LuaModel* )> init_;
		std::function<bool( struct LuaModel* )> update_;
		std::function<double( struct LuaModel* )> result_;
		std::function<void( struct LuaFrame* )> store_;
	};
}
