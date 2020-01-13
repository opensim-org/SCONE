/*
** Joint.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/HasName.h"
#include "scone/core/HasData.h"
#include "scone/core/Vec3.h"

namespace scone
{
	class Dof;
	class Body;
	class Joint;

	class SCONE_API Joint : public HasName, HasData
	{
	public:
		Joint( Body& body, Body& parent_body );
		virtual ~Joint();

		virtual Vec3 GetPos() const = 0;
		virtual Vec3 GetReactionForce() const = 0;
		virtual Real GetLoad() const;

		const Body& GetBody() const { return m_Body; }
		const Body& GetParentBody() const { return m_ParentBody; }

		const std::vector< Dof* >& GetDofs() const;
		void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		Body& m_Body;
		Body& m_ParentBody;
		mutable std::vector< Dof* > m_Dofs;
	};
}
