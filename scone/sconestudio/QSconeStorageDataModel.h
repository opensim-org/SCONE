#pragma once

#include "QDataAnalysis.h"
#include "scone/core/Storage.h"

class QSconeStorageDataModel : public QDataAnalysisModel
{
public:
	QSconeStorageDataModel( const scone::Storage<>* s = nullptr );
	virtual ~QSconeStorageDataModel() {}

	void setStorage( const scone::Storage<>* s );
	virtual size_t getVariableCount() const override;
	virtual QString getLabel( int idx ) const override;

	virtual double getValue( int idx, double time ) const override;

	virtual std::vector< std::pair< float, float > > getSeries( int idx, double min_interval = 0.0 ) const override;
	virtual double getTimeFinish() const override;
	virtual double getTimeStart() const override;

private:
	const scone::Storage<>* storage;
};
