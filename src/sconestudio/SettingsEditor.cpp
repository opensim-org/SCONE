#include "SettingsEditor.h"

#include <QDialog>
#include "qt_tools.h"
#include "QPropNodeItemModel.h"

#include "ui_SconeSettings.h"
#include "scone/core/PropNode.h"
#include "scone/core/system_tools.h"
#include "scone/core/Settings.h"
#include "scone/core/Log.h"
#include "xo/filesystem/path.h"
#include "xo/system/type_class.h"
#include "xo/container/flat_map.h"

namespace scone
{
	int SettingsEditor::showDialog( QWidget* parent )
	{
		QDialog dialog_window( parent );
		Ui::Settings ui;
		ui.setupUi( &dialog_window );

		// init settings
		auto& settings = GetSconeSettings();

		// folders
		ui.scenariosFolder->setText( make_qt( xo::path( GetFolder( SCONE_SCENARIO_FOLDER ) ).make_preferred() ) );
		ui.resultsFolder->setText( make_qt( xo::path( GetFolder( SCONE_RESULTS_FOLDER ) ).make_preferred() ) );
		ui.geometryFolder->setText( make_qt( xo::path( GetFolder( SCONE_GEOMETRY_FOLDER ) ).make_preferred() ) );

		// data checkboxes
		xo::flat_map< string, QListWidgetItem* > data_checkboxes;
		for ( auto& item : settings.schema().get_child( "data" ) )
		{
			if ( item.second.get<string>( "type" ) == "bool" )
			{
				auto* checkbox = new QListWidgetItem( item.second.get< string >( "description" ).c_str() );
				checkbox->setCheckState( settings.get< bool >( "data." + item.first ) ? Qt::Checked : Qt::Unchecked );
				ui.dataList->addItem( checkbox );
				data_checkboxes[ item.first ] = checkbox;
			}
		}

		// advanced settings
		auto settings_pn = settings.data();
		auto* advancedModel = new QPropNodeItemModel( settings_pn );
		ui.advancedTreeView->setModel( advancedModel );
		ui.advancedTreeView->expandAll();

		int ret = dialog_window.exec();
		if ( ret == QDialog::Accepted )
		{
			settings.set( settings_pn );

			// update settings
			settings.set( "folders.scenarios", ui.scenariosFolder->text().toStdString() );
			settings.set( "folders.results", ui.resultsFolder->text().toStdString() );
			settings.set( "folders.geometry", ui.geometryFolder->text().toStdString() );

			// copy checkboxes
			for ( auto& item : data_checkboxes )
				settings.set< bool >( "data." + item.first, item.second->checkState() == Qt::Checked );

			SaveSconeSettings();
		}

		return ret;
	}
}
