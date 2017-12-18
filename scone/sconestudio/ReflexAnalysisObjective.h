#pragma once

#include "scone/core/Storage.h"
#include "scone/core/types.h"
#include "spot/objective.h"
#include "xo/container/storage.h"
#include "xo/filesystem/path.h"

namespace scone
{
	class ReflexAnalysisObjective : public spot::objective
	{
	public:
		ReflexAnalysisObjective( const Storage< Real >& data, const xo::prop_node& pn = xo::prop_node() );
		void set_delays( const xo::prop_node& pn );

		virtual spot::fitness_t evaluate( const spot::search_point& point ) const override;
		void save_report( const xo::path& filename, const spot::search_point& point );

		size_t muscle_count() const { return excitations_.channel_size(); }
		size_t sensor_count() const { return sensors_.channel_size(); }

		bool use_velocity;
		bool use_force;
		bool use_length;

	private:
		xo::storage< double > excitations_;
		xo::storage< double > sensors_;
		std::vector< double > muscle_delay;
		std::vector< double > sensor_delay;
		double frame_rate;
		double delay_multiplier;
	};
}
