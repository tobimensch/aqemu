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

#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include "Utils.h"
#include "Snapshots_Window.h"
#include "Snapshot_Properties_Window.h"
#include "HDD_Image_Info.h"

Snapshots_Window::Snapshots_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

void Snapshots_Window::Set_VM( Virtual_Machine *vm )
{
	Current_VM = vm;
	on_Button_Update_clicked();
	
	if( Current_VM->Get_State() != VM::VMS_Running )
	{
		ui.Button_Create->setEnabled( false );
		ui.Button_Delete->setEnabled( false );
	}
}

void Snapshots_Window::on_Snapshots_Tree_itemSelectionChanged()
{
	QList<QTreeWidgetItem*> sel_list = ui.Snapshots_Tree->selectedItems();
	
	if( sel_list.count() > 0 )
	{
		QString name = sel_list[0]->text( ui.Snapshots_Tree->currentColumn() );
		
		for( int ix = 0; ix < Current_VM->Get_Snapshots().count(); ++ix )
		{
			QString tmp_str = "";
			
			if( Current_VM->Get_Snapshots()[ix].Get_Name().isEmpty() )
			{
				tmp_str = Current_VM->Get_Snapshots()[ix].Get_Tag();
			}
			else
			{
				tmp_str = Current_VM->Get_Snapshots()[ix].Get_Name();
			}
			
			if( tmp_str == name )
			{
				ui.Label_ID->setText( tr("ID:") + " " + Current_VM->Get_Snapshots()[ix].Get_ID() );
				ui.Label_Size->setText( tr("Size:") + " " + Current_VM->Get_Snapshots()[ix].Get_Size() );
				ui.Label_Date->setText( tr("Date:") + " " + Current_VM->Get_Snapshots()[ix].Get_Date() );
				ui.Label_VM_Clock->setText( tr("VM Clock:") + " " + Current_VM->Get_Snapshots()[ix].Get_VM_Clock() );
				ui.Label_Description->setText( tr("Description:") + " " + Current_VM->Get_Snapshots()[ix].Get_Description() );
			}
		}
	}
}

void Snapshots_Window::on_Button_Create_clicked()
{
	Snapshot_Properties_Window *prop_win = new Snapshot_Properties_Window( this );
	
	if( prop_win->exec() == QDialog::Accepted )
	{
		QString tag = Get_FS_Compatible_VM_Name( prop_win->Get_Snapshot_Name() );
		
		bool next = true;
		
		for( int sx = 0; sx < Current_VM->Get_Snapshots().count(); ++sx )
		{
			if( Current_VM->Get_Snapshots()[sx].Get_Tag() == tag )
			{
				next = false;
			}
		}
		
		if( next == false )
		{
			for( int ix = 0; ix < 1024; ++ix )
			{
				tag = "snapshot" + QString::number( ix );
				
				bool find = false;
				
				for( int sx = 0; sx < Current_VM->Get_Snapshots().count(); ++sx )
				{
					if( Current_VM->Get_Snapshots()[sx].Get_Tag() == tag )
					{
						find = true;
					}
				}
				
				if( ! find ) break;
			}
		}
		
		Current_VM->Add_Snapshot( tag, prop_win->Get_Snapshot_Name(), prop_win->Get_Snapshot_Description() );
		Current_VM->Save_VM_State( tag, false );
		
		delete prop_win;
		
		accept();
	}
	else
	{
		delete prop_win;
	}
}

void Snapshots_Window::on_Button_Delete_clicked()
{
	if( ui.Snapshots_Tree->currentItem() == NULL ) return;
	
	if( ui.Snapshots_Tree->currentItem()->text(0).isEmpty() )
	{
		AQGraphic_Warning( tr("Warning!"), tr("Please select snapshot that you want to delete!") );
		return;
	}
	
	int ret = QMessageBox::question( this, tr("Warning"), tr("Delete Current Snapshot?"),
									 QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( ret == QMessageBox::Yes )
	{
		Current_VM->Delete_Snapshot( ui.Snapshots_Tree->currentItem()->text(0) );
		ui.Snapshots_Tree->currentItem()->setHidden( true );
	}
}

void Snapshots_Window::on_Button_Start_clicked()
{
	if( ui.Snapshots_Tree->currentItem() == NULL ) return;
	
	if( Current_VM->Get_State() == VM::VMS_Power_Off ||
		Current_VM->Get_State() == VM::VMS_Saved )
	{
		if( ui.Snapshots_Tree->currentItem()->text(0).isEmpty() )
		{
			AQGraphic_Warning( tr("Warning!"), tr("Please select snapshot that you want to start!") );
			return;
		}
		else
		{
			if( Current_VM->Start_Snapshot(ui.Snapshots_Tree->currentItem()->text(0)) )
			{
				if( Settings.value("Show_Emulator_Control_Window", "yes").toString() == "yes" )
				{
					Current_VM->Show_Emu_Ctl_Win();
				}
				
				accept();
			}
			else
			{
				AQError( "void Snapshots_Window::on_Button_Start_clicked()",
						 "Cannot Start VM!" );
			}
		}
	}
	else
	{
		int ret = QMessageBox::question( this, tr("Warning"), tr("This VM is Running now! Stop VM and Load Snapshot?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
		
		if( ret == QMessageBox::Yes )
		{
			Current_VM->Load_VM_State( ui.Snapshots_Tree->currentItem()->text(0) );
			accept();
		}
	}
}

void Snapshots_Window::on_Button_Properties_clicked()
{
	if( ui.Snapshots_Tree->currentItem() == NULL ) return;
	
	Snapshot_Properties_Window *prop_win = new Snapshot_Properties_Window( this );
	
	QList<QTreeWidgetItem*> sel_list = ui.Snapshots_Tree->selectedItems();
	
	if( sel_list.count() > 0 )
	{
		QString tag = sel_list[0]->text( ui.Snapshots_Tree->currentColumn() );
		
		for( int ix = 0; ix < Current_VM->Get_Snapshots().count(); ++ix )
		{
			QString tmp_str = "";
			
			if( Current_VM->Get_Snapshots()[ix].Get_Name().isEmpty() )
			{
				tmp_str = Current_VM->Get_Snapshots()[ix].Get_Tag();
			}
			else
			{
				tmp_str = Current_VM->Get_Snapshots()[ix].Get_Name();
			}
			
			if( tmp_str == tag )
			{
				if( Current_VM->Get_Snapshots()[ix].Get_Name().isEmpty() )
				{
					prop_win->Set_Snapshot_Name( Current_VM->Get_Snapshots()[ix].Get_Tag() );
				}
				else
				{
					prop_win->Set_Snapshot_Name( Current_VM->Get_Snapshots()[ix].Get_Name() );
				}
				
				prop_win->Set_Snapshot_Description( Current_VM->Get_Snapshots()[ix].Get_Description() );
				
				if( prop_win->exec() == QDialog::Accepted )
				{
					VM_Snapshot tmp = Current_VM->Get_Snapshots()[ix];
					
					tmp.Set_Name( prop_win->Get_Snapshot_Name() );
					tmp.Set_Description( prop_win->Get_Snapshot_Description() );
					
					Current_VM->Set_Snapshot( ix, tmp );
					Current_VM->Save_VM();
					
					on_Button_Update_clicked();
					
					return;
				}
			}
		}
	}
}

void Snapshots_Window::on_Button_Update_clicked()
{
	ui.Snapshots_Tree->clear();
	
	if(  Update_Info() )
	{
		for( int ix = 0; ix < Current_VM->Get_Snapshots().count(); ++ix )
		{
			QTreeWidgetItem *tmp_it = new QTreeWidgetItem( ui.Snapshots_Tree );
			
			if( Current_VM->Get_Snapshots()[ix].Get_Name().isEmpty() )
			{
				tmp_it->setText( 0, Current_VM->Get_Snapshots()[ix].Get_Tag() );
			}
			else
			{
				tmp_it->setText( 0, Current_VM->Get_Snapshots()[ix].Get_Name() );
			}
			
			ui.Snapshots_Tree->insertTopLevelItem( 0, tmp_it );
		}
	}
	
	if( Current_VM->Get_Snapshots().count() < 1 )
	{
		ui.Button_Start->setEnabled( false );
	}
}

bool Snapshots_Window::Update_Info()
{
	if( Current_VM == NULL )
	{
		AQError( "bool Snapshots_Window::Update_Info()", "Current_VM == NULL" );
		return false;
	}
	
	// Find HDD image
	QString imagePath = "";
	
	if( QFile::exists(Current_VM->Get_HDA().Get_File_Name()) ) imagePath = Current_VM->Get_HDA().Get_File_Name();
	else if( QFile::exists(Current_VM->Get_HDB().Get_File_Name()) ) imagePath = Current_VM->Get_HDB().Get_File_Name();
	else if( QFile::exists(Current_VM->Get_HDC().Get_File_Name()) ) imagePath = Current_VM->Get_HDC().Get_File_Name();
	else if( QFile::exists(Current_VM->Get_HDD().Get_File_Name()) ) imagePath = Current_VM->Get_HDD().Get_File_Name();
	else if( Current_VM->Get_Storage_Devices_List().isEmpty() == false )
	{
		for( int ix = 0; ix < Current_VM->Get_Storage_Devices_List().count(); ++ix )
		{
			if( Current_VM->Get_Storage_Devices_List()[ix].Use_Media() == false ||
				Current_VM->Get_Storage_Devices_List()[ix].Get_Media() == VM::DM_Disk )
			{
				if( QFile::exists(Current_VM->Get_Storage_Devices_List()[ix].Get_File_Path()) )
					imagePath = Current_VM->Get_Storage_Devices_List()[ix].Get_File_Path();
			}
		}
	}
	
	if( imagePath.isEmpty() )
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("You must add HDD image in QCOW2 format if you want to use snapshots!") );
		return false;
	}
	
	QProcess *qimg = new QProcess( this );
	QStringList qimg_args;
	qimg_args << "info" << imagePath;
	
	qimg->start( Settings.value("QEMU-IMG_Path", "qemu-img").toString(), qimg_args );
	
	if( ! qimg->waitForFinished(500) )
	{
		AQError( "bool Snapshots_Window::Update_Info()",
				 "qemu-img Not Finished! Hard Disk Info Cannot Read!" );
		return false;
	}
	
	QByteArray info_str_ba = qimg->readAll();
	QString info_str = QString( info_str_ba ); // Create QString
	
	delete qimg;
	
	// Pre Str RegExp:
	// .*Snapshot list:.*ID[\s]+TAG[\s]+VM[\s]+SIZE[\s]+DATE[\s]+VM[\s]+CLOCK(.+)
	QRegExp RegInfo = QRegExp( ".*Snapshot list:.*ID[\\s]+TAG[\\s]+VM[\\s]+SIZE[\\s]+DATE[\\s]+VM[\\s]+CLOCK(.+)" );
	
	if( ! RegInfo.exactMatch(info_str) )
	{
		AQError( "bool Snapshots_Window::Update_Info()",
				 "QRegExp Not Matched!" );
		return false;
	}
	
	QStringList info_lines = RegInfo.capturedTexts();
	
	QList<VM_Snapshot> finded_snapshots;
	
	if( info_lines.count() == 2 )
	{
		// Snapshot Line RegExp:
		// ([\d])+[\s]+([a-zA-Z0-9_]+)[\s]+([0-9KMG]+|[0-9]+.[0-9KMG]+)[\s]+(.+)[\s]{2,}(.+)
		
		QStringList snapshots_list = info_lines[1].split( "\n", QString::SkipEmptyParts );
		QRegExp snap_regexp = QRegExp( "([\\d])+[\\s]+([a-zA-Z0-9_]+)[\\s]+([0-9KMG]+|[0-9]+.[0-9KMG]+)[\\s]+(.+)[\\s]{2,}(.+)" );
		
		for( int sx = 0; sx < snapshots_list.count(); ++sx )
		{
			if( ! snap_regexp.exactMatch(snapshots_list[sx]) ) continue;
			
			QStringList snap_info = snap_regexp.capturedTexts();
			
			if( snap_info.count() != 6 ) continue;
			
			VM_Snapshot tmp_snap;
			tmp_snap.Set_ID( snap_info[1] );
			tmp_snap.Set_Tag( snap_info[2] );
			tmp_snap.Set_Size( snap_info[3] );
			tmp_snap.Set_Date( snap_info[4] );
			tmp_snap.Set_VM_Clock( snap_info[5] );
			
			finded_snapshots << tmp_snap;
		}
	}
	else
	{
		return false;
	}
	
	// Unique
	QList<VM_Snapshot> compare_list = Current_VM->Get_Snapshots();
	
	for( int fx = 0; fx < finded_snapshots.count(); ++fx )
	{
		bool next = true;
		
		for( int cx = 0; next && cx < compare_list.count(); ++cx )
		{
			if( finded_snapshots[fx].Get_Tag() == compare_list[cx].Get_Tag() )
			{
				next = false;
				
				finded_snapshots[fx].Set_Name( compare_list[cx].Get_Name() );
				finded_snapshots[fx].Set_Description( compare_list[cx].Get_Description() );
				
				compare_list.removeAt( cx );
			}
		}
	}
	
	// Add and Save
	Current_VM->Set_Snapshots( finded_snapshots );
	Current_VM->Save_VM();
	
	return true;
}
