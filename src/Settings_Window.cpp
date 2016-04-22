/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
**
** This file is part of AQEMU.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA  02110-1301, USA.
**
****************************************************************************/

#include <QDir>
#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

#include "Utils.h"
#include "Settings_Window.h"
#include "Create_Template_Window.h"
#include "Emulator_Options_Window.h"

Settings_Window::Settings_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	// Minimum Size
	resize( width(), minimumSizeHint().height() );
	
	// Load Settings
	Settings = new QSettings();
	
	// Emulator Control
	bool emul_show = Settings->value( "Show_Emulator_Control_Window", "yes" ).toString() == "yes";
	bool emul_vnc = Settings->value( "Use_VNC_Display", "no" ).toString() == "yes";
	bool emul_include = Settings->value( "Include_Emulator_Control", "yes" ).toString() == "yes";
	
	if( emul_show == true && emul_vnc == false && emul_include == false )
	{
		ui.RB_Emul_Show_Window->setChecked( true );
	}
	else if( emul_show == true && emul_vnc == true && emul_include == false )
	{
		ui.RB_Emul_Show_VNC->setChecked( true );
	}
	else if( emul_show == true && emul_vnc == false && emul_include == true )
	{
		ui.RB_Emul_Show_In_Main_Window->setChecked( true );
	}
	else if( emul_show == true && emul_vnc == true && emul_include == true )
	{
		ui.RB_Emul_Show_VNC_In_Main_Window->setChecked( true );
	}
	else if( emul_show == false && emul_vnc == false && emul_include == false )
	{
		ui.RB_Emul_No_Show->setChecked( true );
	}
	
#ifndef VNC_DISPLAY
	ui.RB_Emul_Show_VNC->setEnabled( false );
	ui.RB_Emul_Show_VNC_In_Main_Window->setEnabled( false );
	
	if( ui.RB_Emul_Show_VNC->isChecked() ) ui.RB_Emul_Show_Window->setChecked( true );
	else if( ui.RB_Emul_Show_VNC_In_Main_Window->isChecked() ) ui.RB_Emul_Show_In_Main_Window->setChecked( true );
#endif
	
	// Virtual Machines Folder
	ui.Edit_VM_Folder->setText( QDir::toNativeSeparators(Settings->value("VM_Directory", QDir::homePath() + "/.aqemu/").toString()) );
	
	// Use Device Manager
	ui.CH_Use_Device_Manager->setChecked( Settings->value("Use_Device_Manager", "no").toString() == "yes" );
	
	// Find All Language Files (*.qm)
	QDir data_dir( Settings->value("AQEMU_Data_Folder", "/usr/share/aqemu/").toString() );
	QFileInfoList lang_files = data_dir.entryInfoList( QStringList("*.qm"), QDir::Files, QDir::Name );
	
	if( lang_files.count() > 0 )
	{
		// Add Languages to List
		for( int dd = 0; dd < lang_files.count(); ++dd )
		{
			ui.CB_Language->addItem( lang_files[dd].completeBaseName() );
			
			if( lang_files[dd].completeBaseName() == Settings->value( "Language", "en" ).toString() )
			{
				ui.CB_Language->setCurrentIndex( dd + 1 ); // First Item 'English'
			}
		}
	}
	
	// Icons Theme
	if( Settings->value("Icon_Theme", "").toString() != "crystalsvg" )
	{
		ui.CB_Icons_Theme->setCurrentIndex( 1 );
	}
	
	// VM Icons Size
	switch( Settings->value("VM_Icons_Size", "48").toInt() )
	{
		case 16:
			ui.CB_VM_Icons_Size->setCurrentIndex( 0 );
			break;
			
		case 24:
			ui.CB_VM_Icons_Size->setCurrentIndex( 1 );
			break;
		
		case 32:
			ui.CB_VM_Icons_Size->setCurrentIndex( 2 );
			break;
			
		case 48:
			ui.CB_VM_Icons_Size->setCurrentIndex( 3 );
			break;
			
		case 64:
			ui.CB_VM_Icons_Size->setCurrentIndex( 4 );
			break;
			
		default:
			ui.CB_VM_Icons_Size->setCurrentIndex( 3 );
			break;
	}
	
	// Screenshot for OS Logo
	ui.CH_Screenshot_for_OS_Logo->setChecked( Settings->value("Use_Screenshot_for_OS_Logo", "yes").toString() == "yes" );
	
	Load_Templates();
	
	connect( ui.RB_Emul_Show_VNC, SIGNAL(toggled(bool)),
			 this, SLOT(VNC_Warning(bool)) );
	
	connect( ui.RB_Emul_Show_VNC_In_Main_Window, SIGNAL(toggled(bool)),
			 this, SLOT(VNC_Warning(bool)) );
	
	connect( ui.CB_Language, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(CB_Language_currentIndexChanged(int)) );
	
	connect( ui.CB_Icons_Theme, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(CB_Icons_Theme_currentIndexChanged(int)) );
}

Settings_Window::~Settings_Window()
{
	if( Settings != NULL ) delete Settings;
}

void Settings_Window::Load_Templates()
{
	QList<QString> all_templates = Get_Templates_List();
	
	ui.CB_Default_VM_Template->clear();
	
	for( int ix = 0; ix < all_templates.count(); ++ix )
	{
		QFileInfo tmp_info = QFileInfo( all_templates[ix] );
		ui.CB_Default_VM_Template->addItem( tmp_info.completeBaseName() );
	}
	
	// no items found
	if( ui.CB_Default_VM_Template->count() < 1 )
	{
		AQGraphic_Warning( tr("Warning"), tr("AQEMU VM Templates Not Found!") );
	}
	else
	{
		// Find default template
		for( int ix = 0; ix < ui.CB_Default_VM_Template->count(); ++ix )
		{
			if( ui.CB_Default_VM_Template->itemText(ix) == Settings->value("Default_VM_Template", "Linux 2.6").toString() )
			{
				ui.CB_Default_VM_Template->setCurrentIndex( ix );
			}
		}
	}
}

void Settings_Window::on_Button_Create_Template_from_VM_clicked()
{
	Create_Template_Window *templ_win = new Create_Template_Window();
	
	if( templ_win->exec() == QDialog::Accepted )
	{
		Load_Templates();
		QMessageBox::information( this, tr("Information"), tr("New template was created!") );
	}
	
	delete templ_win;
}

void Settings_Window::on_TB_VM_Folder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory( this, tr("Set your VM folder"),
														Get_Last_Dir_Path(ui.Edit_VM_Folder->text()) );
	
	if( ! folder.isEmpty() )
	{
		if( ! (folder.endsWith("/") || folder.endsWith("\\")) )
			folder += "/";
		
		ui.Edit_VM_Folder->setText( QDir::toNativeSeparators(folder) );
	}
}

void Settings_Window::CB_Language_currentIndexChanged( int index )
{
	if( Settings->value("Show_Language_Warning", "yes").toString() == "yes" )
	{
		if( QMessageBox::information(this, tr("Information"),
			tr("Language will set after restarting AQEMU\nShow this message in future?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::No )
		{
			Settings->setValue( "Show_Language_Warning", "no" );
		}
	}
}

void Settings_Window::CB_Icons_Theme_currentIndexChanged( int index )
{
	if( Settings->value("Show_Icons_Theme_Warning", "yes").toString() == "yes" )
	{
		if( QMessageBox::information(this, tr("Information"),
			tr("Icons theme be set after restart AQEMU\nShow this message in future?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::No )
		{
			Settings->setValue( "Show_Icons_Theme_Warning", "no" );
		}
	}
}

void Settings_Window::on_Button_OK_clicked()
{
	QDir dir; // For Check on valid
	Settings->setValue( "Default_VM_Template", ui.CB_Default_VM_Template->currentText() );
	ui.Edit_VM_Folder->setText( QDir::toNativeSeparators(ui.Edit_VM_Folder->text()) );

	// VM Folder
	if( ! (ui.Edit_VM_Folder->text().endsWith("/") || ui.Edit_VM_Folder->text().endsWith("\\")) )
		ui.Edit_VM_Folder->setText( ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/") );
	
	if( dir.exists(ui.Edit_VM_Folder->text()) )
	{
		if( ! dir.exists(ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/os_templates/")) )
			dir.mkdir( ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/os_templates/") );
		
		Settings->setValue( "VM_Directory", ui.Edit_VM_Folder->text() );
	}
	else
	{
		int mes_res = QMessageBox::question( this, tr("Invalid Value!"),
											 tr("AQEMU VM folder doesn't exist! Do you want to create it?"),
											 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
		if( mes_res == QMessageBox::Yes )
		{
			if( ! (dir.mkdir(ui.Edit_VM_Folder->text()) &&
				   dir.mkdir(ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/os_templates/"))) )
			{
				AQGraphic_Warning( tr("Error!"), tr("Cannot Create New Folder!") );
				return;
			}
			else Settings->setValue( "VM_Directory", ui.Edit_VM_Folder->text() );
		}
		else return;
	}
	
	// Use Device Manager
	if( ui.CH_Use_Device_Manager->isChecked() ) Settings->setValue( "Use_Device_Manager", "yes" );
	else Settings->setValue( "Use_Device_Manager", "no" );
	
	// Interface Language
	if( ui.CB_Language->currentIndex() == 0 ) Settings->setValue( "Language", "en" );
	else Settings->setValue( "Language", ui.CB_Language->itemText(ui.CB_Language->currentIndex()) );
	
	// Icons Theme
	switch( ui.CB_Icons_Theme->currentIndex() )
	{
		case 0:
			Settings->setValue( "Icon_Theme", "crystalsvg" );
			break;
			
		case 1:
			Settings->setValue( "Icon_Theme", "oxygen" );
			break;
			
		default:
			Settings->setValue( "Icon_Theme", "oxygen" );
			break;
	}
	
	// VM Icons Size
	switch( ui.CB_VM_Icons_Size->currentIndex() )
	{
		case 0:
			Settings->setValue( "VM_Icons_Size", 16 );
			break;
			
		case 1:
			Settings->setValue( "VM_Icons_Size", 24 );
			break;
		
		case 2:
			Settings->setValue( "VM_Icons_Size", 32 );
			break;
			
		case 3:
			Settings->setValue( "VM_Icons_Size", 48 );
			break;
			
		case 4:
			Settings->setValue( "VM_Icons_Size", 64 );
			break;
			
		default:
			Settings->setValue( "VM_Icons_Size", 48 );
			break;
	}
	/*
	// USB Support
	if( ui.CH_Use_USB->isChecked() ) Settings->setValue( "Use_USB", "yes" );
	else Settings->setValue( "Use_USB", "no" );
	*/
	// Screenshot for OS Logo
	if( ui.CH_Screenshot_for_OS_Logo->isChecked() ) Settings->setValue( "Use_Screenshot_for_OS_Logo", "yes" );
	else Settings->setValue( "Use_Screenshot_for_OS_Logo", "no" );
	/*
	// 64x64 Icons
	if( ui.CH_64_Icons->isChecked() ) Settings->setValue( "64x64_Icons", "yes" );
	else Settings->setValue( "64x64_Icons", "no" );*/
	
	Settings->sync();
	
	if( Settings->status() != QSettings::NoError )
	{
		AQError( "void Settings_Window::on_Button_Box_clicked( QAbstractButton* button )",
				 "QSettings Error!" );
	}
	
	// Emulator Control
	if( ui.RB_Emul_Show_Window->isChecked() )
	{
		Settings->setValue( "Show_Emulator_Control_Window", "yes" );
		Settings->setValue( "Use_VNC_Display", "no" );
		Settings->setValue( "Include_Emulator_Control", "no" );
	}
	else if( ui.RB_Emul_Show_VNC->isChecked() )
	{
		Settings->setValue( "Show_Emulator_Control_Window", "yes" );
		Settings->setValue( "Use_VNC_Display", "yes" );
		Settings->setValue( "Include_Emulator_Control", "no" );
	}
	else if( ui.RB_Emul_Show_In_Main_Window->isChecked() )
	{
		Settings->setValue( "Show_Emulator_Control_Window", "yes" );
		Settings->setValue( "Use_VNC_Display", "no" );
		Settings->setValue( "Include_Emulator_Control", "yes" );
	}
	else if( ui.RB_Emul_Show_VNC_In_Main_Window->isChecked() )
	{
		Settings->setValue( "Show_Emulator_Control_Window", "yes" );
		Settings->setValue( "Use_VNC_Display", "yes" );
		Settings->setValue( "Include_Emulator_Control", "yes" );
	}
	else if( ui.RB_Emul_No_Show->isChecked() )
	{
		Settings->setValue( "Show_Emulator_Control_Window", "no" );
		Settings->setValue( "Use_VNC_Display", "no" );
		Settings->setValue( "Include_Emulator_Control", "no" );
	}
	
	accept();
}

void Settings_Window::VNC_Warning( bool state )
{
	if( ! state ) return;
	
	if( Settings->value("Show_VNC_Warning", "yes").toString() == "yes" )
	{
		if( QMessageBox::information(this, tr("Information"),
			tr("Support for this feature is not complete! If there is no picture, click \"View->Reinit VNC\"\nShow This Message Again?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::No )
		{
			Settings->setValue( "Show_VNC_Warning", "no" );
		}
	}
}
