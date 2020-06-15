#pragma once

#include <QtCore/QAbstractItemModel>
#include "spot/spot_types.h"

class ParTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	ParTableModel( QObject* parent = nullptr ) : QAbstractTableModel( parent ), info_( nullptr ) {}
	void setObjectiveInfo( const spot::objective_info* info );
	int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
	int columnCount( const QModelIndex& parent = QModelIndex() ) const override;
	QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
	QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

private:
	const spot::objective_info* info_;
};
