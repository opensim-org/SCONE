#include "SequentialController.h"
#include "xo/string/string_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	SequentialController::SequentialController( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	CompositeController( props, par, model, area )
	{
		SCONE_THROW_IF( controllers_.empty(), "No Controllers defined in SequentialController" );

		double time = 0.0;
		transition_times_.push_back( time );
		if ( auto time_pn = props.try_get_child( "transition_intervals" ) )
		{
			for ( index_t idx = 0; idx < time_pn->size(); ++idx )
			{
				time += par.get( xo::stringf( "dt%d", idx + 1 ), time_pn[ idx ] );
				transition_times_.push_back( time );
			}
		}
	}

	bool SequentialController::ComputeControls( Model& model, double timestamp )
	{
		return controllers_[ GetActiveIdx( timestamp ) ]->UpdateControls( model, timestamp );
	}

	bool SequentialController::PerformAnalysis( const Model& model, double timestamp )
	{
		return controllers_[ GetActiveIdx( timestamp ) ]->UpdateAnalysis( model, timestamp );
	}

	void SequentialController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& c : controllers_ )
			c->StoreData( frame, flags );
	}

	void SequentialController::WriteResult( const path& file ) const
	{
		for ( auto& c : controllers_ )
			c->WriteResult( file );
	}

	xo::index_t SequentialController::GetActiveIdx( double timestamp )
	{
		auto it = std::upper_bound( transition_times_.begin(), transition_times_.end(), timestamp );
		return index_t( xo::clamped<int>( int( it - transition_times_.begin() - 1 ), 0, int( controllers_.size() ) - 1 ) );
	}

	String SequentialController::GetClassSignature() const
	{
		String s = "C";
		for ( auto& c : controllers_ )
			s += '.' + c->GetSignature();
		return s;
	}
}
