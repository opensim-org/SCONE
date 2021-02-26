/*
** SconeStorageDataModel.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "QDataAnalysisView.h"
#include "scone/core/Storage.h"

class SconeStorageDataModel : public QDataAnalysisModel
{
public:
	SconeStorageDataModel( const scone::Storage<>* s = nullptr );
	virtual ~SconeStorageDataModel() {}

	void setStorage( const scone::Storage<>* s );
	virtual size_t seriesCount() const override;

	virtual QString label( int idx ) const override;

	virtual double value( int idx, double time ) const override;

	virtual std::vector< std::pair< float, float > > getSeries( int idx, double min_interval = 0.0 ) const override;

	virtual double timeFinish() const override;
	virtual double timeStart() const override;
	virtual xo::index_t timeIndex( double time ) const override;
	virtual double timeValue( xo::index_t idx ) const override;

private:
	const scone::Storage<>* storage;
	bool equidistant_delta_time;
	mutable std::pair<double, xo::index_t> index_cache;
};
