#pragma once

#include "platform.h"
#include "xo/utility/pointer_types.h"
#include "scone/measures/Measure.h"
#include "scone/core/system_tools.h"
#include <functional>
#include <vector>

namespace scone
{
	/// Measure defined through a Lua script.
	/** Runs the script defined in the file defined by ''script_file'', which is relative to the folder of the scone scenario.
	See also LuaModel, LuaBody, LuaDof, LuaActuator, LuaMuscle, LuaFrame. Example of a Lua measure script:
	\verbatim
	function init( model )
		-- This function is called at the start of the simulation
		-- 'model' can be used to initialize the measure parameters (see LuaModel)
	end

	function update( model )
		-- This function is called at each simulation timestep
		-- Use it to update the internal variables of the measure (if needed)
		return false -- change to 'return true' to terminate the simulation early
	end

	function result( model )
		-- This function is called at the end of the simulation
		-- It should return the result of the measure
		return 0.0
	end

	function store_data( current_frame )
		-- This function is called at each simulation timestep
		-- 'current_frame' can be used to store values for analysis (see LuaFrame)
	end
	\endverbatim
	See Tutorial 6a and 6b for more information.
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

		/// Array of files used by the Lua script; files included by 'require' should be added here
		std::vector<path> external_files;

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
