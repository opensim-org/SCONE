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
		Joint( class Body& body, class Joint* parent = nullptr );
		virtual ~Joint();

		virtual Vec3 GetPos() const = 0;
		virtual size_t GetDofCount() const = 0;
		virtual Real GetDofValue( size_t index = 0 ) const = 0;
		virtual const String& GetDofName( size_t index = 0 ) const = 0;
		virtual Vec3 GetReactionForce() const = 0;
		virtual Real GetLoad() const;

		bool HasDof( const String& dof_name ) const;
		const Joint* GetParent() const { return m_pParent; }
		const class Body& GetBody() const { return m_Body; }
		const std::vector< Dof* >& GetDofs() const;

	protected:
		Body& m_Body;
		Joint* m_pParent;
		mutable std::vector< Dof* > m_Dofs;
	};
}
