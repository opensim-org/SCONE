/*
** SimilarityObjective.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "ModelObjective.h"
#include "xo/filesystem/path.h"

namespace scone
{
	class SimilarityObjective : public ModelObjective
	{
	public:
		SimilarityObjective( const PropNode& props, const path& find_file_folder );
		virtual ~SimilarityObjective() {}
		
		virtual void AdvanceSimulationTo( Model& m, TimeInSeconds t ) const override;
		virtual fitness_t GetResult( Model& m ) const override;
		virtual PropNode GetReport( Model& m ) const override;
		virtual TimeInSeconds GetDuration() const override { return 0.0; }
		virtual result<fitness_t> EvaluateModel( Model& m, const xo::stop_token& st ) const override;

	private:
		path file_;
		ModelObjectiveUP target_;
		ModelUP target_model_;
		String signature_;
		mutable fitness_t result_;

	protected:
		virtual String GetClassSignature() const override;

	};
}
