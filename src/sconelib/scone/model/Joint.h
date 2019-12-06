/*
** Joint.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/HasName.h"
#include "scone/core/Vec3.h"

namespace scone
{
	class Dof;
	class Body;
	class Joint;

	class SCONE_API Joint : public HasName
	{
	public:
		Joint( const Body& body, class Joint* parent = nullptr );
		virtual ~Joint();

		virtual Vec3 GetPos() const = 0;
		virtual Vec3 GetReactionForce() const = 0;
		virtual Real GetLoad() const;

		const Joint* GetParent() const { return m_pParent; }
		const class Body& GetBody() const { return m_Body; }
		const std::vector< Dof* >& GetDofs() const;

	protected:
		const Body& m_Body;
		const Joint* m_pParent;
		mutable std::vector< Dof* > m_Dofs;
	};
}
