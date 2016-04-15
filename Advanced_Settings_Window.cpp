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
#include <QFileDialog>
#include <QHeaderView>
#include <QInputDialog>

#include "Advanced_Settings_Window.h"
#include "System_Info.h"
#include "Utils.h"
#include "Emulator_Options_Window.h"
#include "First_Start_Wizard.h"

Advanced_Settings_Window::Advanced_Settings_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Emulators_Table );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Emulators_Table->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Emulators_Table );
	hv->setResizeMode( QHeaderView::Stretch );
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
	ui.CH_Emulator_Type->setChecked( Settings.value("Info/Emulator_Type", "yes").toString() == "yes" );
	ui.CH_Emulator_Version->setChecked( Settings.value("Info/Emulator_Version", "no").toString() == "yes" );
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
	ui.CH_Acceleration->setChecked( Settings.value("Info/Acceleration", "no").toString() == "yes" );
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
	
	// QEMU/KVM Monitor Type
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
	
	if( Load_Emulators_Info() ) Update_Emulators_Info();
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
	Settings.setValue( "Info/Emulator_Type", ui.CH_Emulator_Type->isChecked() ? "yes" : "no" );
	Settings.setValue( "Info/Emulator_Version", ui.CH_Emulator_Version->isChecked() ? "yes" : "no" );
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
	Settings.setValue( "Info/Acceleration", ui.CH_Acceleration->isChecked() ? "yes" : "no" );
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
	
	// QEMU/KVM Monitor Type
	#ifdef Q_OS_WIN32
	Settings.setValue( "Emulator_Monitor_Type", "tcp" );
	#else
	Settings.setValue( "Emulator_Monitor_Type", ui.RB_Monitor_TCP->isChecked() ? "tcp" : "stdio" );
	#endif
	Settings.setValue( "Emulator_Monitor_Hostname", ui.CB_Monitor_Hostname->currentText() );
	Settings.setValue( "Emulator_Monitor_Port", ui.SB_Monitor_Port->value() );

	// All OK?
	if( Settings.status() != QSettings::NoError )
		AQError( "void Advanced_Settings_Window::on_Button_OK_clicked()", "QSettings Error!" );
	
	if( Save_Emulators_Info() ) accept();
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
		
		// This Default Emulator?
		bool finded = false;
		for( int ix = 0; ix < Emulators.count(); ix++ )
		{
			if( Emulators[ix].Get_Type() == new_emul.Get_Type() &&
				Emulators[ix].Get_Default() )
			{
				finded = true;
			}
		}
		
		if( finded == false ) new_emul.Set_Default( true );
		
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
		
		// Set new default emulator?
		bool q, k, qd, kd; // q - qemu, k - kvm, d - default
		q = k = qd = kd = false;
		
		for( int ex = 0; ex < Emulators.count(); ++ex )
		{
			if( Emulators[ex].Get_Type() == VM::QEMU )
			{
				q = true;
				if( Emulators[ex].Get_Default() ) qd = true;
			}
			else if( Emulators[ex].Get_Type() == VM::KVM )
			{
				k = true;
				if( Emulators[ex].Get_Default() ) kd = true;
			}
		}
		
		if( q && qd == false )
		{
			for( int ex = 0; ex < Emulators.count(); ++ex )
			{
				if( Emulators[ex].Get_Type() == VM::QEMU )
				{
					Emulators[ ex ].Set_Default( true );
					ui.Emulators_Table->item( ex, 3 )->setText( tr("Yes") );
					break;
				}
			}
		}
		
		if( k && kd == false )
		{
			for( int ex = 0; ex < Emulators.count(); ++ex )
			{
				if( Emulators[ex].Get_Type() == VM::KVM )
				{
					Emulators[ ex ].Set_Default( true );
					ui.Emulators_Table->item( ex, 3 )->setText( tr("Yes") );
					break;
				}
			}
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
			if( ix != cur_index &&
				Emulators[cur_index].Get_Type() == Emulators[ix].Get_Type() )
			{
				Emulators[ ix ].Set_Default( false );
			}
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
	bool installed_kvm, installed_qemu, default_kvm, default_qemu;
	installed_kvm = installed_qemu = default_kvm = default_qemu = false;
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		if( Emulators[ix].Get_Type() == VM::QEMU )
		{
			installed_qemu = true;

			if( Emulators[ix].Get_Default() ) default_qemu = true;
		}
		else if( Emulators[ix].Get_Type() == VM::KVM )
		{
			installed_kvm = true;
			
			if( Emulators[ix].Get_Default() ) default_kvm = true;
		}
	}
	
	if( installed_qemu && default_qemu == false )
	{
		AQGraphic_Warning( tr("Error!"), tr("Default QEMU Emulator isn't selected!") );
		return false;
	}
	
	if( installed_kvm && default_kvm == false )
	{
		AQGraphic_Warning( tr("Error!"), tr("Default KVM Emulator isn't selected!") );
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
