#include "stdafx.h"
#include "Joint.h"

namespace scone
{
	namespace sim
	{
		Joint::Joint( Body& body, Joint* parent ) :
		m_Body( body ),
		m_pParent( parent )
		{
		}

		scone::sim::Joint::~Joint()
		{
		}
	}
}
