#include "ReflexController.h"

#include "scone/core/propnode_tools.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"

#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Locality.h"

#include "MuscleReflex.h"
#include <tuple>
#include "SimpleMuscleReflex.h"
#include "flut/string_tools.hpp"

using namespace flut;

namespace scone
{
	ReflexController::ReflexController( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Controller( props, par, model, area )
	{
		bool symmetric = props.get< bool >( "symmetric", true );

		// create normal reflexes
		if ( const PropNode* reflexvec = props.try_get_child( "Reflexes" ) )
		{
			for ( const auto& item : *reflexvec )
			{
				// todo: handle "targets" tag? or create a new class for groups of monosynaptic reflexes?
				m_Reflexes.push_back( CreateReflex( item.second, par, model, area ) );
			}
		}

		// load reflexes from file
		path reflex_file = scone::GetFolder( SCONE_SCENARIO_FOLDER ) / props.get< path >( "reflex_file", "" );
		path delay_file = scone::GetFolder( SCONE_SCENARIO_FOLDER ) / props.get< path >( "delay_file", "" );
		if ( !reflex_file.empty() && !delay_file.empty() )
		{
			auto str = flut::char_stream( reflex_file );
			auto delay_pn = load_prop( delay_file );
			while ( str.good() )
			{
				String name;
				double value, mean, std;
				str >> name >> value >> mean >> std;
				auto names = split_str( name, "-" );
				auto delay = delay_pn.get< double >( GetNameNoSide( names[ 0 ] ), 0.0 );
				m_Reflexes.push_back( std::make_unique< SimpleMuscleReflex >( names[ 0 ], names.size() > 1 ? names[ 1 ] : "", mean, std, 2 * delay, model, par, area ) );
				m_Reflexes.push_back( std::make_unique< SimpleMuscleReflex >( names[ 0 ], names.size() > 1 ? names[ 1 ] : "", mean, std, 2 * delay, model, par, MakeMirrored( area ) ) );
			}
		}
	}

	ReflexController::~ReflexController()
	{
	}

	Controller::UpdateResult ReflexController::UpdateControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// IMPORTANT: delayed storage must have been updated in through Model::UpdateSensorDelayAdapters()
		for ( ReflexUP& r : m_Reflexes )
			r->ComputeControls( timestamp );

		return SuccessfulUpdate;
	}

	scone::String ReflexController::GetClassSignature() const
	{
		return "R";
	}

	void ReflexController::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags )
	{
		for ( auto& r : m_Reflexes )
			r->StoreData( frame, flags );
	}
}
