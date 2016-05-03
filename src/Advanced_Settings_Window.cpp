/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
** Copyright (C) 2016 Tobias Gläßer (Qt5 port)
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
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>

#include "Advanced_Settings_Window.h"
#include "System_Info.h"
#include "Utils.h"
#include "Emulator_Options_Window.h"
#include "First_Start_Wizard.h"
#include "Settings_Widget.h"
#include "Create_Template_Window.h"

Advanced_Settings_Window::Advanced_Settings_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );

    new Settings_Widget( ui.All_Tabs, QBoxLayout::TopToBottom );
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Emulators_Table );
	hv->setSectionResizeMode( QHeaderView::Fixed );
	ui.Emulators_Table->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Emulators_Table );
	hv->setSectionResizeMode( QHeaderView::Stretch );
	ui.Emulators_Table->setHorizontalHeader( hv );
	
	// Minimum Size
	resize( minimumSizeHint().width(), minimumSizeHint().height() );
	
	// Execute Before Start QEMU
	ui.Edit_Before_Start_Command->setText( Settings.value("Run_Before_QEMU", "").toString() );
	
	// Execute After Stop QEMU
	ui.Edit_After_Stop_Command->setText( Settings.value("Run_After_QEMU", "").toString() );
	
	// Use Shared Folder For All Screenshots
	ui.CH_Screenshot_Folder->setChecked( Settings.value("Use_Screenshots_Folder", "no").toString() == "yes" );
	
	// Screenshots Shared Folder Path
	ui.Edit_Screenshot_Folder->setText( Settings.value("Screenshot_Folder_Path", "").toString() );
	
	// Screenshot save format
	QString fmt = Settings.value( "Screenshot_Save_Format", "PNG" ).toString();
	
	if( fmt == "PNG" ) ui.RB_Format_PNG->setChecked( true );
	else if( fmt == "JPEG" ) ui.RB_Format_Jpeg->setChecked( true );
	else ui.RB_Format_PPM->setChecked( true );
	
	// Jpeg Quality
	ui.HS_Jpeg_Quality->setValue( Settings.value("Jpeg_Quality", "75").toString().toInt() );
	
	// Additional CDROM
	int cdrom_count = Settings.value( "Additional_CDROM_Devices/Count", "0" ).toString().toInt();
	
	for( int ix = 0; ix < cdrom_count; ix++ )
	{
		ui.CDROM_List->addItem( Settings.value("Additional_CDROM_Devices/Device" + QString::number(ix), "").toString() );
	}
	
	// Information in Info Tab
	ui.CH_Show_Tab_Info->setChecked( Settings.value("Info/Show_Tab_Info", "yes").toString() == "yes" );
	ui.CH_Show_QEMU_Args->setChecked( Settings.value("Info/Show_QEMU_Args", "no").toString() == "yes" );
	ui.CH_Show_Screenshot_in_Save_Mode->setChecked( Settings.value("Info/Show_Screenshot_in_Save_Mode", "no").toString() == "yes" );
	
	ui.CH_Machine_Details->setChecked( Settings.value("Info/Machine_Details", "yes").toString() == "yes" );
	
	ui.CH_Machine_Name->setChecked( Settings.value("Info/Machine_Name", "no").toString() == "yes" );
	ui.CH_Machine_Accelerator->setChecked( Settings.value("Info/Machine_Accelerator", "yes").toString() == "yes" );
	ui.CH_Computer_Type->setChecked( Settings.value("Info/Computer_Type", "yes").toString() == "yes" );
	ui.CH_Machine_Type->setChecked( Settings.value("Info/Machine_Type", "no").toString() == "yes" );
	ui.CH_Boot_Priority->setChecked( Settings.value("Info/Boot_Priority", "yes").toString() == "yes" );
	ui.CH_CPU_Type->setChecked( Settings.value("Info/CPU_Type", "no").toString() == "yes" );
	ui.CH_Number_of_CPU->setChecked( Settings.value("Info/Number_of_CPU", "yes").toString() == "yes" );
	ui.CH_Video_Card->setChecked( Settings.value("Info/Video_Card", "yes").toString() == "yes" );
	ui.CH_Keyboard_Layout->setChecked( Settings.value("Info/Keyboard_Layout", "no").toString() == "yes" );
	
	ui.CH_Memory_Size->setChecked( Settings.value("Info/Memory_Size", "yes").toString() == "yes" );
	ui.CH_Use_Sound->setChecked( Settings.value("Info/Use_Sound", "yes").toString() == "yes" );
	
	ui.CH_Fullscreen->setChecked( Settings.value("Info/Fullscreen", "yes").toString() == "yes" );
	ui.CH_Snapshot->setChecked( Settings.value("Info/Snapshot", "yes").toString() == "yes" );
	ui.CH_Localtime->setChecked( Settings.value("Info/Localtime", "yes").toString() == "yes" );
	
	ui.CH_Show_FDD->setChecked( Settings.value("Info/Show_FDD", "yes").toString() == "yes" );
	ui.CH_Show_CD->setChecked( Settings.value("Info/Show_CD", "yes").toString() == "yes" );
	ui.CH_Show_HDD->setChecked( Settings.value("Info/Show_HDD", "yes").toString() == "yes" );
	
	ui.CH_Network_Cards->setChecked( Settings.value("Info/Network_Cards", "yes").toString() == "yes" );
	ui.CH_Redirections->setChecked( Settings.value("Info/Redirections", "no").toString() == "yes" );
	
	ui.CH_Serial_Port->setChecked( Settings.value("Info/Serial_Port", "yes").toString() == "yes" );
	ui.CH_Parallel_Port->setChecked( Settings.value("Info/Parallel_Port", "yes").toString() == "yes" );
	ui.CH_USB_Port->setChecked( Settings.value("Info/USB_Port", "yes").toString() == "yes" );
	
	ui.CH_Win2K_Hack->setChecked( Settings.value("Info/Win2K_Hack", "no").toString() == "yes" );
	ui.CH_RTC_TD_Hack->setChecked( Settings.value("Info/RTC_TD_Hack", "no").toString() == "yes" );
	ui.CH_No_Shutdown->setChecked( Settings.value("Info/No_Shutdown", "no").toString() == "yes" );
	ui.CH_No_Reboot->setChecked( Settings.value("Info/No_Reboot", "no").toString() == "yes" );
	ui.CH_Start_CPU->setChecked( Settings.value("Info/Start_CPU", "no").toString() == "yes" );
	ui.CH_Check_Boot_on_FDD->setChecked( Settings.value("Info/Check_Boot_on_FDD", "no").toString() == "yes" );
	ui.CH_ACPI->setChecked( Settings.value("Info/ACPI", "no").toString() == "yes" );
	ui.CH_Start_Date->setChecked( Settings.value("Info/Start_Date", "no").toString() == "yes" );
	
	ui.CH_No_Frame->setChecked( Settings.value("Info/No_Frame", "no").toString() == "yes" );
	ui.CH_Alt_Grab->setChecked( Settings.value("Info/Alt_Grab", "no").toString() == "yes" );
	ui.CH_No_Quit->setChecked( Settings.value("Info/No_Quit", "no").toString() == "yes" );
	ui.CH_Portrait->setChecked( Settings.value("Info/Portrait", "no").toString() == "yes" );
	ui.CH_Curses->setChecked( Settings.value("Info/Curses", "no").toString() == "yes" );
	ui.CH_Show_Cursor->setChecked( Settings.value("Info/Show_Cursor", "no").toString() == "yes" );
	ui.CH_Init_Graphical_Mode->setChecked( Settings.value("Info/Init_Graphical_Mode", "no").toString() == "yes" );
	
	ui.CH_ROM_File->setChecked( Settings.value("Info/ROM_File", "no").toString() == "yes" );
	ui.CH_MTDBlock->setChecked( Settings.value("Info/MTDBlock", "no").toString() == "yes" );
	ui.CH_SD_Image->setChecked( Settings.value("Info/SD_Image", "no").toString() == "yes" );
	ui.CH_PFlash->setChecked( Settings.value("Info/PFlash", "no").toString() == "yes" );
	
	ui.CH_Linux_Boot->setChecked( Settings.value("Info/Linux_Boot", "no").toString() == "yes" );
	ui.CH_VNC->setChecked( Settings.value("Info/VNC", "no").toString() == "yes" );
	ui.CH_SPICE->setChecked( Settings.value("Info/SPICE", "no").toString() == "yes" );
	
	// MAC Address Generation Mode
	ui.RB_MAC_Random->setChecked( Settings.value("MAC_Generation_Mode", "Model").toString() == "Random" );
	ui.RB_MAC_QEMU->setChecked( Settings.value("MAC_Generation_Mode", "Model").toString() == "QEMU_Segment" );
	ui.RB_MAC_Valid->setChecked( Settings.value("MAC_Generation_Mode", "Model").toString() == "Model" );
	
	// Save to Log File
	ui.CH_Log_Save_in_File->setChecked( Settings.value("Log/Save_In_File", "yes").toString() == "yes" );
	
	// Print In StdOut
	ui.CH_Log_Print_in_STDIO->setChecked( Settings.value("Log/Print_In_STDOUT", "yes").toString() == "yes" );
	
	// Log File Path
	ui.Edit_Log_Path->setText( Settings.value("Log/Log_Path", Settings.value("VM_Directory", "").toString() + "aqemu.log").toString() );
	
	// Save to AQEMU Log
	ui.CH_Log_Debug->setChecked( Settings.value("Log/Save_Debug", "yes").toString() == "yes" );
	ui.CH_Log_Warning->setChecked( Settings.value("Log/Save_Warning", "yes").toString() == "yes" );
	ui.CH_Log_Error->setChecked( Settings.value("Log/Save_Error", "yes").toString() == "yes" );
	
	// QEMU-IMG Path
	ui.Edit_QEMU_IMG_Path->setText( Settings.value("QEMU-IMG_Path", "qemu-img").toString() );
	
	// Recent CD Count
	ui.SB_Recent_CD_Count->setValue( Settings.value("CD_ROM_Exits_Images/Max", "5").toString().toInt() );
	
	// Recent FDD Count
	ui.SB_Recent_FDD_Count->setValue( Settings.value("Floppy_Exits_Images/Max", "5").toString().toInt() );
	
	// First VNC Port for Embedded Display
	ui.SB_First_VNC_Port->setValue( Settings.value("First_VNC_Port", "5910").toString().toInt() );
	
	// QEMU Monitor Type
	#ifdef Q_OS_WIN32
	ui.RB_Monitor_STDIO->setEnabled( false );
	ui.RB_Monitor_TCP->setChecked( true );
	#else
	ui.RB_Monitor_TCP->setChecked( Settings.value("Emulator_Monitor_Type", "stdio").toString() == "tcp" );
	#endif
	ui.CB_Monitor_Hostname->setEditText( Settings.value("Emulator_Monitor_Hostname", "localhost").toString() );
	ui.SB_Monitor_Port->setValue( Settings.value("Emulator_MonGitor_Port", 6000).toInt() );

	// QEMU_AUDIO
	ui.CH_Audio_Default->setChecked( Settings.value("QEMU_AUDIO/Use_Default_Driver", "yes").toString() == "no" );
	
	// QEMU_AUDIO_DRV
	for( int ix = 0; ix < ui.CB_Host_Sound_System->count(); ++ix )
	{
		if( ui.CB_Host_Sound_System->itemText(ix) ==
			Settings.value("QEMU_AUDIO/QEMU_AUDIO_DRV", "alsa").toString() )
		{
			ui.CB_Host_Sound_System->setCurrentIndex( ix );
		}
	}
	
	// Tab USB
	ui.RB_USB_Style_device->setChecked( Settings.value("USB_Style", "device").toString() == "device" );
	
	QString usbID = Settings.value( "USB_ID_Style", "BusPath" ).toString();
	
	if( usbID == "BusAdd" )
		ui.RB_USB_ID_BusAddr->setChecked( true );
	else if( usbID == "VendorProduct" )
		ui.RB_USB_ID_VendorProduct->setChecked( true );
	else
		ui.RB_USB_ID_BusPath->setChecked( true );
	
	// Update emulators list
	if( Load_Emulators_Info() ) Update_Emulators_Info();

    ///////////////////////
    // Old Settings Window

	
	// Minimum Size
	resize( width(), minimumSizeHint().height() );
	
	// Emulator Control
	bool emul_show = Settings.value( "Show_Emulator_Control_Window", "yes" ).toString() == "yes";
	bool emul_vnc = Settings.value( "Use_VNC_Display", "no" ).toString() == "yes";
	bool emul_include = Settings.value( "Include_Emulator_Control", "yes" ).toString() == "yes";
	
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
	ui.Edit_VM_Folder->setText( QDir::toNativeSeparators(Settings.value("VM_Directory", QDir::homePath() + "/.aqemu/").toString()) );
	
	// Use New Emulator Control Removable Device Menu
	ui.CH_Use_New_Device_Changer->setChecked( Settings.value("Use_New_Device_Changer", "no").toString() == "yes" );
	
	// Find All Language Files (*.qm)
	QDir data_dir( Settings.value("AQEMU_Data_Folder", "/usr/share/aqemu/").toString() );
	QFileInfoList lang_files = data_dir.entryInfoList( QStringList("*.qm"), QDir::Files, QDir::Name );
	
	if( lang_files.count() > 0 )
	{
		// Add Languages to List
		for( int dd = 0; dd < lang_files.count(); ++dd )
		{
			ui.CB_Language->addItem( lang_files[dd].completeBaseName() );
			
			if( lang_files[dd].completeBaseName() == Settings.value( "Language", "en" ).toString() )
			{
				ui.CB_Language->setCurrentIndex( dd + 1 ); // First Item 'English'
			}
		}
	}
	
	// VM Icons Size
	switch( Settings.value("VM_Icons_Size", "48").toInt() )
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
	ui.CH_Screenshot_for_OS_Logo->setChecked( Settings.value("Use_Screenshot_for_OS_Logo", "yes").toString() == "yes" );
	
	Load_Templates();
	
	connect( ui.RB_Emul_Show_VNC, SIGNAL(toggled(bool)),
			 this, SLOT(VNC_Warning(bool)) );
	
	connect( ui.RB_Emul_Show_VNC_In_Main_Window, SIGNAL(toggled(bool)),
			 this, SLOT(VNC_Warning(bool)) );
	
	connect( ui.CB_Language, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(CB_Language_currentIndexChanged(int)) );
}

void Advanced_Settings_Window::Load_Templates()
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
			if( ui.CB_Default_VM_Template->itemText(ix) == Settings.value("Default_VM_Template", "Linux 2.6").toString() )
			{
				ui.CB_Default_VM_Template->setCurrentIndex( ix );
			}
		}
	}
}


void Advanced_Settings_Window::on_Button_Create_Template_from_VM_clicked()
{
	Create_Template_Window *templ_win = new Create_Template_Window();
	
	if( templ_win->exec() == QDialog::Accepted )
	{
		Load_Templates();
		QMessageBox::information( this, tr("Information"), tr("New template was created!") );
	}
	
	delete templ_win;
}

void Advanced_Settings_Window::on_TB_VM_Folder_clicked()
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

void Advanced_Settings_Window::CB_Language_currentIndexChanged( int index )
{
	if( Settings.value("Show_Language_Warning", "yes").toString() == "yes" )
	{
		if( QMessageBox::information(this, tr("Information"),
			tr("Language will set after restarting AQEMU\nShow this message in future?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::No )
		{
			Settings.setValue( "Show_Language_Warning", "no" );
		}
	}
}

void Advanced_Settings_Window::CB_Icons_Theme_currentIndexChanged( int index )
{
	if( Settings.value("Show_Icons_Theme_Warning", "yes").toString() == "yes" )
	{
		if( QMessageBox::information(this, tr("Information"),
			tr("Icons theme be set after restart AQEMU\nShow this message in future?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::No )
		{
			Settings.setValue( "Show_Icons_Theme_Warning", "no" );
		}
	}
}


void Advanced_Settings_Window::VNC_Warning( bool state )
{
	if( ! state ) return;
	
	if( Settings.value("Show_VNC_Warning", "yes").toString() == "yes" )
	{
		if( QMessageBox::information(this, tr("Information"),
			tr("Support for this feature is not complete! If there is no picture, click \"View->Reinit VNC\"\nShow This Message Again?"),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::No )
		{
			Settings.setValue( "Show_VNC_Warning", "no" );
		}
	}
}

void Advanced_Settings_Window::on_Button_OK_clicked()
{
	// Execute Before Start QEMU
	Settings.setValue( "Run_Before_QEMU", ui.Edit_Before_Start_Command->text() );
	
	// Execute After Stop QEMU
	Settings.setValue( "Run_After_QEMU", ui.Edit_After_Stop_Command->text() );
	
	// Use Shared Folder For All Screenshots
	if( ui.CH_Screenshot_Folder->isChecked() )
	{
		Settings.setValue( "Use_Screenshots_Folder", "yes" );
		
		QDir dir; // For Check on valid
		
		// Screenshots Shared Folder Path
		if( dir.exists(ui.Edit_Screenshot_Folder->text()) )
		{
			Settings.setValue( "Screenshot_Folder_Path", ui.Edit_Screenshot_Folder->text() );
		}
		else
		{
			AQGraphic_Warning( tr("Invalid Value!"), tr("Shared screenshot folder doesn't exist!") );
			return;
		}
	}
	else
	{
		Settings.setValue( "Use_Screenshots_Folder", "no" );
		
		// Screenshots Shared Folder Path
		Settings.setValue( "Screenshot_Folder_Path", ui.Edit_Screenshot_Folder->text() );
	}
	
	// Screenshot save format
	if( ui.RB_Format_PNG->isChecked() ) Settings.setValue( "Screenshot_Save_Format", "PNG" );
	else if( ui.RB_Format_Jpeg->isChecked() ) Settings.setValue( "Screenshot_Save_Format", "JPEG" );
	else Settings.setValue( "Screenshot_Save_Format", "PPM" );
	
	// Jpeg Quality
	Settings.setValue( "Jpeg_Quality", QString::number(ui.HS_Jpeg_Quality->value()) );
	
	// Additional CDROM
	int old_count = Settings.value( "Additional_CDROM_Devices/Count", "0" ).toString().toInt();
	
	if( old_count > ui.CDROM_List->count() )
	{
		// Delete Old Items
		for( int dx = ui.CDROM_List->count()-1; dx < old_count; dx++ )
		{
			Settings.remove( "Additional_CDROM_Devices/Device" + QString::number(dx) );
		}
	}
	
	Settings.setValue( "Additional_CDROM_Devices/Count", QString::number(ui.CDROM_List->count()) );
	
	for( int ix = 0; ix < ui.CDROM_List->count(); ix++ )
	{
		Settings.setValue( "Additional_CDROM_Devices/Device" + QString::number(ix), ui.CDROM_List->item(ix)->text() );
	}
	
	Settings.setValue( "Info/Show_Tab_Info", ui.CH_Show_Tab_Info->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Show_QEMU_Args", ui.CH_Show_QEMU_Args->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Show_Screenshot_in_Save_Mode", ui.CH_Show_Screenshot_in_Save_Mode->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Machine_Details", ui.CH_Machine_Details->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Machine_Name", ui.CH_Machine_Name->isChecked() ? "no" : "no" );
	Settings.setValue( "Info/Machine_Accelerator", ui.CH_Machine_Accelerator->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Computer_Type", ui.CH_Computer_Type->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Machine_Type", ui.CH_Machine_Type->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Boot_Priority", ui.CH_Boot_Priority->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/CPU_Type", ui.CH_CPU_Type->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Number_of_CPU", ui.CH_Number_of_CPU->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Video_Card", ui.CH_Video_Card->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Keyboard_Layout", ui.CH_Keyboard_Layout->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Memory_Size", ui.CH_Memory_Size->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Use_Sound", ui.CH_Use_Sound->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Fullscreen", ui.CH_Fullscreen->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Snapshot", ui.CH_Snapshot->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Localtime", ui.CH_Localtime->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Show_FDD", ui.CH_Show_FDD->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Show_CD", ui.CH_Show_CD->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Show_HDD", ui.CH_Show_HDD->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Network_Cards", ui.CH_Network_Cards->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Redirections", ui.CH_Redirections->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Serial_Port", ui.CH_Serial_Port->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Parallel_Port", ui.CH_Parallel_Port->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/USB_Port", ui.CH_USB_Port->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Win2K_Hack", ui.CH_Win2K_Hack->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/RTC_TD_Hack", ui.CH_RTC_TD_Hack->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/No_Shutdown",  ui.CH_No_Shutdown->isChecked()? "yes" : "no" );
	Settings.setValue( "Info/No_Reboot", ui.CH_No_Reboot->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Start_CPU", ui.CH_Start_CPU->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Check_Boot_on_FDD", ui.CH_Check_Boot_on_FDD->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/ACPI", ui.CH_ACPI->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Start_Date", ui.CH_Start_Date->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/No_Frame", ui.CH_No_Frame->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Alt_Grab", ui.CH_Alt_Grab->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/No_Quit", ui.CH_No_Quit->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Portrait", ui.CH_Portrait->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Curses", ui.CH_Curses->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Show_Cursor", ui.CH_Show_Cursor->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Init_Graphical_Mode", ui.CH_Init_Graphical_Mode->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/ROM_File", ui.CH_ROM_File->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/MTDBlock", ui.CH_MTDBlock->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/SD_Image", ui.CH_SD_Image->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/PFlash", ui.CH_PFlash->isChecked() ? "yes" : "no" );
	
	Settings.setValue( "Info/Linux_Boot", ui.CH_Linux_Boot->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/VNC", ui.CH_VNC->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/SPICE", ui.CH_SPICE->isChecked() ? "yes" : "no" );
	
	// MAC Address Generation Mode
	if( ui.RB_MAC_Random->isChecked() ) Settings.setValue( "MAC_Generation_Mode", "Random" );
	else if( ui.RB_MAC_QEMU->isChecked() ) Settings.setValue( "MAC_Generation_Mode", "QEMU_Segment" );
	else if( ui.RB_MAC_Valid->isChecked() ) Settings.setValue( "MAC_Generation_Mode", "Model" );
	
	// Save to Log File
	if( ui.CH_Log_Save_in_File->isChecked() ) Settings.setValue( "Log/Save_In_File", "yes" );
	else Settings.setValue( "Log/Save_In_File", "no" );
	
	// Print In StdOut
	if( ui.CH_Log_Print_in_STDIO->isChecked() ) Settings.setValue( "Log/Print_In_STDOUT", "yes" );
	else Settings.setValue( "Log/Print_In_STDOUT", "no" );
	
	// Log File Path
	Settings.setValue( "Log/Log_Path", ui.Edit_Log_Path->text() );
	
	// Save to AQEMU Log
	if( ui.CH_Log_Debug->isChecked() ) Settings.setValue( "Log/Save_Debug", "yes" );
	else Settings.setValue( "Log/Save_Debug", "no" );
	
	if( ui.CH_Log_Warning->isChecked() ) Settings.setValue( "Log/Save_Warning", "yes" );
	else Settings.setValue( "Log/Save_Warning", "no" );
	
	if( ui.CH_Log_Error->isChecked() ) Settings.setValue( "Log/Save_Error", "yes" );
	else Settings.setValue( "Log/Save_Error", "no" );
	
	// QEMU-IMG Path
	Settings.setValue( "QEMU-IMG_Path", ui.Edit_QEMU_IMG_Path->text() );
	
	// QEMU_AUDIO
	if( ui.CH_Audio_Default->isChecked() )
		Settings.setValue( "QEMU_AUDIO/Use_Default_Driver", "no" );
	else
		Settings.setValue( "QEMU_AUDIO/Use_Default_Driver", "yes" );
	
	// QEMU_AUDIO_DRV
	Settings.setValue( "QEMU_AUDIO/QEMU_AUDIO_DRV", ui.CB_Host_Sound_System->currentText() );
	
	// Recent CD Count
	Settings.setValue( "CD_ROM_Exits_Images/Max", QString::number(ui.SB_Recent_CD_Count->value()) );
	
	// Recent FDD Count
	Settings.setValue( "Floppy_Exits_Images/Max", QString::number(ui.SB_Recent_FDD_Count->value()) );
	
	// First VNC Port for Embedded Display
	Settings.setValue( "First_VNC_Port", QString::number(ui.SB_First_VNC_Port->value()) );
	
	// QEMU Monitor Type
	#ifdef Q_OS_WIN32
	Settings.setValue( "Emulator_Monitor_Type", "tcp" );
	#else
	Settings.setValue( "Emulator_Monitor_Type", ui.RB_Monitor_TCP->isChecked() ? "tcp" : "stdio" );
	#endif
	Settings.setValue( "Emulator_Monitor_Hostname", ui.CB_Monitor_Hostname->currentText() );
	Settings.setValue( "Emulator_Monitor_Port", ui.SB_Monitor_Port->value() );
	
	// USB	
	if( ui.RB_USB_Style_device->isChecked() )
		Settings.setValue( "USB_Style", "device" );
	else
		Settings.setValue( "USB_Style", "usbdevice" );
	
	if( ui.RB_USB_ID_BusAddr->isChecked() )
		Settings.setValue( "USB_ID_Style", "BusAddr" );
	else if( ui.RB_USB_ID_BusPath->isChecked() )
		Settings.setValue( "USB_ID_Style", "BusPath" );
	else if( ui.RB_USB_ID_VendorProduct->isChecked() )
		Settings.setValue( "USB_ID_Style", "VendorProduct" );
	
	// All OK?
	if( Settings.status() != QSettings::NoError )
		AQError( "void Advanced_Settings_Window::on_Button_OK_clicked()", "QSettings Error!" );
	
	if( Save_Emulators_Info() ) accept();

	QDir dir; // For Check on valid
	Settings.setValue( "Default_VM_Template", ui.CB_Default_VM_Template->currentText() );
	ui.Edit_VM_Folder->setText( QDir::toNativeSeparators(ui.Edit_VM_Folder->text()) );

	// VM Folder
	if( ! (ui.Edit_VM_Folder->text().endsWith("/") || ui.Edit_VM_Folder->text().endsWith("\\")) )
		ui.Edit_VM_Folder->setText( ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/") );
	
	if( dir.exists(ui.Edit_VM_Folder->text()) )
	{
		if( ! dir.exists(ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/os_templates/")) )
			dir.mkdir( ui.Edit_VM_Folder->text() + QDir::toNativeSeparators("/os_templates/") );
		
		Settings.setValue( "VM_Directory", ui.Edit_VM_Folder->text() );
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
			else Settings.setValue( "VM_Directory", ui.Edit_VM_Folder->text() );
		}
		else return;
	}
	
	// Use New Emulator Control Removable Device Menu
	if( ui.CH_Use_New_Device_Changer->isChecked() ) Settings.setValue( "Use_New_Device_Changer", "yes" );
	else Settings.setValue( "Use_New_Device_Changer", "no" );
	
	// Interface Language
	if( ui.CB_Language->currentIndex() == 0 ) Settings.setValue( "Language", "en" );
	else Settings.setValue( "Language", ui.CB_Language->itemText(ui.CB_Language->currentIndex()) );
	
	// VM Icons Size
	switch( ui.CB_VM_Icons_Size->currentIndex() )
	{
		case 0:
			Settings.setValue( "VM_Icons_Size", 16 );
			break;
			
		case 1:
			Settings.setValue( "VM_Icons_Size", 24 );
			break;
		
		case 2:
			Settings.setValue( "VM_Icons_Size", 32 );
			break;
			
		case 3:
			Settings.setValue( "VM_Icons_Size", 48 );
			break;
			
		case 4:
			Settings.setValue( "VM_Icons_Size", 64 );
			break;
			
		default:
			Settings.setValue( "VM_Icons_Size", 48 );
			break;
	}
	/*
	// USB Support
	if( ui.CH_Use_USB->isChecked() ) Settings.setValue( "Use_USB", "yes" );
	else Settings.setValue( "Use_USB", "no" );
	*/
	// Screenshot for OS Logo
	if( ui.CH_Screenshot_for_OS_Logo->isChecked() ) Settings.setValue( "Use_Screenshot_for_OS_Logo", "yes" );
	else Settings.setValue( "Use_Screenshot_for_OS_Logo", "no" );
	/*
	// 64x64 Icons
	if( ui.CH_64_Icons->isChecked() ) Settings.setValue( "64x64_Icons", "yes" );
	else Settings.setValue( "64x64_Icons", "no" );*/
	
	Settings.sync();
	
	if( Settings.status() != QSettings::NoError )
	{
		AQError( "void Settings_Window::on_Button_Box_clicked( QAbstractButton* button )",
				 "QSettings Error!" );
	}
	
	// Emulator Control
	if( ui.RB_Emul_Show_Window->isChecked() )
	{
		Settings.setValue( "Show_Emulator_Control_Window", "yes" );
		Settings.setValue( "Use_VNC_Display", "no" );
		Settings.setValue( "Include_Emulator_Control", "no" );
	}
	else if( ui.RB_Emul_Show_VNC->isChecked() )
	{
		Settings.setValue( "Show_Emulator_Control_Window", "yes" );
		Settings.setValue( "Use_VNC_Display", "yes" );
		Settings.setValue( "Include_Emulator_Control", "no" );
	}
	else if( ui.RB_Emul_Show_In_Main_Window->isChecked() )
	{
		Settings.setValue( "Show_Emulator_Control_Window", "yes" );
		Settings.setValue( "Use_VNC_Display", "no" );
		Settings.setValue( "Include_Emulator_Control", "yes" );
	}
	else if( ui.RB_Emul_Show_VNC_In_Main_Window->isChecked() )
	{
		Settings.setValue( "Show_Emulator_Control_Window", "yes" );
		Settings.setValue( "Use_VNC_Display", "yes" );
		Settings.setValue( "Include_Emulator_Control", "yes" );
	}
	else if( ui.RB_Emul_No_Show->isChecked() )
	{
		Settings.setValue( "Show_Emulator_Control_Window", "no" );
		Settings.setValue( "Use_VNC_Display", "no" );
		Settings.setValue( "Include_Emulator_Control", "no" );
	}
	
	accept();
}

void Advanced_Settings_Window::on_TB_Browse_Before_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select executable"),
													 Get_Last_Dir_Path(ui.Edit_Before_Start_Command->text()),
													 tr("All Files (*);;Scripts (*.sh)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_Before_Start_Command->setText( QDir::toNativeSeparators(fileName) );
}

void Advanced_Settings_Window::on_TB_Browse_After_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select executable"),
													 Get_Last_Dir_Path(ui.Edit_After_Stop_Command->text()),
													 tr("All Files (*);;Scripts (*.sh)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_After_Stop_Command->setText( QDir::toNativeSeparators(fileName) );
}

void Advanced_Settings_Window::on_TB_Log_File_clicked()
{
	QString fileName = QFileDialog::getSaveFileName( this, tr("Select or Create Log File"),
													 Get_Last_Dir_Path(ui.Edit_Log_Path->text()),
													 tr("All Files (*)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_Log_Path->setText( QDir::toNativeSeparators(fileName) );
}

void Advanced_Settings_Window::on_TB_QEMU_IMG_Browse_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select executable"),
													 Get_Last_Dir_Path(ui.Edit_After_Stop_Command->text()),
													 tr("All Files (*)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_QEMU_IMG_Path->setText( QDir::toNativeSeparators(fileName) );
}

void Advanced_Settings_Window::on_TB_Add_Emulator_clicked()
{
	Emulator_Options_Window *emulatorOptionsWin = new Emulator_Options_Window( this );
	emulatorOptionsWin->Set_All_Emulators_Names( Get_All_Emulators_Names() );
	emulatorOptionsWin->Set_Emulator( Emulator() );
	
	if( emulatorOptionsWin->exec() == QDialog::Accepted )
	{
		Emulator new_emul = emulatorOptionsWin->Get_Emulator();
		
		// Is there a default emulator already?
		bool found = false;
		for( int ix = 0; ix < Emulators.count(); ix++ )
		{
			if( Emulators[ix].Get_Default() )
			{
				found = true;
                break;
			}
		}
		
		if( found == false ) new_emul.Set_Default( true );
		
		Emulators << new_emul;
		Update_Emulators_Info();
	}
	
	delete emulatorOptionsWin;
}

void Advanced_Settings_Window::on_TB_Delete_Emulator_clicked()
{
	int cur_index = ui.Emulators_Table->currentRow();
	
	if( cur_index >= 0 && cur_index < Emulators.count() )
	{
		Emulators.removeAt( cur_index );
		ui.Emulators_Table->removeRow( cur_index );

		for( int ex = 0; ex < Emulators.count(); ++ex )
		{
			Emulators[ ex ].Set_Default( true );
			ui.Emulators_Table->item( ex, 3 )->setText( tr("Yes") );
			break;
		}
	}
}

void Advanced_Settings_Window::on_TB_Edit_Emulator_clicked()
{
	int cur_index = ui.Emulators_Table->currentRow();
	
	if( cur_index >= 0 && cur_index < Emulators.count() )
	{
		Emulator_Options_Window *emulatorOptionsWin = new Emulator_Options_Window( this );
		emulatorOptionsWin->Set_All_Emulators_Names( Get_All_Emulators_Names() );
		emulatorOptionsWin->Set_Emulator( Emulators[cur_index] );
		
		if( emulatorOptionsWin->exec() == QDialog::Accepted )
		{
			Emulators[ cur_index ] = emulatorOptionsWin->Get_Emulator();
			Update_Emulators_Info();
		}
		
		delete emulatorOptionsWin;
	}
}

void Advanced_Settings_Window::on_TB_Use_Default_clicked()
{
	int cur_index = ui.Emulators_Table->currentRow();
	
	if( cur_index >= 0 && cur_index < Emulators.count() )
	{
		Emulators[ cur_index ].Set_Default( true );
		
		for( int ix = 0; ix < Emulators.count(); ix++ )
		{
            Emulators[ ix ].Set_Default( false );
		}
		
		Update_Emulators_Info();
	}
}

void Advanced_Settings_Window::on_TB_Find_All_Emulators_clicked()
{
	First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );
	
	if( first_start_win->Find_Emulators() )
	{
		AQDebug( "void Advanced_Settings_Window::on_TB_Find_All_Emulators_clicked()",
				 "Find Emulators and Save Settings Complete" );
		
		// Update Emulators List
		if( Load_Emulators_Info() ) Update_Emulators_Info();
		
		// QEMU-IMG Path
		ui.Edit_QEMU_IMG_Path->setText( Settings.value("QEMU-IMG_Path", "qemu-img").toString() );
	}
	else
	{
		AQGraphic_Error( "void Advanced_Settings_Window::on_TB_Find_All_Emulators_clicked()", tr("Error!"),
						 tr("Cannot find any emulators installed on your OS! Please add them manually!"), false );
	}
	
	delete first_start_win;
}

void Advanced_Settings_Window::on_Emulators_Table_cellDoubleClicked( int row, int column )
{
	on_TB_Edit_Emulator_clicked();
}

void Advanced_Settings_Window::on_Button_CDROM_Add_clicked()
{
	bool ok = false;
	QString text = QInputDialog::getText( this, tr("Add CD/DVD Device"), tr("Enter Device Name. Example: /dev/cdrom"),
										  QLineEdit::Normal, "", &ok );
	
	if( ok && ! text.isEmpty() ) ui.CDROM_List->addItem( text );
}

void Advanced_Settings_Window::on_Button_CDROM_Edit_clicked()
{
	if( ui.CDROM_List->currentRow() >= 0 )
	{
		bool ok = false;
		QString text = QInputDialog::getText( this, tr("Add CD/DVD Device"), tr("Enter Device Name. Sample: /dev/cdrom"),
											  QLineEdit::Normal, ui.CDROM_List->currentItem()->text(), &ok );
	
		if( ok && ! text.isEmpty() ) ui.CDROM_List->currentItem()->setText( text );
	}
}

void Advanced_Settings_Window::on_Button_CDROM_Delete_clicked()
{
	if( ui.CDROM_List->currentRow() >= 0 )
		ui.CDROM_List->takeItem( ui.CDROM_List->currentRow() );
}

bool Advanced_Settings_Window::Load_Emulators_Info()
{
	if( Emulators.count() > 0 ) Emulators.clear();

	Emulators = Get_Emulators_List();
	if( Emulators.count() <= 0 ) return false;
	else return true;
}

bool Advanced_Settings_Window::Save_Emulators_Info()
{
	// FIXME save only if emulators changed
	
	// Check defaults emulators
	bool installed_qemu, default_qemu;
	installed_qemu = default_qemu = false;
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		installed_qemu = true;

		if( Emulators[ix].Get_Default() ) default_qemu = true;
	}
	
	if( installed_qemu && default_qemu == false )
	{
		AQGraphic_Warning( tr("Error!"), tr("Default QEMU Emulator isn't selected!") );
		return false;
	}
	
	// Remove old emulators files
	if( ! Remove_All_Emulators_Files() )
	{
		AQWarning( "bool Advanced_Settings_Window::Save_Emulators_Info()",
				   "Not all old emulators files removed!" );
	}
	
	// Save new files
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		if( ! Emulators[ ix ].Save() )
		{
			AQGraphic_Warning( tr("Error!"),
							   tr("Cannot save emulator \"%1\"!").arg(Emulators[ix].Get_Name()) );
		}
	}
	
	return true;
}

void Advanced_Settings_Window::Update_Emulators_Info()
{
	ui.Emulators_Table->clearContents();
	while( ui.Emulators_Table->rowCount() > 0 ) ui.Emulators_Table->removeRow( 0 );
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		ui.Emulators_Table->insertRow( ui.Emulators_Table->rowCount() );
		
		QTableWidgetItem *newItem = new QTableWidgetItem( Emulators[ix].Get_Name() );
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 0, newItem );
		
		newItem = new QTableWidgetItem( Emulator_Version_To_String(Emulators[ix].Get_Version()) ); // FIXME version,check,force
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 1, newItem );
		
		newItem = new QTableWidgetItem( Emulators[ix].Get_Path() );
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 2, newItem );
		
		newItem = new QTableWidgetItem( Emulators[ix].Get_Default() ? tr("Yes") : tr("No") );
		ui.Emulators_Table->setItem( ui.Emulators_Table->rowCount()-1, 3, newItem );
	}
}

void Advanced_Settings_Window::on_TB_Screenshot_Folder_clicked()
{
	QString folder = QFileDialog::getExistingDirectory( this, tr("Choose screenshot folder"),
														Settings.value("Screenshot_Folder_Path", "~").toString() );
	
	if( ! folder.isEmpty() )
		ui.Edit_Screenshot_Folder->setText( QDir::toNativeSeparators(folder) );
}

QStringList Advanced_Settings_Window::Get_All_Emulators_Names() const
{
	QStringList list;
	for( int ix = 0; ix < Emulators.count(); ix++ ) list << Emulators[ ix ].Get_Name();
	return list;
}
