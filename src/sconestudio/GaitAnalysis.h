#pragma once

#include <QWidget>
#include <QWindow>
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
		void createChart( const String& channel, int row, int col );
		Storage<> sto_;
		QGridLayout* grid_;
	};
}
