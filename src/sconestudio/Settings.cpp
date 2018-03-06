#include "Settings.h"

#include <QDialog>
#include "qt_tools.h"
#include "QPropNodeItemModel.h"

#include "ui_SconeSettings.h"
#include "scone/core/PropNode.h"
#include "scone/core/system_tools.h"
#include "xo/filesystem/path.h"
#include "xo/system/type_class.h"

namespace scone
{
	int Settings::showDialog( QWidget* parent )
	{
		QDialog* dialog_window = new QDialog( parent );
		Ui::Settings ui;
		ui.setupUi( dialog_window );

		// init settings
		auto& settings = UpdateSconeSettings();

		ui.scenariosFolder->setText( make_qt( xo::path( GetFolder( SCONE_SCENARIO_FOLDER ) ).make_preferred() ) );
		ui.resultsFolder->setText( make_qt( xo::path( GetFolder( SCONE_RESULTS_FOLDER ) ).make_preferred() ) );
		ui.geometryFolder->setText( make_qt( xo::path( GetFolder( SCONE_GEOMETRY_FOLDER ) ).make_preferred() ) );

		for ( auto& item : settings.data().get_child( "data" ) )
		{
			if ( item.second.get< xo::type_class >( "_type_" ) == xo::boolean_type_class )
			{
				auto* checkbox = new QListWidgetItem( item.second.get< string >( "_label_" ).c_str() );
				checkbox->setCheckState( item.second.get< bool >() ? Qt::Checked : Qt::Unchecked );
				checkbox->setStatusTip( item.second.get< string >( "_info_" ).c_str() );
				ui.dataList->addItem( checkbox );
			}
		}

		auto settings_pn = settings.extract_settings();
		auto* advancedModel = new QPropNodeItemModel( settings_pn );
		ui.advancedTreeView->setModel( advancedModel );
		ui.advancedTreeView->expandAll();

		int ret = dialog_window->exec();
		if ( ret == QDialog::Accepted )
		{
			settings.inject_settings( settings_pn );

			// update settings
			settings.set( "folders.scenarios", ui.scenariosFolder->text().toStdString() );
			settings.set( "folders.results", ui.resultsFolder->text().toStdString() );
			settings.set( "folders.geometry", ui.geometryFolder->text().toStdString() );

			// copy checkboxes
			int row = 0;
			for ( auto& item : settings.data().get_child( "data" ) )
			{
				if ( item.second.get< xo::type_class >( "_type_" ) == xo::boolean_type_class )
					item.second.set< bool >( ui.dataList->item( row++ )->checkState() == Qt::Checked );
			}

			SaveSconeSettings();
		}

		return ret;
	}
}
