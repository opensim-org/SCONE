#pragma once

#include "Link.h"
#include "Types.h"
#include "../core/Vec3.h"
#include "../core/HasName.h"

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_API Leg : public HasName
		{
		public:
			Leg( const Link& upper, const Link& foot, size_t index, Side side = NoSide, size_t rank = 0 );
			virtual ~Leg();

			const Link& GetUpperLink() const { return m_Upper; }
			const Link& GetFootLink() const { return m_Foot; }
			const Link& GetBaseLink() const { return m_Upper.GetParent(); }
			Side GetSide() const { return m_Side; }
			size_t GetIndex() const { return m_Index; }
			size_t GetRank() const { return m_Rank; }
			const String& GetName() const override { return m_Name; }

			virtual Vec3 GetContactForce() const;
			Real GetLoad() const;
			Real MeasureLength() const;

		private:
			mutable Real m_ForceToLoadFactor; // constant kept for premature optimization of GetLoad();
			size_t m_Index;
			Side m_Side;
			size_t m_Rank;
			const Link& m_Foot;
			const Link& m_Upper;
			String m_Name;
		};
	}
}
