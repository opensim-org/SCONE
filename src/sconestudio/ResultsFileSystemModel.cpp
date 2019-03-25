/*
** ResultsFileSystemModel.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ResultsFileSystemModel.h"
#include "xo/system/log.h"
#include <algorithm>
#include "QDirIterator"
#include <iostream>
#include "xo/container/prop_node_tools.h"
#include "xo/system/error_code.h"
#include "scone/core/Log.h"
#include "xo/string/string_cast.h"

ResultsFileSystemModel::ResultsFileSystemModel( QObject* parent ) : QFileSystemModel( parent )
{}

ResultsFileSystemModel::~ResultsFileSystemModel()
{}

ResultsFileSystemModel::Status ResultsFileSystemModel::getStatus( QFileInfo &fi ) const
{
	Status stat{ -1, 0 };
	if ( fi.isFile() && fi.suffix() == "par" )
	{
		auto split = fi.completeBaseName().split( "_" );
		if ( split.size() > 0 )
			xo::from_str( split[ 0 ].toStdString(), stat.gen );
		if ( split.size() > 2 )
			xo::from_str( split[ 2 ].toStdString(), stat.best );
	}
	else
	{
		fi.refresh();
		auto filename = fi.fileName().toStdString();
		auto cache_it = m_StatusCache.find( filename );
		if ( cache_it != m_StatusCache.end() && cache_it->second.modified == fi.lastModified() )
			return cache_it->second;

		//scone::log::trace( "Scanning folder ", fi.absoluteFilePath().toStdString() );
		for ( QDirIterator dir_it( fi.absoluteFilePath() ); dir_it.hasNext(); )
		{
			QFileInfo fileinf = QFileInfo( dir_it.next() );
			if ( fileinf.isFile() )
			{
				auto fs = getStatus( fileinf );
				if ( fs.gen > stat.gen ) stat = fs;
			}
			else if ( fileinf.fileName() != ".." && fileinf.fileName() != "." )
			{
				// do something?
			}
		}
		stat.modified = fi.lastModified();
		m_StatusCache[ filename ] = stat;
	}
	return stat;
}

QVariant ResultsFileSystemModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	if ( section < QFileSystemModel::columnCount() )
		return QFileSystemModel::headerData( section, orientation, role );

	switch ( section - QFileSystemModel::columnCount() )
	{
//	case StateCol: return QVariant( "State" );
	case GenCol: return QVariant( "Gen" );
	case ScoreCol: return QVariant( "Score" );
	default: return QVariant();
	}
}

int ResultsFileSystemModel::columnCount( const QModelIndex &parent ) const
{
	return QFileSystemModel::columnCount() + ColCount;
}

QVariant ResultsFileSystemModel::data( const QModelIndex &idx, int role ) const
{
	if ( idx.column() < QFileSystemModel::columnCount() )
		return QFileSystemModel::data( idx, role );

	if ( role == Qt::DisplayRole )
	{
		auto fi = fileInfo( idx );
		auto stat = getStatus( fi );
		if ( stat.gen < 0 )
			return QVariant( QString( "" ) );

		switch ( idx.column() - QFileSystemModel::columnCount() )
		{
//		case StateCol: return QVariant( QString( stat.state_str().c_str() ) );
		case GenCol: return QVariant( stat.gen );
		case ScoreCol: return QVariant( QString().sprintf( "%7.3f", stat.best ) );
		default: return QVariant();
		}
	}
	else if ( role == Qt::TextAlignmentRole )
	{
		return Qt::AlignRight;
	}
	else return QVariant();
}

Qt::ItemFlags ResultsFileSystemModel::flags( const QModelIndex &index ) const
{
	if ( index.column() < QFileSystemModel::columnCount() )
		return QFileSystemModel::flags( index );

	return QFileSystemModel::flags( index );
}
