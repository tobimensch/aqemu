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

#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>

#include "Device_Manager_Widget.h"
#include "Add_New_Device_Window.h"
#include "Utils.h"
#include "Create_HDD_Image_Window.h"
#include "System_Info.h"

Device_Manager_Widget::Device_Manager_Widget( QWidget *parent )
	: QWidget( parent )
{
	ui.setupUi( this );
	
	Enabled = true;
	
	HDA_Info = new HDD_Image_Info();
	HDB_Info = new HDD_Image_Info();
	HDC_Info = new HDD_Image_Info();
	HDD_Info = new HDD_Image_Info();
	
	connect( HDA_Info, SIGNAL(Completed(bool)),
			 this, SLOT(Update_HDA(bool)) );
	
	connect( HDB_Info, SIGNAL(Completed(bool)),
			 this, SLOT(Update_HDB(bool)) );
	
	connect( HDC_Info, SIGNAL(Completed(bool)),
			 this, SLOT(Update_HDC(bool)) );
	
	connect( HDD_Info, SIGNAL(Completed(bool)),
			 this, SLOT(Update_HDD(bool)) );
	
	pw = new Properties_Window( this );
	Context_Menu = new QMenu( ui.Devices_List );
	
	ui.Devices_List->setSpacing( 3 );
	ui.Devices_List->setFlow( QListView::TopToBottom );
	ui.Devices_List->setViewMode( QListView::ListMode );
}

Device_Manager_Widget::~Device_Manager_Widget()
{
	if( pw != NULL ) delete pw;
	if( Context_Menu != NULL ) delete Context_Menu;
}

void Device_Manager_Widget::Set_VM( const Virtual_Machine &vm )
{
	ui.Devices_List->clear();
	ui.Label_Connected_To->setText( "" );
	
	if( vm.Get_Current_Emulator_Devices()->PSO_Drive ) ui.TB_Add_Device->setEnabled( true ); 
	
	Floppy1 = VM_Storage_Device( vm.Get_FD0() );
	Floppy2 = VM_Storage_Device( vm.Get_FD1() );
	CD_ROM = VM_Storage_Device( vm.Get_CD_ROM() );
	
	HDA = VM_HDD( vm.Get_HDA() );
	HDB = VM_HDD( vm.Get_HDB() );
	HDC = VM_HDD( vm.Get_HDC() );
	HDD = VM_HDD( vm.Get_HDD() );
	
	if( QFile::exists(HDA.Get_File_Name()) )
		HDA_Info->Update_Disk_Info( HDA.Get_File_Name() );
	
	if( QFile::exists(HDB.Get_File_Name()) )
		HDB_Info->Update_Disk_Info( HDB.Get_File_Name() );
	
	if( QFile::exists(HDC.Get_File_Name()) )
		HDC_Info->Update_Disk_Info( HDC.Get_File_Name() );
	
	if( QFile::exists(HDD.Get_File_Name()) )
		HDD_Info->Update_Disk_Info( HDD.Get_File_Name() );
	
	Storage_Devices.clear();
	
	for( int ix = 0; ix < vm.Get_Storage_Devices_List().count(); ++ix )
	{
		Storage_Devices << vm.Get_Storage_Devices_List()[ix];
	}
	
	Update_Icons();
	Update_Enabled_Actions();
	
	Current_Machine_Devices = vm.Get_Current_Emulator_Devices();
}

void Device_Manager_Widget::Set_Enabled( bool on )
{
	Enabled = on;
	
	ui.Label_Add_Devices->setEnabled( on );
	ui.TB_Add_Floppy->setEnabled( on );
	ui.TB_Add_CDROM->setEnabled( on );
	ui.TB_Add_HDD->setEnabled( on );
	ui.TB_Add_Device->setEnabled( on );
	
	ui.Label_Manage_Devices->setEnabled( on );
	ui.TB_Edit_Device->setEnabled( on );
	ui.TB_Delete_Device->setEnabled( on );
	ui.TB_Format_HDD->setEnabled( on );
	ui.TB_Quick_Format->setEnabled( on );
	
	ui.Label_View_Mode->setEnabled( on );
	ui.TB_Icon_Mode->setEnabled( on );
	ui.TB_List_Mode->setEnabled( on );
	
	ui.Label_Devices_List->setEnabled( on );
	ui.Label_Information->setEnabled( on );
	ui.Label_Connected_To->setEnabled( on );
}

void Device_Manager_Widget::Update_Enabled_Actions()
{
	// Adds
	if( Floppy1.Get_Enabled() && Floppy2.Get_Enabled() )
	{
		ui.actionAdd_Floppy->setEnabled( false );
		ui.TB_Add_Floppy->setEnabled( false );
	}
	else
	{
		ui.actionAdd_Floppy->setEnabled( true );
		ui.TB_Add_Floppy->setEnabled( true );
	}
	
	if( CD_ROM.Get_Enabled() )
	{
		ui.actionAdd_CD_ROM->setEnabled( false );
		ui.TB_Add_CDROM->setEnabled( false );
	}
	else
	{
		ui.actionAdd_CD_ROM->setEnabled( true );
		ui.TB_Add_CDROM->setEnabled( true );
	}
	
	if( HDA.Get_Enabled() && HDB.Get_Enabled() &&
		HDC.Get_Enabled() &&  HDD.Get_Enabled() )
	{
		ui.actionAdd_HDD->setEnabled( false );
		ui.TB_Add_HDD->setEnabled( false );
	}
	else if( HDA.Get_Enabled() && HDB.Get_Enabled() &&
			 HDD.Get_Enabled() && CD_ROM.Get_Enabled() )
	{
		ui.actionAdd_HDD->setEnabled( false );
		ui.TB_Add_HDD->setEnabled( false );
	}
	else
	{
		ui.actionAdd_HDD->setEnabled( true );
		ui.TB_Add_HDD->setEnabled( true );
	}
	
	// Update Information
	if( ui.Devices_List->currentItem() != NULL )
	{
		// item type
		if( ui.Devices_List->currentItem()->data(512).toString() == "fd1" ||
			ui.Devices_List->currentItem()->data(512).toString() == "fd2" )
		{
			ui.TB_Edit_Device->setEnabled( true );
			ui.actionProperties->setEnabled( true );
			
			ui.TB_Delete_Device->setEnabled( true );
			ui.actionDelete->setEnabled( true );
			
			ui.TB_Format_HDD->setEnabled( false );
			ui.actionFormat_HDD->setEnabled( false );
			
			ui.TB_Quick_Format->setEnabled( false );
			ui.actionQuick_Format->setEnabled( false );
			
			if( ui.Devices_List->currentItem()->data(512).toString() == "fd1" )
			{
				if( It_Host_Device(Floppy1.Get_File_Name()) )
				{
					ui.Label_Connected_To->setText( tr("Type: Host Device") );
				}
				else
				{
					QFileInfo fd_img = QFileInfo( Floppy1.Get_File_Name() );
					
					if( fd_img.exists() )
					{
						qint64 size_in_bytes = fd_img.size();
						
						if( size_in_bytes <= 0 )
						{
							ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
									tr("On Disk Size: ") + QString::number(0) + tr("KB") );
						}
						else
						{
							ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
									tr("On Disk Size: ") + QString::number((int)size_in_bytes / 1024.0) + tr("KB") );
						}
					}
					else
					{
						ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
								tr("On Disk Size: ") + QString::number(0) + tr("KB") );
					}
				}
			}
			else
			{
				if( It_Host_Device(Floppy2.Get_File_Name()) )
				{
					ui.Label_Connected_To->setText( tr("Type: Host Device") );
				}
				else
				{
					QFileInfo fd_img = QFileInfo( Floppy2.Get_File_Name() );
					
					if( fd_img.exists() )
					{
						qint64 size_in_bytes = fd_img.size();
						
						if( size_in_bytes <= 0 )
						{
							ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
									tr("On Disk Size: ") + QString::number(0) + tr("KB") );
						}
						else
						{
							ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
									tr("On Disk Size: ") + QString::number((int)size_in_bytes / 1024.0) + tr("KB") );
						}
					}
					else
					{
						ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
								tr("On Disk Size: ") + QString::number(0) + tr("MB") );
					}
				}
			}
		}
		else if( ui.Devices_List->currentItem()->data(512).toString() == "cd" )
		{
			ui.TB_Edit_Device->setEnabled( true );
			ui.actionProperties->setEnabled( true );
			
			ui.TB_Delete_Device->setEnabled( true );
			ui.actionDelete->setEnabled( true );
			
			ui.TB_Format_HDD->setEnabled( false );
			ui.actionFormat_HDD->setEnabled( false );
			
			ui.TB_Quick_Format->setEnabled( false );
			ui.actionQuick_Format->setEnabled( false );
			
			if( It_Host_Device(CD_ROM.Get_File_Name()) )
			{
				ui.Label_Connected_To->setText( tr("Type: Host Device") );
			}
			else
			{
				QFileInfo cd_img = QFileInfo( CD_ROM.Get_File_Name() );
				
				if( cd_img.exists() )
				{
					qint64 size_in_bytes = cd_img.size();
					
					if( size_in_bytes <= 0 )
					{
						ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
								tr("On Disk Size: ") + QString::number(0) + tr("MB") );
					}
					else
					{
						ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
								tr("On Disk Size: ") + QString::number((float)(size_in_bytes / 1024.0 / 1024.0), 'f', 2) + tr("MB") );
					}
				}
				else
				{
					ui.Label_Connected_To->setText( tr("Type: Image") + "\n" +
							tr("On Disk Size: ") + QString::number(0) + tr("MB") );
				}
			}
		}
		else if( ui.Devices_List->currentItem()->data(512).toString() == "hda" ||
				 ui.Devices_List->currentItem()->data(512).toString() == "hdb" ||
				 ui.Devices_List->currentItem()->data(512).toString() == "hdc" ||
				 ui.Devices_List->currentItem()->data(512).toString() == "hdd" )
		{
			ui.TB_Edit_Device->setEnabled( true );
			ui.actionProperties->setEnabled( true );
			
			ui.TB_Delete_Device->setEnabled( true );
			ui.actionDelete->setEnabled( true );
			
			ui.TB_Format_HDD->setEnabled( true );
			ui.actionFormat_HDD->setEnabled( true );
			
			ui.TB_Quick_Format->setEnabled( true );
			ui.actionQuick_Format->setEnabled( true );
			
			if( ui.Devices_List->currentItem()->data(512).toString() == "hda" )
			{
				HDA_Info->Update_Disk_Info( HDA.Get_File_Name() );
				
				ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDA.Get_Virtual_Size().Size) +
												Get_TR_Size_Suffix(HDA.Get_Virtual_Size()) + "\n" +
												tr("Image On Disk Size: ") + QString::number(HDA.Get_Disk_Size().Size) +
												Get_TR_Size_Suffix(HDA.Get_Disk_Size()) );
			}
			else if( ui.Devices_List->currentItem()->data(512).toString() == "hdb" )
			{
				HDB_Info->Update_Disk_Info( HDB.Get_File_Name() );
				
				ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDB.Get_Virtual_Size().Size) +
												Get_TR_Size_Suffix(HDB.Get_Virtual_Size()) + "\n" +
												tr("Image On Disk Size: ") + QString::number(HDB.Get_Disk_Size().Size) +
												Get_TR_Size_Suffix(HDB.Get_Disk_Size()) );
			}
			else if( ui.Devices_List->currentItem()->data(512).toString() == "hdc" )
			{
				HDC_Info->Update_Disk_Info( HDC.Get_File_Name() );
				
				ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDC.Get_Virtual_Size().Size) +
												Get_TR_Size_Suffix(HDC.Get_Virtual_Size())  + "\n" +
												tr("Image On Disk Size: ") + QString::number(HDC.Get_Disk_Size().Size) +
												Get_TR_Size_Suffix(HDC.Get_Disk_Size()));
			}
			else if( ui.Devices_List->currentItem()->data(512).toString() == "hdd" )
			{
				HDD_Info->Update_Disk_Info( HDD.Get_File_Name() );
				
				ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDD.Get_Virtual_Size().Size) +
												Get_TR_Size_Suffix(HDD.Get_Virtual_Size())  + "\n" +
												tr("Image On Disk Size: ") + QString::number(HDD.Get_Disk_Size().Size) +
												Get_TR_Size_Suffix(HDD.Get_Disk_Size()));
			}
		}
		else
		{
			bool finded = false;
			
			for( int fx = 0; fx < 32; ++fx )
			{
				if( ui.Devices_List->currentItem()->data(512).toString() == "device" + QString::number(fx) )
				{
					finded = true;
					
					ui.Label_Connected_To->setText( tr("Type: Storage Device") );
					
					ui.TB_Edit_Device->setEnabled( true );
					ui.actionProperties->setEnabled( true );
					
					ui.TB_Delete_Device->setEnabled( true );
					ui.actionDelete->setEnabled( true );
					
					ui.TB_Format_HDD->setEnabled( false );
					ui.actionFormat_HDD->setEnabled( false );
					
					ui.TB_Quick_Format->setEnabled( false );
					ui.actionQuick_Format->setEnabled( false );
				}
			}
			
			if( ! finded )
			{
				ui.TB_Edit_Device->setEnabled( false );
				ui.actionProperties->setEnabled( false);
				
				ui.TB_Delete_Device->setEnabled( false );
				ui.actionDelete->setEnabled( false );
				
				ui.TB_Format_HDD->setEnabled( false );
				ui.actionFormat_HDD->setEnabled( false );
				
				ui.TB_Quick_Format->setEnabled( false );
				ui.actionQuick_Format->setEnabled( false );
			}
		}
	}
	else
	{
		ui.TB_Edit_Device->setEnabled( false );
		ui.actionProperties->setEnabled( false);
			
		ui.TB_Delete_Device->setEnabled( false );
		ui.actionDelete->setEnabled( false );
			
		ui.TB_Format_HDD->setEnabled( false );
		ui.actionFormat_HDD->setEnabled( false );
		
		ui.TB_Quick_Format->setEnabled( false );
		ui.actionQuick_Format->setEnabled( false );
	}
	
	// Disable widgets
	if( ! Enabled )
	{
		ui.actionAdd_Floppy->setEnabled( false );
		ui.TB_Add_Floppy->setEnabled( false );
		
		ui.actionAdd_CD_ROM->setEnabled( false );
		ui.TB_Add_CDROM->setEnabled( false );
		
		ui.actionAdd_HDD->setEnabled( false );
		ui.TB_Add_HDD->setEnabled( false );
		
		ui.TB_Delete_Device->setEnabled( false );
		ui.actionDelete->setEnabled( false );
		
		ui.TB_Format_HDD->setEnabled( false );
		ui.actionFormat_HDD->setEnabled( false );
		
		ui.TB_Quick_Format->setEnabled( false );
		ui.actionQuick_Format->setEnabled( false );
	}
}

void Device_Manager_Widget::Update_List_Mode()
{
	if( ui.Devices_List->viewMode() == QListView::IconMode )
	{
		ui.Devices_List->setSpacing( 10 );
		ui.Devices_List->setFlow( QListView::LeftToRight );
		ui.Devices_List->setViewMode( QListView::IconMode );
	}
	else
	{
		ui.Devices_List->setSpacing( 3 );
		ui.Devices_List->setFlow( QListView::TopToBottom );
		ui.Devices_List->setViewMode( QListView::ListMode );
	}
}

void Device_Manager_Widget::on_Devices_List_customContextMenuRequested( const QPoint &pos )
{
	QListWidgetItem *it = ui.Devices_List->itemAt( pos );
	
	if( it != NULL )
	{
		if( it->data(512).toString() == "fd1" ||
			it->data(512).toString() == "fd2" )
		{
			Context_Menu = new QMenu( ui.Devices_List );
			
			Context_Menu->addAction( ui.actionProperties );
			Context_Menu->addAction( ui.actionDelete );
			
			Context_Menu->exec( ui.Devices_List->mapToGlobal(pos) );
		}
		else if( it->data(512).toString() == "cd" )
		{
			Context_Menu = new QMenu( ui.Devices_List );
			
			Context_Menu->addAction( ui.actionProperties );
			Context_Menu->addAction( ui.actionDelete );
			
			Context_Menu->exec( ui.Devices_List->mapToGlobal(pos) );
		}
		else if( it->data(512).toString() == "hda" ||
				 it->data(512).toString() == "hdb" ||
				 it->data(512).toString() == "hdc" ||
				 it->data(512).toString() == "hdd" )
		{
			Context_Menu = new QMenu( ui.Devices_List );
			
			Context_Menu->addAction( ui.actionProperties );
			Context_Menu->addAction( ui.actionDelete );
			Context_Menu->addAction( ui.actionFormat_HDD );
			Context_Menu->addAction( ui.actionQuick_Format );
			
			Context_Menu->exec( ui.Devices_List->mapToGlobal(pos) );
		}
		else
		{
			bool finded = false;
			
			for( int fx = 0; fx < 32; ++fx )
			{
				if( ui.Devices_List->currentItem()->data(512).toString() == "device" + QString::number(fx) )
				{
					finded = true;
					
					Context_Menu = new QMenu( ui.Devices_List );
					
					Context_Menu->addAction( ui.actionProperties );
					Context_Menu->addAction( ui.actionDelete );
					
					Context_Menu->exec( ui.Devices_List->mapToGlobal(pos) );
				}
			}
			
			if( ! finded )
			{
				AQError( "void Device_Manager_Widget::on_Devices_List_customContextMenuRequested( const QPoint &pos )",
						 "Incorrect Device!" );
			}
		}
	}
	else
	{
		Context_Menu = new QMenu( ui.Devices_List );
		
		Context_Menu->addAction( ui.actionAdd_Floppy );
		Context_Menu->addAction( ui.actionAdd_CD_ROM );
		Context_Menu->addAction( ui.actionAdd_HDD );
		Context_Menu->addSeparator();
		Context_Menu->addAction( ui.actionIcon_Mode );
		Context_Menu->addAction( ui.actionList_Mode );
		
		Context_Menu->exec( ui.Devices_List->mapToGlobal(pos) );
	}
}

void Device_Manager_Widget::on_Devices_List_currentItemChanged(
				QListWidgetItem *current, QListWidgetItem *previous )
{
	Update_Enabled_Actions();
}

void Device_Manager_Widget::on_Devices_List_itemDoubleClicked( QListWidgetItem *item )
{
	on_actionProperties_triggered();
}

void Device_Manager_Widget::on_actionAdd_Floppy_triggered()
{
	if( ! Floppy1.Get_Enabled() )
	{
		pw = new Properties_Window();
		pw->Set_Floppy( Floppy1, tr("Floppy 1") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			Floppy1 = pw->Get_Floppy();
			
			QString dev_name = Floppy1.Get_File_Name();
			
			QListWidgetItem *fdit = new QListWidgetItem( QIcon(":/fdd.png"),
														 tr("Floppy 1") + " (" + dev_name + ")" );
			fdit->setData( 512, "fd1" );
			
			ui.Devices_List->addItem( fdit );
			
			emit Device_Changet();
		}
	}
	else if( ! Floppy2.Get_Enabled() )
	{
		pw = new Properties_Window();
		pw->Set_Floppy( Floppy2, tr("Floppy 2") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			Floppy2 = pw->Get_Floppy();
			
			QString dev_name = Floppy2.Get_File_Name();
			
			QListWidgetItem *fdit = new QListWidgetItem( QIcon(":/fdd.png"),
														 tr("Floppy 2") + " (" + dev_name + ")" );
			fdit->setData( 512, "fd2" );
			
			ui.Devices_List->addItem( fdit );
			
			emit Device_Changet();
		}
	}
	else
	{
		AQGraphic_Warning( tr("Warning!"),
						   tr("Maximum Floppy Disk Count is 2") );
	}
}

void Device_Manager_Widget::on_actionAdd_CD_ROM_triggered()
{
	if( ! CD_ROM.Get_Enabled() )
	{
		pw = new Properties_Window();
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		pw->Set_CD_ROM( CD_ROM, tr("CD/DVD-ROM") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			CD_ROM = pw->Get_CD_ROM();
			CD_ROM.Set_Enabled( true );
			
			QString dev_name = CD_ROM.Get_File_Name();
			
			QListWidgetItem *cdit = new QListWidgetItem( QIcon(":/cdrom.png"),
														 tr("CD-ROM") + " (" + dev_name + ")" , ui.Devices_List );
			cdit->setData( 512, "cd" );
			
			ui.Devices_List->addItem( cdit );
			
			emit Device_Changet();
		}
	}
	else
	{
		AQGraphic_Warning( tr("Warning!"),
						   tr("Maximum CD-ROM Disk Count is 1") );
	}
}

void Device_Manager_Widget::on_actionAdd_HDD_triggered()
{
	if( ! HDA.Get_Enabled() )
	{
		pw = new Properties_Window();
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		pw->Set_HDD( HDA, tr("HDA (First Master)") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			HDA = pw->Get_HDD();
			
			QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
														 tr("HDA") + " (" + HDA.Get_File_Name() + ")", ui.Devices_List );
			hdit->setData( 512, "hda" );
			
			ui.Devices_List->addItem( hdit );
			
			emit Device_Changet();
		}
	}
	else if( ! HDB.Get_Enabled() )
	{
		pw = new Properties_Window();
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		pw->Set_HDD( HDB, tr("HDB (First Slave)") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			HDB = pw->Get_HDD();
			
			QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
														 tr("HDB") + " (" + HDB.Get_File_Name() + ")", ui.Devices_List );
			hdit->setData( 512, "hdb" );
			
			ui.Devices_List->addItem( hdit );
			
			emit Device_Changet();
		}
	}
	else if( HDC.Get_Enabled() == false && CD_ROM.Get_Enabled() == false )
	{
		pw = new Properties_Window();
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		pw->Set_HDD( HDC, tr("HDC (Second Master)") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			HDC = pw->Get_HDD();
			
			QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
														 tr("HDC") + " (" + HDC.Get_File_Name() + ")", ui.Devices_List );
			hdit->setData( 512, "hdc" );
		
			ui.Devices_List->addItem( hdit );
			
			emit Device_Changet();
		}
	}
	else if( ! HDD.Get_Enabled() )
	{
		pw = new Properties_Window();
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		pw->Set_HDD( HDD, tr("HDD (Second Slave)") );
		
		if( pw->exec() == QDialog::Accepted )
		{
			HDD = pw->Get_HDD();
			
			QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
														 tr("HDD") + " (" + HDD.Get_File_Name() + ")", ui.Devices_List );
			hdit->setData( 512, "hdd" );
			
			ui.Devices_List->addItem( hdit );
			
			emit Device_Changet();
		}
	}
	else
	{
		AQGraphic_Warning( tr("Warning!"),
						   tr("Maximum Hard Disk Count is 4 excluding CD-ROM") );
	}
}

void Device_Manager_Widget::on_actionAdd_Device_triggered()
{
	Device_Window = new Add_New_Device_Window();
	VM_Nativ_Storage_Device tmp_dev;
	Device_Window->Set_Emulator_Devices( *Current_Machine_Devices );
	Device_Window->Set_Device( tmp_dev );
	
	if( Device_Window->exec() == QDialog::Accepted )
	{
		Storage_Devices << Device_Window->Get_Device();
		
		QListWidgetItem *devit = new QListWidgetItem( QIcon(":blockdevice.png"),
													  Device_Window->Get_Device().Get_QEMU_Device_Name(), ui.Devices_List );
		devit->setData( 512, "device" + QString::number(Storage_Devices.count() - 1) );
		
		ui.Devices_List->addItem( devit );
		
		emit Device_Changet();
	}
}

void Device_Manager_Widget::on_actionProperties_triggered()
{
	if( ui.Devices_List->currentItem()->data(512).toString() == "fd1" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_Floppy( Floppy1, tr("Floppy 1") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( Floppy1 != pw->Get_Floppy() )
			{
				Floppy1 = pw->Get_Floppy();
				
				ui.Devices_List->currentItem()->setText( tr("Floppy 1") + " (" + Floppy1.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "fd2" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_Floppy( Floppy2, tr("Floppy 2") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( Floppy2 != pw->Get_Floppy() )
			{
				Floppy2 = pw->Get_Floppy();
				
				ui.Devices_List->currentItem()->setText( tr("Floppy 2") + " (" + Floppy2.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "cd" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_CD_ROM( CD_ROM, tr("CD/DVD-ROM") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( CD_ROM != pw->Get_CD_ROM() )
			{
				CD_ROM = pw->Get_CD_ROM();
				
				ui.Devices_List->currentItem()->setText( tr("CD-ROM") + " (" + CD_ROM.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hda" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDA, tr("HDA (First Master)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDA != pw->Get_HDD() )
			{
				HDA = pw->Get_HDD();
				
				ui.Devices_List->currentItem()->setText( tr("HDA") + " (" + HDA.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdb" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDB, tr("HDB (First Slave)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDB != pw->Get_HDD() )
			{
				HDB = pw->Get_HDD();
				
				ui.Devices_List->currentItem()->setText( tr("HDB") + " (" + HDB.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdc" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDC, tr("HDC (Second Master)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDC != pw->Get_HDD() )
			{
				HDC = pw->Get_HDD();
				
				ui.Devices_List->currentItem()->setText( tr("HDC") + " (" + HDC.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdd" )
	{
		pw = new Properties_Window();
		pw->Set_Enabled( Enabled );
		pw->Set_HDD( HDD, tr("HDD (Second Slave)") );
		
		if( ! Current_Machine_Devices )
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Current_Machine_Devices == NULL" );
		pw->Set_Current_Machine_Devices( Current_Machine_Devices );
		
		if( pw->exec() == QDialog::Accepted )
		{
			if( HDD != pw->Get_HDD() )
			{
				HDD = pw->Get_HDD();
				
				ui.Devices_List->currentItem()->setText( tr("HDD") + " (" + HDC.Get_File_Name() + ")" );
				
				emit Device_Changet();
			}
		}
	}
	else
	{
		bool finded = false;
		
		for( int fx = 0; fx < 32; ++fx )
		{
			if( ui.Devices_List->currentItem()->data(512).toString() == "device" + QString::number(fx) )
			{
				finded = true;
				
				Device_Window = new Add_New_Device_Window();
				Device_Window->Set_Enabled( Enabled );
				Device_Window->Set_Device( Storage_Devices[fx] );
				
				if( ! Current_Machine_Devices )
					AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
							 "Current_Machine_Devices == NULL" );
				Device_Window->Set_Emulator_Devices( *Current_Machine_Devices );
				
				if( Device_Window->exec() == QDialog::Accepted )
				{
					if( Storage_Devices[fx] != Device_Window->Get_Device() )
					{
						Storage_Devices[fx] = Device_Window->Get_Device();
						
						emit Device_Changet();
					}
				}
			}
		}
		
		if( ! finded )
		{
			AQError( "void Device_Manager_Widget::on_actionProperties_triggered()",
					 "Incorrect Device!" );
			return;
		}
	}
	
	Update_Enabled_Actions();
}

void Device_Manager_Widget::on_actionDelete_triggered()
{
	int mes_ret = QMessageBox::question( this, tr("Delete?"),
			tr("Delete Device?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( mes_ret == QMessageBox::No ) return;
	
	if( ui.Devices_List->currentItem()->data(512).toString() == "fd1" )
	{
		Floppy1 = VM_Storage_Device();
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "fd2" )
	{
		Floppy2 = VM_Storage_Device();
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "cd" )
	{
		CD_ROM = VM_Storage_Device();
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hda" )
	{
		HDA = VM_HDD( false, "" );
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdb" )
	{
		HDB = VM_HDD( false, "" );
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdc" )
	{
		HDC = VM_HDD( false, "" );
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdd" )
	{
		HDD = VM_HDD( false, "" );
	}
	else
	{
		bool finded = false;
		
		for( int fx = 0; fx < 32; ++fx )
		{
			if( ui.Devices_List->currentItem()->data(512).toString() == "device" + QString::number(fx) )
			{
				finded = true;
				
				Storage_Devices.removeAt( fx );
			}
		}
		
		if( ! finded )
		{
			AQError( "void Device_Manager_Widget::on_actionDelete_triggered()",
					 "Incorrect Device!" );
			return;
		}
	}
	
	ui.Devices_List->takeItem( ui.Devices_List->currentRow() );
	
	emit Device_Changet();
}

void Device_Manager_Widget::on_actionFormat_HDD_triggered()
{
	Create_HDD_Image_Window *hdd_win = new Create_HDD_Image_Window( this );
	
	if( ui.Devices_List->currentItem()->data(512).toString() == "hda" )
	{
		hdd_win->Set_Image_File_Name( HDA.Get_File_Name() );
		hdd_win->Set_Image_Info( HDA_Info->Get_Disk_Info() );
		hdd_win->exec();
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdb" )
	{
		hdd_win->Set_Image_File_Name( HDB.Get_File_Name() );
		hdd_win->Set_Image_Info( HDB_Info->Get_Disk_Info() );
		hdd_win->exec();
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdc" )
	{
		hdd_win->Set_Image_File_Name( HDC.Get_File_Name() );
		hdd_win->Set_Image_Info( HDC_Info->Get_Disk_Info() );
		hdd_win->exec();
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdd" )
	{
		hdd_win->Set_Image_File_Name( HDD.Get_File_Name() );
		hdd_win->Set_Image_Info( HDD_Info->Get_Disk_Info() );
		hdd_win->exec();
	}
	else
	{
		AQGraphic_Error( "void Device_Manager_Widget::on_actionFormat_HDD_triggered()",
						 tr("Error!"), tr("Incorrect Device!"), false );
		return;
	}
	
	delete hdd_win;
	
	Update_Enabled_Actions();
	
	emit Device_Changet();
}

void Device_Manager_Widget::on_actionQuick_Format_triggered()
{
	int mes_ret = QMessageBox::question( this, tr("Format?"),
			tr("Format Virtual Hard Disk?"),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( mes_ret == QMessageBox::No ) return;
	
	if( ui.Devices_List->currentItem()->data(512).toString() == "hda" )
	{
		if( Format_HDD_Image(HDA.Get_File_Name(), HDA_Info->Get_Disk_Info()) )
		{
			QMessageBox::information( this, tr("Information"), tr("Complete!") );
		}
		else
		{
			QMessageBox::critical( this, tr("Error!"), tr("Cannot Format!") );
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdb" )
	{
		if( Format_HDD_Image(HDB.Get_File_Name(), HDB_Info->Get_Disk_Info()) )
		{
			QMessageBox::information( this, tr("Information"), tr("Complete!") );
		}
		else
		{
			QMessageBox::critical( this, tr("Error!"), tr("Cannot Format!") );
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdc" )
	{
		if( Format_HDD_Image(HDC.Get_File_Name(), HDC_Info->Get_Disk_Info()) )
		{
			QMessageBox::information( this, tr("Information"), tr("Complete!") );
		}
		else
		{
			QMessageBox::critical( this, tr("Error!"), tr("Cannot Format!") );
		}
	}
	else if( ui.Devices_List->currentItem()->data(512).toString() == "hdd" )
	{
		if( Format_HDD_Image(HDD.Get_File_Name(), HDD_Info->Get_Disk_Info()) )
		{
			QMessageBox::information( this, tr("Information"), tr("Complete!") );
		}
		else
		{
			QMessageBox::critical( this, tr("Error!"), tr("Cannot Format!") );
		}
	}
	else
	{
		AQGraphic_Error( "void Device_Manager_Widget::on_actionQuick_Format_triggered()",
						 tr("Error!"), tr("Incorrect Device!"), false );
		return;
	}
	
	Update_Enabled_Actions();
	
	emit Device_Changet();
}

void Device_Manager_Widget::on_actionIcon_Mode_triggered()
{
	ui.Devices_List->setSpacing( 10 );
	ui.Devices_List->setFlow( QListView::LeftToRight );
	ui.Devices_List->setViewMode( QListView::IconMode );
}

void Device_Manager_Widget::on_actionList_Mode_triggered()
{
	ui.Devices_List->setSpacing( 3 );
	ui.Devices_List->setFlow( QListView::TopToBottom );
	ui.Devices_List->setViewMode( QListView::ListMode );
}

void Device_Manager_Widget::Update_Icons()
{
	ui.Devices_List->clear();
	
	if( Floppy1.Get_Enabled() )
	{
		QListWidgetItem *fdit = new QListWidgetItem( QIcon(":/fdd.png"),
													 tr("Floppy 1") + " (" + Floppy1.Get_File_Name() + ")" );
		fdit->setData( 512, "fd1" );
		
		ui.Devices_List->addItem( fdit );
	}
	
	if( Floppy2.Get_Enabled() )
	{
		QListWidgetItem *fdit = new QListWidgetItem( QIcon(":/fdd.png"),
													 tr("Floppy 2") + " (" + Floppy2.Get_File_Name() + ")" );
		fdit->setData( 512, "fd2" );
		
		ui.Devices_List->addItem( fdit );
	}
	
	if( CD_ROM.Get_Enabled() )
	{
		QListWidgetItem *cdit = new QListWidgetItem( QIcon(":/cdrom.png"),
													 tr("CD-ROM") + " (" + CD_ROM.Get_File_Name() + ")" , ui.Devices_List );
		cdit->setData( 512, "cd" );
		
		ui.Devices_List->addItem( cdit );
	}
	
	if( HDA.Get_Enabled() )
	{
		QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
													 tr("HDA") + " (" + HDA.Get_File_Name() + ")", ui.Devices_List );
		hdit->setData( 512, "hda" );
		
		ui.Devices_List->addItem( hdit );
	}
	
	if( HDB.Get_Enabled() )
	{
		QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
													 tr("HDB") + " (" + HDB.Get_File_Name() + ")", ui.Devices_List );
		hdit->setData( 512, "hdb" );
		
		ui.Devices_List->addItem( hdit );
	}
	
	if( HDC.Get_Enabled() )
	{
		QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
													 tr("HDC") + " (" + HDC.Get_File_Name() + ")", ui.Devices_List );
		hdit->setData( 512, "hdc" );
		
		ui.Devices_List->addItem( hdit );
	}
	
	if( HDD.Get_Enabled() )
	{
		QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/hdd.png"),
													 tr("HDD") + " (" + HDD.Get_File_Name() + ")", ui.Devices_List );
		hdit->setData( 512, "hdd" );
		
		ui.Devices_List->addItem( hdit );
	}
	
	// Storage Devices
	if( Storage_Devices.count() )
	{
		for( int ix = 0; ix < Storage_Devices.count(); ++ix )
		{
			QListWidgetItem *hdit = new QListWidgetItem( QIcon(":/blockdevice.png"),
														 Storage_Devices[ix].Get_QEMU_Device_Name(), ui.Devices_List );
			hdit->setData( 512, "device" + QString::number(ix) );
			
			ui.Devices_List->addItem( hdit );
		}
	}
}

void Device_Manager_Widget::Update_HDA( bool ok )
{
	HDA.Set_Disk_Info( HDA_Info->Get_Disk_Info() );
	
	ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDA.Get_Virtual_Size().Size) +
			Get_TR_Size_Suffix(HDA.Get_Virtual_Size()) + "\n" +
			tr("Image On Disk Size: ") + QString::number(HDA.Get_Disk_Size().Size) +
			Get_TR_Size_Suffix(HDA.Get_Disk_Size()) );
}

void Device_Manager_Widget::Update_HDB( bool ok )
{
	HDB.Set_Disk_Info( HDB_Info->Get_Disk_Info() );
	
	ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDB.Get_Virtual_Size().Size) +
			Get_TR_Size_Suffix(HDB.Get_Virtual_Size()) + "\n" +
			tr("Image On Disk Size: ") + QString::number(HDB.Get_Disk_Size().Size) +
			Get_TR_Size_Suffix(HDB.Get_Disk_Size()) );
}

void Device_Manager_Widget::Update_HDC( bool ok )
{
	HDC.Set_Disk_Info( HDC_Info->Get_Disk_Info() );
	
	ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDC.Get_Virtual_Size().Size) +
			Get_TR_Size_Suffix(HDC.Get_Virtual_Size())  + "\n" +
			tr("Image On Disk Size: ") + QString::number(HDC.Get_Disk_Size().Size) +
			Get_TR_Size_Suffix(HDC.Get_Disk_Size()));
}

void Device_Manager_Widget::Update_HDD( bool ok )
{
	HDD.Set_Disk_Info( HDD_Info->Get_Disk_Info() );
	
	ui.Label_Connected_To->setText( tr("Image Virtual Size: ") + QString::number(HDD.Get_Virtual_Size().Size) +
			Get_TR_Size_Suffix(HDD.Get_Virtual_Size())  + "\n" +
			tr("Image On Disk Size: ") + QString::number(HDD.Get_Disk_Size().Size) +
			Get_TR_Size_Suffix(HDD.Get_Disk_Size()));
}
