#pragma once

#include <QtCore/QAbstractItemModel>

class ParTableModel : public QAbstractTableModel
{
public:
	ParTableModel( QObject* parent = nullptr ) : QAbstractTableModel( parent ) {}
	int rowCount( const QModelIndex& parent = QModelIndex() ) const override;
	int columnCount( const QModelIndex& parent = QModelIndex() ) const override;
	QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const override;
};
