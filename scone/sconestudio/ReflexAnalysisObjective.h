#pragma once

#include "scone/core/Storage.h"
#include "scone/core/types.h"
#include "spot/objective.h"
#include "flut/storage.hpp"
#include "flut/system/path.hpp"

namespace scone
{
	class ReflexAnalysisObjective : public spot::objective
	{
	public:
		ReflexAnalysisObjective( const Storage< Real >& data, const flut::prop_node& pn = flut::prop_node() );
		virtual spot::fitness_t evaluate( const spot::search_point& point ) const override;

		void save_report( const flut::path& filename, const spot::search_point& point );

		size_t muscle_count() const { return excitations_.channel_size(); }
		size_t sensor_count() const { return sensors_.channel_size(); }

		bool use_velocity;
		bool use_force;
		bool use_length;

	private:
		flut::storage< double > excitations_;
		flut::storage< double > sensors_;
	};
}
