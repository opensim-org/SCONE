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
	Reflexes( props.try_get_child( "Reflexes" ) )
	{
		INIT_PROP( props, symmetric, true );

		// create normal reflexes
		if ( Reflexes )
		{
			for ( const auto& item : *Reflexes )
			{
				// todo: handle "targets" tag? or create a new class for groups of monosynaptic reflexes?
				m_Reflexes.push_back( CreateReflex( item.second, par, model, loc ) );
			}
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
