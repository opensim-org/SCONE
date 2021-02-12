/*
** SettingsEditor.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SettingsEditor.h"

#include <QDialog>
#include <QMessageBox>
#include "qt_convert.h"
#include "QPropNodeItemModel.h"

#include "ui_SconeSettings.h"
#include "scone/core/PropNode.h"
#include "scone/core/system_tools.h"
#include "scone/core/Settings.h"
#include "scone/core/Log.h"
#include "xo/filesystem/path.h"
#include "xo/system/type_class.h"
#include "xo/container/flat_map.h"
#include "StudioSettings.h"
#include "QSettingsItemModel.h"
#include "scone/sconelib_config.h"
#include "ui_LicenseDialog.h"

namespace scone
{
	int SettingsEditor::showDialog( QWidget* parent )
	{
		QDialog dialog_window( parent );
		Ui::Settings ui;
		ui.setupUi( &dialog_window );

		// init SCONE settings
		auto& scone_settings = GetSconeSettings();

		// folders
		ui.scenariosFolder->init( QFileEdit::Directory, "", to_qt( GetFolder( SCONE_SCENARIO_FOLDER ).make_preferred() ) );
		ui.resultsFolder->init( QFileEdit::Directory, "", to_qt( GetFolder( SCONE_RESULTS_FOLDER ).make_preferred() ) );
		ui.geometryFolder->init( QFileEdit::Directory, "", to_qt( GetFolder( SCONE_GEOMETRY_FOLDER ).make_preferred() ) );
		ui.gaitAnalysisFolder->init( QFileEdit::OpenFile, "Gait Analysis Templates (*.zml)", to_qt( GetStudioSetting<xo::path>( "gait_analysis.template" ).make_preferred() ) );

		// data checkboxes
		xo::flat_map< string, QListWidgetItem* > data_checkboxes;
		for ( auto& item : scone_settings.schema().get_child( "data" ) )
		{
			if ( item.second.get<string>( "type" ) != "bool" )
				continue;
			if ( !GetExperimentalFeaturesEnabled() && item.second.get<bool>( "experimental", 0 ) )
				continue;

			auto* checkbox = new QListWidgetItem( item.second.get<string>( "label" ).c_str() );
			checkbox->setCheckState( scone_settings.get< bool >( "data." + item.first ) ? Qt::Checked : Qt::Unchecked );
			ui.dataList->addItem( checkbox );
			data_checkboxes[ item.first ] = checkbox;
		}

		// advanced settings
		auto* advancedModel = new QSettingsItemModel( scone_settings );
		ui.advancedTree->setModel( advancedModel );
		ui.advancedTree->expandAll();

		// init STUDIO settings
		auto& studio_settings = GetStudioSettings();
		auto* studioModel = new QSettingsItemModel( studio_settings );
		ui.studioTree->setModel( studioModel );
		ui.studioTree->expandAll();

#ifdef SCONE_HYFYDY
		bool hfd_enabled = scone_settings.get<bool>( "hyfydy.enabled" );
		auto hfd_license = scone_settings.get<String>( "hyfydy.license" );
		ui.hfdEnabled->setCheckState( hfd_enabled ? Qt::Checked : Qt::Unchecked );
		ui.hfdLicenseKey->setPlainText( to_qt( hfd_license ) );
		QObject::connect( ui.hfdRequest, &QPushButton::clicked, []() {
			auto hid = to_qt( GetHardwareId() );
			QApplication::clipboard()->setText( hid );
			auto message = "Please send the following hardware ID along with your request (copied to clipboard):\n\n" + hid;
			QMessageBox::information( NULL, "Hyfydy License Request", message ); });
#else
		ui.tabWidget->removeTab( ui.tabWidget->indexOf( ui.hfdTab ) );
#endif

		for ( int i = 0; i < 3; i++ )
		{
			ui.advancedTree->resizeColumnToContents( i );
			ui.studioTree->resizeColumnToContents( i );
		}

		int ret = dialog_window.exec();
		if ( ret == QDialog::Accepted )
		{
			// update settings
			scone_settings.set( "folders.scenarios", ui.scenariosFolder->text().toStdString() );
			scone_settings.set( "folders.results", ui.resultsFolder->text().toStdString() );
			scone_settings.set( "folders.geometry", ui.geometryFolder->text().toStdString() );
			studio_settings.set( "gait_analysis.template", ui.gaitAnalysisFolder->text().toStdString() );

			// copy checkboxes
			for ( auto& item : data_checkboxes )
				scone_settings.set< bool >( "data." + item.first, item.second->checkState() == Qt::Checked );

#ifdef SCONE_HYFYDY
			auto hfd_new_license = ui.hfdLicenseKey->toPlainText().toStdString();
			auto hfd_new_enabled = ui.hfdEnabled->isChecked();
			if ( hfd_new_license != hfd_license || hfd_enabled != hfd_new_enabled )
			{
				scone_settings.set( "hyfydy.license", hfd_new_license );
				scone_settings.set( "hyfydy.enabled", hfd_new_enabled );
				if ( hfd_new_enabled )
				{
					const char* license_key = GetSconeSetting<String>( "hyfydy.license" ).c_str();
					auto agreement = GetHfdLicenseAgreement( license_key );
					if ( agreement.isValid() )
					{
						QDialog lic_dlg( parent );
						Ui::LicenseDialog ui;
						ui.setupUi( &lic_dlg );
						ui.textBrowser->setText( to_qt( agreement.licenseAgreement ) );
						lic_dlg.setWindowTitle( to_qt( agreement.licenseType + " Agreement" ) );
						ui.checkBox->setText( to_qt( "I agree to the terms and conditions of the " + agreement.licenseType ) );
						auto* okButton = ui.buttonBox->button( QDialogButtonBox::Ok );
						okButton->setDisabled( true );
						QWidget::connect( ui.checkBox, &QCheckBox::stateChanged,
							[&]( int i ) { okButton->setDisabled( i != Qt::Checked ); } );
						const auto result = lic_dlg.exec();
						if ( result == QDialog::Accepted && ui.checkBox->isChecked() )
						{
							scone_settings.set( "hyfydy.license_agreement_accepted_version", agreement.licenseVersion );
							RegisterSconeHfd( GetSconeSetting<String>( "hyfydy.license" ).c_str() );
						}
						else
						{
							scone_settings.set( "hyfydy.enabled", false );
							scone_settings.set( "hyfydy.license_agreement_accepted_version", agreement.licenseVersion );
						}
					}
				}
			}
#endif
			scone_settings.save();
			studio_settings.save();
		}
		else
		{
			// cancel was pressed, reload old settings
			scone_settings.load();
			studio_settings.load();
		}

		return ret;
	}
}
