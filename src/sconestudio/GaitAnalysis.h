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
		void reset();
		void update( const Storage<>& sto, const path& filename );

		const QString& info() const { return info_; }

	private:
		Storage<> sto_;
		QGridLayout* grid_;
		QString info_;
		std::vector< class GaitPlot* > plots_;
	};
}
