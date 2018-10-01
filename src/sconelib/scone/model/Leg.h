/*
** Leg.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Link.h"
#include "memory_tools.h"
#include "scone/core/HasName.h"
#include "Body.h"
#include "Location.h"

namespace scone
{
	class SCONE_API Leg : public HasName
	{
	public:
		Leg( Link& upper, Link& foot, size_t index, Side side = NoSide, size_t rank = 0 );
		virtual ~Leg();

		const Link& GetUpperLink() const { return m_Upper; }
		const Link& GetFootLink() const { return m_Foot; }
		const Link& GetBaseLink() const { return m_Upper.GetParent(); }
		Model& GetModel() const;
		Side GetSide() const { return m_Side; }
		//Locality GetArea() const { return Locality( m_Side ); }
		size_t GetIndex() const { return m_Index; }
		size_t GetRank() const { return m_Rank; }
		const String& GetName() const override { return m_Name; }

		Vec3 GetContactForce() const;
		Vec3 GetContactCop() const;
		Vec3 GetRelFootPos() const;
		void GetContactForceMomentCop( Vec3& force, Vec3& moment, Vec3& cop ) const;
		Real GetLoad() const;
		Real GetLength() const { return m_LegLength; }

	private:
		Real MeasureLength() const;
		Real m_LegLength;
		size_t m_Index;
		Side m_Side; // TODO: use loc instead of side
		size_t m_Rank;
		Link& m_Foot;
		Link& m_Upper;
		String m_Name;
	};

	inline index_t GetLegIndex( index_t idx, bool mirrored ) { return mirrored ? ( idx ^ 1 ) : idx; }
}
