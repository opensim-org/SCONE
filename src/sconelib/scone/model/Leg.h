/*
** Leg.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "memory_tools.h"
#include "scone/core/HasName.h"
#include "Body.h"
#include "Location.h"

namespace scone
{
	class SCONE_API Leg : public HasName
	{
	public:
		Leg( Body& upper, Body& foot, size_t index, Side side = NoSide, size_t rank = 0, const ContactForce* cf = nullptr );
		virtual ~Leg();

		const Body& GetUpperBody() const { return m_Upper; }
		const Body& GetFootBody() const { return m_Foot; }
		const Body& GetBaseBody() const { return m_Base; }
		Side GetSide() const { return m_Side; }
		size_t GetIndex() const { return m_Index; }
		size_t GetRank() const { return m_Rank; }
		const String& GetName() const override { return m_Name; }

		Vec3 GetContactForce() const;
		Vec3 GetRelFootPos() const;
		void GetContactForceMomentCop( Vec3& force, Vec3& moment, Vec3& cop ) const;
		Real GetLoad() const;
		Real GetLength() const { return m_LegLength; }

	private:
		Real MeasureLength() const;
		Real m_LegLength;
		Side m_Side; // #todo: use loc instead of side
		size_t m_Rank;
		size_t m_Index;
		const Body& m_Upper;
		const Body& m_Foot;
		const Body& m_Base;
		String m_Name;
		const ContactForce* m_ContactForce;
	};

	inline index_t GetLegIndex( index_t idx, bool mirrored ) { return mirrored ? ( idx ^ 1 ) : idx; }
}
