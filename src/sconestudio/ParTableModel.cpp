#include "ParTableModel.h"

#include "spot/objective_info.h"
#include "qt_convert.h"

void ParTableModel::setObjectiveInfo( const spot::objective_info* info )
{
	beginResetModel();
	info_ = info;
	endResetModel();
}

int ParTableModel::rowCount( const QModelIndex& parent ) const
{
	return info_ ? int( info_->dim() ) : 0;
}

int ParTableModel::columnCount( const QModelIndex& parent ) const
{
	return 3;
}

QVariant ParTableModel::data( const QModelIndex& index, int role ) const
{
	if ( info_ && role == Qt::DisplayRole )
	{
		auto& par_info = ( *info_ )[ index.row() ];
		switch ( index.column() )
		{
		case 0: return to_qt( par_info.name );
		case 1: return QString().sprintf("%.6f", par_info.mean );
		case 2: return QString().sprintf( "%.6f", par_info.std );
		default: return "???";
		}
	}

	return QVariant();
}

QVariant ParTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
	QStringList column_names = { "Name", "Mean", "Std" };
	if ( role == Qt::DisplayRole )
	{
		if ( orientation == Qt::Horizontal )
			return column_names[ section ];
		else return QString().sprintf( "%d", section + 1 );
	}
	return QVariant();
}
