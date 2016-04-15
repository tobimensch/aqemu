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

#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

#include "VM.h"
#include "Utils.h"
#include "Settings_Window.h"

Create_Template_Window::Create_Template_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	ui.Edit_Template_Folder->setText( QDir::toNativeSeparators(Settings.value("VM_Directory","").toString() + "/os_templates/") );
}

void Create_Template_Window::Set_VM_Path( const QString &path )
{
	ui.Edit_VM_File->setText( path );
}

void Create_Template_Window::on_TB_VM_File_Browse_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open VM File"),
													 QDir::toNativeSeparators(Settings.value("VM_Directory","").toString() + "/os_templates/"),
													 tr("AQEMU VM Files (*.aqemu)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_VM_File->setText( QDir::toNativeSeparators(fileName) );
}

void Create_Template_Window::on_TB_Template_Folder_Browse_clicked()
{
	QString fileName = QFileDialog::getExistingDirectory( this,tr("Select Template Folder"),
														  Get_Last_Dir_Path(ui.Edit_Template_Folder->text()),
														  QFileDialog::ShowDirsOnly );
	
	if( ! fileName.isEmpty() )
		ui.Edit_Template_Folder->setText( QDir::toNativeSeparators(fileName) );
}

void Create_Template_Window::on_Button_Create_clicked()
{
	// Check Values
	if( ui.Edit_Template_Name->text().isEmpty() )
	{
		AQGraphic_Warning( tr("Warning!"),
						   tr("Template Name is Empty!") );
		return;
	}
	
	if( ! Name_is_Unique() )
	{
		AQWarning( "Create_Template_Window::on_Button_Create_clicked()",
				   "Template Name Not Unique!" );
		return;
	}
	
	if( ! QFile::exists(ui.Edit_VM_File->text()) )
	{
		AQGraphic_Warning( tr("Warning!"),
						   tr("VM file doesn't exist!") );
		return;
	}
	
	if( ! QFile::exists(ui.Edit_Template_Folder->text()) )
	{
		AQGraphic_Warning( tr("Warning!"),
						   tr("Cannot Locate Template Folder!") );
		return;
	}
	
	if( ui.Edit_Template_Folder->text().at(ui.Edit_Template_Folder->text().count()-1) != '/' )
	{
		ui.Edit_Template_Folder->setText( QDir::toNativeSeparators(ui.Edit_Template_Folder->text() + "/") );
	}
	
	// All OK. Creating new template
	Virtual_Machine *tmp_vm = new Virtual_Machine();
	
	if( ! tmp_vm->Load_VM(ui.Edit_VM_File->text()) )
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("Cannot Load VM!") );
		delete tmp_vm;
		return;
	}
	
	Template_Options tmp_options = Template_Save_Default;
	
	if( ui.CH_FDD_CD->isChecked() )
	{
		tmp_options = tmp_options | Template_Save_FDD_CD;
	}
	
	if( ui.CH_HDD->isChecked() )
	{
		tmp_options = tmp_options | Template_Save_HDD;
	}
	
	if( ui.CH_Network->isChecked() )
	{
		tmp_options = tmp_options | Template_Save_Network;
	}
	
	if( ui.CH_Ports->isChecked() )
	{
		tmp_options = tmp_options | Template_Save_Ports;
	}
	
	// Create FS Valid File Name
	QString new_template_name = ui.Edit_Template_Name->text();
	
	QRegExp VM_Name_Val = QRegExp( "[^a-zA-Z0-9_]" );
	new_template_name = new_template_name.replace( VM_Name_Val, "_" );
	new_template_name = new_template_name.replace( "__", "_" );
	
	if( ! tmp_vm->Create_Template(ui.Edit_Template_Folder->text() +
		new_template_name + ".aqvmt",
		ui.Edit_Template_Name->text(), tmp_options ) )
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("Cannot Create Template!") );
		delete tmp_vm;
		return;
	}
	
	delete tmp_vm;
	
	accept();
}

bool Create_Template_Window::Name_is_Unique()
{
	if( ui.CH_Use_Default_Folder->isChecked() ||
		ui.Edit_Template_Folder->text() == QDir::toNativeSeparators(Settings.value("VM_Directory", "").toString() + "/os_templates/") )
	{
		List_Templates = Get_Templates_List();
		
		for( int ix = 0; ix < List_Templates.count(); ++ix )
		{
			QFileInfo tmp_info = QFileInfo( List_Templates[ix] );
			
			// Create FS Valid File Name
			QString new_template_name = ui.Edit_Template_Name->text();
	
			QRegExp VM_Name_Val = QRegExp( "[^a-zA-Z0-9_]" );
			new_template_name = new_template_name.replace( VM_Name_Val, "_" );
			new_template_name = new_template_name.replace( "__", "_" );
			
			if( tmp_info.completeBaseName() == new_template_name )
			{
				int mes_ret = QMessageBox::question( this, tr("Replace"),
						tr("Your template name is already used! Do you want replace previous template?"),
						QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
				
				if( mes_ret == QMessageBox::Yes )
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
		
		return true;
	}
	else
	{
		// Other Folder
		if( QFile::exists(ui.Edit_Template_Folder->text()) )
		{
			int mes_ret = QMessageBox::question( this, tr("Replace"),
					tr("You Name For Template in Not Unique! Replace Previous Template?"),
					QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
				
			if( mes_ret == QMessageBox::Yes )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}
}
