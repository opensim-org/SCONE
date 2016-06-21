#include "SconeStudio.h"
#include "scone/core/system_tools.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QFileSystemModel.h>
    #include <QtWidgets/QMessageBox.h>
#endif

using namespace scone;
using namespace std;

SconeStudio::SconeStudio(QWidget *parent, Qt::WindowFlags flags) :
QMainWindow(parent, flags)
{
	ui.setupUi(this);
}

bool SconeStudio::init()
{
	// init file model and browser widget
	QString path = QString( scone::GetSconeFolder( "output" ).c_str() );
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
