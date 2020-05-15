#include "ParTableModel.h"

int ParTableModel::rowCount( const QModelIndex& parent ) const
{
	return 10;
}

int ParTableModel::columnCount( const QModelIndex& parent ) const
{
	return 3;
}

QVariant ParTableModel::data( const QModelIndex& index, int role ) const
{
	if ( role == Qt::DisplayRole )
		return QString( "%1.%2" )
		.arg( index.row() + 1 )
		.arg( index.column() + 1 );

	return QVariant();
}
