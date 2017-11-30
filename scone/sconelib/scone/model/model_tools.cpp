#include "model_tools.h"

#include "scone/core/string_tools.h"
#include "Sensors.h"
#include "Muscle.h"
#include "Joint.h"
#include "Dof.h"
#include "Side.h"

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
		std::vector< std::pair< string, double > > r( 1 );

		for ( auto& j : mus.GetJoints() )
		{
			auto joint_name = j->GetName();
			auto& dofs = j->GetDofs();
			if ( dofs.size() == 1 )
			{
				auto& dof = *dofs[ 0 ];
				auto mom = mus.GetMomentArm( dof );
				r.front().first += GetNameNoSide( dof.GetName() ) + SignChar( mom );
			}
		}
		r.front().second = 1.0;

		return r;
	}
}
