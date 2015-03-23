#include "stdafx.h"
#include "SconeStudio.h"
#include "../core/system.h"

using namespace scone;
using namespace std;

SconeStudio::SconeStudio(QWidget *parent, Qt::WFlags flags) :
QMainWindow(parent, flags)
{
	ui.setupUi(this);
}

bool SconeStudio::init()
{
	// init file model and browser widget
	QString path = QString( m_Manager.GetSettings().GetStr( "folders.output", "C:/" ).c_str() );
	m_pFileModel = new QFileSystemModel( this );
	QStringList filters( "*.par" );
	m_pFileModel->setNameFilters( filters );
	ui.browserView->setModel( m_pFileModel );
	ui.browserView->setRootIndex( m_pFileModel->setRootPath( path ) );
	ui.browserView->setColumnHidden( 1, true );
	ui.browserView->setColumnHidden( 2, true );
	ui.browserView->setColumnHidden( 3, true );

	return true;
}

SconeStudio::~SconeStudio()
{
}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	try
	{
		m_Manager.SimulateObjective( m_pFileModel->fileInfo( idx ).absoluteFilePath().toStdString() );
	}
	catch ( std::exception& e )
	{
		QMessageBox::critical( this, "Exception", e.what() );
	}
}

