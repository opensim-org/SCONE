#include "ResultsFileSystemModel.h"
#include "flut/system/log.hpp"
#include <algorithm>
#include "QDirIterator"
#include <iostream>
#include "flut/prop_node_tools.hpp"
#include "flut/system/error_code.hpp"
#include "scone/core/Log.h"

ResultsFileSystemModel::ResultsFileSystemModel( QObject* parent ) : QFileSystemModel( parent )
{}

ResultsFileSystemModel::~ResultsFileSystemModel()
{}

ResultsFileSystemModel::Status ResultsFileSystemModel::getStatus( QFileInfo &fi ) const
{
	Status stat{ 0, 0 };
	if ( !fi.isDir() )
	{
		auto split = fi.completeBaseName().split( "_" );
		if ( split.size() > 0 )
			stat.gen = split[ 0 ].toInt();
		if ( split.size() > 2 )
			stat.best = split[ 2 ].toDouble();
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
			if ( !fileinf.isDir() )
			{
				auto fs = getStatus( fileinf );
				if ( fs.gen > stat.gen ) stat = fs;
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
		auto stat = getStatus( fileInfo( idx ) );
		if ( stat.gen == 0 && stat.best == 0.0 )
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
