#include "Joint.h"
#include "Body.h"
#include "Model.h"
#include "Dof.h"

namespace scone
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
		for ( index_t i = 0; i < GetDofCount(); ++i )
		{
			if ( GetDofName( i ) == dof_name )
				return true;
		}
		return false;
	}

	const std::vector< Dof* >& Joint::GetDofs() const
	{
		if ( m_Dofs.empty() )
		{
			for ( int i = 0; i < GetDofCount(); ++i )
				m_Dofs.push_back( FindByName( m_Body.GetModel().GetDofs(), GetDofName( i ) ).get() );
		}
		return m_Dofs;
	}

	scone::Joint::~Joint()
	{
	}
}
