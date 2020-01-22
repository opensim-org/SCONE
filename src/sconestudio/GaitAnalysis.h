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

	private:
		Storage<> sto_;
		QGridLayout* grid_;
		std::vector< class GaitPlot* > plots_;
	};
}
