/*
** Link.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <vector>

#include "scone/core/platform.h"
#include "scone/core/Exception.h"
#include "scone/core/types.h"

namespace scone
{
	class SCONE_API Link
	{
	public:
		Link(Body& b, Joint& j, Link* parent = nullptr) :
			m_Body(&b),
			m_Parent(parent),
			m_Joint(&j)
		{}
		Link( Body& b ) :
			m_Body( &b ),
			m_Parent( nullptr ),
			m_Joint( nullptr )
		{}

		// serialization, can be used for debug printing
		String ToString( const String& prefix = "" ) const;

		// get body
		Body& GetBody() { return *m_Body; }
		const Body& GetBody() const { return *m_Body; }

		// get joint
		bool HasJoint() const { return m_Joint != nullptr; }
		Joint& GetJoint() { return *m_Joint; }
		const Joint& GetJoint() const { return *m_Joint; }

		// get children
		std::vector< LinkUP >& GetChildren() { return m_Children; }
		const std::vector< LinkUP >& GetChildren() const { return m_Children; }
		const Link& GetChild( size_t i ) const { return *m_Children[ i ]; }
		Link& GetChild( size_t i ) { return *m_Children[ i ]; }

		// get parent
		const Link& GetParent() const { return *m_Parent; }

		// find link
		const Link* FindLink( const String& body ) const;
		Link* FindLink( const String& body );

	private:
		Body* m_Body;
		Link* m_Parent;
		Joint* m_Joint;
		std::vector< LinkUP > m_Children;

	private: // noncopyable
		Link( const Link& );
		Link& operator=( const Link& );
	};
}
