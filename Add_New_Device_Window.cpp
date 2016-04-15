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

#include "Utils.h"
#include "Add_New_Device_Window.h"

Add_New_Device_Window::Add_New_Device_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

VM_Nativ_Storage_Device Add_New_Device_Window::Get_Device() const
{
	return Device;
}

void Add_New_Device_Window::Set_Device( const VM_Nativ_Storage_Device &dev )
{
	Device = dev;
	
	// Update View...
	ui.CH_Interface->setChecked( Device.Use_Interface() );
	
	// Interface
	switch( Device.Get_Interface() )
	{
		case VM::DI_IDE:
			ui.CB_Interface->setCurrentIndex( 0 );
			break;
			
		case VM::DI_SCSI:
			ui.CB_Interface->setCurrentIndex( 1 );
			break;
			
		case VM::DI_SD:
			ui.CB_Interface->setCurrentIndex( 2 );
			break;
			
		case VM::DI_MTD:
			ui.CB_Interface->setCurrentIndex( 3 );
			break;
			
		case VM::DI_Floppy:
			ui.CB_Interface->setCurrentIndex( 4 );
			break;
			
		case VM::DI_PFlash:
			ui.CB_Interface->setCurrentIndex( 5 );
			break;
			
		case VM::DI_Virtio:
			ui.CB_Interface->setCurrentIndex( 6 );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::Set_Device( const VM_Nativ_Storage_Device &dev )",
					 "Interface Default Section! Use IDE!" );
			break;
	}
	
	// Media
	ui.CH_Media->setChecked( Device.Use_Media() );
	
	switch( Device.Get_Media() )
	{
		case VM::DM_Disk:
			ui.CB_Media->setCurrentIndex( 0 );
			break;
			
		case VM::DM_CD_ROM:
			ui.CB_Media->setCurrentIndex( 1 );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::Set_Device( const VM_Nativ_Storage_Device &dev )",
					 "Media Default Section! Use Disk!" );
			break;
	}
	
	// File Path
	ui.CH_File->setChecked( Device.Use_File_Path() );
	ui.Edit_File_Path->setText( Device.Get_File_Path() );
	
	// Index
	ui.CH_Index->setChecked( Device.Use_Index() );
	ui.SB_Index->setValue( Device.Get_Index() );
	
	// Bus, Unit
	ui.CH_Bus_Unit->setChecked( Device.Use_Bus_Unit() );
	ui.SB_Bus->setValue( Device.Get_Bus() );
	ui.SB_Unit->setValue( Device.Get_Unit() );
	
	// Snapshot
	ui.CH_Snapshot->setChecked( Device.Use_Snapshot() );
	ui.CB_Snapshot->setCurrentIndex( Device.Get_Snapshot() ? 0 : 1 );
	
	// Cache
	ui.CH_Cache->setChecked( Device.Use_Cache() );
	int index = ui.CB_Cache->findText( Device.Get_Cache() );
	if( index != -1 )
		ui.CB_Cache->setCurrentIndex( index );
	else
		AQError( "void Add_New_Device_Window::Set_Device( const VM_Nativ_Storage_Device &dev )",
				 "Cache: " + Device.Get_Cache() );
	
	// AIO
	ui.CH_AIO->setChecked( Device.Use_AIO() );
	index = ui.CB_AIO->findText( Device.Get_AIO() );
	if( index != -1 ) ui.CB_AIO->setCurrentIndex( index );
	else
		AQError( "void Add_New_Device_Window::Set_Device( const VM_Nativ_Storage_Device &dev )",
				 "AIO: " + Device.Get_AIO() );
	
	// Boot
	ui.CH_Boot->setChecked( Device.Use_Boot() );
	ui.CB_Boot->setCurrentIndex( Device.Get_Boot() ? 0 : 1 );
	
	// cyls, heads, secs, trans
	ui.GB_hdachs_Settings->setChecked( Device.Use_hdachs() );
	ui.Edit_Cyls->setText( QString::number(Device.Get_Cyls()) );
	ui.Edit_Heads->setText( QString::number(Device.Get_Heads()) );
	ui.Edit_Secs->setText( QString::number(Device.Get_Secs()) );
	ui.Edit_Trans->setText( QString::number(Device.Get_Trans()) );
}

void Add_New_Device_Window::Set_Emulator_Devices( const Available_Devices &devices )
{
	if( devices.PSO_Drive_File )
	{
		ui.CH_File->setVisible( true );
		ui.Edit_File_Path->setVisible( true );
		ui.TB_File_Path_Browse->setVisible( true );
	}
	else
	{
		ui.CH_File->setVisible( false );
		ui.Edit_File_Path->setVisible( false );
		ui.TB_File_Path_Browse->setVisible( false );
	}
	
	if( devices.PSO_Drive_If )
	{
		ui.CH_Interface->setVisible( true );
		ui.CB_Interface->setVisible( true );
	}
	else
	{
		ui.CH_Interface->setVisible( false );
		ui.CB_Interface->setVisible( false );
	}
	
	if( devices.PSO_Drive_Bus_Unit )
	{
		ui.CH_Bus_Unit->setVisible( true );
		ui.SB_Bus->setVisible( true );
		ui.SB_Unit->setVisible( true );
	}
	else
	{
		ui.CH_Bus_Unit->setVisible( false );
		ui.SB_Bus->setVisible( false );
		ui.SB_Unit->setVisible( false );
	}
	
	if( devices.PSO_Drive_Index )
	{
		ui.CH_Index->setVisible( true );
		ui.SB_Index->setVisible( true );
	}
	else
	{
		ui.CH_Index->setVisible( false );
		ui.SB_Index->setVisible( false );
	}
		
	if( devices.PSO_Drive_Media )
	{
		ui.CH_Media->setVisible( true );
		ui.CB_Media->setVisible( true );
	}
	else
	{
		ui.CH_Media->setVisible( false );
		ui.CB_Media->setVisible( false );
	}
		
	if( devices.PSO_Drive_Cyls_Heads_Secs_Trans )
		ui.GB_hdachs_Settings->setVisible( true );
	else
		ui.GB_hdachs_Settings->setVisible( false );
		
	if( devices.PSO_Drive_Snapshot )
	{
		ui.CH_Snapshot->setVisible( true );
		ui.CB_Snapshot->setVisible( true );
	}
	else
	{
		ui.CH_Snapshot->setVisible( false );
		ui.CB_Snapshot->setVisible( false );
	}
		
	if( devices.PSO_Drive_Cache )
	{
		ui.CH_Cache->setVisible( true );
		ui.CB_Cache->setVisible( true );
	}
	else
	{
		ui.CH_Cache->setVisible( false );
		ui.CB_Cache->setVisible( false );
	}
		
	if( devices.PSO_Drive_AIO )
	{
		ui.CH_AIO->setVisible( true );
		ui.CB_AIO->setVisible( true );
	}
	else
	{
		ui.CH_AIO->setVisible( false );
		ui.CB_AIO->setVisible( false );
	}
		
	/* FIXME
	if( devices.PSO_Drive_Format )
	{
		ui.->setVisible( true );
		ui.->setVisible( true );
	}
	else
	{
		ui.->setVisible( false );
		ui.->setVisible( false );
	}
	
	if( devices.PSO_Drive_Serial )
	{
		ui.->setVisible( true );
		ui.->setVisible( true );
	}
	else
	{
		ui.->setVisible( false );
		ui.->setVisible( false );
	}
	
	if( devices.PSO_Drive_ADDR )
	{
		ui.->setVisible( true );
		ui.->setVisible( true );
	}
	else
	{
		ui.->setVisible( false );
		ui.->setVisible( false );
	}*/
	
	if( devices.PSO_Drive_Boot )
	{
		ui.CH_Boot->setVisible( true );
		ui.CB_Boot->setVisible( true );
	}
	else
	{
		ui.CH_Boot->setVisible( false );
		ui.CB_Boot->setVisible( false );
	}
	
	// Minimum Size
	resize( minimumSizeHint().width(), minimumSizeHint().height() );
}

void Add_New_Device_Window::Set_Enabled( bool enabled )
{
	ui.Button_OK->setEnabled( enabled );
}

void Add_New_Device_Window::on_CB_Interface_currentIndexChanged( const QString &text )
{
	if( text == "ide" || text == "floppy" )
	{
		ui.CH_Index->setEnabled( true );
		ui.SB_Index->setEnabled( true );
		
		ui.CH_Bus_Unit->setEnabled( false );
		ui.SB_Bus->setEnabled( false );
		ui.SB_Unit->setEnabled( false );
	}
	else if( text == "scsi" )
	{
		ui.CH_Index->setEnabled( false );
		ui.SB_Index->setEnabled( false );
		
		ui.CH_Bus_Unit->setEnabled( true );
		ui.SB_Bus->setEnabled( true );
		ui.SB_Unit->setEnabled( true );
	}
	else if( text == "virtio" )
	{
		ui.CH_Index->setEnabled( true );
		ui.SB_Index->setEnabled( true );
		
		ui.CH_Bus_Unit->setEnabled( true );
		ui.SB_Bus->setEnabled( true );
		ui.SB_Unit->setEnabled( true );
	}
	else
	{
		ui.CH_Index->setEnabled( false );
		ui.SB_Index->setEnabled( false );
		
		ui.CH_Bus_Unit->setEnabled( false );
		ui.SB_Bus->setEnabled( false );
		ui.SB_Unit->setEnabled( false );
	}
}

void Add_New_Device_Window::on_TB_File_Path_Browse_clicked()
{
	QString file_name = QFileDialog::getOpenFileName( this, tr("Select your device"),
													  Get_Last_Dir_Path(ui.Edit_File_Path->text()),
													  tr("All Files (*)") );
	
	if( ! file_name.isEmpty() )
		ui.Edit_File_Path->setText( QDir::toNativeSeparators(file_name) );
}

void Add_New_Device_Window::on_Button_OK_clicked()
{
	// Interface
	switch( ui.CB_Interface->currentIndex() )
	{
		case 0:
			Device.Set_Interface( VM::DI_IDE );
			break;
			
		case 1:
			Device.Set_Interface( VM::DI_SCSI );
			break;
			
		case 2:
			Device.Set_Interface( VM::DI_SD );
			break;
			
		case 3:
			Device.Set_Interface( VM::DI_MTD );
			break;
			
		case 4:
			Device.Set_Interface( VM::DI_Floppy );
			break;
			
		case 5:
			Device.Set_Interface( VM::DI_PFlash );
			break;
			
		case 6:
			Device.Set_Interface( VM::DI_Virtio );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::on_Button_OK_clicked()",
					 "Invalid Interface Index! Use IDE" );
			Device.Set_Interface( VM::DI_IDE );
			break;
	}
	
	Device.Use_Interface( ui.CH_Interface->isChecked() );
	
	// Media
	switch( ui.CB_Media->currentIndex() )
	{
		case 0:
			Device.Set_Media( VM::DM_Disk );
			break;
			
		case 1:
			Device.Set_Media( VM::DM_CD_ROM );
			break;
			
		default:
			AQError( "void Add_New_Device_Window::on_Button_OK_clicked()",
					 "Invalid Media Index! Use Disk" );
			Device.Set_Media( VM::DM_Disk );
			break;
	}
	
	Device.Use_Media( ui.CH_Media->isChecked() );
	
	// File Path
	if( ui.CH_File->isChecked() )
	{
		if( ! QFile::exists(ui.Edit_File_Path->text()) )
		{
			AQGraphic_Warning( tr("Error!"), tr("File does not exist!") );
			return;
		}
	}
	
	Device.Use_File_Path( ui.CH_File->isChecked() );
	Device.Set_File_Path( ui.Edit_File_Path->text() );
	
	// Index
	Device.Use_Index( ui.CH_Index->isChecked() );
	Device.Set_Index( ui.SB_Index->value() );
	
	// Bus, Unit
	Device.Use_Bus_Unit( ui.CH_Bus_Unit->isChecked() );
	Device.Set_Bus( ui.SB_Bus->value() );
	Device.Set_Unit( ui.SB_Unit->value() );
	
	// Snapshot
	Device.Use_Snapshot( ui.CH_Snapshot->isChecked() );
	Device.Set_Snapshot( (ui.CB_Snapshot->currentIndex() == 0) ? true : false );
	
	// Cache
	Device.Use_Cache( ui.CH_Cache->isChecked() );
	Device.Set_Cache( ui.CB_Cache->currentText() );
	
	// AIO
	Device.Use_AIO( ui.CH_AIO->isChecked() );
	Device.Set_AIO( ui.CB_AIO->currentText() );
	
	// Boot
	Device.Use_Boot( ui.CH_Boot->isChecked() );
	Device.Set_Boot( (ui.CB_Boot->currentIndex() == 0) ? true : false );
	
	// hdachs
	if( ui.GB_hdachs_Settings->isChecked() )
	{
		bool ok;
		
		qulonglong cyls = ui.Edit_Cyls->text().toULongLong( &ok, 10 );
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Cyls\" value is incorrect!") );
			return;
		}
		
		qulonglong heads = ui.Edit_Heads->text().toULongLong( &ok, 10 );
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Heads\" value is incorrect!") );
			return;
		}
		
		qulonglong secs = ui.Edit_Secs->text().toULongLong( &ok, 10) ;
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Secs\" value is incorrect!") );
			return;
		}
		
		qulonglong trans = ui.Edit_Trans->text().toULongLong( &ok, 10 );
		if( ! ok )
		{
			AQGraphic_Warning( tr("Warning!"), tr("\"Trans\" value is incorrect!") );
			return;
		}
		
		Device.Use_hdachs( ui.GB_hdachs_Settings->isChecked() );
		Device.Set_Cyls( cyls );
		Device.Set_Heads( heads );
		Device.Set_Secs( secs );
		Device.Set_Trans( trans );
	}
	
	accept();
}
