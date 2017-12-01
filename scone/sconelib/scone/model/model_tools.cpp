#include "model_tools.h"

#include "scone/core/string_tools.h"
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

	SCONE_API std::vector< std::pair< string, double > > GetVirtualMuscles( const Muscle& mus )
	{
		vector< pair< string, double > > vm( 1 );

		auto& joints = mus.GetJoints();
		vector< vector< Dof* > > dof_list( joints.size() );

		for ( Index joint_idx = 0; joint_idx < joints.size(); ++joint_idx )
		{
			auto& joint = joints[ joint_idx ];
			auto& dofs = joint->GetDofs();
			for ( Index dof_idx = 0; dof_idx < dofs.size(); ++dof_idx )
			{
				auto& dof = dofs[ dof_idx ];
				auto mom = mus.GetNormalizedMomentArm( *dof );
				auto name = GetNameNoSide( dof->GetName() ) + SignChar( mom );
				dof_list[ joint_idx ].emplace_back( dofs[ dof_idx ] );

				// HACK for trivial case
				vm.front().first += name;
				vm.front().second = 1.0;
			}
		}

		// TODO: flatten vector?


		return vm;
	}
}
