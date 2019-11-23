/*
** ResultsFileSystemModel.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "QFileSystemModel"
#include "QDateTime"

class ResultsFileSystemModel : public QFileSystemModel
{
	Q_OBJECT

public:
	ResultsFileSystemModel( QObject* parent );
	virtual ~ResultsFileSystemModel();

	enum Column { GenCol = 0, ScoreCol = 1, ColCount = 2 };
	virtual int columnCount( const QModelIndex &parent = QModelIndex() ) const override;

private:
	struct Status {
		int gen;
		double best;
		QDateTime modified;
	};
	Status getStatus( QFileInfo &fi ) const;
	virtual QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;
	virtual QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const override;
	virtual Qt::ItemFlags flags( const QModelIndex &index ) const override;

	mutable std::map< std::string, Status > m_StatusCache;
};
