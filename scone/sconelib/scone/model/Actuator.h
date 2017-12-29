#pragma once

#include "scone/core/HasName.h"
#include "scone/core/HasData.h"
#include "scone/core/Storage.h"
#include "xo/container/circular_deque.h"

namespace scone
{
	class SCONE_API Actuator : public virtual HasName, public HasData
	{
	public:
		Actuator();
		virtual ~Actuator();

		virtual void AddInput( double v ) { m_ActuatorControlValue += v; }
		virtual void ClearInput();
		virtual double GetInput() const;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

		virtual void SetDelay( TimeInSeconds d, TimeInSeconds control_step_size );
		virtual TimeInSeconds GetDelay( TimeInSeconds control_step_size );

	protected:
		size_t m_DelaySamples;
		xo::circular_deque< double > m_DelayBuffer;

		double m_ActuatorControlValue;
	};
}
