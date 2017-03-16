#pragma once

#include "QStorageView.h"
#include "scone/core/Storage.h"
#include "QLineSeries"

class QSconeStorageDataModel : public QStorageDataModel
{
public:
	QSconeStorageDataModel( const scone::Storage<>* s = nullptr ) : storage( s ) {}
	virtual ~QSconeStorageDataModel() {}

	void setStorage( const scone::Storage<>* s ) { storage = s; }

	virtual QString getLabel( int idx ) override {
		SCONE_ASSERT( storage );
		return QString( storage->GetLables()[ idx ].c_str() );
	}

	virtual QtCharts::QLineSeries* getSeries( int idx ) override {
		SCONE_ASSERT( storage );
		QtCharts::QLineSeries* ls = new QtCharts::QLineSeries();
		for ( size_t i = 0; i < storage->GetFrameCount(); ++i ) {
			auto& f = storage->GetFrame( i );
			ls->append( f.GetTime(), f[ idx ] );
		}
		return ls;
	}

	virtual size_t getSize() override { if ( storage ) return storage->GetChannelCount(); else return 0; }
	virtual double getValue( int idx, double time ) override { SCONE_ASSERT( storage ); return storage->GetInterpolatedFrame( time ).value( idx ); }

private:
	const scone::Storage<>* storage;
};
