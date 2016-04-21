/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
** Copyright (C) 2016 Tobias Gläßer
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

#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>

#include "Folder_Sharing_Widget.h"
#include "Add_New_Device_Window.h"
#include "Utils.h"
#include "Create_HDD_Image_Window.h"
#include "System_Info.h"

Folder_Sharing_Widget::Folder_Sharing_Widget( QWidget *parent )
	: QWidget( parent )
{
	ui.setupUi( this );
	
	Enabled = true;
	
	//pw = new Properties_Window( this );
	Context_Menu = new QMenu( ui.Folders_List );
	
	ui.Folders_List->setSpacing( 3 );
	ui.Folders_List->setFlow( QListView::TopToBottom );
	ui.Folders_List->setViewMode( QListView::ListMode );
}

Folder_Sharing_Widget::~Folder_Sharing_Widget()
{
	//if( pw != NULL ) delete pw;
	if( Context_Menu != NULL ) delete Context_Menu;
}

void Folder_Sharing_Widget::Set_VM( const Virtual_Machine &vm )
{
	ui.Folders_List->clear();
	ui.Label_Connected_To->setText( "" );
	
	if( vm.Get_Current_Emulator_Devices()->PSO_Drive ) ui.TB_Add_Folder->setEnabled( true ); 
	
	//Shared_Folders.clear();
	
    /*
	for( int ix = 0; ix < vm.Get_Storage_Folders_List().count(); ++ix )
	{
		Shared_Folders << vm.Get_Storage_Folders_List()[ix];
	} //TODO
    */
	
	Update_Icons();
	Update_Enabled_Actions();
	
	Current_Machine_Devices = vm.Get_Current_Emulator_Devices();
}

void Folder_Sharing_Widget::Set_Enabled( bool on )
{
	Enabled = on;
	
	ui.Label_Add_Folders->setEnabled( on );
	ui.TB_Add_Folder->setEnabled( on );
	
	ui.Label_Manage_Folders->setEnabled( on );
	ui.TB_Edit_Folder->setEnabled( on );
	ui.TB_Remove_Folder->setEnabled( on );
	
	ui.Label_View_Mode->setEnabled( on );
	ui.TB_Icon_Mode->setEnabled( on );
	ui.TB_List_Mode->setEnabled( on );
	
	ui.Label_Folders_List->setEnabled( on );
	ui.Label_Information->setEnabled( on );
	ui.Label_Connected_To->setEnabled( on );
}

void Folder_Sharing_Widget::Update_Enabled_Actions()
{
	// Adds
	
	ui.actionAdd_Folder->setEnabled( true );
	ui.TB_Add_Folder->setEnabled( true );
	
	// Update Information
	if( ui.Folders_List->currentItem() != NULL )
	{
		// item type
		if( ui.Folders_List->currentItem()->data(512).toString() == "fd1" ||
			ui.Folders_List->currentItem()->data(512).toString() == "fd2" )
		{
			ui.TB_Edit_Folder->setEnabled( true );
			ui.actionProperties->setEnabled( true );
			
			ui.TB_Remove_Folder->setEnabled( true );
			ui.actionRemove->setEnabled( true );
			
		{
			bool found = false;
			
			for( int fx = 0; fx < 32; ++fx )
			{
				if( ui.Folders_List->currentItem()->data(512).toString() == "folder" + QString::number(fx) )
				{
					found = true;
					
					ui.Label_Connected_To->setText( tr("Type: Shared Folder") );
					
					ui.TB_Edit_Folder->setEnabled( true );
					ui.actionProperties->setEnabled( true );
					
					ui.TB_Remove_Folder->setEnabled( true );
					ui.actionRemove->setEnabled( true );
					
				}
			}
			
			if( ! found )
			{
				ui.TB_Edit_Folder->setEnabled( false );
				ui.actionProperties->setEnabled( false);
				
				ui.TB_Remove_Folder->setEnabled( false );
				ui.actionRemove->setEnabled( false );
				
			}
		}
        }
	}
	else
	{
		ui.TB_Edit_Folder->setEnabled( false );
		ui.actionProperties->setEnabled( false);
			
		ui.TB_Remove_Folder->setEnabled( false );
		ui.actionRemove->setEnabled( false );
			
	}
	
	// Disable widgets
	if( ! Enabled )
	{
		ui.actionAdd_Folder->setEnabled( false );
		ui.TB_Add_Folder->setEnabled( false );
		
		ui.TB_Remove_Folder->setEnabled( false );
		ui.actionRemove->setEnabled( false );
	}
}

void Folder_Sharing_Widget::Update_List_Mode()
{
	if( ui.Folders_List->viewMode() == QListView::IconMode )
	{
		ui.Folders_List->setSpacing( 10 );
		ui.Folders_List->setFlow( QListView::LeftToRight );
		ui.Folders_List->setViewMode( QListView::IconMode );
	}
	else
	{
		ui.Folders_List->setSpacing( 3 );
		ui.Folders_List->setFlow( QListView::TopToBottom );
		ui.Folders_List->setViewMode( QListView::ListMode );
	}
}

void Folder_Sharing_Widget::on_Folders_List_customContextMenuRequested( const QPoint &pos )
{
	QListWidgetItem *it = ui.Folders_List->itemAt( pos );
	
	if( it != NULL )
	{
		{
			bool found = false;
			
			for( int fx = 0; fx < 32; ++fx )
			{
				if( ui.Folders_List->currentItem()->data(512).toString() == "folder" + QString::number(fx) )
				{
					found = true;
					
					Context_Menu = new QMenu( ui.Folders_List );
					
					Context_Menu->addAction( ui.actionProperties );
					Context_Menu->addAction( ui.actionRemove );
					
					Context_Menu->exec( ui.Folders_List->mapToGlobal(pos) );
				}
			}
			
			if( ! found )
			{
				AQError( "void Folder_Sharing_Widget::on_Folders_List_customContextMenuRequested( const QPoint &pos )",
						 "Incorrect Device!" );
			}
		}
	}
	else
	{
		Context_Menu = new QMenu( ui.Folders_List );
		
		Context_Menu->addAction( ui.actionAdd_Folder );
		Context_Menu->addSeparator();
		Context_Menu->addAction( ui.actionIcon_Mode );
		Context_Menu->addAction( ui.actionList_Mode );
		
		Context_Menu->exec( ui.Folders_List->mapToGlobal(pos) );
	}
}

void Folder_Sharing_Widget::on_Folders_List_currentItemChanged(
				QListWidgetItem *current, QListWidgetItem *previous )
{
	Update_Enabled_Actions();
}

void Folder_Sharing_Widget::on_Folders_List_itemDoubleClicked( QListWidgetItem *item )
{
	on_actionProperties_triggered();
}

void Folder_Sharing_Widget::on_actionAdd_Folder_triggered()
{
    /*
	Device_Window = new Add_New_Device_Window();
	VM_Nativ_Storage_Device tmp_dev;
	Device_Window->Set_Emulator_Devices( *Current_Machine_Devices );
	Device_Window->Set_Device( tmp_dev );
	
	if( Device_Window->exec() == QDialog::Accepted )
	{
		Shared_Folders << Device_Window->Get_Device();
		
		QListWidgetItem *devit = new QListWidgetItem( QIcon(":blockdevice.png"),
													  Device_Window->Get_Device().Get_QEMU_Device_Name(), ui.Folders_List );
		devit->setData( 512, "device" + QString::number(Shared_Folders.count() - 1) );
		
		ui.Folders_List->addItem( devit );
		
		emit Folder_Changed();
	}
    */
}

void Folder_Sharing_Widget::on_actionProperties_triggered()
{
 /*
	if( ui.Folders_List->currentItem()->data(512).toString() == "fd1" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_Floppy( Floppy1, tr("Floppy 1") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( Floppy1 != pw->Get_Floppy() )
			{
				Floppy1 = pw->Get_Floppy();
				
				ui.Folders_List->currentItem()->setText( tr("Floppy 1") + " (" + Floppy1.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else if( ui.Folders_List->currentItem()->data(512).toString() == "fd2" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_Floppy( Floppy2, tr("Floppy 2") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( Floppy2 != pw->Get_Floppy() )
			{
				Floppy2 = pw->Get_Floppy();
				
				ui.Folders_List->currentItem()->setText( tr("Floppy 2") + " (" + Floppy2.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else if( ui.Folders_List->currentItem()->data(512).toString() == "cd" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_CD_ROM( CD_ROM, tr("CD/DVD-ROM") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( CD_ROM != pw->Get_CD_ROM() )
			{
				CD_ROM = pw->Get_CD_ROM();
				
				ui.Folders_List->currentItem()->setText( tr("CD-ROM") + " (" + CD_ROM.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else if( ui.Folders_List->currentItem()->data(512).toString() == "hda" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDA, tr("HDA (First Master)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDA != pw->Get_HDD() )
			{
				HDA = pw->Get_HDD();
				
				ui.Folders_List->currentItem()->setText( tr("HDA") + " (" + HDA.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else if( ui.Folders_List->currentItem()->data(512).toString() == "hdb" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDB, tr("HDB (First Slave)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDB != pw->Get_HDD() )
			{
				HDB = pw->Get_HDD();
				
				ui.Folders_List->currentItem()->setText( tr("HDB") + " (" + HDB.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else if( ui.Folders_List->currentItem()->data(512).toString() == "hdc" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDC, tr("HDC (Second Master)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDC != pw->Get_HDD() )
			{
				HDC = pw->Get_HDD();
				
				ui.Folders_List->currentItem()->setText( tr("HDC") + " (" + HDC.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else if( ui.Folders_List->currentItem()->data(512).toString() == "hdd" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDD, tr("HDD (Second Slave)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDD != pw->Get_HDD() )
			{
				HDD = pw->Get_HDD();
				
				ui.Folders_List->currentItem()->setText( tr("HDD") + " (" + HDC.Get_File_Name() + ")" );
				
				emit Folder_Changed();
			}
		}
	}
	else
	{
		bool found = false;
		
		for( int fx = 0; fx < 32; ++fx )
		{
			if( ui.Folders_List->currentItem()->data(512).toString() == "device" + QString::number(fx) )
			{
				found = true;
				
				Device_Window = new Add_New_Device_Window();
				Device_Window->Set_Enabled( Enabled );
				Device_Window->Set_Device( Shared_Folders[fx] );
				
				if( ! Current_Machine_Devices )
					AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
							 "Current_Machine_Devices == NULL" );
				Device_Window->Set_Emulator_Devices( *Current_Machine_Devices );
				
				if( Device_Window->exec() == QDialog::Accepted )
				{
					if( Shared_Folders[fx] != Device_Window->Get_Device() )
					{
						Shared_Folders[fx] = Device_Window->Get_Device();
						
						emit Folder_Changed();
					}
				}
			}
		}
		
		if( ! found )
		{
			AQError( "void Folder_Sharing_Widget::on_actionProperties_triggered()",
					 "Incorrect Device!" );
			return;
		}
	}
	
	Update_Enabled_Actions();
 */
}

void Folder_Sharing_Widget::on_actionRemove_triggered()
{
	int mes_ret = QMessageBox::question( this, tr("Remove?"),
			tr("Remove Folder?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( mes_ret == QMessageBox::No ) return;
	
	/* //TODO
        bool found = false;
	
	for( int fx = 0; fx < 32; ++fx )
	{
		if( ui.Folders_List->currentItem()->data(512).toString() == "device" + QString::number(fx) )
		{
			found = true;
			
			Shared_Folders.removeAt( fx );
		}
	}
	
	if( ! found )
	{
		AQError( "void Folder_Sharing_Widget::on_actionDelete_triggered()",
				 "Incorrect Device!" );
		return;
	}*/
	
	ui.Folders_List->takeItem( ui.Folders_List->currentRow() );
	
	emit Folder_Changed();
}

void Folder_Sharing_Widget::on_actionIcon_Mode_triggered()
{
	ui.Folders_List->setSpacing( 10 );
	ui.Folders_List->setFlow( QListView::LeftToRight );
	ui.Folders_List->setViewMode( QListView::IconMode );
}

void Folder_Sharing_Widget::on_actionList_Mode_triggered()
{
	ui.Folders_List->setSpacing( 3 );
	ui.Folders_List->setFlow( QListView::TopToBottom );
	ui.Folders_List->setViewMode( QListView::ListMode );
}

void Folder_Sharing_Widget::Update_Icons()
{
	ui.Folders_List->clear();
	
	// Storage Devices
	if( Shared_Folders.count() )
	{
		for( int ix = 0; ix < Shared_Folders.count(); ++ix )
		{
			QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/folder-open.png"),
														 Shared_Folders[ix].Get_Folder(), ui.Folders_List );
			hdit->setData( 512, "folder" + QString::number(ix) );
			
			ui.Folders_List->addItem( hdit );
		}
	}
}

