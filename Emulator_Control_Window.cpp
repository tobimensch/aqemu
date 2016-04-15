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

#include <QUuid>
#include <QDir>
#include <QCloseEvent>
#include <QLineEdit>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDesktopWidget>
#ifdef Q_OS_WIN32
// FIXME
#else
#include <QTest>
#endif

#include "Utils.h"
#include "Ports_Tab_Widget.h"
#include "Emulator_Control_Window.h"
#include "System_Info.h"
#include "Snapshots_Window.h"

Emulator_Control_Window::Emulator_Control_Window( QWidget *parent )
	: QMainWindow( parent )
{
	ui.setupUi( this );
	
	First_Start = true;
	Fullscreen_Menu_Added = false;
	
	Show_Close_Warning = true;
	Mon_Win = new Monitor_Window();
	
	AQDebug( "Emulator_Control_Window::Emulator_Control_Window( QWidget *parent )",
			 "Created" );
}

void Emulator_Control_Window::Apply_Full_Size( int w, int h )
{
	if( ! ui.actionDisplay_Scaling->isChecked() )
	{
		setMaximumSize( w, h + menuWidget()->height() );
		
		QDesktopWidget desktop;
		if( desktop.screenGeometry(desktop.primaryScreen()).width() > w )
			resize( w, h + menuWidget()->height() );
	}
	else
	{
		setMaximumSize( 4096, 2048 );
	}
}

void Emulator_Control_Window::QEMU_Quit()
{
	#ifdef VNC_DISPLAY
	if( Machine_View && Use_VNC() )
		Machine_View->disconnectVNC();
	#endif
}

void Emulator_Control_Window::VM_State_Changet( Virtual_Machine *vm, VM::VM_State state )
{
	switch( state )
	{
		case VM::VMS_Pause:
			this->setWindowTitle( vm->Get_Machine_Name() + tr(" [Paused]") );
			break;
			
		case VM::VMS_Saved:
			this->setWindowTitle( vm->Get_Machine_Name() + tr(" [Saved]") );
			break;
			
		default:
			this->setWindowTitle( vm->Get_Machine_Name() );
	}	
}

void Emulator_Control_Window::closeEvent( QCloseEvent *event )
{
	#ifdef VNC_DISPLAY
	if( this->isVisible() == false || Show_Close_Warning == false )
	{
		if( Settings.value("Use_VNC_Display", "yes").toString() == "yes" &&
			ui.actionFullscreen_Mode->isChecked() )
		{
			Machine_View->Set_Scaling( ui.actionDisplay_Scaling->isChecked() );
			Machine_View->Set_Fullscreen( false );
		}
		
		Mon_Win->hide();
		event->accept();
		return;
	}
	
	if( Use_VNC() )
	{
		int ret = QMessageBox::question( this, tr("Power Off This VM?"),
										 tr("Selected VM is running now. Shutdown anyway?"),
										 QMessageBox::Yes | QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel,
										 QMessageBox::Cancel );
		
		if( ret == QMessageBox::Yes )
		{
			Cur_VM->Stop();
			
			Mon_Win->hide();
			event->accept();
			return;
		}
		else if( ret == QMessageBox::Save )
		{
			on_actionSave_VM_triggered();
			
			Mon_Win->hide();
			event->accept();
			return;
		}
		else if( ret == QMessageBox::Cancel )
		{
			event->ignore();
			return;
		}
		
		// For Section QMessageBox::Close Next Code
	}
	#endif
	
	Mon_Win->hide();
	event->accept();
}

void Emulator_Control_Window::Set_Current_VM( Virtual_Machine *vm )
{
	if( vm == NULL ) return;
	Cur_VM = vm;
	
	// VM stop
	connect( Cur_VM, SIGNAL(QEMU_End()),
			 this, SLOT(QEMU_Quit()) );
	
	// VM state changet
	connect( Cur_VM, SIGNAL(State_Changet(Virtual_Machine*,VM::VM_State)),
			 this, SLOT(VM_State_Changet(Virtual_Machine*,VM::VM_State)) );
	
	#ifdef VNC_DISPLAY
	if( Use_VNC() )
	{
		this->setWindowTitle( Cur_VM->Get_Machine_Name() );
		
		connect( Cur_VM, SIGNAL(Loading_Complete()),
				 this, SLOT(Connect_VNC()) );
	}
	#else
	this->setWindowTitle( Cur_VM->Get_Machine_Name() + tr(" (Emulator Control)") );
	#endif
	
	// USB
	for( int ix = 0; ix < Cur_VM->Get_USB_Ports().count(); ++ix )
	{
		QAction *new_act = new QAction( Cur_VM->Get_USB_Ports()[ix].Get_Product_Name(), ui.menuDisconnect );
		new_act->setData( Cur_VM->Get_USB_Ports()[ix].Get_ID_Line() );
		
		connect( new_act, SIGNAL(triggered()),
				 this, SLOT(Delete_USB_From_VM()) );
		
		ui.menuDisconnect->addAction( new_act );
	}
}

void Emulator_Control_Window::Init()
{
	First_Start = false;
	Show_Close_Warning = true;
	
	// Create Recent Menu Items
	int max = Settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	for( int ix = 0; ix < max; ++ix )
	{
		QAction *tmp_act = new QAction( this );
		Recent_Files_CD_Items << tmp_act;
		
		Recent_Files_CD_Items[ ix ]->setText( "" );
		Recent_Files_CD_Items[ ix ]->setData( "" );
		Recent_Files_CD_Items[ ix ]->setVisible( false );
		
		connect( Recent_Files_CD_Items[ix], SIGNAL(triggered()),
				 this, SLOT(Open_Recent_CD_ROM_Image()) );
	}
	
	Update_Recent_CD_ROM_Images_List();
	
	// Floppy
	max = Settings.value( "Floppy_Exits_Images/Max", "5" ).toString().toInt();
	
	for( int ix = 0; ix < max; ++ix )
	{
		QAction *tmp_act = new QAction( this );
		Recent_Files_FD0_Items << tmp_act;
		
		Recent_Files_FD0_Items[ ix ]->setText( "" );
		Recent_Files_FD0_Items[ ix ]->setData( "" );
		Recent_Files_FD0_Items[ ix ]->setVisible( false );
		
		connect( Recent_Files_FD0_Items[ix], SIGNAL(triggered()),
				 this, SLOT(Open_Recent_Floppy0_Image()) );
	}
	
	for( int ix = 0; ix < max; ++ix )
	{
		QAction *tmp_act = new QAction( this );
		Recent_Files_FD1_Items << tmp_act;
		
		Recent_Files_FD1_Items[ ix ]->setText( "" );
		Recent_Files_FD1_Items[ ix ]->setData( "" );
		Recent_Files_FD1_Items[ ix ]->setVisible( false );
		
		connect( Recent_Files_FD1_Items[ix], SIGNAL(triggered()),
				 this, SLOT(Open_Recent_Floppy1_Image()) );
	}
	
	Update_Recent_Floppy_Images_List();
	
	// Use VNC Embedded Display
	#ifdef VNC_DISPLAY
	if( ! Use_VNC() )
	{
		ui.actionDisplay_Scaling->setEnabled( false );
		ui.actionFullscreen_Mode->setEnabled( false );
		ui.actionReinit_VNC->setEnabled( false );
	}
	else
	{
		setMaximumSize( 4096, 2048 );
		
		// Add VNC Display Widget
		if( ! Machine_View )
			delete Machine_View;
		
		Machine_View = new MachineView( this );
		
		QVBoxLayout *vnc_layout = new QVBoxLayout( centralWidget() );
		Machine_View->setLayout( vnc_layout );
		setCentralWidget( Machine_View );
		Machine_View->show();
		
		connect( Machine_View, SIGNAL(Full_Size(int,int)),
				 this, SLOT(Apply_Full_Size(int,int)) );
		
		Machine_View->Set_VNC_URL( "localhost", Cur_VM->Get_Embedded_Display_Port() +
				(Settings.value("First_VNC_Port", "5910").toString().toInt()) );
	}
	#else
	ui.actionDisplay_Scaling->setEnabled( false );
	ui.actionFullscreen_Mode->setEnabled( false );
	ui.actionReinit_VNC->setEnabled( false );
	#endif
}

void Emulator_Control_Window::Set_Show_Close_Warning( bool on )
{
	Show_Close_Warning = on;
}

void Emulator_Control_Window::Use_Minimal_Size( bool use )
{
	this->setMaximumSize( 4096, use ? 1 : 4096 );
}

void Emulator_Control_Window::on_actionSave_Screenshot_triggered()
{
	if( Settings.value("Use_Screenshots_Folder", "no").toString() == "yes" )
	{
		if( ! Settings.value("Screenshot_Folder_Path", "").toString().isEmpty() )
		{
			// create unique file name
			QString unic_name = QUuid::createUuid();
			unic_name = unic_name.mid( 25, 12 );
			
			// save screenshot
			Cur_VM->Take_Screenshot( Settings.value("Screenshot_Folder_Path", "").toString() +
					unic_name + "." + Settings.value( "Screenshot_Save_Format", "PNG" ).toString() );
		}
	}
	else
	{
		AQGraphic_Warning( "Warning!",
						   "Shared Screenshots Folder Not Enabled!\nPlease Enter Name for New Screenshot." );
		on_actionSave_Screenshot_As_triggered();
	}
}

void Emulator_Control_Window::on_actionSave_Screenshot_As_triggered()
{
	static QString fileName = QDir::homePath(); // For save value
	fileName = QFileDialog::getSaveFileName( this, tr("Save Screenshot As..."),
											 fileName, tr("All Files (*)") );
	
	if( ! fileName.isEmpty() )
		Cur_VM->Take_Screenshot( QDir::toNativeSeparators(fileName) );
}

void Emulator_Control_Window::on_actionSave_VM_triggered()
{
	Cur_VM->Save_VM_State();
}

void Emulator_Control_Window::on_actionManage_Snapshots_triggered()
{
	Snapshots_Window *snapshot_win = new Snapshots_Window( this );
	snapshot_win->Set_VM( Cur_VM );
	snapshot_win->exec();
	delete snapshot_win;
}

void Emulator_Control_Window::on_actionCommit_triggered()
{
	emit Ready_Read_Command( "commit all" );
}

void Emulator_Control_Window::on_actionPause_VM_triggered()
{
	Cur_VM->Pause();
}

void Emulator_Control_Window::on_actionPower_Off_triggered()
{
	if( QMessageBox::question(this, tr("Are you sure?"),
		tr("Are you sure to shutdown VM \"%1\"?").arg(Cur_VM->Get_Machine_Name()),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No )
	{
		return;
	}
	
	Cur_VM->Stop();
}

void Emulator_Control_Window::on_actionReset_VM_triggered()
{
	if( QMessageBox::question(this, tr("Are you sure?"),
		tr("Are you sure to reboot VM \"%1\"?").arg(Cur_VM->Get_Machine_Name()),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No )
	{
		return;
	}
	
	Cur_VM->Reset();
}

void Emulator_Control_Window::on_actionQEMU_Monitor_triggered()
{
	connect( Mon_Win, SIGNAL(Command_Sended(QString)), Cur_VM,
			 SLOT(Execute_Emu_Ctl_Command(QString)) );
	
	connect( Cur_VM, SIGNAL(Clean_Console(QString)), Mon_Win,
			 SLOT(Add_QEMU_Out(QString)) );
	
	Mon_Win->show();
}

void Emulator_Control_Window::on_actionQuit_triggered()
{
	Show_Close_Warning = false;
	this->close();
}

void Emulator_Control_Window::on_actionFD0_dev_fd0_triggered()
{
	if( ! FD0_Available() ) return;
	
	on_actionFD0_Eject_triggered();
	Set_Device( "fda", "/dev/fd0" );
}

void Emulator_Control_Window::on_actionFD0_dev_null_triggered()
{
	if( ! FD0_Available() ) return;
	
	on_actionFD0_Eject_triggered();
	Set_Device( "fda", "/dev/null" );
}

void Emulator_Control_Window::on_actionFD0_Other_triggered()
{
	if( ! FD0_Available() ) return;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Device or Image File"),
													 Get_Last_Dir_Path(Cur_VM->Get_FD0().Get_File_Name()),
													 tr("All Files (*);;Images Files (*.img *.ima)") );
	
	if( ! fileName.isEmpty() )
	{
		fileName = QDir::toNativeSeparators( fileName );

		on_actionFD0_Eject_triggered();
		Set_Device( "fda", fileName );
		
		// Add to Recent Menu
		Add_To_Recent_FDD_Files( fileName );
		Update_Recent_Floppy_Images_List();
	}
}

void Emulator_Control_Window::on_actionFD0_Eject_triggered()
{
	if( ! FD0_Available() ) return;
	
	if( Cur_VM->Get_Emulator().Get_Version() == VM::QEMU_0_9_0 )
		emit Ready_Read_Command( "eject -f fda" );
	else
		emit Ready_Read_Command( "eject -f floppy0" );
}

void Emulator_Control_Window::Open_Recent_Floppy0_Image()
{
	QAction *tmp_act = qobject_cast<QAction*>( sender() );
	
	if( tmp_act )
	{
		on_actionFD0_Eject_triggered();
		Set_Device( "fda", tmp_act->data().toString() );
	}
}

void Emulator_Control_Window::on_actionFD1_dev_fd0_triggered()
{
	if( ! FD1_Available() ) return;
	
	on_actionFD1_Eject_triggered();
	Set_Device( "fdb", "/dev/fd0" );
}

void Emulator_Control_Window::on_actionFD1_dev_null_triggered()
{
	if( ! FD1_Available() ) return;
	
	on_actionFD1_Eject_triggered();
	Set_Device( "fdb", "/dev/null" );
}

void Emulator_Control_Window::on_actionFD1_Other_triggered()
{
	if( ! FD1_Available() ) return;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Device or Image File"),
													 Get_Last_Dir_Path(Cur_VM->Get_FD1().Get_File_Name()),
													 tr("All Files (*);;Images Files (*.img *.ima)") );
	
	if( ! fileName.isEmpty() )
	{
		fileName = QDir::toNativeSeparators( fileName );

		on_actionFD1_Eject_triggered();
		Set_Device( "fdb", fileName );
		
		// Add to Recent Menu
		Add_To_Recent_FDD_Files( fileName );
		Update_Recent_Floppy_Images_List();
	}
}

void Emulator_Control_Window::on_actionFD1_Eject_triggered()
{
	if( ! FD1_Available() ) return;
	
	if( Cur_VM->Get_Emulator().Get_Version() == VM::QEMU_0_9_0 )
		emit Ready_Read_Command( "eject -f fdb" );
	else
		emit Ready_Read_Command( "eject -f floppy1" );
}

void Emulator_Control_Window::Open_Recent_Floppy1_Image()
{
	QAction *tmp_act = qobject_cast<QAction*>( sender() );
	
	if( tmp_act )
	{
		on_actionFD1_Eject_triggered();
		Set_Device( "fdb", tmp_act->data().toString() );
	}
}

void Emulator_Control_Window::on_actionCDROM_dev_cdrom_triggered()
{
	if( ! CD_ROM_Available() ) return;
	
	on_actionCDROM_Eject_triggered();
	Set_Device( "cdrom", "/dev/cdrom" );
}

void Emulator_Control_Window::on_actionCDROM_dev_null_triggered()
{
	if( ! CD_ROM_Available() ) return;
	
	on_actionCDROM_Eject_triggered();
	Set_Device( "cdrom", "/dev/null" );
}

void Emulator_Control_Window::on_actionCDROM_Other_triggered()
{
	if( ! CD_ROM_Available() ) return;
	
	QString fileName = QFileDialog::getOpenFileName( this, tr("Open Device or Image File"),
													 Get_Last_Dir_Path(Cur_VM->Get_CD_ROM().Get_File_Name()),
													 tr("All Files (*);;Images Files (*.iso)") );
	
	if( ! fileName.isEmpty() )
	{
		fileName = QDir::toNativeSeparators( fileName );

		on_actionCDROM_Eject_triggered();
		Set_Device( "cdrom", fileName );
		
		// Add to Recent Menu
		Add_To_Recent_CD_Files( fileName );
		Update_Recent_CD_ROM_Images_List();
	}
}

void Emulator_Control_Window::on_actionCDROM_Eject_triggered()
{
	if( ! CD_ROM_Available() ) return;
	
	if( Cur_VM->Get_Emulator().Get_Version() == VM::QEMU_0_9_0 )
		emit Ready_Read_Command( "eject -f cdrom" );
	else
		emit Ready_Read_Command( "eject -f ide1-cd0" );
}

void Emulator_Control_Window::Open_Recent_CD_ROM_Image()
{
	QAction *tmp_act = qobject_cast<QAction*>( sender() );
	
	if( tmp_act )
	{
		if( ! CD_ROM_Available() ) return;
	
		on_actionCDROM_Eject_triggered();
		Set_Device( "cdrom", tmp_act->data().toString() );
	}
}

void Emulator_Control_Window::Delete_USB_From_VM()
{
	QAction *usb_item = qobject_cast<QAction*>( sender() );
	
	if( usb_item )
	{
		QList<QAction*> ac_list = ui.menuDisconnect->actions();
		
		for( int ix = 0; ix < ac_list.count(); ++ix )
		{
			if( usb_item->data().toString() == ac_list[ix]->data().toString() )
			{
				// Delete from Running VM
				QString bus_addr; // FIXME = Cur_VM->Get_USB_Bus_Address( usb_item->data().toString() );
				
				if( bus_addr.isEmpty() ) return;
				
				emit Ready_Read_Command( "usb_del " + bus_addr );
				
				// Delete item
				ui.menuDisconnect->removeAction( usb_item );
				
				return;
			}
		}
		
		// Cannot Find
		AQGraphic_Error( "void Emulator_Control_Window::Delete_USB_From_VM()",
						 tr("Error!"), tr("Cannot Find USB Device!"), false );
	}
	else
	{
		AQError( "void Emulator_Control_Window::Delete_USB_From_VM()",
				 "Cannot convert to QAction!" );
	}
}

void Emulator_Control_Window::Add_USB_To_VM()
{
	QAction *usb_item = qobject_cast<QAction*>( sender() );
	
	if( usb_item )
	{
		QList<QAction*> ac_list = ui.menuDisconnect->actions();
		
		for( int ix = 0; ix < ac_list.count(); ++ix )
		{
			// Unique value
			if( usb_item->data().toString() == ac_list[ix]->data().toString() )
			{
				AQWarning( "void Emulator_Control_Window::Add_USB_To_VM()",
						   "This is not unique value: " + usb_item->data().toString() );
				return;
			}
		}
		
		// Create Item for Disconnect Menu
		QAction *dis_act = new QAction( usb_item->text(), ui.menuDisconnect );
		dis_act->setData( usb_item->data() );
		
		connect( dis_act, SIGNAL(triggered()), this, SLOT(Delete_USB_From_VM()) );
		
		ui.menuDisconnect->addAction( dis_act );
		
		// Add To Running VM
		emit Ready_Read_Command( "usb_add host:" + usb_item->data().toString() );
	}
	else
	{
		AQError( "void Emulator_Control_Window::Add_USB_To_VM()",
				 "Cannot convert to QAction!" );
	}
}

void Emulator_Control_Window::on_actionUSB_Update_Device_List_triggered()
{
	// Get New USB Host List
	QList<VM_USB> tmp_list = System_Info::Get_All_Host_USB();
	
	if( tmp_list.isEmpty() )
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("Cannot Get USB Information From System!") );
		return;
	}
	
	QList<VM_USB> used_list = System_Info::Get_Used_USB_List();
	
	// Delete all used USB devices
	for( int ax = 0; ax < tmp_list.count(); ax++ )
	{
		for( int bx = 0; bx < used_list.count(); bx++ )
		{
			if( tmp_list[ax] == used_list[bx] )
			{
				tmp_list.removeAt( ax );
				ax--;
				break;
			}
		}
	}
	
	QList<QAction*> ac_list = ui.menuUSB_Connect->actions();
	
	// Delete Previous Menu (is avirable)
	if( ac_list.count() > 1 )
	{
		ui.menuUSB_Connect->clear();
		
		ui.menuUSB_Connect->addAction( ui.actionUSB_Update_Device_List );
		ui.menuUSB_Connect->addSeparator();
	}
	
	// Add items
	for( int ix = 0; ix < tmp_list.count(); ++ix )
	{
		QAction *new_act = new QAction( tmp_list[ix].Get_Product_Name(), ui.menuUSB_Connect );
		new_act->setData( tmp_list[ix].Get_ID_Line() );
		
		connect( new_act, SIGNAL(triggered()), this, SLOT(Add_USB_To_VM()) );
		
		ui.menuUSB_Connect->addAction( new_act );
	}
}

void Emulator_Control_Window::on_actionBy_Bus_Address_triggered()
{
	bool ok = false;
	
	QString bus_addr = QInputDialog::getText( this, tr("Disconnect USB"),
										  tr("Enter You Bus.Address VM USB Value"),
										  QLineEdit::Normal, "", &ok );
	
	if( ok && ! bus_addr.isEmpty() )
	{
		emit Ready_Read_Command( "usb_del " + bus_addr );
	}
}

void Emulator_Control_Window::on_actionUSB_Disconnect_All_Devices_triggered()
{
	QList<QAction*> ac_list = ui.menuDisconnect->actions();
	
	for( int dx = 0; dx < ac_list.count(); ++dx )
	{
		if( ac_list[dx]->data().isNull() == false &&
			ac_list[dx]->data().isValid() == true )
		{
			QString bus_addr; // FIXME = Cur_VM->Get_USB_Bus_Address( ac_list[dx]->data().toString() );
			
			if( bus_addr.isEmpty() ) return;
			
			emit Ready_Read_Command( "usb_del " + bus_addr );
		}
	}
	
	ui.menuDisconnect->clear();
	ui.menuDisconnect->addAction( ui.actionUSB_Disconnect_All_Devices );
	ui.menuDisconnect->addAction( ui.actionBy_Bus_Address );
	ui.menuDisconnect->addSeparator();
}

void Emulator_Control_Window::on_actionCtrl_Alt_Delete_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-delete" );
}

void Emulator_Control_Window::on_actionOther_Keys_triggered()
{
	bool ok = false;
	
	QString keys = QInputDialog::getText( this, tr("Send Keys"),
			tr("Enter your key combinations such as ctrl-alt-del or alt-f2"),
			QLineEdit::Normal, "", &ok );
	
	if( ok && ! keys.isEmpty() )
		emit Ready_Read_Command( "sendkey " + keys );
}

void Emulator_Control_Window::on_actionCtrl_Alt_Backspace_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-backspace" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F1_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f1" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F2_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f2" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F3_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f3" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F4_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f4" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F5_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f5" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F6_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f6" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F7_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f7" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F8_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f8" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F9_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f9" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F10_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f10" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F11_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f11" );
}

void Emulator_Control_Window::on_actionCtrl_Alt_F12_triggered()
{
	emit Ready_Read_Command( "sendkey ctrl-alt-f12" );
}

#ifdef VNC_DISPLAY

void Emulator_Control_Window::on_actionDisplay_Scaling_triggered()
{
	Machine_View->Set_Scaling( ui.actionDisplay_Scaling->isChecked() );
}

void Emulator_Control_Window::on_actionFullscreen_Mode_triggered()
{
	if( Settings.value("Show_Fullscreen_Warning", "yes").toString() == "yes" )
	{
		int ret = QMessageBox::question( this, tr("Fullscreen mode"),
										 tr("To enable window mode press Ctrl-Alt-F\nDo you want to show this message again?"),
										 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		
		if( ret == QMessageBox::No )
			Settings.setValue( "Show_Fullscreen_Warning", "no" );
	}
	
	if( ! Fullscreen_Menu_Added )
	{
		Machine_View->addAction( ui.actionFullscreen_Mode );
		Fullscreen_Menu_Added = true;
	}
	
	setMaximumSize( 4096, 2048 );
	Machine_View->Set_Scaling( ui.actionFullscreen_Mode->isChecked() );
	Machine_View->Set_Fullscreen( ui.actionFullscreen_Mode->isChecked() );
}

void Emulator_Control_Window::on_actionReinit_VNC_triggered()
{
	Machine_View->initView();
}

void Emulator_Control_Window::Connect_VNC()
{
	QTest::qWait( 500 ); // This for init QEMU VNC server
	Machine_View->initView();
}

bool Emulator_Control_Window::Use_VNC()
{
	if( Settings.value("Use_VNC_Display", "no").toString() == "yes" &&
		Cur_VM->Use_VNC() == false &&
		Cur_VM->Get_Video_Card() != "none" )
	{
		return true;
	}
	else
	{
		return false;
	}
}

#endif

void Emulator_Control_Window::on_actionAbout_Emulator_Control_triggered()
{
	QMessageBox::information( this, tr("About Emulator Control..."),
							  tr("Emulator Control is a Tool for Managing QEMU in Runtime.") );
}

bool Emulator_Control_Window::FD0_Available()
{
	return true;
	// FIXME
 	if( Cur_VM->Get_FD0().Get_Enabled() == false )
 	{
 		QMessageBox::information( this, tr("Warning!"), tr("In This VM Floppy 1 Not Found!") );
 		return false;
 	}
 	else
 	{
 		return true;
 	}
}

bool Emulator_Control_Window::FD1_Available()
{
	return true;
	// FIXME
	if( Cur_VM->Get_FD1().Get_Enabled() == false )
	{
		QMessageBox::information( this, tr("Warning!"), tr("In This VM Floppy 2 Not Found!") );
		return false;
	}
	else
	{
		return true;
	}
}

bool Emulator_Control_Window::CD_ROM_Available()
{
	return true;
	// FIXME
 	if( Cur_VM->Get_HDC().Get_Enabled() == true ||
 		Cur_VM->Get_CD_ROM().Get_Enabled() == false )
 	{
 		QMessageBox::information( this, tr("Warning!"), tr("In This VM CD-ROM Not Found!") );
 		return false;
 	}
 	else
 	{
 		return true;
 	}
}

void Emulator_Control_Window::Update_Recent_CD_ROM_Images_List()
{
	// Clear Menu
	for( int dx = 0; dx < Recent_Files_CD_Items.count(); ++dx )
	{
		Recent_Files_CD_Items[ dx ]->setText( "" );
		Recent_Files_CD_Items[ dx ]->setData( "" );
		Recent_Files_CD_Items[ dx ]->setVisible( false );
	}
	
	// Add New
	int max = Settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	while( max > Recent_Files_CD_Items.count() )
	{
		QAction *tmp_act = new QAction( this );
		Recent_Files_CD_Items << tmp_act;
		
		Recent_Files_CD_Items[ Recent_Files_CD_Items.count() -1 ]->setText( "" );
		Recent_Files_CD_Items[ Recent_Files_CD_Items.count() -1 ]->setData( "" );
		Recent_Files_CD_Items[ Recent_Files_CD_Items.count() -1 ]->setVisible( false );
		
		connect( Recent_Files_CD_Items[Recent_Files_CD_Items.count() -1], SIGNAL(triggered()),
				 this, SLOT(Open_Recent_CD_ROM_Image()) );
	}
	
	for( int ix = 0; ix < max; ++ix )
	{
		if( ix < Get_CD_Recent_Images_List().count() && Get_CD_Recent_Images_List()[ix].isEmpty() == false )
		{
			QFileInfo info = QFileInfo( Get_CD_Recent_Images_List()[ix] );
			
			Recent_Files_CD_Items[ ix ]->setText( info.fileName() );
			Recent_Files_CD_Items[ ix ]->setData( Get_CD_Recent_Images_List()[ix] );
			Recent_Files_CD_Items[ ix ]->setVisible( true );
			
			ui.menuCDROM_Recent_Files->addAction( Recent_Files_CD_Items[ix] );
		}
	}
}

void Emulator_Control_Window::Update_Recent_Floppy_Images_List()
{
	// Clear Menu
	for( int dx = 0; dx < Recent_Files_FD0_Items.count(); ++dx )
	{
		Recent_Files_FD0_Items[ dx ]->setText( "" );
		Recent_Files_FD0_Items[ dx ]->setData( "" );
		Recent_Files_FD0_Items[ dx ]->setVisible( false );
		
		Recent_Files_FD1_Items[ dx ]->setText( "" );
		Recent_Files_FD1_Items[ dx ]->setData( "" );
		Recent_Files_FD1_Items[ dx ]->setVisible( false );
	}
	
	// Add New
	int max = Settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	while( max > Recent_Files_FD0_Items.count() )
	{
		QAction *tmp_act = new QAction( this );
		Recent_Files_FD0_Items << tmp_act;
		
		Recent_Files_FD0_Items[ Recent_Files_FD0_Items.count() -1 ]->setText( "" );
		Recent_Files_FD0_Items[ Recent_Files_FD0_Items.count() -1 ]->setData( "" );
		Recent_Files_FD0_Items[ Recent_Files_FD0_Items.count() -1 ]->setVisible( false );
		
		connect( Recent_Files_FD0_Items[Recent_Files_FD0_Items.count() -1], SIGNAL(triggered()),
				 this, SLOT(Open_Recent_Floppy0_Image()) );
	}
	
	while( max > Recent_Files_FD1_Items.count() )
	{
		QAction *tmp_act = new QAction( this );
		Recent_Files_FD1_Items << tmp_act;
		
		Recent_Files_FD1_Items[ Recent_Files_FD1_Items.count() -1 ]->setText( "" );
		Recent_Files_FD1_Items[ Recent_Files_FD1_Items.count() -1 ]->setData( "" );
		Recent_Files_FD1_Items[ Recent_Files_FD1_Items.count() -1 ]->setVisible( false );
		
		connect( Recent_Files_FD1_Items[Recent_Files_FD1_Items.count() -1], SIGNAL(triggered()),
				 this, SLOT(Open_Recent_Floppy1_Image()) );
	}
	
	QStringList fd_list = Get_FDD_Recent_Images_List();
	
	for( int ix = 0; ix < max; ++ix )
	{
		if( ix < fd_list.count() && fd_list[ix].isEmpty() == false )
		{
			QFileInfo info = QFileInfo( fd_list[ix] );
			
			Recent_Files_FD0_Items[ ix ]->setText( info.fileName() );
			Recent_Files_FD0_Items[ ix ]->setData( fd_list[ix] );
			Recent_Files_FD0_Items[ ix ]->setVisible( true );
			
			ui.menuFD0_Recent_Files->addAction( Recent_Files_FD0_Items[ix] );
		}
	}
	
	for( int ix = 0; ix < max; ++ix )
	{
		if( ix < fd_list.count() && fd_list[ix].isEmpty() == false )
		{
			QFileInfo info = QFileInfo( fd_list[ix] );
			
			Recent_Files_FD1_Items[ ix ]->setText( info.fileName() );
			Recent_Files_FD1_Items[ ix ]->setData( fd_list[ix] );
			Recent_Files_FD1_Items[ ix ]->setVisible( true );
			
			ui.menuFD1_Recent_Files->addAction( Recent_Files_FD1_Items[ix] );
		}
	}
}

void Emulator_Control_Window::Set_Device( const QString &dev_name, const QString &path )
{
	if( Cur_VM->Get_Emulator().Get_Version() != VM::QEMU_0_9_0 )
	{
		QString new_dev_name;
		
		if( dev_name == "fda" ) new_dev_name = "floppy0";
		else if( dev_name == "fdb" ) new_dev_name = "floppy1";
		else if( dev_name == "cdrom" ) new_dev_name = "ide1-cd0";
		else
		{
			AQError( "void Emulator_Control_Window::Set_Device( const QString &dev_name, const QString &path )",
					 "Cannot Found Device: " + dev_name );
		}
		
		emit Ready_Read_Command( "change " + new_dev_name + " \"" + path + "\"" );
	}
	else
	{
		// For Version 0.9.0 and Old
		emit Ready_Read_Command( "change " + dev_name + " \"" + path + "\"" );
	}
	
	if( dev_name == "fda" ) Cur_VM->Set_FD0( VM_Storage_Device(true, path) );
	else if( dev_name == "fdb" ) Cur_VM->Set_FD1( VM_Storage_Device(true, path) );
	else if( dev_name == "cdrom" ) Cur_VM->Set_CD_ROM( VM_Storage_Device(true, path) );
	else
	{
		AQError( "void Emulator_Control_Window::Set_Device( const QString &dev_name, const QString &path )",
				 "Cannot Found Device: " + dev_name );
	}
}
