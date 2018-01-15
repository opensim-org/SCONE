#include "Dof.h"

#pragma warning( disable: 4355 )

namespace scone
{
	Dof::Dof( Joint& j ) : m_Joint( j ) {}
	Dof::~Dof() {}
}
