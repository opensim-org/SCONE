#include "model_tools.h"

#include "scone/core/string_tools.h"
#include "scone/core/Profiler.h"
#include "Sensors.h"
#include "Muscle.h"
#include "Joint.h"
#include "Dof.h"
#include "Side.h"

using std::vector;
using std::pair;

namespace scone
{
	SCONE_API Vec3 GetGroundCop( const Vec3& force, const Vec3& moment, Real min_force )
	{
		if ( force.y >= min_force )
			return Vec3( moment.z / force.y, 0, -moment.x / force.y );
		else return Vec3::zero();
	}

	std::vector< std::pair< string, double > > GetVirtualMusclesRecursive( const Muscle* mus, Index joint_idx )
	{
		auto& joints = mus->GetJoints();
		if ( joint_idx >= joints.size() )
			return std::vector< std::pair< string, double > >();

		auto children = GetVirtualMusclesRecursive( mus, joint_idx + 1 );

		auto& joint = joints[ joint_idx ];
		auto& dofs = joint->GetDofs();
		if ( dofs.empty() )
			return children;

		std::vector< std::pair< string, double > > results;
		for ( Index dof_idx = 0; dof_idx < dofs.size(); ++dof_idx )
		{
			auto& dof = dofs[ dof_idx ];
			auto mom = mus->GetNormalizedMomentArm( *dof );
			auto name = GetNameNoSide( dof->GetName() ) + SignChar( mom );
			if ( !children.empty() )
			{
				for ( auto& ch : children )
					results.emplace_back( name + ch.first, abs( mom ) * ch.second );
			}
			else results.emplace_back( name, abs( mom ) );
		}

		return results;
	}

	SCONE_API std::vector< std::pair< string, double > > GetVirtualMuscles( const Muscle* mus )
	{
		SCONE_PROFILE_FUNCTION;

		return GetVirtualMusclesRecursive( mus, 0 );
	}
}
