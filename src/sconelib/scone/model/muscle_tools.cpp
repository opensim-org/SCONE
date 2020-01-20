#include "muscle_tools.h"

#include "Muscle.h"
#include "Dof.h"
#include "scone/core/Storage.h"
#include "scone/core/math.h"
#include <cmath>
#include "xo/string/string_tools.h"
#include "Model.h"
#include "xo/container/storage.h"
#include "xo/utility/frange.h"
#include <functional>
#include <fstream>
#include "scone/core/Log.h"
#include "xo/numerical/bounds.h"

namespace scone
{
	using DofRangeVec = std::vector< Real >;

	xo::bounds<Real> GetDofBounds( const Dof& dof ) {
		auto deg_range = Range< Degree >( Range<Radian>( dof.GetRange() ) );
		return { std::round( deg_range.min.value ), std::round( deg_range.max.value ) };
	}

	DofRangeVec GetDofRangeVec( const Dof& dof, Real increment )
	{
		auto range = xo::frange( GetDofBounds( dof ), increment );
		return DofRangeVec( range.begin(), range.end() );
	}

	std::vector<Real> GetDofInfo( Dof& dof, const DofRangeVec& range, std::function<Real()> func ) {
		std::vector<Real> vec;
		vec.reserve( range.size() );
		for ( auto d : range ) {
			dof.SetPos( Degree( d ).rad_value(), false );
			vec.push_back( func() );
		}
		return vec;
	}

	void WriteMuscleInfo( xo::storage<Real>& sto, const string& name, Dof& dof, Muscle& mus, const DofRangeVec& range )
	{
		if ( sto.empty() )
			sto.add_channel( "", range );
		SCONE_ASSERT( sto.frame_size() == range.size() );

		for ( index_t i = 0; i < range.size(); ++i )
		{
			dof.SetPos( Degree( range[ i ] ).rad_value(), false );
			sto[ i ][ name + ".CE" ] = mus.GetLength() - mus.GetTendonSlackLength();
		}
	}

	void WriteMuscleInfo( Model& model )
	{
		path file = model.GetModelFile() + ".muscle_info.txt";
		auto str = std::ofstream( file.str() );
		for ( auto& dof : model.GetDofs() )
		{
			xo::storage<Real> sto;
			auto range = GetDofRangeVec( *dof, 1.0 );
			for ( auto& mus : model.GetMuscles() )
			{
				if ( mus->HasMomentArm( *dof ) )
				{
					model.SetNullState();
					auto name = dof->GetName() + '.' + mus->GetName();
					auto mus_dofs = mus->GetDofs();
					mus_dofs.erase( std::remove( mus_dofs.begin(), mus_dofs.end(), dof.get() ) );

					if ( !mus_dofs.empty() )
					{
						for ( auto other_dof : mus_dofs )
						{
							auto other_range = xo::frange( GetDofBounds( *other_dof ), 3 );
							for ( auto other_val : other_range )
							{
								const_cast<Dof*>( other_dof )->SetPos( other_val ); // yes, there's a const_cast, but this is a C++ fail
								auto postfix = xo::stringf( "@%.0f", other_val ) + other_dof->GetName();
								WriteMuscleInfo( sto, name + postfix, *dof, *mus, range );
							}
						}
					}
					else WriteMuscleInfo( sto, name, *dof, *mus, range );
				}
			}
			if ( !sto.empty() )
			{
				str << sto;
				str << std::endl;
				log::info( "Results of ", dof->GetName(), " written to ", file );
			}
		}
	}
}
