#pragma once

#include <QWidget>
#include "scone/core/Storage.h"
#include <QGridLayout>

namespace scone
{
	class GaitAnalysis : public QWidget
	{
	public:
		GaitAnalysis( QWidget* parent = nullptr );
		void update( const Storage<>& sto, const path& filename );

		Real threshold_;
		int skip_first_;
		int skip_last_;

		QString info() const { return info_; }

	private:
		Storage<> sto_;
		QGridLayout* grid_;
		QString info_;
		std::vector< class GaitPlot* > plots_;
	};
}
