#include "SconeStudio.h"
#include "scone/core/system.h"

#include <osgDB/ReadFile>

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

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
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

	osg::Node* scene = osgDB::readNodeFile( "resources/osg/axes.osgt" );
	ui.osgViewer->setScene( scene );

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
