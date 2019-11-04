#include "Scenario.h"

#include "qt_convert.h"
#include "scone/core/Log.h"
#include "scone/core/StorageIo.h"
#include "scone/optimization/opt_tools.h"
#include "xo/time/timer.h"

#include <QMessageBox>
#include <QString>

namespace scone
{
	Scenario::Scenario( vis::scene& s, const path& file ) :
		is_evaluating( false )
	{
		// todo: create the following items:
		// * optimization (optional, only with .scenario)
		// * model_objective
		// * model (only ONCE, always part of the objective?)
		// * current search_point

		// create the objective from par file or config file
		auto scenario_pn = xo::load_file_with_include( FindScenario( file ), "INCLUDE" );
		model_objective_ = CreateModelObjective( scenario_pn, file.parent_path() );
		log::info( "Created objective ", model_objective_->GetSignature(), "; dim=", model_objective_->dim(), " source=", file.filename() );

		// create model from par or with default parameters
		if ( file.extension_no_dot() == "par" )
			model_ = model_objective_->CreateModelFromParFile( file );
		else
			model_ = model_objective_->CreateModelFromParams( SearchPoint( model_objective_->info() ) );

		// accept filename and clear data
		filename_ = file;

		// load results if the file is an sto
		if ( file.extension_no_dot() == "sto" )
		{
			xo::timer t;
			log::debug( "Reading ", file );
			ReadStorageSto( storage_, file );
			InitStateDataIndices();
			log::trace( "Read ", file, " in ", t(), " seconds" );
		}
		else {
			// start evaluation
			is_evaluating = true;
			model_->SetStoreData( true );
			log::debug( "Evaluating ", filename_ );
			EvaluateTo( 0 ); // evaluate one step so we can init vis
		}

		vis_ = std::make_unique<ModelVis>( *model_, s );
		UpdateVis( 0 );
	}

	Scenario::~Scenario()
	{}

	void Scenario::InitStateDataIndices()
	{
		// setup state_data_index (lazy init)
		SCONE_ASSERT( state_data_index.empty() );
		model_state = model_->GetState();
		state_data_index.resize( model_state.GetSize() );
		for ( size_t state_idx = 0; state_idx < state_data_index.size(); state_idx++ )
		{
			auto data_idx = ( storage_.GetChannelIndex( model_state.GetName( state_idx ) ) );
			SCONE_ASSERT_MSG( data_idx != NoIndex, "Could not find state channel " + model_state.GetName( state_idx ) );
			state_data_index[ state_idx ] = data_idx;
		}
	}

	void Scenario::UpdateVis( TimeInSeconds time )
	{
		if ( !is_evaluating )
		{
			// update model state from data
			SCONE_ASSERT( !state_data_index.empty() );
			for ( index_t i = 0; i < model_state.GetSize(); ++i )
				model_state[ i ] = storage_.GetInterpolatedValue( time, state_data_index[ i ] );
			model_->SetState( model_state, time );
		}

		vis_->Update( *model_ );
	}

	void Scenario::EvaluateTo( TimeInSeconds t )
	{
		SCONE_ASSERT( IsEvaluating() );
		try
		{
			model_objective_->AdvanceSimulationTo( *model_, t );
			if ( model_->HasSimulationEnded() )
				FinalizeEvaluation( true );
		}
		catch ( std::exception& e )
		{
			FinalizeEvaluation( false );
			QString title = "Error evaluating " + to_qt( filename_.filename() );
			QString msg = e.what();
			log::error( title.toStdString(), msg.toStdString() );
			QMessageBox::critical( nullptr, title, msg );
		}
	}

	void Scenario::FinalizeEvaluation( bool output_results )
	{
		// copy data and init data
		storage_ = model_->GetData();
		if ( !storage_.IsEmpty() )
			InitStateDataIndices();

		if ( output_results )
		{
			auto fitness = model_objective_->GetResult( *model_ );
			log::info( "fitness = ", fitness );
			PropNode results;
			results.push_back( "result", model_objective_->GetReport( *model_ ) );
			auto result_files = model_->WriteResults( filename_ );

			log::debug( "Results written to ", path( filename_ ).replace_extension( "sto" ) );
			log::info( results );
		}

		is_evaluating = false;
	}

	void Scenario::ApplyViewSettings( const ModelVis::ViewSettings& flags )
	{
		vis_->ApplyViewSettings( flags );
		vis_->Update( *model_ );
	}
}
