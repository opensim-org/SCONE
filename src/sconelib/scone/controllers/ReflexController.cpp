#include "ReflexController.h"

#include "scone/core/Log.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"

#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Location.h"

#include "MuscleReflex.h"
#include <tuple>
#include "SimpleMuscleReflex.h"
#include "xo/string/string_tools.h"

using namespace xo;

namespace scone
{
	ReflexController::ReflexController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc ),
	Reflexes( props.has_key( "Reflexes" ) ? props.get_child( "Reflexes" ) : props )
	{
		INIT_PROP( props, symmetric, loc.symmetric );

		for ( const auto& item : Reflexes.select( "Reflex" ) )
		{
			if ( loc.side == NoSide )
			{
				// create reflexes for both sides
				for ( auto side : { LeftSide, RightSide } )
					m_Reflexes.push_back( CreateReflex( item.second, par, model, Location( side, symmetric ) ) );
			}
			else m_Reflexes.push_back( CreateReflex( item.second, par, model, Location( loc.side, symmetric ) ) );
		}
	}

	ReflexController::~ReflexController()
	{}

	bool ReflexController::ComputeControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// IMPORTANT: delayed storage must have been updated in through Model::UpdateSensorDelayAdapters()
		for ( ReflexUP& r : m_Reflexes )
			r->ComputeControls( timestamp );

		return false;
	}

	scone::String ReflexController::GetClassSignature() const
	{
		return "R";
	}

	void ReflexController::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& r : m_Reflexes )
			r->StoreData( frame, flags );
	}
}
