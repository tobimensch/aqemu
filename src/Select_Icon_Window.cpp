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
#include <QFileInfoList>
#include <QDir>

#include "Utils.h"
#include "Select_Icon_Window.h"

Select_Icon_Window::Select_Icon_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	// load all icons
	QDir *icons_dir = new QDir( QDir::toNativeSeparators(Settings.value("AQEMU_Data_Folder", "").toString() + "/os_icons/") );
	QFileInfoList icon_files = icons_dir->entryInfoList( QDir::Files, QDir::Name | QDir::IgnoreCase );
	
	for( int ix = 0; ix < icon_files.count(); ++ix )
	{
		QListWidgetItem *ic = new QListWidgetItem( QIcon(icon_files[ix].absoluteFilePath()), icon_files[ix].baseName(), ui.All_Icons_List );
		ic->setData( 128, icon_files[ix].absoluteFilePath() );
	}
}

void Select_Icon_Window::Set_Previous_Icon_Path( const QString& path )
{
	// Analise path...
	if( path[0] == ':' ) // AQEMU Default Icons
	{
		if( path.contains("linux", Qt::CaseInsensitive) ) ui.RB_Icon_Linux->setChecked( true );
		else if( path.contains("windows", Qt::CaseInsensitive) ) ui.RB_Icon_Windows->setChecked( true );
		else ui.RB_Icon_Other->setChecked( true );
	}
	else if( path.indexOf(QDir::toNativeSeparators(Settings.value("AQEMU_Data_Folder", "").toString() + "/os_icons/")) == 0 ) // AQEMU Icons Folder
	{
		ui.RB_All_System_Icons->setChecked( true );
		
		QFileInfo fl = QFileInfo( path );
		
		for( int ix = 0; ix < ui.All_Icons_List->count(); ++ix )
		{
			if( ui.All_Icons_List->item(ix)->text() == fl.baseName() )
			{
				ui.All_Icons_List->setCurrentRow( ix );
				
				// This hack :)
				ui.All_Icons_List->setViewMode( QListView::ListMode );
				ui.All_Icons_List->setViewMode( QListView::IconMode );
				return;
			}
		}
	}
	else // Other Path
	{
		ui.RB_User_Icons->setChecked( true );
		ui.Edit_Other_Icon_Path->setText( path );
	}
}

QString Select_Icon_Window::Get_New_Icon_Path() const
{
	return New_Icon_Path;
}

void Select_Icon_Window::on_Button_OK_clicked()
{
	// Check values
	if( ui.RB_Icon_Other->isChecked() )
	{
		New_Icon_Path = ":/images/other.png";
		accept();
	}
	else if( ui.RB_Icon_Windows->isChecked() )
	{
		New_Icon_Path = ":/images/default_windows.png";
		accept();
	}
	else if( ui.RB_Icon_Linux->isChecked() )
	{
		New_Icon_Path = ":/images/default_linux.png";
		accept();
	}
	else if( ui.RB_All_System_Icons->isChecked() )
	{
		if( ui.All_Icons_List->currentItem() != NULL )
		{
			New_Icon_Path = ui.All_Icons_List->currentItem()->data( 128 ).toString();
			accept();
		}
	}
	else if( ui.RB_User_Icons->isChecked() )
	{
		if( QFile::exists(ui.Edit_Other_Icon_Path->text()) )
		{
			New_Icon_Path = ui.Edit_Other_Icon_Path->text();
			accept();
		}
		else
		{
			AQGraphic_Warning( tr("Error!"), tr("Icon file doesn't exist!") );
		}
	}
}

void Select_Icon_Window::on_Button_Browse_clicked()
{
	QString iconPath = QFileDialog::getOpenFileName( this, tr("Select Icon File:"),
													 Get_Last_Dir_Path(ui.Edit_Other_Icon_Path->text()),
													 tr("PNG Images (*.png)") );
	
	if( iconPath.isEmpty() ) return;
	iconPath = QDir::toNativeSeparators( iconPath );

	if( ! QFile::exists(iconPath) )
	{
		AQError( "void Select_Icon_Window::on_Button_Browse_clicked()",
				 "File No Exists!" );
	}
	else
	{
		ui.Edit_Other_Icon_Path->setText( iconPath );
	}
}

void Select_Icon_Window::on_All_Icons_List_itemDoubleClicked( QListWidgetItem *item )
{
	on_Button_OK_clicked();
}
