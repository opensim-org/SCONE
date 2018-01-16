#pragma once

#include "ModelObjective.h"
#include "xo/filesystem/path.h"

namespace scone
{
	class SimilarityObjective : public ModelObjective
	{
	public:
		SimilarityObjective( const PropNode& props );
		virtual ~SimilarityObjective() {}
		
		virtual void AdvanceModel( Model& m, TimeInSeconds t ) const override;
		virtual fitness_t GetResult( Model& m ) const override;
		virtual PropNode GetReport( Model& m ) const override;
		virtual TimeInSeconds GetDuration() const override { return 0.0; }
		virtual fitness_t EvaluateModel( Model& m ) const override;
		virtual std::vector<path> GetExternalFiles() const override { return external_files_; }

	private:
		std::vector< path > external_files_;
		path file_;
		ModelObjectiveUP target_;
		ModelUP target_model_;
		String signature_;
		mutable fitness_t result_;

	protected:
		virtual String GetClassSignature() const override;

	};
}
