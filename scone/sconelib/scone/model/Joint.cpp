#include "Joint.h"
#include "Body.h"
#include "Model.h"

namespace scone
{
	namespace sim
	{
		Joint::Joint( Body& body, Joint* parent ) :
		m_Body( body ),
		m_pParent( parent )
		{
		}

		scone::Real Joint::GetLoad() const
		{
			return GetReactionForce().length() / m_Body.GetModel().GetBW();
		}

		bool Joint::HasDof( const String& dof_name ) const
		{
			for ( Index i = 0; i < GetDofCount(); ++i )
			{
				if ( GetDofName( i ) == dof_name )
					return true;
			}
			return false;
		}

		scone::sim::Joint::~Joint()
		{
		}
	}
}
