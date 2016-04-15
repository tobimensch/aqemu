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
#include <QRegExp>
#include <QFileDialog>

#include "Utils.h"
#include "Settings_Window.h"
#include "VM_Wizard_Window.h"
#include "System_Info.h"

VM_Wizard_Window::VM_Wizard_Window( QWidget *parent )
	: QDialog(parent)
{
	ui.setupUi( this );
	ui.Label_Page->setBackgroundRole( QPalette::Base );
	
	New_VM = new Virtual_Machine();
	
	// Loadind All Templates
	if( Load_OS_Templates() )
	{
		// Find default template
		for( int ix = 0; ix < ui.CB_OS_Type->count(); ++ix )
		{
			if( ui.CB_OS_Type->itemText(ix) == Settings.value("Default_VM_Template", "Linux 2.6").toString() )
			{
				ui.CB_OS_Type->setCurrentIndex( ix );
			}
		}
	}
	else
	{
		AQWarning( "void VM_Wizard_Window::on_Button_Next_clicked()",
				   "No VM Templates Found!" );
	}
}

void VM_Wizard_Window::Set_VM_List( QList<Virtual_Machine*> *list )
{
	VM_List = list;
}

void VM_Wizard_Window::on_Button_Back_clicked()
{
	ui.Button_Next->setEnabled( true );
	
	if( ui.Welcome_Page == ui.Wizard_Pages->currentWidget() )
	{
	}
	else if( ui.Wizard_Mode_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Welcome_Page );
		ui.Label_Page->setText( tr("New Virtual Machine Wizard") );
		ui.Button_Back->setEnabled( false );
	}
	else if( ui.Template_Page == ui.Wizard_Pages->currentWidget() )
	{
		if( Use_Emulator_Type_Page )
		{
			ui.Wizard_Pages->setCurrentWidget( ui.Emulator_Type_Page );
			ui.Label_Page->setText( tr("Emulator Type") );
		}
		else
		{
			ui.Wizard_Pages->setCurrentWidget( ui.Wizard_Mode_Page );
			ui.Label_Page->setText( tr("Wizard Mode") );
		}
	}
	else if( ui.Emulator_Type_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Wizard_Mode_Page );
		ui.Label_Page->setText( tr("Wizard Mode") );
	}
	else if( ui.General_Settings_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Template_Page );
		ui.Label_Page->setText( tr("VM Hardware Template") );
	}
	else if( ui.Memory_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.General_Settings_Page );
		ui.Label_Page->setText( tr("VM Name and CPU Type") );
		ui.Label_Caption_CPU_Type->setVisible( true );
		ui.Line_CPU_Type->setVisible( true );
		ui.Label_CPU_Type->setVisible( true );
		ui.CB_CPU_Type->setVisible( true );
	}
	else if( ui.Typical_HDD_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.General_Settings_Page );
		ui.Label_Page->setText( tr("Virtual Machine Name") );
		ui.Label_Caption_CPU_Type->setVisible( false );
		ui.Line_CPU_Type->setVisible( false );
		ui.Label_CPU_Type->setVisible( false );
		ui.CB_CPU_Type->setVisible( false );
	}
	else if( ui.Custom_HDD_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Memory_Page );
		ui.Label_Page->setText( tr("Memory") );
	}
	else if( ui.Network_Page == ui.Wizard_Pages->currentWidget() )
	{
		if( ui.RB_Typical->isChecked() ) // typcal or custom mode
		{
			ui.Wizard_Pages->setCurrentWidget( ui.Typical_HDD_Page );
			ui.Label_Page->setText( tr("Hard Disk Size") );
		}
		else
		{
			ui.Wizard_Pages->setCurrentWidget( ui.Custom_HDD_Page );
			ui.Label_Page->setText( tr("Virtual Hard Disk") );
		}
	}
	else if( ui.Finish_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Network_Page );
		ui.Label_Page->setText( tr("Network") );
		ui.Button_Next->setText( tr("&Next") );
	}
	else
	{
		// Default
		AQError( "void VM_Wizard_Window::on_Button_Back_clicked()",
				 "Default Section!" );
	}
}

void VM_Wizard_Window::on_Button_Next_clicked()
{
	if( ui.Welcome_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Wizard_Mode_Page );
		ui.Button_Back->setEnabled( true );
		ui.Label_Page->setText( tr("Wizard Mode") );
	}
	else if( ui.Wizard_Mode_Page == ui.Wizard_Pages->currentWidget() )
	{
		bool q = ! Get_Default_Emulator( VM::QEMU ).Get_Name().isEmpty(); // FIXME what it?
		bool k = ! Get_Default_Emulator( VM::KVM ).Get_Name().isEmpty();
		
		if( q && k )
		{
			Use_Emulator_Type_Page = true;
			ui.Wizard_Pages->setCurrentWidget( ui.Emulator_Type_Page );
			ui.RB_Emulator_KVM->setChecked( true );
			ui.Label_Page->setText( tr("Emulator Type") );
		}
		else
		{
			if( q )
			{
				ui.RB_Emulator_QEMU->setChecked( true );
				Current_Emulator = Get_Default_Emulator( VM::QEMU );
			}
			else if( k )
			{
				ui.RB_Emulator_KVM->setChecked( true );
				Current_Emulator = Get_Default_Emulator( VM::KVM );
			}
			else
			{
				AQGraphic_Warning( tr("Error!"), tr("Please Add One Or More Emulators in Advanced Settings!") );
				return;
			}
			
			// All Find Systems
			All_Systems = Current_Emulator.Get_Devices();
			if( All_Systems.isEmpty() )
			{
				AQError( "void VM_Wizard_Window::on_Button_Next_clicked()",
						 "Cannot get devices!" );
				return;
			}
			
			// Comp types
			ui.CB_Computer_Type->clear();
			ui.CB_Computer_Type->addItem( tr("No Selected") );
			for( QMap<QString, Available_Devices>::const_iterator it = All_Systems.constBegin(); it != All_Systems.constEnd(); it++ )
				ui.CB_Computer_Type->addItem( it.value().System.Caption );
			
			Use_Emulator_Type_Page = false;
			ui.Wizard_Pages->setCurrentWidget( ui.Template_Page );
			on_RB_VM_Template_toggled( ui.RB_VM_Template->isChecked() );
			ui.Label_Page->setText( tr("Template For VM") );
		}
	}
	else if( ui.Emulator_Type_Page == ui.Wizard_Pages->currentWidget() )
	{
		if( ui.RB_Emulator_QEMU->isChecked() )
			Current_Emulator = Get_Default_Emulator( VM::QEMU );
		else if( ui.RB_Emulator_KVM->isChecked() )
			Current_Emulator = Get_Default_Emulator( VM::KVM );
		else
		{
			AQError( "void VM_Wizard_Window::on_Button_Next_clicked()",
					 "Emulator Type Not Set!" );
			return;
		}
		
		// All Find Systems FIXME ^^^
		All_Systems = Current_Emulator.Get_Devices();
		if( All_Systems.isEmpty() )
		{
			AQError( "void VM_Wizard_Window::on_Button_Next_clicked()",
					 "Cannot get devices!" );
			return;
		}
		
		// Comp types
		ui.CB_Computer_Type->clear();
		ui.CB_Computer_Type->addItem( tr("No Selected") );
		for( QMap<QString, Available_Devices>::const_iterator it = All_Systems.constBegin(); it != All_Systems.constEnd(); it++ )
			ui.CB_Computer_Type->addItem( it.value().System.Caption );
		
		ui.Wizard_Pages->setCurrentWidget( ui.Template_Page );
		on_RB_VM_Template_toggled( ui.RB_VM_Template->isChecked() );
		ui.Label_Page->setText( tr("Template For VM") );
	}
	else if( ui.Template_Page == ui.Wizard_Pages->currentWidget() )
	{
		// Use Selected Template
		if( ui.RB_VM_Template->isChecked() )
		{
			if( ! New_VM->Load_VM(OS_Templates_List[ui.CB_OS_Type->currentIndex()-1].filePath()) )
			{
				AQGraphic_Error( "void VM_Wizard_Window::Create_New_VM()", tr("Error!"),
								 tr("Cannot Create New VM from Template!") );
				return;
			}
		}
		
		// Emulator
		New_VM->Set_Emulator( Current_Emulator );
		
		// Find CPU List For This Template
		bool devices_finded = false;
		
		if( ui.RB_Emulator_KVM->isChecked() )
		{
			New_VM->Set_Computer_Type( "qemu-kvm" );
			
			if( New_VM->Get_Audio_Cards().Audio_es1370 )
			{
				VM::Sound_Cards tmp_audio = New_VM->Get_Audio_Cards();
				tmp_audio.Audio_es1370 = false;
				tmp_audio.Audio_AC97 = true;
				New_VM->Set_Audio_Cards( tmp_audio );
			}
			
			Current_Devices = &All_Systems[ "qemu-kvm" ];
			devices_finded = true;
		}
		else
		{
			Current_Devices = &All_Systems[ New_VM->Get_Computer_Type() ];
			if( ! Current_Devices->System.QEMU_Name.isEmpty() ) devices_finded = true;
		}
		
		// Use Selected Template
		if( ui.RB_VM_Template->isChecked() )
		{
			// Name
			ui.Edit_VM_Name->setText( New_VM->Get_Machine_Name() );
			
			// Memory
			ui.Memory_Size->setValue( New_VM->Get_Memory_Size() );
			
			// HDA
			double hda_size = New_VM->Get_HDA().Get_Virtual_Size_in_GB();
			
			if( hda_size != 0.0 )
			{
				ui.SB_HDD_Size->setValue( hda_size );
			}
			else
			{
				ui.SB_HDD_Size->setValue( 10.0 );
			}
			
			// Network
			ui.RB_User_Mode_Network->setChecked( New_VM->Get_Use_Network() );
			
			// Find CPU List For This Template
			Current_Devices = &All_Systems[ New_VM->Get_Computer_Type() ];
			if( ! Current_Devices->System.QEMU_Name.isEmpty() ) devices_finded = true;
		}
		else // Create New VM in Date Mode
		{
			// Select Memory Size, and HDD Size
			switch( ui.CB_Relese_Date->currentIndex() )
			{
				case 0:
					AQError( "void VM_Wizard_Window::Create_New_VM()",
							 "Relese Date Not Selected!" );
					ui.Memory_Size->setValue( 512 );
					break;
					
				case 1: // 1985-1990
					ui.Memory_Size->setValue( 16 );
					ui.SB_HDD_Size->setValue( 1.0 );
					break;
					
				case 2: // 1990-1995
					ui.Memory_Size->setValue( 64 );
					ui.SB_HDD_Size->setValue( 2.0 );
					break;
					
				case 3: // 1995-2000
					ui.Memory_Size->setValue( 256 );
					ui.SB_HDD_Size->setValue( 10.0 );
					break;
					
				case 4: // 2000-2005
					ui.Memory_Size->setValue( 512 );
					ui.SB_HDD_Size->setValue( 20.0 );
					break;
					
				case 5: // 2005-2010
					ui.Memory_Size->setValue( 1024 );
					ui.SB_HDD_Size->setValue( 40.0 );
					break;
					
				default:
					AQError( "void VM_Wizard_Window::Create_New_VM()",
							 "Relese Date Default Section!" );
					ui.Memory_Size->setValue( 512 );
					break;
			}
			
			// Find CPU List For This Template
			QString compCaption = ui.CB_Computer_Type->currentText();
			for( QMap<QString, Available_Devices>::const_iterator it = All_Systems.constBegin(); it != All_Systems.constEnd(); it++ )
			{
				if( it.value().System.Caption == compCaption )
				{
					Current_Devices = &it.value();
					if( ! Current_Devices->System.QEMU_Name.isEmpty() ) devices_finded = true;
				}
			}
		}
		
		if( ! devices_finded )
		{
			AQGraphic_Error( "void VM_Wizard_Window::on_Button_Next_clicked()", tr("Error!"),
							tr("Cannot Find Emulator System ID!") );
		}
		else
		{
			// Add CPU's
			ui.CB_CPU_Type->clear();
			for( int cx = 0; cx < Current_Devices->CPU_List.count(); ++cx )
				ui.CB_CPU_Type->addItem( Current_Devices->CPU_List[cx].Caption );
		}
		
		// Typical or custom mode
		if( ui.RB_Typical->isChecked() )
		{
			ui.Label_Page->setText( tr("Virtual Machine Name") );
			on_Edit_VM_Name_textEdited( ui.Edit_VM_Name->text() );
			
			ui.Label_Caption_CPU_Type->setVisible( false );
			ui.Line_CPU_Type->setVisible( false );
			ui.Label_CPU_Type->setVisible( false );
			ui.CB_CPU_Type->setVisible( false );
		}
		else
		{
			ui.Label_Page->setText( tr("VM Name and CPU Type") );
			on_Edit_VM_Name_textEdited( ui.Edit_VM_Name->text() );
			
			ui.Label_Caption_CPU_Type->setVisible( true );
			ui.Line_CPU_Type->setVisible( true );
			ui.Label_CPU_Type->setVisible( true );
			ui.CB_CPU_Type->setVisible( true );
		}
		
		// Next tab
		ui.Wizard_Pages->setCurrentWidget( ui.General_Settings_Page );
	}
	else if( ui.General_Settings_Page == ui.Wizard_Pages->currentWidget() )
	{
		for( int vx = 0; vx < VM_List->count(); ++vx )
		{
			if( VM_List->at(vx)->Get_Machine_Name() == ui.Edit_VM_Name->text() )
			{
				AQGraphic_Warning( tr("Warning"), tr("This VM Name is Already Exists!") );
				return;
			}
		}
		
		if( ui.RB_Typical->isChecked() )
		{
			ui.Wizard_Pages->setCurrentWidget( ui.Typical_HDD_Page );
			ui.Label_Page->setText( tr("Hard Disk Size") );
		}
		else
		{
			ui.Wizard_Pages->setCurrentWidget( ui.Memory_Page );
			ui.Label_Page->setText( tr("Memory") );
		}
	}
	else if( ui.Memory_Page == ui.Wizard_Pages->currentWidget() )
	{
		on_CH_Remove_RAM_Size_Limitation_stateChanged( Qt::Unchecked ); // It for update max avairable RAM size
		ui.Wizard_Pages->setCurrentWidget( ui.Custom_HDD_Page );
		ui.Label_Page->setText( tr("Virtual Hard Disk") );
	}
	else if( ui.Typical_HDD_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Network_Page );
		ui.Label_Page->setText( tr("Network") );
	}
	else if( ui.Custom_HDD_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Network_Page );
		ui.Label_Page->setText( tr("Network") );
	}
	else if( ui.Network_Page == ui.Wizard_Pages->currentWidget() )
	{
		ui.Wizard_Pages->setCurrentWidget( ui.Finish_Page );
		ui.Button_Next->setText( tr("&Finish") );
		ui.Label_Page->setText( tr("Finish!") );
	}
	else if( ui.Finish_Page == ui.Wizard_Pages->currentWidget() )
	{
		if( Create_New_VM() ) accept();
	}
	else
	{
		AQError( "void VM_Wizard_Window::on_Button_Next_clicked()",
				 "Default Section!" );
	}
}

bool VM_Wizard_Window::Load_OS_Templates()
{
	QList<QString> tmp_list = Get_Templates_List();
	
	for( int ax = 0; ax < tmp_list.count(); ++ax )
	{
		OS_Templates_List.append( QFileInfo(tmp_list[ax]) );
	}

	for( int ix = 0; ix < OS_Templates_List.count(); ++ix )
	{
		ui.CB_OS_Type->addItem( OS_Templates_List[ix].completeBaseName() );
	}
	
	 // no items found
	if( ui.CB_OS_Type->count() < 2 ) return false;
	else return true;
}

bool VM_Wizard_Window::Create_New_VM()
{
	// Icon
	QString icon_path = Find_OS_Icon( ui.Edit_VM_Name->text() );
	if( icon_path.isEmpty() )
	{
		AQWarning( "void VM_Wizard_Window::Create_New_VM()", "Icon for new VM not Found!" );
		New_VM->Set_Icon_Path( "other.png" );
	}
	else
	{
		New_VM->Set_Icon_Path( icon_path );
	}
	
	// Name
	New_VM->Set_Machine_Name( ui.Edit_VM_Name->text() );
	
	// Create path valid string
	QString VM_File_Name = Get_FS_Compatible_VM_Name( ui.Edit_VM_Name->text() );
	
	// Set Computer Type?
	if( ui.RB_Generate_VM->isChecked() )
	{
		New_VM->Set_Computer_Type( Current_Devices->System.QEMU_Name );
	}
	
	// RAM
	New_VM->Set_Memory_Size( ui.Memory_Size->value() );
	
	// Wizard Mode
	if( ui.RB_Typical->isChecked() )
	{
		// Hard Disk
		VM::Device_Size hd_size;
		hd_size.Size = ui.SB_HDD_Size->value();
		hd_size.Suffix = VM::Size_Suf_Gb;
		
		QString hd_path = Settings.value( "VM_Directory", "~" ).toString() + VM_File_Name;
		
		Create_New_HDD_Image( hd_path + "_HDA.img", hd_size );
		
		New_VM->Set_HDA( VM_HDD(true, hd_path + "_HDA.img") );
		
		// Other HDD's
		if( New_VM->Get_HDB().Get_Enabled() )
		{
			Create_New_HDD_Image( hd_path + "_HDB.img", New_VM->Get_HDB().Get_Virtual_Size() );
			New_VM->Set_HDB( VM_HDD(true, hd_path + "_HDB.img") );
		}
		
		if( New_VM->Get_HDC().Get_Enabled() )
		{
			Create_New_HDD_Image( hd_path + "_HDC.img", New_VM->Get_HDC().Get_Virtual_Size() );
			New_VM->Set_HDC( VM_HDD(true, hd_path + "_HDC.img") );
		}
		
		if( New_VM->Get_HDD().Get_Enabled() )
		{
			Create_New_HDD_Image( hd_path + "_HDD.img", New_VM->Get_HDD().Get_Virtual_Size() );
			New_VM->Set_HDD( VM_HDD(true, hd_path + "_HDD.img") );
		}
	}
	else
	{
		bool devices_finded = false;
		
		// CPU Type
		if( ui.RB_VM_Template->isChecked() )
		{
			// Find QEMU System Name in VM
			if( ui.RB_Emulator_KVM->isChecked() )
			{
				Current_Devices = &All_Systems[ "qemu-kvm" ];
				if( ! Current_Devices->System.QEMU_Name.isEmpty() ) devices_finded = true;
			}
			else // QEMU
			{
				Current_Devices = &All_Systems[ New_VM->Get_Computer_Type() ];
				if( ! Current_Devices->System.QEMU_Name.isEmpty() ) devices_finded = true;
			}
		}
		else
		{
			// Find QEMU System Name in CB_Computer_Type
			if( ui.RB_Emulator_KVM->isChecked() )
			{
				Current_Devices = &All_Systems[ "qemu-kvm" ];
				if( ! Current_Devices->System.QEMU_Name.isEmpty() ) devices_finded = true;
			}
			else // QEMU
			{
				for( QMap<QString, Available_Devices>::const_iterator it = All_Systems.constBegin(); it != All_Systems.constEnd(); it++ )
				{
					if( it.value().System.Caption == ui.CB_Computer_Type->currentText() )
					{
						Current_Devices = &it.value();
						devices_finded = true;
						break;
					}
				}
			}
		}
		
		if( ! devices_finded )
		{
			AQGraphic_Error( "bool VM_Wizard_Window::Create_New_VM()", tr("Error!"),
							 tr("Cannot Find QEMU System ID!") );
			return false;
		}
		
		New_VM->Set_CPU_Type( Current_Devices->CPU_List[ui.CB_CPU_Type->currentIndex()].QEMU_Name );
		
		// Hard Disk
		if( ! ui.Edit_HDA_File_Name->text().isEmpty() )
			New_VM->Set_HDA( VM_HDD(true, ui.Edit_HDA_File_Name->text()) );
		else
			New_VM->Set_HDA( VM_HDD(false, "") );
	}
	
	// Network
	if( ui.RB_User_Mode_Network->isChecked() )
	{
		if( New_VM->Get_Network_Cards_Count() == 0 )
		{
			New_VM->Set_Use_Network( true );
			VM_Net_Card net_card;
			net_card.Set_Net_Mode( VM::Net_Mode_Usermode );
			
			New_VM->Add_Network_Card( net_card );
		}
	}
	else if( ui.RB_No_Network->isChecked() )
	{
		New_VM->Set_Use_Network( false );
		
		for( int rx = 0; rx < New_VM->Get_Network_Cards_Count(); ++rx )
		{
			New_VM->Delete_Network_Card( 0 );
		}
	}
	
	// Set Emulator Name (version) to Default ("")
	Emulator tmp_emul = New_VM->Get_Emulator();
	tmp_emul.Set_Name( "" );
	New_VM->Set_Emulator( tmp_emul );
	
	// Create New VM XML File
	New_VM->Create_VM_File( Settings.value("VM_Directory", "~").toString() + VM_File_Name + ".aqemu", false );
	
	return true;
}

QString VM_Wizard_Window::Find_OS_Icon( const QString os_name )
{
	if( os_name.isEmpty() )
	{
		AQError( "QString VM_Wizard_Window::Find_OS_Icon( const QString os_name )",
				 "os_name is Empty!" );
		return "";
	}
	else
	{
		// Find all os icons
		QDir icons_dir( QDir::toNativeSeparators(Settings.value("AQEMU_Data_Folder","").toString() + "/os_icons/") );
		QFileInfoList all_os_icons = icons_dir.entryInfoList( QStringList("*.png"), QDir::Files, QDir::Unsorted );
		
		QRegExp rex;
		rex.setPatternSyntax( QRegExp::Wildcard );
		rex.setCaseSensitivity( Qt::CaseInsensitive );
		
		for( int i = 0; i < all_os_icons.count(); i++ )
		{
			rex.setPattern( "*" + all_os_icons[i].baseName() + "*" );
			
			if( rex.exactMatch(os_name) )
			{
				return all_os_icons[ i ].absoluteFilePath();
			}
		}
		
		// select os family...
		
		// Linux
		rex.setPattern( "*linux*" );
		if( rex.exactMatch(os_name) )
		{
			return ":/images/default_linux.png";
		}
		
		// Windows
		rex.setPattern( "*windows*" );
		if( rex.exactMatch(os_name) )
		{
			return ":/images/default_windows.png";
		}
		
		return ":/images/other.png";
	}
}

void VM_Wizard_Window::on_RB_VM_Template_toggled( bool on )
{
	if( on )
	{
		if( ui.CB_OS_Type->currentIndex() == 0 )
		{
			ui.Button_Next->setEnabled( false );
		}
		else
		{
			ui.Button_Next->setEnabled( true );
		}
	}
}

void VM_Wizard_Window::on_RB_Generate_VM_toggled( bool on )
{
	if( on )
	{
		if( ui.CB_Computer_Type->currentIndex() == 0 ||
		  	ui.CB_Relese_Date->currentIndex() == 0 )
		{
			ui.Button_Next->setEnabled( false );
		}
		else
		{
			ui.Button_Next->setEnabled( true );
		}
	}
}

void VM_Wizard_Window::on_CB_OS_Type_currentIndexChanged( int index )
{
	if( index == 0 )
	{
		ui.Button_Next->setEnabled( false );
	}
	else
	{
		ui.Button_Next->setEnabled( true );
	}
}

void VM_Wizard_Window::on_CB_Computer_Type_currentIndexChanged( int index )
{
	if( index == 0 )
	{
		ui.Button_Next->setEnabled( false );
	}
	else
	{
		if( ui.CB_Relese_Date->currentIndex() != 0 )
		{
			ui.Button_Next->setEnabled( true );
		}
	}
}

void VM_Wizard_Window::on_CB_Relese_Date_currentIndexChanged( int index )
{
	if( index == 0 )
	{
		ui.Button_Next->setEnabled( false );
	}
	else
	{
		if( ui.CB_Computer_Type->currentIndex() != 0 )
		{
			ui.Button_Next->setEnabled( true );
		}
	}
}

void VM_Wizard_Window::on_Memory_Size_valueChanged( int value )
{
	int cursorPos = ui.CB_RAM_Size->lineEdit()->cursorPosition();
	
	if( value % 1024 == 0 ) ui.CB_RAM_Size->setEditText( QString("%1 GB").arg(value / 1024) );
	else ui.CB_RAM_Size->setEditText( QString("%1 MB").arg(value) );
	
	ui.CB_RAM_Size->lineEdit()->setCursorPosition( cursorPos );
}

void VM_Wizard_Window::on_CB_RAM_Size_editTextChanged( const QString &text )
{
	if( text.isEmpty() ) return;
	
	QRegExp rx( "\\s*([\\d]+)\\s*(MB|GB|M|G|)\\s*" ); // like: 512MB or 512
	if( ! rx.exactMatch(text.toUpper()) )
	{
		AQGraphic_Warning( tr("Error"),
						   tr("Cannot convert \"%1\" to memory size!").arg(text) );
		return;
	}
	
	QStringList ramStrings = rx.capturedTexts();
	if( ramStrings.count() != 3 )
	{
		AQGraphic_Warning( tr("Error"),
						   tr("Cannot convert \"%1\" to memory size!").arg(text) );
		return;
	}
	
	bool ok = false;
	int value = ramStrings[1].toInt( &ok, 10 );
	if( ! ok )
	{
		AQGraphic_Warning( tr("Error"),
						   tr("Cannot convert \"%1\" to integer!").arg(ramStrings[1]) );
		return;
	}
	
	if( ramStrings[2] == "MB" || ramStrings[2] == "M" ); // Size in megabytes
	else if( ramStrings[2] == "GB" || ramStrings[2] == "G" ) value *= 1024;
	else
	{
		AQGraphic_Warning( tr("Error"),
						   tr("Cannot convert \"%1\" to size suffix! Valid suffixes: MB, GB").arg(ramStrings[2]) );
		return;
	}
	
	if( value <= 0 )
	{
		AQGraphic_Warning( tr("Error"), tr("Memory size < 0! Valid size is 1 or more") );
		return;
	}
	
	on_TB_Update_Available_RAM_Size_clicked();
	if( (value > ui.Memory_Size->maximum()) &&
		(ui.CH_Remove_RAM_Size_Limitation->isChecked() == false) )
	{
		AQGraphic_Warning( tr("Error"),
						   tr("Your memory size %1 MB > %2 MB - all free RAM on this system!\n"
							  "To setup this value, check \"Remove limitation on maximum amount of memory\".")
						   .arg(value).arg(ui.Memory_Size->maximum()) );
		
		on_Memory_Size_valueChanged( ui.Memory_Size->value() ); // Set valid size
		return;
	}
	
	// All OK. Set memory size
	ui.Memory_Size->setValue( value );
}

void VM_Wizard_Window::on_CH_Remove_RAM_Size_Limitation_stateChanged( int state )
{
	if( state == Qt::Checked )
	{
		ui.Memory_Size->setMaximum( 32768 );
		ui.Label_Available_Free_Memory->setText( "32 GB" );
		Update_RAM_Size_ComboBox( 32768 );
	}
	else
	{
		int allRAM = 0, freeRAM = 0;
		System_Info::Get_Free_Memory_Size( allRAM, freeRAM );
		
		if( allRAM < ui.Memory_Size->value() )
			AQGraphic_Warning( tr("Error"), tr("Current memory size bigger than all existing host memory!\nUsing maximum available size.") );
		
		ui.Memory_Size->setMaximum( allRAM );
		ui.Label_Available_Free_Memory->setText( QString("%1 MB").arg(allRAM) );
		Update_RAM_Size_ComboBox( allRAM );
	}
}

void VM_Wizard_Window::on_TB_Update_Available_RAM_Size_clicked()
{
	int allRAM = 0, freeRAM = 0;
	System_Info::Get_Free_Memory_Size( allRAM, freeRAM );
	ui.TB_Update_Available_RAM_Size->setText( tr("Free memory: %1 MB").arg(freeRAM) );
	
	if( ! ui.CH_Remove_RAM_Size_Limitation->isChecked() )
	{
		ui.Memory_Size->setMaximum( allRAM );
		Update_RAM_Size_ComboBox( allRAM );
	}
}

void VM_Wizard_Window::Update_RAM_Size_ComboBox( int freeRAM )
{
	static int oldRamSize = 0;
	if( freeRAM == oldRamSize ) return;
	else oldRamSize = freeRAM;
	
	QStringList ramSizes;
	ramSizes << "32 MB" << "64 MB" << "128 MB" << "256 MB" << "512 MB"
			 << "1 GB" << "2 GB" << "3 GB" << "4 GB" << "8 GB" << "16 GB" << "32 GB";
	int maxRamIndex = 0;
	if( freeRAM >= 32768 ) maxRamIndex = 12;
	else if( freeRAM >= 16384 ) maxRamIndex = 11;
	else if( freeRAM >= 8192 ) maxRamIndex = 10;
	else if( freeRAM >= 4096 ) maxRamIndex = 9;
	else if( freeRAM >= 3072 ) maxRamIndex = 8;
	else if( freeRAM >= 2048 ) maxRamIndex = 7;
	else if( freeRAM >= 1024 ) maxRamIndex = 6;
	else if( freeRAM >= 512 ) maxRamIndex = 5;
	else if( freeRAM >= 256 ) maxRamIndex = 4;
	else if( freeRAM >= 128 ) maxRamIndex = 3;
	else if( freeRAM >= 64 ) maxRamIndex = 2;
	else if( freeRAM >= 32 ) maxRamIndex = 1;
	else
	{
		AQGraphic_Warning( tr("Error"), tr("Free memory on this system is lower than 32 MB!") );
		return;
	}
	
	if( maxRamIndex > ramSizes.count() )
	{
		AQError( "void VM_Wizard_Window::Update_RAM_Size_ComboBox( int freeRAM )",
				 "maxRamIndex > ramSizes.count()" );
		return;
	}
	
	QString oldText = ui.CB_RAM_Size->currentText();
	
	ui.CB_RAM_Size->clear();
	for( int ix = 0; ix < maxRamIndex; ix++ ) ui.CB_RAM_Size->addItem( ramSizes[ix] );
	
	ui.CB_RAM_Size->setEditText( oldText );
}

void VM_Wizard_Window::on_Edit_VM_Name_textEdited( const QString &text )
{
	if( ui.Edit_VM_Name->text().isEmpty() ) ui.Button_Next->setEnabled( false );
	else ui.Button_Next->setEnabled( true );
}

void VM_Wizard_Window::on_Button_New_HDD_clicked()
{
	Create_HDD_Image_Window *Create_HDD_Win = new Create_HDD_Image_Window( this );
	
	Create_HDD_Win->Set_Image_Size( ui.SB_HDD_Size->value() ); // Set Initial HDA Size
	
	if( Create_HDD_Win->exec() == QDialog::Accepted )
		ui.Edit_HDA_File_Name->setText( Create_HDD_Win->Get_Image_File_Name() );
	
	delete Create_HDD_Win;
}

void VM_Wizard_Window::on_Button_Existing_clicked()
{
	QString hddPath = QFileDialog::getOpenFileName( this, tr("Select HDD Image"),
													Get_Last_Dir_Path(ui.Edit_HDA_File_Name->text()),
													tr("All Files (*)") );

	if( ! hddPath.isEmpty() )
		ui.Edit_HDA_File_Name->setText( QDir::toNativeSeparators(hddPath) );
}
