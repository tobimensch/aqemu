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

#include <QList>
#include <QObject>
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QMessageBox>
#include <QSettings>
#include <QUuid>
#include <QPainter>

#ifdef Q_OS_WIN32
#include <windows.h>
#else
#include <QTest>
#endif

#include <QRect>
#include <QLabel>
#include <QHBoxLayout>
#include <QDesktopWidget>

//GenerateHTMLInfo
#include <QTextEdit>
#include <QTextFrame>
#include <QTextTableCell>
#include <QPalette>
//GenerateHTMLInfo

#include "VM.h"
#include "Utils.h"
#include "Emulator_Control_Window.h"
#include "System_Info.h"
#include "VNC_Password_Window.h"

using namespace TinyXML2QDomWrapper;

// VM Class -----------------------------------------------------------------

Virtual_Machine::Virtual_Machine()
{
    // Default Machine
	Shared_Constructor();
}

Virtual_Machine::Virtual_Machine( const QString &name )
{
	Shared_Constructor();
    Machine_Name = name;
}

Virtual_Machine::Virtual_Machine( const Virtual_Machine &vm )
{
	QEMU_Process = new QProcess();
	Monitor_Socket = new QTcpSocket( this );
	Use_Monitor_TCP = false;
	Monitor_Hostname = "localhost";
	Monitor_Port = 6000;
	State = vm.Get_State();
	Emu_Ctl = new Emulator_Control_Window();
	VM_XML_File_Path = vm.Get_VM_XML_File_Path();
	Build_QEMU_Args_for_Script_Mode = false;
	Build_QEMU_Args_for_Tab_Info = false;
	UID = vm.Get_UID();
	
	// Accel
	Machine_Accelerator = vm.Get_Machine_Accelerator();
	Current_Emulator = vm.Get_Emulator();
	
	QObject::connect( Emu_Ctl, SIGNAL(Ready_Read_Command(QString)),
					  this, SLOT(Execute_Emu_Ctl_Command(QString)) );
	
	QObject::connect( QEMU_Process, SIGNAL(readyReadStandardError()),
					  this, SLOT(Parse_StdErr()) );
	
	QObject::connect( QEMU_Process, SIGNAL(readyReadStandardOutput()),
					  this, SLOT(Parse_StdOut()) );

	QObject::connect( Monitor_Socket, SIGNAL(readyRead()),
					  this, SLOT(Parse_StdOut()) );
	
	QObject::connect( QEMU_Process, SIGNAL(started()),
					  this, SLOT(QEMU_Started()) );
	
	QObject::connect( QEMU_Process, SIGNAL(finished(int,QProcess::ExitStatus)),
					  this, SLOT(QEMU_Finished(int,QProcess::ExitStatus)) );
	
	this->Icon_Path = vm.Get_Icon_Path();
	this->Screenshot_Path = vm.Get_Screenshot_Path();
	
	// General Tab
	this->Machine_Name = vm.Get_Machine_Name();
	this->Computer_Type = vm.Get_Computer_Type();
	this->Machine_Type = vm.Get_Machine_Type();
	this->CPU_Type = vm.Get_CPU_Type();
	this->SMP = vm.Get_SMP();
	this->Keyboard_Layout = vm.Get_Keyboard_Layout();
	this->Boot_Order_List = vm.Get_Boot_Order_List();
	this->Show_Boot_Menu = vm.Get_Show_Boot_Menu();
    this->Video_Card = vm.Get_Video_Card();
	
	this->Audio_Card = vm.Get_Audio_Cards();
	
	this->Memory_Size = vm.Get_Memory_Size();
	this->Remove_RAM_Size_Limitation = vm.Get_Remove_RAM_Size_Limitation();
	
	this->Fullscreen = vm.Use_Fullscreen_Mode();
	this->Win2K_Hack = vm.Use_Win2K_Hack();
	this->Local_Time = vm.Use_Local_Time();
	this->Check_FDD_Boot_Sector = vm.Use_Check_FDD_Boot_Sector();
	this->ACPI = vm.Use_ACPI();
	this->Snapshot_Mode = vm.Use_Snapshot_Mode();
	this->Start_CPU = vm.Use_Start_CPU();
	this->No_Reboot = vm.Use_No_Reboot();
	this->No_Shutdown = vm.Use_No_Shutdown();
	
	// FDD/CD/DVD Tab
	this->FD0 = vm.Get_FD0();
	this->FD1 = vm.Get_FD1();
	this->CD_ROM = vm.Get_CD_ROM();
	
	// HDD Tab
	this->HDA = vm.Get_HDA();
	this->HDB = vm.Get_HDB();
	this->HDC = vm.Get_HDC();
	this->HDD = vm.Get_HDD();
	
	// Snapshots
	this->Set_Snapshots( vm.Get_Snapshots() );
	
	// Storage Devices
	this->Set_Storage_Devices_List( vm.Get_Storage_Devices_List() );
	
	// Shared Folders
	this->Set_Shared_Folders_List( vm.Get_Shared_Folders_List() );

	// Network Tab
	this->Use_Network = vm.Get_Use_Network();
    this->Native_Network = vm.Use_Native_Network();
	this->Use_Redirections = vm.Get_Use_Redirections();
	
	this->Network_Cards.clear();
	for( int nx = 0; nx < vm.Get_Network_Cards_Count(); nx++ )
	{
		VM_Net_Card n_card = vm.Get_Network_Card( nx );
		
		this->Network_Cards.append( VM_Net_Card(n_card) );
	}
	
	// Nativ
	this->Network_Cards_Nativ.clear();
	this->Network_Cards_Nativ = vm.Get_Network_Cards_Nativ();
	
	this->Network_Redirections.clear();
	for( int rx = 0; rx < vm.Get_Network_Redirections_Count(); rx++ )
	{
		VM_Redirection n_redir = vm.Get_Network_Redirection( rx );
		
		this->Network_Redirections.append( VM_Redirection(n_redir) );
	}
	
	this->SMB_Directory = vm.Get_SMB_Directory();
	this->TFTP_Prefix = vm.Get_TFTP_Prefix();
	
	// Ports Tab
	this->Serial_Ports = vm.Get_Serial_Ports();
	this->Parallel_Ports = vm.Get_Parallel_Ports();
	
	// USB
	this->USB_Ports.clear();
	for( int ux = 0; ux < vm.Get_USB_Ports().count(); ++ux )
	{
		this->USB_Ports.append( VM_USB(vm.Get_USB_Ports()[ux]) );
	}
	
	// Other Tab
	this->Linux_Boot = vm.Get_Use_Linux_Boot();
	this->bzImage_Path = vm.Get_bzImage_Path();
	this->Initrd_Path = vm.Get_Initrd_Path();
	this->Kernel_ComLine = vm.Get_Kernel_ComLine();
	
	this->Additional_Args = vm.Get_Additional_Args();
	this->Only_User_Args = vm.Get_Only_User_Args();
	this->Use_User_Emulator_Binary = vm.Get_Use_User_Emulator_Binary();
	
	this->Use_ROM_File = vm.Get_Use_ROM_File();
	this->ROM_File = vm.Get_ROM_File();
	
	this->MTDBlock = vm.Use_MTDBlock_File();
	this->MTDBlock_File = vm.Get_MTDBlock_File();
	
	this->SecureDigital = vm.Use_SecureDigital_File();
	this->SecureDigital_File = vm.Get_SecureDigital_File();
	
	this->PFlash = vm.Use_PFlash_File();
	this->PFlash_File = vm.Get_PFlash_File();
	
	this->Enable_KVM = vm.Use_KVM();
	this->KVM_IRQChip = vm.Use_KVM_IRQChip();
	this->No_KVM_Pit = vm.Use_No_KVM_Pit();
	this->KVM_No_Pit_Reinjection = vm.Use_KVM_No_Pit_Reinjection();
	this->KVM_Nesting = vm.Use_KVM_Nesting();
	this->KVM_Shadow_Memory = vm.Use_KVM_Shadow_Memory();
	this->KVM_Shadow_Memory_Size = vm.Get_KVM_Shadow_Memory_Size();
	
	this->Init_Graphic_Mode = vm.Get_Init_Graphic_Mode();
	
	this->No_Frame = vm.Use_No_Frame();
	this->Alt_Grab = vm.Use_Alt_Grab();
	this->No_Quit = vm.Use_No_Quit();
	this->Portrait = vm.Use_Portrait();
	this->Show_Cursor = vm.Use_Show_Cursor();
	this->Curses = vm.Use_Curses();
	this->RTC_TD_Hack = vm.Use_RTC_TD_Hack();
	
	this->Start_Date = vm.Use_Start_Date();
	this->Start_DateTime = vm.Get_Start_Date();
	
	this->SPICE = vm.Get_SPICE();
	
	this->VNC = vm.Use_VNC();
	this->VNC_Socket_Mode = vm.Get_VNC_Socket_Mode();
	this->VNC_Unix_Socket_Path = vm.Get_VNC_Unix_Socket_Path();
	this->VNC_Display_Number = vm.Get_VNC_Display_Number();
	this->VNC_Password = vm.Use_VNC_Password();
	this->VNC_TLS = vm.Use_VNC_TLS();
	this->VNC_x509 = vm.Use_VNC_x509();
	this->VNC_x509_Folder_Path = vm.Get_VNC_x509_Folder_Path();
	this->VNC_x509verify = vm.Use_VNC_x509verify();
	this->VNC_x509verify_Folder_Path = vm.Get_VNC_x509verify_Folder_Path();
	
	this->Load_VM_Window = nullptr;
	this->Save_VM_Window = nullptr;
	this->QEMU_Error_Win = nullptr;
	this->Load_Mode = false;
	this->Quit_Before_Save = false;
	
	Update_Current_Emulator_Devices(); // FIXME
}

Virtual_Machine::~Virtual_Machine()
{
	if( QEMU_Process != NULL ) delete QEMU_Process;
	if( Emu_Ctl != NULL ) delete Emu_Ctl;
	
	Boot_Order_List.clear();
	Snapshots.clear();
	Storage_Devices.clear();
	Shared_Folders.clear();
	Network_Cards.clear();
	Network_Redirections.clear();
	USB_Ports.clear();
	
	delete Load_VM_Window;
	delete Save_VM_Window;
	delete QEMU_Error_Win;
}

void Virtual_Machine::Shared_Constructor()
{
	QEMU_Process = new QProcess();
	Monitor_Socket = new QTcpSocket( this );
	Use_Monitor_TCP = false;
	Monitor_Hostname = "localhost";
	Monitor_Port = 6000;
	this->State = VM::VMS_Power_Off;
	Emu_Ctl = new Emulator_Control_Window();
	Removable_Devices_List = "";
	Update_Removable_Devices_Mode = false;
	VM_XML_File_Path = "";
	Build_QEMU_Args_for_Script_Mode = false;
	Build_QEMU_Args_for_Tab_Info = false;
	UID = "";
	
	// Accel
	Machine_Accelerator = VM::TCG;
	Current_Emulator = Emulator();
	Current_Emulator_Devices = Available_Devices();
	
	QObject::connect( Emu_Ctl, SIGNAL(Ready_Read_Command(QString)),
					  this, SLOT(Execute_Emu_Ctl_Command(QString)) );
	
	QObject::connect( QEMU_Process, SIGNAL(readyReadStandardError()),
					  this, SLOT(Parse_StdErr()) );
	
	QObject::connect( QEMU_Process, SIGNAL(readyReadStandardOutput()),
					  this, SLOT(Parse_StdOut()) );

	QObject::connect( Monitor_Socket, SIGNAL(readyRead()),
					  this, SLOT(Parse_StdOut()) );
	
	QObject::connect( QEMU_Process, SIGNAL(started()),
					  this, SLOT(QEMU_Started()) );
	
	QObject::connect( QEMU_Process, SIGNAL(finished(int, QProcess::ExitStatus)),
					  this, SLOT(QEMU_Finished(int, QProcess::ExitStatus)) );
	
	Icon_Path = ":/other.png";
	Screenshot_Path = "";
	
	Machine_Name = "NO_NAME";
	Computer_Type = "";
	Machine_Type = "";
	CPU_Type = "";
	SMP.SMP_Count = 1;
	Keyboard_Layout = "Default";
	
	VM::Boot_Order tmpBootOrder;
	
	tmpBootOrder.Enabled = true;
	tmpBootOrder.Type = VM::Boot_From_FDA;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Enabled = false;
	tmpBootOrder.Type = VM::Boot_From_FDB;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Enabled = true;
	tmpBootOrder.Type = VM::Boot_From_CDROM;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Type = VM::Boot_From_HDD;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Enabled = false;
	tmpBootOrder.Type = VM::Boot_From_Network1;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Type = VM::Boot_From_Network2;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Type = VM::Boot_From_Network3;
	this->Boot_Order_List << tmpBootOrder;
	
	tmpBootOrder.Type = VM::Boot_From_Network4;
	this->Boot_Order_List << tmpBootOrder;
	
	Show_Boot_Menu = true;
	
    Video_Card = "";
	Audio_Card = VM::Sound_Cards();
	Remove_RAM_Size_Limitation = false;
	Memory_Size = 128;
	
	Fullscreen = false;
	Win2K_Hack = false;
	Local_Time = true;
	Check_FDD_Boot_Sector = true;
	ACPI = true;
	Snapshot_Mode = false;
	Start_CPU = true;
	No_Reboot = false;
	No_Shutdown = false;
	
	FD0 = VM_Storage_Device();
	FD1 = VM_Storage_Device();
	CD_ROM = VM_Storage_Device();
	HDA = VM_HDD();
	HDB = VM_HDD();
	HDC = VM_HDD();
	HDD = VM_HDD();
	
	Use_Network = true;
    Native_Network = false;
	Use_Redirections = false;
	TFTP_Prefix = "";
	SMB_Directory = "";
	
	Linux_Boot = false;
	bzImage_Path = "";
	Initrd_Path = "";
	Kernel_ComLine = "";
	
	Additional_Args = "";
	Only_User_Args = false;
	Use_User_Emulator_Binary = false;
	
	Use_ROM_File = false;
	ROM_File = "";
	
	MTDBlock = false;
	MTDBlock_File = "";
	
	SecureDigital = false;
	SecureDigital_File = "";
	
	PFlash = false;
	PFlash_File = "";
	
	Enable_KVM = true;
	KVM_IRQChip = false;
	No_KVM_Pit = false;
	KVM_No_Pit_Reinjection = false;
	KVM_Nesting = false;
	KVM_Shadow_Memory = false;
	KVM_Shadow_Memory_Size = 0;
	
	Init_Graphic_Mode = VM_Init_Graphic_Mode();
	
	No_Frame = false;
	Alt_Grab = false;
	No_Quit = false;
	Portrait = false;
	Show_Cursor = false;
	Curses = false;
	RTC_TD_Hack = false;
	
	Start_Date = false;
	Start_DateTime = QDateTime::fromString( "20.10.2000 23:59:00", "dd.MM.yyyy HH:mm:ss" );
	
	SPICE = VM_SPICE();
	
	VNC = false;
	VNC_Socket_Mode = false;
	VNC_Unix_Socket_Path = "";
	VNC_Display_Number = 1;
	VNC_Password = false;
	VNC_TLS = false;
	VNC_x509 = "";
	VNC_x509_Folder_Path = "";
	VNC_x509verify = false;
	VNC_x509verify_Folder_Path = "";
	
	Embedded_Display_Port = -1;
	
	Template_Opts = Create_Template_Window::Template_Save_Default;
	
	Load_VM_Window = nullptr;
	Save_VM_Window = nullptr;
	QEMU_Error_Win = nullptr;
	Load_Mode = false;
	Quit_Before_Save = false;
}

bool Virtual_Machine::operator==( const Virtual_Machine &vm ) const
{
	if( this->Icon_Path == vm.Get_Icon_Path() &&
		this->Computer_Type == vm.Get_Computer_Type() &&
		this->Machine_Name == vm.Get_Machine_Name() &&
        this->Machine_Accelerator == vm.Get_Machine_Accelerator() &&
		this->Machine_Type == vm.Get_Machine_Type() &&
		this->CPU_Type == vm.Get_CPU_Type() &&
		this->SMP == vm.Get_SMP() &&
		this->Keyboard_Layout == vm.Get_Keyboard_Layout() &&
		this->Show_Boot_Menu == vm.Get_Show_Boot_Menu() &&
        this->Video_Card == vm.Get_Video_Card() &&
		this->Audio_Card == vm.Get_Audio_Cards() &&
		this->Memory_Size == vm.Get_Memory_Size() &&
		this->Remove_RAM_Size_Limitation == vm.Get_Remove_RAM_Size_Limitation() &&
		this->Fullscreen == vm.Use_Fullscreen_Mode() &&
		this->Win2K_Hack == vm.Use_Win2K_Hack() &&
		this->Local_Time == vm.Use_Local_Time() &&
		this->Check_FDD_Boot_Sector == vm.Use_Check_FDD_Boot_Sector() &&
		this->ACPI == vm.Use_ACPI() &&
		this->Snapshot_Mode == vm.Use_Snapshot_Mode() &&
		this->No_Shutdown == vm.Use_No_Shutdown() &&
		this->Start_CPU == vm.Use_Start_CPU() &&
		this->No_Reboot == vm.Use_No_Reboot() &&
		this->FD0 == vm.Get_FD0() &&
		this->FD1 == vm.Get_FD1() &&
		this->CD_ROM == vm.Get_CD_ROM() &&
		this->HDA == vm.Get_HDA() &&
		this->HDB == vm.Get_HDB() &&
		this->HDC == vm.Get_HDC() &&
		this->HDD == vm.Get_HDD() &&
		this->Use_Network == vm.Get_Use_Network() &&
        this->Native_Network == vm.Use_Native_Network() &&
		this->Use_Redirections == vm.Get_Use_Redirections() &&
		this->Get_Network_Cards_Count() == vm.Get_Network_Cards_Count() &&
		this->Get_Network_Redirections_Count() == vm.Get_Network_Redirections_Count() &&
		this->TFTP_Prefix == vm.Get_TFTP_Prefix() &&
		this->SMB_Directory == vm.Get_SMB_Directory() &&
		this->Linux_Boot == vm.Get_Use_Linux_Boot() &&
		this->bzImage_Path == vm.Get_bzImage_Path() &&
		this->Initrd_Path == vm.Get_Initrd_Path() &&
		this->Kernel_ComLine == vm.Get_Kernel_ComLine() &&
		this->Additional_Args == vm.Get_Additional_Args() &&
		this->Only_User_Args == vm.Get_Only_User_Args() &&
		this->Use_User_Emulator_Binary == vm.Get_Use_User_Emulator_Binary() &&
		this->Use_ROM_File == vm.Get_Use_ROM_File() &&
		this->ROM_File == vm.Get_ROM_File() &&
		this->MTDBlock == vm.Use_MTDBlock_File() &&
		this->MTDBlock_File == vm.Get_MTDBlock_File() &&
		this->SecureDigital == vm.Use_SecureDigital_File() &&
		this->SecureDigital_File == vm.Get_SecureDigital_File() &&
		this->PFlash == vm.Use_PFlash_File() &&
		this->PFlash_File == vm.Get_PFlash_File() &&
		this->Enable_KVM == vm.Use_KVM() &&
		this->KVM_IRQChip == vm.Use_KVM_IRQChip() &&
		this->No_KVM_Pit == vm.Use_No_KVM_Pit() &&
		this->KVM_No_Pit_Reinjection == vm.Use_KVM_No_Pit_Reinjection() &&
		this->KVM_Nesting == vm.Use_KVM_Nesting() &&
		this->KVM_Shadow_Memory == vm.Use_KVM_Shadow_Memory() &&
		this->KVM_Shadow_Memory_Size == vm.Get_KVM_Shadow_Memory_Size() &&
		this->Init_Graphic_Mode == vm.Get_Init_Graphic_Mode() &&
		this->No_Frame == vm.Use_No_Frame() &&
		this->Alt_Grab == vm.Use_Alt_Grab() &&
		this->No_Quit == vm.Use_No_Quit() &&
		this->Portrait  == vm.Use_Portrait() &&
		this->Show_Cursor == vm.Use_Show_Cursor() &&
		this->Curses == vm.Use_Curses() &&
		this->RTC_TD_Hack == vm.Use_RTC_TD_Hack() &&
		this->Start_Date == vm.Use_Start_Date() &&
		this->Start_DateTime == vm.Get_Start_Date() &&
		this->SPICE == vm.Get_SPICE() &&
		this->VNC == vm.Use_VNC() &&
		this->VNC_Socket_Mode == vm.Get_VNC_Socket_Mode() &&
		this->VNC_Unix_Socket_Path == vm.Get_VNC_Unix_Socket_Path() &&
		this->VNC_Display_Number == vm.Get_VNC_Display_Number() &&
		this->VNC_Password == vm.Use_VNC_Password() &&
		this->VNC_TLS == vm.Use_VNC_TLS() &&
		this->VNC_x509 == vm.Use_VNC_x509() &&
		this->VNC_x509_Folder_Path == vm.Get_VNC_x509_Folder_Path() &&
		this->VNC_x509verify == vm.Use_VNC_x509verify() &&
		this->VNC_x509verify_Folder_Path == vm.Get_VNC_x509verify_Folder_Path() )
	{
		// Boot Order
		if( Boot_Order_List.count() == vm.Get_Boot_Order_List().count() )
		{
			for( int bx = 0; bx < Boot_Order_List.count(); bx++ )
			{
				if( (Boot_Order_List[bx].Enabled != vm.Get_Boot_Order_List()[bx].Enabled) ||
					(Boot_Order_List[bx].Type != vm.Get_Boot_Order_List()[bx].Type) ) return false;
			}
		}
		else return false;
		
		// Storage Devices
		if( Storage_Devices.count() == vm.Get_Storage_Devices_List().count() )
		{
			for( int sx = 0; sx < Storage_Devices.count(); ++sx )
			{
				if( Storage_Devices[sx] != vm.Get_Storage_Devices_List()[sx] ) return false;
			}
		}
		else return false;

		// Storage Devices
		if( Shared_Folders.count() == vm.Get_Shared_Folders_List().count() )
		{
			for( int sx = 0; sx < Shared_Folders.count(); ++sx )
			{
				if( Shared_Folders[sx] != vm.Get_Shared_Folders_List()[sx] ) return false;
			}
		}
		else return false;
		
		// Network Cards
		if( Get_Network_Cards_Count() == vm.Get_Network_Cards_Count() )
		{
			for( int nx = 0; nx < Get_Network_Cards_Count(); ++nx )
			{
				if( Network_Cards[nx] != vm.Get_Network_Card(nx) ) return false;
			}
		}
		else return false;
		
		// Network Cards Nativ
        QList<VM_Net_Card_Native> tmp_card_nativ = vm.Get_Network_Cards_Nativ();
		
		if( Network_Cards_Nativ.count() == tmp_card_nativ.count() )
		{
			for( int nx = 0; nx < tmp_card_nativ.count(); ++nx )
			{
				if( Network_Cards_Nativ[nx] != tmp_card_nativ[nx] ) return false;
			}
		}
		else return false;
		
		// Network Redirections
		if( Get_Network_Redirections_Count() == vm.Get_Network_Redirections_Count() )
		{
			for( int rx = 0; rx < Get_Network_Redirections_Count(); ++rx )
			{
				if( Network_Redirections[rx] != vm.Get_Network_Redirection(rx) ) return false;
			}
		}
		else return false;
		
		// Serial Port
		if( Serial_Ports.count() == vm.Get_Serial_Ports().count() )
		{
			for( int sx = 0; sx < Serial_Ports.count(); ++sx )
			{
				if( Serial_Ports[sx] != vm.Get_Serial_Ports()[sx] ) return false;
			}
		}
		else return false;
		
		// Parallel Ports
		if( Parallel_Ports.count() == vm.Get_Parallel_Ports().count() )
		{
			for( int px = 0; px < Parallel_Ports.count(); ++px )
			{
				if( Parallel_Ports[px] != vm.Get_Parallel_Ports()[px] ) return false;
			}
		}
		else return false;
		
		// USB
		if( USB_Ports.count() == vm.Get_USB_Ports().count() )
		{
			for( int ux = 0; ux < USB_Ports.count(); ++ux )
			{
				if( USB_Ports[ux] != vm.Get_USB_Ports()[ux] ) return false;
			}
		}
		else return false;

		return true;
	}
	else
    {
		return false;
	}
}

bool Virtual_Machine::operator!=( const Virtual_Machine &v1 ) const
{
	return ! ( operator==(v1) );
}

Virtual_Machine &Virtual_Machine::operator=( const Virtual_Machine &vm )
{
	QEMU_Process = new QProcess();
	Monitor_Socket = new QTcpSocket( this );
	Use_Monitor_TCP = false;
	Monitor_Hostname = "localhost";
	Monitor_Port = 6000;
    delete Emu_Ctl;
	Emu_Ctl = new Emulator_Control_Window();
	VM_XML_File_Path = vm.Get_VM_XML_File_Path();
	State = vm.Get_State();
	Build_QEMU_Args_for_Script_Mode = false;
	Build_QEMU_Args_for_Tab_Info = false;
	UID = vm.Get_UID();
	
	// Accel
	Machine_Accelerator = vm.Get_Machine_Accelerator();
	Current_Emulator = vm.Get_Emulator();
	
	QObject::connect( Emu_Ctl, SIGNAL(Ready_Read_Command(QString)),
					  this, SLOT(Execute_Emu_Ctl_Command(QString)) );
	
	QObject::connect( QEMU_Process, SIGNAL(readyReadStandardError()),
					  this, SLOT(Parse_StdErr()) );
	
	QObject::connect( QEMU_Process, SIGNAL(readyReadStandardOutput()),
					  this, SLOT(Parse_StdOut()) );

	QObject::connect( Monitor_Socket, SIGNAL(readyRead()),
					  this, SLOT(Parse_StdOut()) );
	
	QObject::connect( QEMU_Process, SIGNAL(started()),
					  this, SLOT(QEMU_Started()) );
	
	QObject::connect( QEMU_Process, SIGNAL(finished(int,QProcess::ExitStatus)),
					  this, SLOT(QEMU_Finished(int,QProcess::ExitStatus)) );
	
	this->Icon_Path = vm.Get_Icon_Path();
	this->Screenshot_Path = vm.Get_Screenshot_Path();
	
	// General Tab
	this->Machine_Name = vm.Get_Machine_Name();
	this->Computer_Type = vm.Get_Computer_Type();
    this->Machine_Accelerator = vm.Get_Machine_Accelerator();
	this->Machine_Type = vm.Get_Machine_Type();
	this->CPU_Type = vm.Get_CPU_Type();
	this->SMP = vm.Get_SMP();
	this->Keyboard_Layout = vm.Get_Keyboard_Layout();
	this->Boot_Order_List = vm.Get_Boot_Order_List();
	this->Show_Boot_Menu = vm.Get_Show_Boot_Menu();
    this->Video_Card = vm.Get_Video_Card();
	this->Audio_Card = vm.Get_Audio_Cards();
	this->Memory_Size = vm.Get_Memory_Size();
	this->Remove_RAM_Size_Limitation = vm.Get_Remove_RAM_Size_Limitation();
	this->Fullscreen = vm.Use_Fullscreen_Mode();
	this->Win2K_Hack = vm.Use_Win2K_Hack();
	this->Local_Time = vm.Use_Local_Time();
	this->Check_FDD_Boot_Sector = vm.Use_Check_FDD_Boot_Sector();
	this->ACPI = vm.Use_ACPI();
	this->Snapshot_Mode = vm.Use_Snapshot_Mode();
	this->Start_CPU = vm.Use_Start_CPU();
	this->No_Reboot = vm.Use_No_Reboot();
	this->No_Shutdown = vm.Use_No_Shutdown();
	
	// FDD/CD/DVD Tab
	this->FD0 = vm.Get_FD0();
	this->FD1 = vm.Get_FD1();
	this->CD_ROM = vm.Get_CD_ROM();
	
	// HDD Tab
	this->HDA = vm.Get_HDA();
	this->HDB = vm.Get_HDB();
	this->HDC = vm.Get_HDC();
	this->HDD = vm.Get_HDD();
	
	// Snapshots
	this->Set_Snapshots( vm.Get_Snapshots() );
	
	// Storage Devices
	this->Set_Storage_Devices_List( vm.Get_Storage_Devices_List() );
	
	// Shared Folders
	this->Set_Shared_Folders_List( vm.Get_Shared_Folders_List() );

	// Network Tab
	this->Use_Network = vm.Get_Use_Network();
    this->Native_Network = vm.Use_Native_Network();
	this->Use_Redirections = vm.Get_Use_Redirections();
	
	this->Network_Cards.clear();
	for( int nx = 0; nx < vm.Get_Network_Cards_Count(); nx++ )
	{
		VM_Net_Card n_card = vm.Get_Network_Card( nx );
		
		this->Network_Cards.append( VM_Net_Card(n_card) );
	}
	
	// Nativ
	this->Network_Cards_Nativ.clear();
	this->Network_Cards_Nativ = vm.Get_Network_Cards_Nativ();
	
	this->Network_Redirections.clear();
	for( int rx = 0; rx < vm.Get_Network_Redirections_Count(); rx++ )
	{
		VM_Redirection n_redir = vm.Get_Network_Redirection( rx );
		
		this->Network_Redirections.append( VM_Redirection(n_redir) );
	}
	
	this->SMB_Directory = vm.Get_SMB_Directory();
	this->TFTP_Prefix = vm.Get_TFTP_Prefix();
	
	// Port tab
	this->Serial_Ports = vm.Get_Serial_Ports();
	this->Parallel_Ports = vm.Get_Parallel_Ports();
	
	this->USB_Ports.clear();
	for( int ux = 0; ux < vm.Get_USB_Ports().count(); ++ux )
	{
		this->USB_Ports.append( VM_USB(vm.Get_USB_Ports()[ux]) );
	}
	
	// Other Tab
	this->Linux_Boot = vm.Get_Use_Linux_Boot();
	this->bzImage_Path = vm.Get_bzImage_Path();
	this->Initrd_Path = vm.Get_Initrd_Path();
	this->Kernel_ComLine = vm.Get_Kernel_ComLine();
	
	this->Additional_Args = vm.Get_Additional_Args();
	this->Only_User_Args = vm.Get_Only_User_Args();
	this->Use_User_Emulator_Binary = vm.Get_Use_User_Emulator_Binary();
	
	this->Use_ROM_File = vm.Get_Use_ROM_File();
	this->ROM_File = vm.Get_ROM_File();
	
	this->MTDBlock = vm.Use_MTDBlock_File();
	this->MTDBlock_File = vm.Get_MTDBlock_File();
	
	this->SecureDigital = vm.Use_SecureDigital_File();
	this->SecureDigital_File = vm.Get_SecureDigital_File();
	
	this->PFlash = vm.Use_PFlash_File();
	this->PFlash_File = vm.Get_PFlash_File();
	
	this->Enable_KVM = vm.Use_KVM();
	this->KVM_IRQChip = vm.Use_KVM_IRQChip();
	this->No_KVM_Pit = vm.Use_No_KVM_Pit();
	this->KVM_No_Pit_Reinjection = vm.Use_KVM_No_Pit_Reinjection();
	this->KVM_Nesting = vm.Use_KVM_Nesting();
	this->KVM_Shadow_Memory = vm.Use_KVM_Shadow_Memory();
	this->KVM_Shadow_Memory_Size = vm.Get_KVM_Shadow_Memory_Size();
	
	this->Init_Graphic_Mode = vm.Get_Init_Graphic_Mode();
	
	this->No_Frame = vm.Use_No_Frame();
	this->Alt_Grab = vm.Use_Alt_Grab();
	this->No_Quit = vm.Use_No_Quit();
	this->Portrait = vm.Use_Portrait();
	this->Show_Cursor = vm.Use_Show_Cursor();
	this->Curses = vm.Use_Curses();
	this->RTC_TD_Hack = vm.Use_RTC_TD_Hack();
	
	this->Start_Date = vm.Use_Start_Date();
	this->Start_DateTime = vm.Get_Start_Date();
	
	this->SPICE = vm.Get_SPICE();
	
	this->VNC = vm.Use_VNC();
	this->VNC_Socket_Mode = vm.Get_VNC_Socket_Mode();
	this->VNC_Unix_Socket_Path = vm.Get_VNC_Unix_Socket_Path();
	this->VNC_Display_Number = vm.Get_VNC_Display_Number();
	this->VNC_Password = vm.Use_VNC_Password();
	this->VNC_TLS = vm.Use_VNC_TLS();
	this->VNC_x509 = vm.Use_VNC_x509();
	this->VNC_x509_Folder_Path = vm.Get_VNC_x509_Folder_Path();
	this->VNC_x509verify = vm.Use_VNC_x509verify();
	this->VNC_x509verify_Folder_Path = vm.Get_VNC_x509verify_Folder_Path();
	
	this->Load_VM_Window = nullptr;
	this->Save_VM_Window = nullptr;
	this->Load_Mode = false;
	
    Update_Current_Emulator_Devices();
	
	return *this;
}

const QString &Virtual_Machine::Get_VM_XML_File_Path() const
{
	return VM_XML_File_Path;
}

void Virtual_Machine::Set_VM_XML_File_Path( const QString &path )
{
	VM_XML_File_Path = path;
}

bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )
{
	VM_XML_File_Path = file_name;
	
	if( file_name.isEmpty() )
	{
		AQError( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
				 "File Name is Empty!" );
		return false;
	}
	
	QDomDocument New_Dom_Document( "AQEMU" );
	QDomProcessingInstruction Pro_Instr = New_Dom_Document.createProcessingInstruction( "xml", "version=\"1.0\" encoding=\"UTF-8\"" );
	New_Dom_Document.appendChild( Pro_Instr );
	QDomElement Root_Element;
	
	if( template_mode )
		Root_Element = New_Dom_Document.createElement( "AQEMU_Template" );
	else
		Root_Element = New_Dom_Document.createElement( "AQEMU" );
	
	Root_Element.setAttribute( "version", "0.8" ); // This is AQEMU Version
	New_Dom_Document.appendChild( Root_Element );
	
	QDomElement VM_Element = New_Dom_Document.createElement( "Virtual_Machine" );
	Root_Element.appendChild( VM_Element );
	
	// Save New VM Data
	QDomElement Dom_Element;
	QDomText Dom_Text;
	
	if( template_mode )
	{
		// Temlate Options
		Dom_Element = New_Dom_Document.createElement( "Template_Options" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number((int)Template_Opts) );
		Dom_Element.appendChild( Dom_Text );
	}
	
	// VM State
	Dom_Element = New_Dom_Document.createElement( "VM_State" );
	VM_Element.appendChild( Dom_Element );
	
	if( template_mode )
	{
		Dom_Text = New_Dom_Document.createTextNode( "Power_Off" );
	}
	else
	{
		switch( State )
		{
			case VM::VMS_Power_Off:
				Dom_Text = New_Dom_Document.createTextNode( "Power_Off" );
				break;
		
			case VM::VMS_Saved:
				Dom_Text = New_Dom_Document.createTextNode( "Saved" );
				break;
			
			default:
				Dom_Text = New_Dom_Document.createTextNode( "Power_Off" );
				break;
		}
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// Icon Path
	Dom_Element = New_Dom_Document.createElement( "Icon_Path" );
	VM_Element.appendChild( Dom_Element );

    QSettings settings;
    QString data_folder = settings.value("AQEMU_Data_Folder", "").toString();
    if ( ! data_folder.isEmpty() )
    {
        if ( Icon_Path.startsWith( data_folder ) ) //save relative path if possible
            Dom_Text = New_Dom_Document.createTextNode( QString(Icon_Path).replace(data_folder, "") );
        else    
    	    Dom_Text = New_Dom_Document.createTextNode( Icon_Path );
    }
	Dom_Element.appendChild( Dom_Text );
	
	// Screenshot Path
	Dom_Element = New_Dom_Document.createElement( "Screenshot_Path" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Screenshot_Path );
	Dom_Element.appendChild( Dom_Text );
	
	// Machine Name
	Dom_Element = New_Dom_Document.createElement( "Machine_Name" );
	VM_Element.appendChild( Dom_Element );
	
	if( template_mode )
		Dom_Text = New_Dom_Document.createTextNode( Template_Name );
	else
		Dom_Text = New_Dom_Document.createTextNode( Machine_Name );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Emulator Name
	Dom_Element = New_Dom_Document.createElement( "Emulator_Name" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Current_Emulator.Get_Name() );
	Dom_Element.appendChild( Dom_Text );
	
	// Emulator Type (legacy name) = now Machine_Accelerator
	Dom_Element = New_Dom_Document.createElement( "Machine_Accelerator" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( VM::Accel_To_String(Machine_Accelerator) );
	Dom_Element.appendChild( Dom_Text );
	
	// Computer Type
	Dom_Element = New_Dom_Document.createElement( "Computer_Type" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Computer_Type );
	Dom_Element.appendChild( Dom_Text );
	
	// Machine Type
	Dom_Element = New_Dom_Document.createElement( "Machine_Type" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Machine_Type );
	Dom_Element.appendChild( Dom_Text );
	
	// CPU Type
	Dom_Element = New_Dom_Document.createElement( "CPU_Type" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( CPU_Type );
	Dom_Element.appendChild( Dom_Text );
	
	// SMP_CPU_Count
	Dom_Element = New_Dom_Document.createElement( "SMP_CPU_Count" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SMP.SMP_Count) );
	Dom_Element.appendChild( Dom_Text );
	
	// SMP_Cores
	Dom_Element = New_Dom_Document.createElement( "SMP_Cores" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SMP.SMP_Cores) );
	Dom_Element.appendChild( Dom_Text );
	
	// SMP_Threads
	Dom_Element = New_Dom_Document.createElement( "SMP_Threads" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SMP.SMP_Threads) );
	Dom_Element.appendChild( Dom_Text );
	
	// SMP_Sockets
	Dom_Element = New_Dom_Document.createElement( "SMP_Sockets" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SMP.SMP_Sockets) );
	Dom_Element.appendChild( Dom_Text );
	
	// SMP_MaxCPUs
	Dom_Element = New_Dom_Document.createElement( "SMP_MaxCPUs" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SMP.SMP_MaxCPUs) );
	Dom_Element.appendChild( Dom_Text );
	
	// Keyboard Layout
	Dom_Element = New_Dom_Document.createElement( "Keyboard_Layout" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Keyboard_Layout );
	Dom_Element.appendChild( Dom_Text );
	
	// Create new element
	QDomElement Sec_Element;
	
	// Boot Device
	Dom_Element = New_Dom_Document.createElement( "Boot_Device_Count" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(Boot_Order_List.count()) );
	Dom_Element.appendChild( Dom_Text );
	
	for( int bx = 0; bx < Boot_Order_List.count(); bx++ )
	{
		Dom_Element = New_Dom_Document.createElement( "Boot_Device_" + QString::number(bx) );
		
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		if( Boot_Order_List[bx].Enabled ) Dom_Text = New_Dom_Document.createTextNode( "true" );
		else Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		Sec_Element = New_Dom_Document.createElement( "Type" );
		Dom_Element.appendChild( Sec_Element );
		
		switch( Boot_Order_List[bx].Type )
		{
			case VM::Boot_From_FDA:
				Dom_Text = New_Dom_Document.createTextNode( "FDA" );
				break;
				
			case VM::Boot_From_FDB:
				Dom_Text = New_Dom_Document.createTextNode( "FDB" );
				break;
				
			case VM::Boot_From_CDROM:
				Dom_Text = New_Dom_Document.createTextNode( "CDROM" );
				break;
				
			case VM::Boot_From_HDD:
				Dom_Text = New_Dom_Document.createTextNode( "HDD" );
				break;
				
			case VM::Boot_From_Network1:
				Dom_Text = New_Dom_Document.createTextNode( "Network1" );
				break;
				
			case VM::Boot_From_Network2:
				Dom_Text = New_Dom_Document.createTextNode( "Network2" );
				break;
				
			case VM::Boot_From_Network3:
				Dom_Text = New_Dom_Document.createTextNode( "Network3" );
				break;
				
			case VM::Boot_From_Network4:
				Dom_Text = New_Dom_Document.createTextNode( "Network4" );
				break;
				
			default:
				AQWarning( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
						   "Use Default Boot Device: CD-ROM" );
				Dom_Text = New_Dom_Document.createTextNode( "CDROM" );
				break;
		}
		
		Sec_Element.appendChild( Dom_Text );
		VM_Element.appendChild( Dom_Element );
	}
	
	// Show Boot Menu
	Dom_Element = New_Dom_Document.createElement( "Show_Boot_Menu" );
	VM_Element.appendChild( Dom_Element );
	if( Show_Boot_Menu ) Dom_Text = New_Dom_Document.createTextNode( "true" );
	else Dom_Text = New_Dom_Document.createTextNode( "false" );
	Dom_Element.appendChild( Dom_Text );
	
	// Video_Card
	Dom_Element = New_Dom_Document.createElement( "Video_Card" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Video_Card );
	Dom_Element.appendChild( Dom_Text );
	
	// Audio Cards
	Dom_Element = New_Dom_Document.createElement( "Audio_Cards" );
	
	// Audio_sb16
	Sec_Element = New_Dom_Document.createElement( "Audio_sb16" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_sb16 )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Audio_es1370
	Sec_Element = New_Dom_Document.createElement( "Audio_es1370" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_es1370 )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Audio_Adlib
	Sec_Element = New_Dom_Document.createElement( "Audio_Adlib" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_Adlib )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Audio_PC_Speaker
	Sec_Element = New_Dom_Document.createElement( "Audio_PC_Speaker" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_PC_Speaker )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Audio_GUS
	Sec_Element = New_Dom_Document.createElement( "Audio_GUS" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_GUS )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Audio_AC97
	Sec_Element = New_Dom_Document.createElement( "Audio_AC97" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_AC97 )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	VM_Element.appendChild( Dom_Element );
	
	// Audio_HDA
	Sec_Element = New_Dom_Document.createElement( "Audio_HDA" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_HDA )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	VM_Element.appendChild( Dom_Element );
	
	// Audio_cs4231a
	Sec_Element = New_Dom_Document.createElement( "Audio_cs4231a" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Audio_Card.Audio_cs4231a )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	VM_Element.appendChild( Dom_Element );
	
	// Check Free RAM
	Dom_Element = New_Dom_Document.createElement( "Remove_RAM_Size_Limitation" );
	VM_Element.appendChild( Dom_Element );
	
	if( Remove_RAM_Size_Limitation )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Memory Size ( RAM )
	Dom_Element = New_Dom_Document.createElement( "Memory_Size" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(Memory_Size) );
	Dom_Element.appendChild( Dom_Text );
	
	// Fullscreen
	Dom_Element = New_Dom_Document.createElement( "Fullscreen" );
	VM_Element.appendChild( Dom_Element );
	
	if( Fullscreen )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Win2K_Hack
	Dom_Element = New_Dom_Document.createElement( "Win2K_Hack" );
	VM_Element.appendChild( Dom_Element );
	
	if( Win2K_Hack )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Localtime
	Dom_Element = New_Dom_Document.createElement( "Local_Time" );
	VM_Element.appendChild( Dom_Element );
	
	if( Local_Time )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Check FDD Boot Sector
	Dom_Element = New_Dom_Document.createElement( "Check_FDD_Boot_Sector" );
	VM_Element.appendChild( Dom_Element );
	
	if( Check_FDD_Boot_Sector )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// ACPI
	Dom_Element = New_Dom_Document.createElement( "ACPI" );
	VM_Element.appendChild( Dom_Element );
	
	if( ACPI )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Snapshot_Mode
	Dom_Element = New_Dom_Document.createElement( "Snapshot_Mode" );
	VM_Element.appendChild( Dom_Element );
	
	if( Snapshot_Mode )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Start_CPU
	Dom_Element = New_Dom_Document.createElement( "Start_CPU" );
	VM_Element.appendChild( Dom_Element );
	
	if( Start_CPU )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// No_Reboot
	Dom_Element = New_Dom_Document.createElement( "No_Reboot" );
	VM_Element.appendChild( Dom_Element );
	
	if( No_Reboot )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// No_Shutdown
	Dom_Element = New_Dom_Document.createElement( "No_Shutdown" );
	VM_Element.appendChild( Dom_Element );
	
	if( No_Shutdown )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Floppy's and CD-ROM
	if( template_mode &&
		! (Template_Opts & Create_Template_Window::Template_Save_FDD_CD) )
	{
		// Floppy 0
		Dom_Element = New_Dom_Document.createElement( "FD0" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// Floppy 1
		Dom_Element = New_Dom_Document.createElement( "FD1" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// CD-ROM
		Dom_Element = New_Dom_Document.createElement( "CD_ROM" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
	}
	else
	{
		// Floppy 0
		Dom_Element = New_Dom_Document.createElement( "FD0" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		if( FD0.Get_Enabled() ) Dom_Text = New_Dom_Document.createTextNode( "true" );
		else Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( FD0.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, FD0.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// Floppy 1
		Dom_Element = New_Dom_Document.createElement( "FD1" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		if( FD1.Get_Enabled() ) Dom_Text = New_Dom_Document.createTextNode( "true" );
		else Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( FD1.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, FD1.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// CD-ROM
		Dom_Element = New_Dom_Document.createElement( "CD_ROM" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		if( CD_ROM.Get_Enabled() ) Dom_Text = New_Dom_Document.createTextNode( "true" );
		else Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( CD_ROM.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, CD_ROM.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
	}
	
	if( template_mode &&
		! (Template_Opts & Create_Template_Window::Template_Save_HDD) )
	{
		// HDA
		Dom_Element = New_Dom_Document.createElement( "HDA" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// HDB
		Dom_Element = New_Dom_Document.createElement( "HDB" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// HDC
		Dom_Element = New_Dom_Document.createElement( "HDC" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// HDD
		Dom_Element = New_Dom_Document.createElement( "HDD" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, VM_Native_Storage_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
	}
	else
	{
		// HDA
		Dom_Element = New_Dom_Document.createElement( "HDA" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		
		if( HDA.Get_Enabled() )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( HDA.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, HDA.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// HDB
		Dom_Element = New_Dom_Document.createElement( "HDB" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		
		if( HDB.Get_Enabled() )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( HDB.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, HDB.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// HDC
		Dom_Element = New_Dom_Document.createElement( "HDC" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		
		if( HDC.Get_Enabled() )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( HDC.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, HDC.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
		
		// HDD
		Dom_Element = New_Dom_Document.createElement( "HDD" );
		
		// Enabled
		Sec_Element = New_Dom_Document.createElement( "Enabled" );
		Dom_Element.appendChild( Sec_Element );
		
		if( HDD.Get_Enabled() )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Sec_Element.appendChild( Dom_Text );
		
		// Image File Name
		Sec_Element = New_Dom_Document.createElement( "Image_File_Name" );
		Dom_Element.appendChild( Sec_Element );
		Dom_Text = New_Dom_Document.createTextNode( HDD.Get_File_Name() );
		Sec_Element.appendChild( Dom_Text );
		
		// Nativ Device
        Sec_Element = New_Dom_Document.createElement( "Native_Device" );
        Save_VM_Native_Storage_Device( New_Dom_Document, Sec_Element, HDD.Get_Native_Device() );
		Dom_Element.appendChild( Sec_Element );
		
		VM_Element.appendChild( Dom_Element );
	}
	
	// Storage Devices
	if( Get_Current_Emulator_Devices()->PSO_Drive &&
	    (template_mode == false || (template_mode == true && Template_Opts & Create_Template_Window::Template_Save_HDD)) )
	{
		// Storage Device Count
		Dom_Element = New_Dom_Document.createElement( "Storage_Device_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Storage_Devices.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int sx = 0; sx < Storage_Devices.count(); ++sx )
		{
			Dom_Element = New_Dom_Document.createElement( "Storage_Device_" + QString::number(sx) );
            Save_VM_Native_Storage_Device( New_Dom_Document, Dom_Element, Storage_Devices[sx] );
			VM_Element.appendChild( Dom_Element );
		}
	}
	else
	{
		// Not Devices
		Dom_Element = New_Dom_Document.createElement( "Storage_Device_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
	}

	// Shared Folders
	if( true /*(template_mode == false || (template_mode == true && Template_Opts & Create_Template_Window::Template_Save_Shared_Folders)) //TODO: not using template mode, not sure if it's needed */ )
	{
		// Storage Device Count
		Dom_Element = New_Dom_Document.createElement( "Shared_Folder_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Shared_Folders.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int sx = 0; sx < Shared_Folders.count(); ++sx )
		{
			Dom_Element = New_Dom_Document.createElement( "Shared_Folder_" + QString::number(sx) );
			Save_VM_Shared_Folder( New_Dom_Document, Dom_Element, Shared_Folders[sx] );
			VM_Element.appendChild( Dom_Element );
		}
	} /*
	else
	{
		// Not Devices
		Dom_Element = New_Dom_Document.createElement( "Shared_Folders_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
	}*/
	
	// Snapshots
	if( template_mode )
	{
		// Snapshots Count
		Dom_Element = New_Dom_Document.createElement( "Snapshots_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( "0" );
		Dom_Element.appendChild( Dom_Text );
	}
	else
	{
		// Snapshots Count
		Dom_Element = New_Dom_Document.createElement( "Snapshots_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Snapshots.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int sx = 0; sx < Snapshots.count(); ++sx )
		{
			Dom_Element = New_Dom_Document.createElement( "Snapshot_" + QString::number(sx) );
			
			// Tag
			Sec_Element = New_Dom_Document.createElement( "Tag" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Snapshots[sx].Get_Tag() );
			Sec_Element.appendChild( Dom_Text );
			
			// Name
			Sec_Element = New_Dom_Document.createElement( "Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Snapshots[sx].Get_Name() );
			Sec_Element.appendChild( Dom_Text );
			
			// Description
			Sec_Element = New_Dom_Document.createElement( "Description" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Snapshots[sx].Get_Description() );
			Sec_Element.appendChild( Dom_Text );
			
			// END
			VM_Element.appendChild( Dom_Element );
		}
	}
	
	// Network
	if( template_mode &&
		! (Template_Opts & Create_Template_Window::Template_Save_Network) )
	{
		// Use Network
		Dom_Element = New_Dom_Document.createElement( "Use_Network" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Dom_Element.appendChild( Dom_Text );
		
        // Use Native_Network
        Dom_Element = New_Dom_Document.createElement( "Native_Network" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Dom_Element.appendChild( Dom_Text );
		
		// Use Redirections
		Dom_Element = New_Dom_Document.createElement( "Use_Redirections" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( "false" );
		Dom_Element.appendChild( Dom_Text );
		
		// Network Cards Count
		Dom_Element = New_Dom_Document.createElement( "Network_Cards_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
		
		// Network Cards Count
        Dom_Element = New_Dom_Document.createElement( "Network_Cards_Native_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
		
		// Redirections count
		Dom_Element = New_Dom_Document.createElement( "Redirections_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
		
		// TFTP Prefix
		Dom_Element = New_Dom_Document.createElement( "TFTP_Prefix" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Dom_Element.appendChild( Dom_Text );
		
		// SMB Directory
		Dom_Element = New_Dom_Document.createElement( "SMB_Directory" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( "" );
		Dom_Element.appendChild( Dom_Text );
	}
	else
	{
		// Use Network
		Dom_Element = New_Dom_Document.createElement( "Use_Network" );
		VM_Element.appendChild( Dom_Element );
		
		if( Use_Network )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Dom_Element.appendChild( Dom_Text );
		
		// Use Redirections
		Dom_Element = New_Dom_Document.createElement( "Use_Redirections" );
		VM_Element.appendChild( Dom_Element );
		
		if( Use_Redirections )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Dom_Element.appendChild( Dom_Text );
		
		// Network Cards Count
		Dom_Element = New_Dom_Document.createElement( "Network_Cards_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int nx = 0; nx < Network_Cards.count(); nx++ )
		{
			Dom_Element = New_Dom_Document.createElement( "Network_Card_" + QString::number(nx) );
			
			// Card Model
			Sec_Element = New_Dom_Document.createElement( "Card_Model" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards[nx].Get_Card_Model() );
			Sec_Element.appendChild( Dom_Text );
			
			// Network Mode
			Sec_Element = New_Dom_Document.createElement( "Network_Mode" );
			Dom_Element.appendChild( Sec_Element );
			
			switch( Network_Cards[nx].Get_Net_Mode() )
			{
				case VM::Net_Mode_None:
					Dom_Text = New_Dom_Document.createTextNode( "None" );
					break;
				
				case VM::Net_Mode_Usermode:
					Dom_Text = New_Dom_Document.createTextNode( "Usermode" );
					break;
				
				case VM::Net_Mode_Tuntap:
					Dom_Text = New_Dom_Document.createTextNode( "Tuntap" );
					break;
				
				case VM::Net_Mode_Tuntapfd:
					Dom_Text = New_Dom_Document.createTextNode( "Tuntapfd" );
					break;
				
				case VM::Net_Mode_Tcplisten:
					Dom_Text = New_Dom_Document.createTextNode( "Tcplisten" );
					break;
				
				case VM::Net_Mode_Tcpfd:
					Dom_Text = New_Dom_Document.createTextNode( "Tcpfd" );
					break;
				
				case VM::Net_Mode_Tcpconnect:
					Dom_Text = New_Dom_Document.createTextNode( "Tcpconnect" );
					break;
				
				case VM::Net_Mode_Multicast:
					Dom_Text = New_Dom_Document.createTextNode( "Multicast" );
					break;
				
				case VM::Net_Mode_Multicastfd:
					Dom_Text = New_Dom_Document.createTextNode( "Multicastfd" );
					break;
				
				default:
					Dom_Text = New_Dom_Document.createTextNode( "Dummy" );
					break;
			}
			
			Sec_Element.appendChild( Dom_Text );
			
			// Hostname
			Sec_Element = New_Dom_Document.createElement( "Hostname" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards[nx].Get_Hostname() );
			Sec_Element.appendChild( Dom_Text );
			
			// IP Address
			Sec_Element = New_Dom_Document.createElement( "IP_Address" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards[nx].Get_IP_Address() );
			Sec_Element.appendChild( Dom_Text );
			
			// MAC Address
			Sec_Element = New_Dom_Document.createElement( "MAC_Address" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards[nx].Get_MAC_Address() );
			Sec_Element.appendChild( Dom_Text );
			
			// Port
			Sec_Element = New_Dom_Document.createElement( "Port" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards[nx].Get_Port()) );
			Sec_Element.appendChild( Dom_Text );
			
			// VLAN
			Sec_Element = New_Dom_Document.createElement( "VLAN" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards[nx].Get_VLAN()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Use TUN TAP Script
			Sec_Element = New_Dom_Document.createElement( "Use_TUN_TAP_Script" );
			Dom_Element.appendChild( Sec_Element );
			
			if( Network_Cards[nx].Get_Use_TUN_TAP_Script() )
				Dom_Text = New_Dom_Document.createTextNode( "true" );
			else
				Dom_Text = New_Dom_Document.createTextNode( "false" );
			
			Sec_Element.appendChild( Dom_Text );
			
			// TUN TAP Script
			Sec_Element = New_Dom_Document.createElement( "TUN_TAP_Script" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards[nx].Get_TUN_TAP_Script() );
			Sec_Element.appendChild( Dom_Text );
			
			// Interface Name
			Sec_Element = New_Dom_Document.createElement( "Interface_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards[nx].Get_Interface_Name() );
			Sec_Element.appendChild( Dom_Text );
			
			// File Descriptor
			Sec_Element = New_Dom_Document.createElement( "File_Descriptor" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards[nx].Get_File_Descriptor()) );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
		}
		
        // Use Native_Network
        Dom_Element = New_Dom_Document.createElement( "Native_Network" );
		VM_Element.appendChild( Dom_Element );
		
        if( Native_Network )
			Dom_Text = New_Dom_Document.createTextNode( "true" );
		else
			Dom_Text = New_Dom_Document.createTextNode( "false" );
		
		Dom_Element.appendChild( Dom_Text );
		
		// Nativ Network Card
		// Network Cards Count
        Dom_Element = New_Dom_Document.createElement( "Network_Cards_Native_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int nx = 0; nx < Network_Cards_Nativ.count(); nx++ )
		{
            Dom_Element = New_Dom_Document.createElement( "Network_Card_Native_" + QString::number(nx) );
			
			Sec_Element = New_Dom_Document.createElement( "Network_Type" );
			Dom_Element.appendChild( Sec_Element );
			
			switch( Network_Cards_Nativ[nx].Get_Network_Type() )
			{
                case VM::Net_Mode_Native_NIC:
					Dom_Text = New_Dom_Document.createTextNode( "NIC" );
					break;
					
                case VM::Net_Mode_Native_User:
					Dom_Text = New_Dom_Document.createTextNode( "User" );
					break;
					
                case VM::Net_Mode_Native_Chanel:
					Dom_Text = New_Dom_Document.createTextNode( "Chanel" );
                    break;

                case VM::Net_Mode_Native_Bridge:
                    Dom_Text = New_Dom_Document.createTextNode( "Bridge" );
                    break;

                case VM::Net_Mode_Native_TAP:
					Dom_Text = New_Dom_Document.createTextNode( "TAP" );
					break;
					
                case VM::Net_Mode_Native_Socket:
					Dom_Text = New_Dom_Document.createTextNode( "Socket" );
					break;
					
                case VM::Net_Mode_Native_MulticastSocket:
					Dom_Text = New_Dom_Document.createTextNode( "MulticastSocket" );
					break;
					
                case VM::Net_Mode_Native_VDE:
					Dom_Text = New_Dom_Document.createTextNode( "VDE" );
					break;
					
                case VM::Net_Mode_Native_Dump:
					Dom_Text = New_Dom_Document.createTextNode( "Dump" );
					break;
					
				default:
					AQWarning( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
							   "Invalid Network Type. Use Default: User" );
					Dom_Text = New_Dom_Document.createTextNode( "User" );
					break;
			}
			
			Sec_Element.appendChild( Dom_Text );
			
			// Card_Model
			Sec_Element = New_Dom_Document.createElement( "Card_Model" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Card_Model() );
			Sec_Element.appendChild( Dom_Text );
			
			// MAC_Address
			Sec_Element = New_Dom_Document.createElement( "Use_MAC_Address" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_MAC_Address() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "MAC_Address" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_MAC_Address() );
			Sec_Element.appendChild( Dom_Text );
			
			// VLAN
			Sec_Element = New_Dom_Document.createElement( "Use_VLAN" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_VLAN() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "VLAN" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_VLAN()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Name
			Sec_Element = New_Dom_Document.createElement( "Use_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Name() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Name() );
			Sec_Element.appendChild( Dom_Text );
			
			// Hostname
			Sec_Element = New_Dom_Document.createElement( "Use_Hostname" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Hostname() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Hostname" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Hostname() );
			Sec_Element.appendChild( Dom_Text );
			
			// PortDev
			Sec_Element = New_Dom_Document.createElement( "PortDev" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_PortDev() );
			Sec_Element.appendChild( Dom_Text );
			
			// File_Descriptor
			Sec_Element = New_Dom_Document.createElement( "Use_File_Descriptor" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_File_Descriptor() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "File_Descriptor" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_File_Descriptor()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Interface_Name
			Sec_Element = New_Dom_Document.createElement( "Use_Interface_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Interface_Name() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Interface_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Interface_Name() );
			Sec_Element.appendChild( Dom_Text );
			
			// Bridge_Name
			Sec_Element = New_Dom_Document.createElement( "Use_Bridge_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Bridge_Name() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );

			Sec_Element = New_Dom_Document.createElement( "Bridge_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Bridge_Name() );
			Sec_Element.appendChild( Dom_Text );

			// TUN_TAP_Script
			Sec_Element = New_Dom_Document.createElement( "Use_TUN_TAP_Script" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_TUN_TAP_Script() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "TUN_TAP_Script" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_TUN_TAP_Script() );
			Sec_Element.appendChild( Dom_Text );
			
			// TUN_TAP_Down_Script
			Sec_Element = New_Dom_Document.createElement( "Use_TUN_TAP_Down_Script" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_TUN_TAP_Down_Script() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "TUN_TAP_Down_Script" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_TUN_TAP_Down_Script() );
			Sec_Element.appendChild( Dom_Text );
			
			// Bridge_Helper
			Sec_Element = New_Dom_Document.createElement( "Use_Bridge_Helper" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Bridge_Helper() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );

			Sec_Element = New_Dom_Document.createElement( "Bridge_Helper" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Bridge_Helper() );
			Sec_Element.appendChild( Dom_Text );

			// Listen
			Sec_Element = New_Dom_Document.createElement( "Use_Listen" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Listen() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Listen" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Listen() );
			Sec_Element.appendChild( Dom_Text );
			
			// Connect
			Sec_Element = New_Dom_Document.createElement( "Use_Connect" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Connect() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Connect" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Connect() );
			Sec_Element.appendChild( Dom_Text );
			
			// MCast
			Sec_Element = New_Dom_Document.createElement( "Use_MCast" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_MCast() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "MCast" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_MCast() );
			Sec_Element.appendChild( Dom_Text );
			
			// Sock
			Sec_Element = New_Dom_Document.createElement( "Use_Sock" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Sock() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Sock" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Sock() );
			Sec_Element.appendChild( Dom_Text );
			
			// Port
			Sec_Element = New_Dom_Document.createElement( "Use_Port" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Port() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Port" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_Port()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Group
			Sec_Element = New_Dom_Document.createElement( "Use_Group" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Group() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Group" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Group() );
			Sec_Element.appendChild( Dom_Text );
			
			// Mode
			Sec_Element = New_Dom_Document.createElement( "Use_Mode" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Mode() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Mode" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Mode() );
			Sec_Element.appendChild( Dom_Text );
			
			// File
			Sec_Element = New_Dom_Document.createElement( "Use_File" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_File() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "File" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_File() );
			Sec_Element.appendChild( Dom_Text );
			
			// Len
			Sec_Element = New_Dom_Document.createElement( "Use_Len" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Len() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Len" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_Len()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Addr
			Sec_Element = New_Dom_Document.createElement( "Use_Addr" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Addr() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Addr" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Addr() );
			Sec_Element.appendChild( Dom_Text );
			
			// Vectors
			Sec_Element = New_Dom_Document.createElement( "Use_Vectors" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Vectors() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Vectors" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_Vectors()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Net
			Sec_Element = New_Dom_Document.createElement( "Use_Net" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Net() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Net" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Net() );
			Sec_Element.appendChild( Dom_Text );
			
			// Host
			Sec_Element = New_Dom_Document.createElement( "Use_Host" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Host() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Host" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Host() );
			Sec_Element.appendChild( Dom_Text );
			
			// Restrict
			Sec_Element = New_Dom_Document.createElement( "Use_Restrict" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Restrict() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Restrict" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Restrict() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			// DHCPstart
			Sec_Element = New_Dom_Document.createElement( "Use_DHCPstart" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_DHCPstart() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "DHCPstart" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_DHCPstart() );
			Sec_Element.appendChild( Dom_Text );
			
			// DNS
			Sec_Element = New_Dom_Document.createElement( "Use_DNS" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_DNS() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "DNS" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_DNS() );
			Sec_Element.appendChild( Dom_Text );
			
			// Tftp
			Sec_Element = New_Dom_Document.createElement( "Use_Tftp" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Tftp() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Tftp" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Tftp() );
			Sec_Element.appendChild( Dom_Text );
			
			// Bootfile
			Sec_Element = New_Dom_Document.createElement( "Use_Bootfile" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Bootfile() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Bootfile" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_Bootfile() );
			Sec_Element.appendChild( Dom_Text );
			
			// HostFwd
			Sec_Element = New_Dom_Document.createElement( "Use_HostFwd" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_HostFwd() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "HostFwd" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_HostFwd() );
			Sec_Element.appendChild( Dom_Text );
			
			// GuestFwd
			Sec_Element = New_Dom_Document.createElement( "Use_GuestFwd" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_GuestFwd() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "GuestFwd" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_GuestFwd() );
			Sec_Element.appendChild( Dom_Text );
			
			// SMB
			Sec_Element = New_Dom_Document.createElement( "Use_SMB" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_SMB() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "SMB" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_SMB() );
			Sec_Element.appendChild( Dom_Text );
			
			// SMBserver
			Sec_Element = New_Dom_Document.createElement( "Use_SMBserver" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_SMBserver() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "SMBserver" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_SMBserver() );
			Sec_Element.appendChild( Dom_Text );
			
			// Sndbuf
			Sec_Element = New_Dom_Document.createElement( "Use_Sndbuf" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_Sndbuf() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "Sndbuf" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_Sndbuf()) );
			Sec_Element.appendChild( Dom_Text );
			
			// VNet_hdr
			Sec_Element = New_Dom_Document.createElement( "Use_VNet_hdr" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_VNet_hdr() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "VNet_hdr" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_VNet_hdr() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			// VHost
			Sec_Element = New_Dom_Document.createElement( "Use_VHost" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_VHost() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "VHost" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Get_VHost() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			// VHostFd
			Sec_Element = New_Dom_Document.createElement( "Use_VHostFd" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Cards_Nativ[nx].Use_VHostFd() ? "yes" : "no" );
			Sec_Element.appendChild( Dom_Text );
			
			Sec_Element = New_Dom_Document.createElement( "VHostFd" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Cards_Nativ[nx].Get_VHostFd()) );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
		}
		
		// Redirections count
		Dom_Element = New_Dom_Document.createElement( "Redirections_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Redirections.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		// Network Redirections
		for( int rx = 0; rx < Network_Redirections.count(); rx++ )
		{
			Dom_Element = New_Dom_Document.createElement( "Network_Redirections_" + QString::number(rx) );
			
			// Protocol
			Sec_Element = New_Dom_Document.createElement( "Protocol" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Redirections[rx].Get_Protocol() );
			Sec_Element.appendChild( Dom_Text );
			
			// Host Port
			Sec_Element = New_Dom_Document.createElement( "Host_Port" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Redirections[rx].Get_Host_Port()) );
			Sec_Element.appendChild( Dom_Text );
			
			// Guest IP
			Sec_Element = New_Dom_Document.createElement( "Guest_IP" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Network_Redirections[rx].Get_Guest_IP() );
			Sec_Element.appendChild( Dom_Text );
			
			// Guest Port
			Sec_Element = New_Dom_Document.createElement( "Guest_Port" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( QString::number(Network_Redirections[rx].Get_Guest_Port()) );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
		}
		
		// TFTP Prefix
		Dom_Element = New_Dom_Document.createElement( "TFTP_Prefix" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( TFTP_Prefix );
		Dom_Element.appendChild( Dom_Text );
		
		// SMB Directory
		Dom_Element = New_Dom_Document.createElement( "SMB_Directory" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( SMB_Directory );
		Dom_Element.appendChild( Dom_Text );
	}
	
	if( template_mode &&
		! (Template_Opts & Create_Template_Window::Template_Save_Ports) )
	{
		// Serial Port
		Dom_Element = New_Dom_Document.createElement( "Serial_Ports_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
		
		// Parallel Port
		Dom_Element = New_Dom_Document.createElement( "Parallel_Ports_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
		
		// USB Ports
		Dom_Element = New_Dom_Document.createElement( "USB_Ports_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(0) );
		Dom_Element.appendChild( Dom_Text );
	}
	else
	{
		// Serial Port
		Dom_Element = New_Dom_Document.createElement( "Serial_Port_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Serial_Ports.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int px = 0; px < Serial_Ports.count(); px++ )
		{
			Dom_Element = New_Dom_Document.createElement( "Serial_Port_" + QString::number(px) );
			
			// Port Redirection
			Sec_Element = New_Dom_Document.createElement( "Port_Redirection" );
			Dom_Element.appendChild( Sec_Element );
			
			switch( Serial_Ports[px].Get_Port_Redirection() )
			{
				case VM::PR_vc:
					Dom_Text = New_Dom_Document.createTextNode( "vc" );
					break;
				
				case VM::PR_pty:
					Dom_Text = New_Dom_Document.createTextNode( "pty" );
					break;
				
				case VM::PR_none:
					Dom_Text = New_Dom_Document.createTextNode( "none" );
					break;
				
				case VM::PR_null:
					Dom_Text = New_Dom_Document.createTextNode( "null" );
					break;
				
				case VM::PR_dev:
					Dom_Text = New_Dom_Document.createTextNode( "dev" );
					break;
				
				case VM::PR_file:
					Dom_Text = New_Dom_Document.createTextNode( "file" );
					break;
				
				case VM::PR_stdio:
					Dom_Text = New_Dom_Document.createTextNode( "stdio" );
					break;
				
				case VM::PR_pipe:
					Dom_Text = New_Dom_Document.createTextNode( "pipe" );
					break;
				
				case VM::PR_udp:
					Dom_Text = New_Dom_Document.createTextNode( "udp" );
					break;
				
				case VM::PR_tcp:
					Dom_Text = New_Dom_Document.createTextNode( "tcp" );
					break;
				
				case VM::PR_telnet:
					Dom_Text = New_Dom_Document.createTextNode( "telnet" );
					break;
				
				case VM::PR_unix:
					Dom_Text = New_Dom_Document.createTextNode( "unix" );
					break;
					
				case VM::PR_com:
					Dom_Text = New_Dom_Document.createTextNode( "com" );
					break;
					
				case VM::PR_msmouse:
					Dom_Text = New_Dom_Document.createTextNode( "msmouse" );
					break;
					
				case VM::PR_mon:
					Dom_Text = New_Dom_Document.createTextNode( "mon" );
					break;
					
				case VM::PR_braille:
					Dom_Text = New_Dom_Document.createTextNode( "braille" );
					break;
				
				default:
					Dom_Text = New_Dom_Document.createTextNode( "Default" );
					break;
			}
			
			Sec_Element.appendChild( Dom_Text );
			
			// Parametrs Line
			Sec_Element = New_Dom_Document.createElement( "Parametrs" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Serial_Ports[px].Get_Parametrs_Line() );
			Sec_Element.appendChild( Dom_Text );
		
			VM_Element.appendChild( Dom_Element );
		}
		
		// Parallel Port
		Dom_Element = New_Dom_Document.createElement( "Parallel_Port_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(Parallel_Ports.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		for( int px = 0; px < Parallel_Ports.count(); px++ )
		{
			Dom_Element = New_Dom_Document.createElement( "Parallel_Port_" + QString::number(px) );
			
			// Port Redirection
			Sec_Element = New_Dom_Document.createElement( "Port_Redirection" );
			Dom_Element.appendChild( Sec_Element );
			
			switch( Parallel_Ports[px].Get_Port_Redirection() )
			{
				case VM::PR_vc:
					Dom_Text = New_Dom_Document.createTextNode( "vc" );
					break;
				
				case VM::PR_pty:
					Dom_Text = New_Dom_Document.createTextNode( "pty" );
					break;
				
				case VM::PR_none:
					Dom_Text = New_Dom_Document.createTextNode( "none" );
					break;
				
				case VM::PR_null:
					Dom_Text = New_Dom_Document.createTextNode( "null" );
					break;
				
				case VM::PR_dev:
					Dom_Text = New_Dom_Document.createTextNode( "dev" );
					break;
					
				case VM::PR_file:
					Dom_Text = New_Dom_Document.createTextNode( "file" );
					break;
				
				case VM::PR_stdio:
					Dom_Text = New_Dom_Document.createTextNode( "stdio" );
					break;
				
				case VM::PR_pipe:
					Dom_Text = New_Dom_Document.createTextNode( "pipe" );
					break;
				
				case VM::PR_udp:
					Dom_Text = New_Dom_Document.createTextNode( "udp" );
					break;
				
				case VM::PR_tcp:
					Dom_Text = New_Dom_Document.createTextNode( "tcp" );
					break;
				
				case VM::PR_telnet:
					Dom_Text = New_Dom_Document.createTextNode( "telnet" );
					break;
				
				case VM::PR_unix:
					Dom_Text = New_Dom_Document.createTextNode( "unix" );
					break;
				
				case VM::PR_com:
					Dom_Text = New_Dom_Document.createTextNode( "com" );
					break;
					
				case VM::PR_msmouse:
					Dom_Text = New_Dom_Document.createTextNode( "msmouse" );
					break;
					
				case VM::PR_mon:
					Dom_Text = New_Dom_Document.createTextNode( "mon" );
					break;
					
				case VM::PR_braille:
					Dom_Text = New_Dom_Document.createTextNode( "braille" );
					break;
					
				default:
					Dom_Text = New_Dom_Document.createTextNode( "Default" );
					break;
			}
			
			Sec_Element.appendChild( Dom_Text );
			
			// Parametrs Line
			Sec_Element = New_Dom_Document.createElement( "Parametrs" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( Parallel_Ports[px].Get_Parametrs_Line() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
		}
		
		// USB Ports
		Dom_Element = New_Dom_Document.createElement( "USB_Ports_Count" );
		VM_Element.appendChild( Dom_Element );
		Dom_Text = New_Dom_Document.createTextNode( QString::number(USB_Ports.count()) );
		Dom_Element.appendChild( Dom_Text );
		
		// USB Devices
		for( int ux = 0; ux < USB_Ports.count(); ++ux )
		{
			Dom_Element = New_Dom_Document.createElement( "USB_Port_" + QString::number(ux) );
			
			// Use Host Device
			Sec_Element = New_Dom_Document.createElement( "Use_Host_Device" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Use_Host_Device() ? "true" : "false" );
			Sec_Element.appendChild( Dom_Text );
			
			// Manufacturer Name
			Sec_Element = New_Dom_Document.createElement( "Manufacturer_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Manufacturer_Name() );
			Sec_Element.appendChild( Dom_Text );
			
			// Product Name
			Sec_Element = New_Dom_Document.createElement( "Product_Name" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Product_Name() );
			Sec_Element.appendChild( Dom_Text );
			
			// Vendor_ID
			Sec_Element = New_Dom_Document.createElement( "Vendor_ID" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Vendor_ID() );
			Sec_Element.appendChild( Dom_Text );
			
			// Product_ID
			Sec_Element = New_Dom_Document.createElement( "Product_ID" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Product_ID() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
			
			// Bus
			Sec_Element = New_Dom_Document.createElement( "Bus" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Bus() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
			
			// Addr
			Sec_Element = New_Dom_Document.createElement( "Addr" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Addr() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
			
			// Path
			Sec_Element = New_Dom_Document.createElement( "Path" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_DevPath() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
			
			// Serial Number
			Sec_Element = New_Dom_Document.createElement( "Serial_Number" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Serial_Number() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
			
			// Speed
			Sec_Element = New_Dom_Document.createElement( "Speed" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( USB_Ports[ux].Get_Speed() );
			Sec_Element.appendChild( Dom_Text );
			
			VM_Element.appendChild( Dom_Element );
			
			// QEMU USB Devices
			bool usb_k, usb_m, usb_t, usb_wt, usb_b;
			usb_k = usb_m = usb_t = usb_wt = usb_b = false;
			USB_Ports[ux].Get_USB_QEMU_Devices( usb_k, usb_m, usb_t, usb_wt, usb_b );
			
			// Keyboard
			Dom_Element = New_Dom_Document.createElement( "USB_Keyboard" );
			VM_Element.appendChild( Dom_Element );
			Dom_Text = New_Dom_Document.createTextNode( usb_k ? "true" : "false" );
			Dom_Element.appendChild( Dom_Text );
			
			// Mouse
			Dom_Element = New_Dom_Document.createElement( "USB_Mouse" );
			VM_Element.appendChild( Dom_Element );
			Dom_Text = New_Dom_Document.createTextNode( usb_m ? "true" : "false" );
			Dom_Element.appendChild( Dom_Text );
			
			// Tablet
			Dom_Element = New_Dom_Document.createElement( "USB_Tablet" );
			VM_Element.appendChild( Dom_Element );
			Dom_Text = New_Dom_Document.createTextNode( usb_t ? "true" : "false" );
			Dom_Element.appendChild( Dom_Text );
			
			// Wacom Tablet
			Dom_Element = New_Dom_Document.createElement( "USB_WacomTablet" );
			VM_Element.appendChild( Dom_Element );
			Dom_Text = New_Dom_Document.createTextNode( usb_wt ? "true" : "false" );
			Dom_Element.appendChild( Dom_Text );
			
			// Braille
			Dom_Element = New_Dom_Document.createElement( "USB_Braille" );
			VM_Element.appendChild( Dom_Element );
			Dom_Text = New_Dom_Document.createTextNode( usb_b ? "true" : "false" );
			Dom_Element.appendChild( Dom_Text );
		}
	}
	
	// Linux Boot
	Dom_Element = New_Dom_Document.createElement( "Linux_Boot" );
	VM_Element.appendChild( Dom_Element );
	
	if( Linux_Boot )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// bzImage Path
	Dom_Element = New_Dom_Document.createElement( "bzImage_Path" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( bzImage_Path );
	Dom_Element.appendChild( Dom_Text );
	
	// Initrd Path
	Dom_Element = New_Dom_Document.createElement( "Initrd_Path" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Initrd_Path );
	Dom_Element.appendChild( Dom_Text );
	
	// Kernel Command Line Arguments
	Dom_Element = New_Dom_Document.createElement( "Kernel_ComLine" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Kernel_ComLine );
	Dom_Element.appendChild( Dom_Text );
	
	// Use ROM File
	Dom_Element = New_Dom_Document.createElement( "Use_ROM_File" );
	VM_Element.appendChild( Dom_Element );
	
	if( Use_ROM_File )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// ROM File Path
	Dom_Element = New_Dom_Document.createElement( "ROM_File" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( ROM_File );
	Dom_Element.appendChild( Dom_Text );
	
	// Use MTDBlock File
	Dom_Element = New_Dom_Document.createElement( "Use_MTDBlock_File" );
	VM_Element.appendChild( Dom_Element );
	
	if( MTDBlock )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// MTDBlock File
	Dom_Element = New_Dom_Document.createElement( "MTDBlock_File" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( MTDBlock_File );
	Dom_Element.appendChild( Dom_Text );
	
	// Use SecureDigital File
	Dom_Element = New_Dom_Document.createElement( "Use_SecureDigital_File" );
	VM_Element.appendChild( Dom_Element );
	
	if( SecureDigital )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// SecureDigital File
	Dom_Element = New_Dom_Document.createElement( "SecureDigital_File" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( SecureDigital_File );
	Dom_Element.appendChild( Dom_Text );
	
	// Use PFlash File
	Dom_Element = New_Dom_Document.createElement( "Use_PFlash_File" );
	VM_Element.appendChild( Dom_Element );
	
	if( PFlash )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// PFlash File
	Dom_Element = New_Dom_Document.createElement( "PFlash_File" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( PFlash_File );
	Dom_Element.appendChild( Dom_Text );
	
	// Additional Arguments
	Dom_Element = New_Dom_Document.createElement( "Additional_Args" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Additional_Args );
	Dom_Element.appendChild( Dom_Text );
	
	// Only_User_Args
	Dom_Element = New_Dom_Document.createElement( "Only_User_Args" );
	VM_Element.appendChild( Dom_Element );
	
	if( Only_User_Args )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use_User_Emulator_Binary
	Dom_Element = New_Dom_Document.createElement( "Use_User_Emulator_Binary" );
	VM_Element.appendChild( Dom_Element );
	
	if( Use_User_Emulator_Binary )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Enable KVM
	Dom_Element = New_Dom_Document.createElement( "Enable_KVM" );
	VM_Element.appendChild( Dom_Element );
	
	if( Enable_KVM )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use KVM IRQChip
	Dom_Element = New_Dom_Document.createElement( "Use_KVM_IRQChip" );
	VM_Element.appendChild( Dom_Element );
	
	if( KVM_IRQChip )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use No KVM Pit
	Dom_Element = New_Dom_Document.createElement( "Use_No_KVM_Pit" );
	VM_Element.appendChild( Dom_Element );
	
	if( No_KVM_Pit )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// KVM_Nesting
	Dom_Element = New_Dom_Document.createElement( "Use_KVM_Nesting" );
	VM_Element.appendChild( Dom_Element );
	
	if( KVM_Nesting )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// KVM_No_Pit_Reinjection
	Dom_Element = New_Dom_Document.createElement( "Use_KVM_No_Pit_Reinjection" );
	VM_Element.appendChild( Dom_Element );
	
	if( KVM_No_Pit_Reinjection )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use_KVM_Shadow_Memory
	Dom_Element = New_Dom_Document.createElement( "Use_KVM_Shadow_Memory" );
	VM_Element.appendChild( Dom_Element );
	
	if( KVM_Shadow_Memory )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// KVM_Shadow_Memory_Size
	Dom_Element = New_Dom_Document.createElement( "KVM_Shadow_Memory_Size" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(KVM_Shadow_Memory_Size) );
	Dom_Element.appendChild( Dom_Text );
	
	// Init_Graphic_Mode
	Dom_Element = New_Dom_Document.createElement( "Init_Graphic_Mode" );
	
	// Enabled
	Sec_Element = New_Dom_Document.createElement( "Enabled" );
	Dom_Element.appendChild( Sec_Element );
	
	if( Init_Graphic_Mode.Get_Enabled() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Width
	Sec_Element = New_Dom_Document.createElement( "Width" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(Init_Graphic_Mode.Get_Width()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Height
	Sec_Element = New_Dom_Document.createElement( "Height" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(Init_Graphic_Mode.Get_Height()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Depth
	Sec_Element = New_Dom_Document.createElement( "Depth" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(Init_Graphic_Mode.Get_Depth()) );
	Sec_Element.appendChild( Dom_Text );
	
	VM_Element.appendChild( Dom_Element );
	
	// Use No Frame
	Dom_Element = New_Dom_Document.createElement( "Use_No_Frame" );
	VM_Element.appendChild( Dom_Element );
	
	if( No_Frame )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use Alt Grab
	Dom_Element = New_Dom_Document.createElement( "Use_Alt_Grab" );
	VM_Element.appendChild( Dom_Element );
	
	if( Alt_Grab )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use No Quit
	Dom_Element = New_Dom_Document.createElement( "Use_No_Quit" );
	VM_Element.appendChild( Dom_Element );
	
	if( No_Quit )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use Portrait
	Dom_Element = New_Dom_Document.createElement( "Use_Portrait" );
	VM_Element.appendChild( Dom_Element );
	
	if( Portrait )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use Show_Cursor
	Dom_Element = New_Dom_Document.createElement( "Show_Cursor" );
	VM_Element.appendChild( Dom_Element );
	
	if( Show_Cursor )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use Curses
	Dom_Element = New_Dom_Document.createElement( "Curses" );
	VM_Element.appendChild( Dom_Element );
	
	if( Curses )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use RTC_TD_Hack
	Dom_Element = New_Dom_Document.createElement( "RTC_TD_Hack" );
	VM_Element.appendChild( Dom_Element );
	
	if( RTC_TD_Hack )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Start Date
	Dom_Element = New_Dom_Document.createElement( "Use_Start_Date" );
	VM_Element.appendChild( Dom_Element );
	
	if( Start_Date )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Dom_Element.appendChild( Dom_Text );
	
	// Start_DateTime
	Dom_Element = New_Dom_Document.createElement( "Start_Date" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( Start_DateTime.toString("dd.MM.yyyy HH:mm:ss") );
	Dom_Element.appendChild( Dom_Text );
	
	// SPICE
	Dom_Element = New_Dom_Document.createElement( "SPICE" );
		
	// Use SPICE
	Sec_Element = New_Dom_Document.createElement( "Use_SPICE" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_SPICE() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// GXL_Devices_Count
	Sec_Element = New_Dom_Document.createElement( "GXL_Devices_Count" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SPICE.Get_GXL_Devices_Count()) );
	Sec_Element.appendChild( Dom_Text );
		
	// RAM_Size
	Sec_Element = New_Dom_Document.createElement( "RAM_Size" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SPICE.Get_RAM_Size()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Port
	Sec_Element = New_Dom_Document.createElement( "Port" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SPICE.Get_Port()) );
	Sec_Element.appendChild( Dom_Text );
		
	// Use_SPort
	Sec_Element = New_Dom_Document.createElement( "Use_SPort" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_SPort() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// SPort
	Sec_Element = New_Dom_Document.createElement( "SPort" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(SPICE.Get_SPort()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Use_Hostname
	Sec_Element = New_Dom_Document.createElement( "Use_Hostname" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_Hostname() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// Hostname
	Sec_Element = New_Dom_Document.createElement( "Hostname" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Get_Hostname() );
	Sec_Element.appendChild( Dom_Text );
	
	// Use_Image_Compression
	Sec_Element = New_Dom_Document.createElement( "Use_Image_Compression" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_Image_Compression() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// Image_Compression_Type
	Sec_Element = New_Dom_Document.createElement( "Image_Compression_Type" );
	Dom_Element.appendChild( Sec_Element );
	
	switch( SPICE.Get_Image_Compression() )
	{
		case VM::SPICE_IC_Type_on:
			Dom_Text = New_Dom_Document.createTextNode( "on" );
			break;
			
		case VM::SPICE_IC_Type_auto_glz:
			Dom_Text = New_Dom_Document.createTextNode( "auto_glz" );
			break;
			
		case VM::SPICE_IC_Type_auto_lz:
			Dom_Text = New_Dom_Document.createTextNode( "auto_lz" );
			break;
			
		case VM::SPICE_IC_Type_quic:
			Dom_Text = New_Dom_Document.createTextNode( "quic" );
			break;
			
		case VM::SPICE_IC_Type_glz:
			Dom_Text = New_Dom_Document.createTextNode( "glz" );
			break;
			
		case VM::SPICE_IC_Type_lz:
			Dom_Text = New_Dom_Document.createTextNode( "lz" );
			break;
			
		case VM::SPICE_IC_Type_off:
			Dom_Text = New_Dom_Document.createTextNode( "off" );
			break;
			
		default:
			AQError( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
					 "SPICE image compression type invalid!" );
			Dom_Text = New_Dom_Document.createTextNode( "on" );
			break;
	}
	
	Sec_Element.appendChild( Dom_Text );
	
	// Use_Video_Stream_Compression
	Sec_Element = New_Dom_Document.createElement( "Use_Video_Stream_Compression" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_Video_Stream_Compression() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// Use_Renderer
	Sec_Element = New_Dom_Document.createElement( "Use_Renderer" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_Renderer() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// Renderers_List
	QDomElement rendererListElement = New_Dom_Document.createElement( "Renderers_List" );
	
	for( int ix = 0; ix < SPICE.Get_Renderer_List().count(); ++ix )
	{
		switch( SPICE.Get_Renderer_List()[ix] )
		{
			case VM::SPICE_Renderer_oglpbuf:
				Sec_Element = New_Dom_Document.createElement( "oglpbuf" );
				break;
				
			case VM::SPICE_Renderer_oglpixmap:
				Sec_Element = New_Dom_Document.createElement( "oglpixmap" );
				break;
				
			case VM::SPICE_Renderer_cairo:
				Sec_Element = New_Dom_Document.createElement( "cairo" );
				break;
				
			default:
				AQError( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
						 "SPICE renderer type invalid!" );
				Sec_Element = New_Dom_Document.createElement( "cairo" );
				break;
		}
		
		rendererListElement.appendChild( Sec_Element );
	}
	
	Dom_Element.appendChild( rendererListElement );
	
	// Use_Playback_Compression
	Sec_Element = New_Dom_Document.createElement( "Use_Playback_Compression" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_Playback_Compression() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// Use_Password
	Sec_Element = New_Dom_Document.createElement( "Use_Password" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Use_Password() ? "true" : "false" );
	Sec_Element.appendChild( Dom_Text );
	
	// Password
	Sec_Element = New_Dom_Document.createElement( "Password" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( SPICE.Get_Password() );
	Sec_Element.appendChild( Dom_Text );
	
	VM_Element.appendChild( Dom_Element );

	// Use VNC
	Dom_Element = New_Dom_Document.createElement( "Use_VNC" );
	VM_Element.appendChild( Dom_Element );
	
	if( VNC )
	{
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	}
	else
	{
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// VNC Socket Mode
	Dom_Element = New_Dom_Document.createElement( "VNC_Socket_Mode" );
	VM_Element.appendChild( Dom_Element );
	
	if( VNC_Socket_Mode )
	{
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	}
	else
	{
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// VNC Unix Socket Path
	Dom_Element = New_Dom_Document.createElement( "VNC_Unix_Socket_Path" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( VNC_Unix_Socket_Path );
	Dom_Element.appendChild( Dom_Text );
	
	// VNC Display Number
	Dom_Element = New_Dom_Document.createElement( "VNC_Display_Number" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(VNC_Display_Number) );
	Dom_Element.appendChild( Dom_Text );
	
	// Use VNC Password
	Dom_Element = New_Dom_Document.createElement( "Use_VNC_Password" );
	VM_Element.appendChild( Dom_Element );
	
	if( VNC_Password )
	{
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	}
	else
	{
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use VNC TLS
	Dom_Element = New_Dom_Document.createElement( "Use_VNC_TLS" );
	VM_Element.appendChild( Dom_Element );
	
	if( VNC_TLS )
	{
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	}
	else
	{
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// Use VNC x509
	Dom_Element = New_Dom_Document.createElement( "Use_VNC_x509" );
	VM_Element.appendChild( Dom_Element );
	
	if( VNC_x509 )
	{
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	}
	else
	{
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// VNC x509 Folder Path
	Dom_Element = New_Dom_Document.createElement( "VNC_x509_Folder_Path" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( VNC_x509_Folder_Path );
	Dom_Element.appendChild( Dom_Text );
	
	// Use VNC x509verify
	Dom_Element = New_Dom_Document.createElement( "Use_VNC_x509verify" );
	VM_Element.appendChild( Dom_Element );
	
	if( VNC_x509verify )
	{
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	}
	else
	{
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	}
	
	Dom_Element.appendChild( Dom_Text );
	
	// VNC x509verify Folder Path
	Dom_Element = New_Dom_Document.createElement( "VNC_x509verify_Folder_Path" );
	VM_Element.appendChild( Dom_Element );
	Dom_Text = New_Dom_Document.createTextNode( VNC_x509verify_Folder_Path );
	Dom_Element.appendChild( Dom_Text );
	
	// Create File and Save Data
	QFile VM_XML_File( file_name );
	
	if( ! VM_XML_File.open(QFile::WriteOnly | QFile::Truncate) )
	{
		AQError( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
				 QString("Cannot Create New VM File \"%1\"").arg(file_name) );
		return false;
	}
	
	QTextStream Out_Stream( &VM_XML_File );
	New_Dom_Document.save( Out_Stream, 4 );
	
	return true;
}

bool Virtual_Machine::Create_Template( const QString &vm_path, const QString &template_name,
									   Create_Template_Window::Template_Options opts )
{
	Template_Name = template_name;
	Template_Opts = opts;
	return Create_VM_File( vm_path, true );
}

bool Virtual_Machine::Load_VM( const QString &file_name )
{

    TXML2QDOM::QDomDocument VM_Dom_Document; // vm xml file

	VM_XML_File_Path = file_name;
	
	QFile VM_File( file_name );
	
	if( ! VM_File.open(QFile::ReadOnly | QFile::Text) )
	{
		AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
				 QString("Cannot Read VM File \"%1\"! Error String: %2")
				 .arg(file_name).arg(VM_File.errorString()) );
		
		return false;
	}
	else
	{
		QString Error_String;
		int Error_Line, Error_Column;
		
		if( ! VM_Dom_Document.setContent(&VM_File, true, &Error_String, &Error_Line, &Error_Column) )
		{
			AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
					 QString("Parse Error!\nLine: %1\nColumn: %2\nString: %3" )
					 .arg(Error_Line).arg(Error_Column).arg(Error_String) );
			
			return false;
		}
		else
		{
			QDomElement Root_Element = VM_Dom_Document.documentElement();
			
			bool template_mode = false;
			
			if( Root_Element.tagName() != "AQEMU" )
			{
				if( Root_Element.tagName() != "AQEMU_Template" )
				{
					AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
							 "This is not a AQEMU VM File!" );
					return false;
				}
				else
				{
					template_mode = true;
				}
			}
			
			// Read All Data in File
			QDomElement Child_Element = Root_Element.firstChildElement( "Virtual_Machine" );
			bool load_emulator = true;
			bool old_version_storage_devices = false;
			bool load_boot_order_setcton = false;
			QString aqemu_vm_file_version = "";
			
			if( Root_Element.hasAttribute("version") )
			{
				aqemu_vm_file_version = Root_Element.attribute( "version" );
			}
			
			if( aqemu_vm_file_version == "0.8" )
			{
				AQDebug( "bool Virtual_Machine::Load_VM( const QString &file_name )",
						 "This is AQEMU VM File Version 0.8!" );
				load_boot_order_setcton = true; // for QEMU > 0.11
			}
			else if( aqemu_vm_file_version == "0.7.2" )
			{
				AQWarning( "bool Virtual_Machine::Load_VM( const QString &file_name )",
						   "This is AQEMU VM File Version 0.7.2!" );
				
				old_version_storage_devices = true;
			}
			else if( aqemu_vm_file_version == "0.5" )
			{
				AQWarning( "bool Virtual_Machine::Load_VM( const QString &file_name )",
						   "Uses compatible mode for config version 0.5" );
				
			    Current_Emulator = Get_Default_Emulator( );
			    Current_Emulator.Set_Name( "" );
			    Machine_Accelerator = VM::TCG;
				
				// KVM ?
				if( Child_Element.firstChildElement("Computer_Type").text() == "qemu-kvm" )
				{
					Machine_Accelerator = VM::KVM;
			    }

				old_version_storage_devices = true;
				load_emulator = false;
			}
			else
			{
				AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
						 "This is AQEMU VM File Version Not Support!" );
				
				int ret_but = QMessageBox::question( NULL, tr("Version Invalid!"),
													 tr("Version of this AQEMU VM File isn't 0.5\nLoad This File anyway?"),
													 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
				
				if( ret_but != QMessageBox::Yes ) return false;
			}
			
			if( template_mode )
			{
				// Template Options
				Template_Opts = (Create_Template_Window::Template_Options)Child_Element.firstChildElement("Template_Options").text().toInt();
			}
			
			if( load_emulator )
			{
				// Emulator Name
				QString emul_name = Child_Element.firstChildElement("Emulator_Name").text();
				
				// Emulator Type (legacy option) = equals Machine Name

                QString accel = "";
                if( ! Child_Element.firstChildElement("Machine_Accelerator").text().isEmpty() )
                {
                    accel = Child_Element.firstChildElement("Machine_Accelerator").text();
                }
                else
                {
                    if  ( ! Child_Element.firstChildElement("Emulator_Type").text().isEmpty() )
                        accel = Child_Element.firstChildElement("Emulator_Type").text();
                }

                Machine_Accelerator = VM::String_To_Accel(accel);
				
    			Current_Emulator = Get_Default_Emulator();
				Current_Emulator.Set_Name( "" );
			}
			
			// Computer Type
			Computer_Type = Child_Element.firstChildElement("Computer_Type").text();
			if( Computer_Type == "qemu-system-x86" ) Computer_Type = "qemu-system-x86_64";
			if( Computer_Type == "qemu-kvm" )
            {
                Computer_Type = "qemu-system-x86_64";            
		        Machine_Accelerator = VM::KVM;
			}

			// VM State
			if( Child_Element.firstChildElement("VM_State").text() == "Saved" )
			{
				Set_State( VM::VMS_Saved );
			}
			else
			{
				Set_State( VM::VMS_Power_Off );
			}
			

			// Check Emulator
			AQDebug( "bool Virtual_Machine::Load_VM( const QString &file_name )",
					 "Emulator Name: " + Current_Emulator.Get_Name() );
			
			// Emulator Found?
			/*if( Current_Emulator.Get_Name().isEmpty() && (Current_Emulator.Get_Version() == VM::Obsolete) )
			{
				AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
						 "Cannot Load Emulator!" );
				
				Set_State( VM::VMS_In_Error );
			}
			else
			{*/
				Update_Current_Emulator_Devices();
			/*}*/
			
			// Icon Path
			Icon_Path = Child_Element.firstChildElement("Icon_Path").text();
            if ( Icon_Path.startsWith(":/images/") ) //for compatibility with old .aqemu files
			{
                Icon_Path.replace(":/images/",":/");
            }

            if ( ! Icon_Path.startsWith(":/") )
            {

                //correct absolute paths saved by old versions
		        QStringList dataDirs;
		        dataDirs << "/usr/share/aqemu/"
				         << "/usr/share/apps/aqemu/"
				         << "/usr/local/share/aqemu/";

                QSettings settings;    
                bool abs_found = false;
                for ( int i = 0; i < dataDirs.count(); i++ )
                {
                    if ( Icon_Path.startsWith(dataDirs.at(i)) )
                    {
                        Icon_Path.replace(dataDirs.at(i),settings.value( "AQEMU_Data_Folder","").toString());
                        abs_found = true;
                        break;
                    }
                }

                if ( ! abs_found )
                {
                    Icon_Path = settings.value( "AQEMU_Data_Folder","").toString() + Icon_Path;
                }
            }

			// Screenshot Path
			Screenshot_Path = Child_Element.firstChildElement("Screenshot_Path").text();
			
			// Machine Name
			Machine_Name = Child_Element.firstChildElement("Machine_Name").text();
			
			// Machine Type
			Machine_Type = Child_Element.firstChildElement("Machine_Type").text();
			
			// CPU Type
			CPU_Type = Child_Element.firstChildElement("CPU_Type").text();
			
			// SMP
			SMP.SMP_Count = Child_Element.firstChildElement("SMP_CPU_Count").text().toInt();
			SMP.SMP_Cores = Child_Element.firstChildElement("SMP_Cores").text().toInt();
			SMP.SMP_Threads = Child_Element.firstChildElement("SMP_Threads").text().toInt();
			SMP.SMP_Sockets = Child_Element.firstChildElement("SMP_Sockets").text().toInt();
			SMP.SMP_MaxCPUs = Child_Element.firstChildElement("SMP_MaxCPUs").text().toInt();
			
			// Keyboard Layout
			Keyboard_Layout = Child_Element.firstChildElement("Keyboard_Layout").text();
			
			QDomElement Second_Element;
			
			// Boot Device
			Boot_Order_List.clear();
			
			if( load_boot_order_setcton && Child_Element.firstChildElement("Boot_Device").text().isEmpty() )
			{
				int bootOrderCount = Child_Element.firstChildElement( "Boot_Device_Count" ).text().toInt();
				
				if( bootOrderCount == 0 )
				{
					VM::Boot_Order tmpBootOrder;
					
					tmpBootOrder.Enabled = true;
					tmpBootOrder.Type = VM::Boot_From_FDA;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Enabled = false;
					tmpBootOrder.Type = VM::Boot_From_FDB;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Enabled = true;
					tmpBootOrder.Type = VM::Boot_From_CDROM;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Type = VM::Boot_From_HDD;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Enabled = false;
					tmpBootOrder.Type = VM::Boot_From_Network1;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Type = VM::Boot_From_Network2;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Type = VM::Boot_From_Network3;
					this->Boot_Order_List << tmpBootOrder;
					
					tmpBootOrder.Type = VM::Boot_From_Network4;
					this->Boot_Order_List << tmpBootOrder;
				}
				
				for( int bx = 0; bx < bootOrderCount; bx++ )
				{
					Second_Element = Child_Element.firstChildElement( "Boot_Device_" + QString::number(bx) );
					
					VM::Boot_Order tmpBootDev;
					tmpBootDev.Enabled = Second_Element.firstChildElement( "Enabled" ).text() == "true";
					QString bootType = Second_Element.firstChildElement( "Type" ).text();
					
					if( bootType == "FDA" ) tmpBootDev.Type = VM::Boot_From_FDA;
					else if( bootType == "FDB" ) tmpBootDev.Type = VM::Boot_From_FDB;
					else if( bootType == "CDROM" ) tmpBootDev.Type = VM::Boot_From_CDROM;
					else if( bootType == "HDD" ) tmpBootDev.Type = VM::Boot_From_HDD;
					else if( bootType == "Network1" ) tmpBootDev.Type = VM::Boot_From_Network1;
					else if( bootType == "Network2" ) tmpBootDev.Type = VM::Boot_From_Network2;
					else if( bootType == "Network3" ) tmpBootDev.Type = VM::Boot_From_Network3;
					else if( bootType == "Network4" ) tmpBootDev.Type = VM::Boot_From_Network4;
					else
					{
						AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
								 "Incorrect boot device type! Use default: CDROM" );
						tmpBootDev.Type = VM::Boot_From_HDD;
					}
					
					Boot_Order_List << tmpBootDev;
				}
			}
			else
			{
				VM::Boot_Order tmpBootOrder;
				tmpBootOrder.Enabled = false;
				
				tmpBootOrder.Type = VM::Boot_From_FDA;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_FDB;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_CDROM;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_HDD;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_Network1;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_Network2;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_Network3;
				this->Boot_Order_List << tmpBootOrder;
				
				tmpBootOrder.Type = VM::Boot_From_Network4;
				this->Boot_Order_List << tmpBootOrder;
				
				QString tmp_str = Child_Element.firstChildElement("Boot_Device").text();
				VM::Boot_Device bootDev;
				
				if( tmp_str == "FDD" ) bootDev = VM::Boot_From_FDA;
				else if( tmp_str == "CDROM" ) bootDev = VM::Boot_From_CDROM;
				else if( tmp_str == "HDD" ) bootDev = VM::Boot_From_HDD;
				else if( tmp_str == "Network" ) bootDev = VM::Boot_From_Network1;
				else if( tmp_str == "None" ) bootDev = VM::Boot_None;
				else
				{
					bootDev = VM::Boot_From_CDROM;
					AQWarning( "bool Virtual_Machine::Load_VM( const QString &file_name )",
							   "Use Default Boot Device: CD-ROM" );
				}
				
				for( int bx = 0; bx < Boot_Order_List.count(); bx++ )
				{
					if( Boot_Order_List[bx].Type == bootDev ) Boot_Order_List[ bx ].Enabled = true;
				}
			}
			
			// Show Boot Menu
			Show_Boot_Menu = (Child_Element.firstChildElement("Show_Boot_Menu").text() == "true");
			
			// Video Card
			Video_Card = Child_Element.firstChildElement("Video_Card").text();
			
			// Audio Cards
			Second_Element = Child_Element.firstChildElement( "Audio_Cards" );
			
			VM::Sound_Cards snd_card;
			
			// Audio_sb16
			snd_card.Audio_sb16 = Second_Element.firstChildElement("Audio_sb16").text() == "true";
			
			// Audio_es1370
			snd_card.Audio_es1370 = Second_Element.firstChildElement("Audio_es1370").text() == "true" ;
			
			// Audio_Adlib
			snd_card.Audio_Adlib = Second_Element.firstChildElement("Audio_Adlib").text() == "true";
			
			// Audio_PC_Speaker
			snd_card.Audio_PC_Speaker = Second_Element.firstChildElement("Audio_PC_Speaker").text() == "true";
			
			// Audio_GUS
			snd_card.Audio_GUS = Second_Element.firstChildElement("Audio_GUS").text() == "true";
			
			// Audio_AC97
			snd_card.Audio_AC97 = Second_Element.firstChildElement("Audio_AC97").text() == "true";
			
			// Audio_HDA
			snd_card.Audio_HDA = Second_Element.firstChildElement("Audio_HDA").text() == "true";
			
			// Audio_cs4231
			snd_card.Audio_cs4231a = Second_Element.firstChildElement("Audio_cs4231a").text() == "true";
			
			Set_Audio_Cards( snd_card );
			
			// Check Free RAM
			Remove_RAM_Size_Limitation = (Child_Element.firstChildElement("Remove_RAM_Size_Limitation").text() == "true");
			
			// Memory Size ( RAM )
			Memory_Size = Child_Element.firstChildElement("Memory_Size").text().toInt();
			
			// Fullscreen
			Fullscreen = (Child_Element.firstChildElement("Fullscreen").text() == "true");
			
			// Win2K_Hack
			Win2K_Hack = (Child_Element.firstChildElement("Win2K_Hack").text() == "true");
			
			// Localtime
			Local_Time = (Child_Element.firstChildElement("Local_Time").text() == "true");
			
			// Check FDD Boot Sector
			Check_FDD_Boot_Sector = (Child_Element.firstChildElement("Check_FDD_Boot_Sector").text() == "true");
			
			// ACPI
			ACPI = (Child_Element.firstChildElement("ACPI").text() == "true");
			
			// Snapshot_Mode
			Snapshot_Mode = (Child_Element.firstChildElement("Snapshot_Mode").text() == "true");
			
			// Start_CPU
			Start_CPU = (Child_Element.firstChildElement("Start_CPU").text() == "true");
			
			// No_Reboot
			No_Reboot = (Child_Element.firstChildElement("No_Reboot").text() == "true");
			
			// No_Shutdown
			No_Shutdown = (Child_Element.firstChildElement("No_Shutdown").text() == "true");
			
			// Floppy and CD-ROM
			if( template_mode &&
				Template_Opts ^ Create_Template_Window::Template_Save_FDD_CD )
			{
				// Floppy 0
				FD0 = VM_Storage_Device( false, "" );
				
				// Floppy 1
				FD1 = VM_Storage_Device( false, "" );
				
				// CD-ROM
				CD_ROM = VM_Storage_Device( false, "" );
			}
			else
			{
				if( old_version_storage_devices )
				{
					// Floppy 0
					FD0 = VM_Storage_Device();
					
					Second_Element = Child_Element.firstChildElement( "FD0" );
					FD0.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					
					if( Second_Element.firstChildElement("Host_Device").text() == "true" )
						FD0.Set_File_Name( Second_Element.firstChildElement("Host_File_Name").text() );
					else
						FD0.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
					
					// Floppy 1
					FD1 = VM_Storage_Device();
					
					Second_Element = Child_Element.firstChildElement( "FD1" );
					FD1.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					
					if( Second_Element.firstChildElement("Host_Device").text() == "true" )
						FD1.Set_File_Name( Second_Element.firstChildElement("Host_File_Name").text() );
					else
						FD1.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
					
					// CD-ROM
					CD_ROM = VM_Storage_Device();
					
					Second_Element = Child_Element.firstChildElement( "CD_ROM" );
					CD_ROM.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					
					if( Second_Element.firstChildElement("Host_Device").text() == "true" )
						CD_ROM.Set_File_Name( Second_Element.firstChildElement("Host_File_Name").text() );
					else
						CD_ROM.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
				}
				else
				{
					// Floppy 0
					FD0 = VM_Storage_Device();
					Second_Element = Child_Element.firstChildElement( "FD0" );
					FD0.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					FD0.Set_File_Name( Second_Element.firstChildElement("File_Name").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    FD0.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
					
					// Floppy 1
					FD1 = VM_Storage_Device();
					Second_Element = Child_Element.firstChildElement( "FD1" );
					FD1.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					FD1.Set_File_Name( Second_Element.firstChildElement("File_Name").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    FD1.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
					
					// CD-ROM
					CD_ROM = VM_Storage_Device();
					Second_Element = Child_Element.firstChildElement( "CD_ROM" );
					CD_ROM.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					CD_ROM.Set_File_Name( Second_Element.firstChildElement("File_Name").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    CD_ROM.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
				}
			}
			
			// Hard Disk's
			if( template_mode )
			{
				if( Template_Opts | Create_Template_Window::Template_Save_HDD )
				{
					// HDA
					Second_Element = Child_Element.firstChildElement( "HDA" );
					HDA.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					HDA.Set_Virtual_Size( HDA.String_to_Device_Size(Second_Element.firstChildElement("Size").text()) );
					HDA.Set_Image_Format( Second_Element.firstChildElement("Format").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    HDA.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
					
					// HDB
					Second_Element = Child_Element.firstChildElement( "HDB" );
					HDB.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					HDB.Set_Virtual_Size( HDB.String_to_Device_Size(Second_Element.firstChildElement("Size").text()) );
					HDB.Set_Image_Format( Second_Element.firstChildElement("Format").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    HDB.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
					
					// HDC
					Second_Element = Child_Element.firstChildElement( "HDC" );
					HDC.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					HDC.Set_Virtual_Size( HDC.String_to_Device_Size(Second_Element.firstChildElement("Size").text()) );
					HDC.Set_Image_Format( Second_Element.firstChildElement("Format").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    HDC.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
					
					// HDD
					Second_Element = Child_Element.firstChildElement( "HDD" );
					HDD.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
					HDD.Set_Virtual_Size( HDD.String_to_Device_Size(Second_Element.firstChildElement("Size").text()) );
					HDD.Set_Image_Format( Second_Element.firstChildElement("Format").text() );
					
                    Second_Element = Second_Element.firstChildElement( "Native_Device" );
                    HDD.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
				}
				else
				{
					// HDA
					Second_Element = Child_Element.firstChildElement( "HDA" );
					HDA.Set_Enabled( false );
					HDA.Set_File_Name( "" );
					
					// HDB
					Second_Element = Child_Element.firstChildElement( "HDB" );
					HDB.Set_Enabled( false );
					HDB.Set_File_Name( "" );
				
					// HDC
					Second_Element = Child_Element.firstChildElement( "HDC" );
					HDC.Set_Enabled( false );
					HDC.Set_File_Name( "" );
				
					// HDD
					Second_Element = Child_Element.firstChildElement( "HDD" );
					HDD.Set_Enabled( false );
					HDD.Set_File_Name( "" );
				}
			}
			else
			{
				// HDA
				Second_Element = Child_Element.firstChildElement( "HDA" );
				HDA.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
				HDA.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
				
                Second_Element = Second_Element.firstChildElement( "Native_Device" );
                HDA.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
				
				// HDB
				Second_Element = Child_Element.firstChildElement( "HDB" );
				HDB.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
				HDB.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
				
                Second_Element = Second_Element.firstChildElement( "Native_Device" );
                HDB.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
				
				// HDC
				Second_Element = Child_Element.firstChildElement( "HDC" );
				HDC.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
				HDC.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
				
                Second_Element = Second_Element.firstChildElement( "Native_Device" );
                HDC.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
				
				// HDD
				Second_Element = Child_Element.firstChildElement( "HDD" );
				HDD.Set_Enabled( (Second_Element.firstChildElement("Enabled").text() == "true") );
				HDD.Set_File_Name( Second_Element.firstChildElement("Image_File_Name").text() );
				
                Second_Element = Second_Element.firstChildElement( "Native_Device" );
                HDD.Set_Native_Device( Load_VM_Native_Storage_Device(Second_Element) );
			}
			
			// Nativ Storage Devices
			int Storage_Device_Count = Child_Element.firstChildElement( "Storage_Device_Count" ).text().toInt();
			
			for( int sx = 0; sx < Storage_Device_Count; ++sx )
			{
				Second_Element = Child_Element.firstChildElement( "Storage_Device_" + QString::number(sx) );
				
				// Add Device
                Storage_Devices << Load_VM_Native_Storage_Device( Second_Element );
			}

			// Shared Folders
			int Shared_Folder_Count = Child_Element.firstChildElement( "Shared_Folder_Count" ).text().toInt();
			
			for( int sx = 0; sx < Shared_Folder_Count; ++sx )
			{
				Second_Element = Child_Element.firstChildElement( "Shared_Folder_" + QString::number(sx) );
				
				// Add Device
				Shared_Folders << Load_VM_Shared_Folder( Second_Element );
			}
						
			// Snapshots
			int Snapshots_Count = Child_Element.firstChildElement( "Snapshots_Count" ).text().toInt();
			
			for( int sx = 0; sx < Snapshots_Count; ++sx )
			{
				Second_Element = Child_Element.firstChildElement( "Snapshot_" + QString::number(sx) );
				
				VM_Snapshot tmp_snapshot;
				
				// Tag
				tmp_snapshot.Set_Tag( Second_Element.firstChildElement("Tag").text() );
				
				// Name
				tmp_snapshot.Set_Name( Second_Element.firstChildElement("Name").text() );
				
				// Description
				tmp_snapshot.Set_Description( Second_Element.firstChildElement("Description").text() );
				
				// add snapshot
				Snapshots << tmp_snapshot;
			}
			
			// Use Network
			Use_Network = (Child_Element.firstChildElement("Use_Network").text() == "true");
			
			// Use Redirections
			Use_Redirections = (Child_Element.firstChildElement("Use_Redirections").text() == "true");
			
			// Network Cards Count
			int NetCardsCount = Child_Element.firstChildElement( "Network_Cards_Count" ).text().toInt();
			Network_Cards.clear();
			
			// Network Cards...
			for( int nx = 0; nx < NetCardsCount; nx++ )
			{
				Second_Element = Child_Element.firstChildElement( "Network_Card_" + QString::number(nx) );
				
				VM_Net_Card tmp_card;
				
				// Card Model
				tmp_card.Set_Card_Model(
						Second_Element.firstChildElement( "Card_Model" ).text() );
				
                QString tmp_str = Second_Element.firstChildElement( "Network_Mode" ).text();
				
				// Network Mode
				if( tmp_str == "None" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_None );
				}
				else if( tmp_str == "Usermode" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Usermode );
				}
				else if( tmp_str == "Tuntap" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Tuntap );
				}
				else if( tmp_str == "Tuntapfd" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Tuntapfd );
				}
				else if( tmp_str == "Tcplisten" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Tcplisten );
				}
				else if( tmp_str == "Tcpfd" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Tcpfd );
				}
				else if( tmp_str == "Tcpconnect" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Tcpconnect );
				}
				else if( tmp_str == "Multicast" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Multicast );
				}
				else if( tmp_str == "Multicastfd" )
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Multicastfd );
				}
				else
				{
					tmp_card.Set_Net_Mode( VM::Net_Mode_Dummy );
					AQWarning( "bool Virtual_Machine::Load_VM( const QString &file_name )",
							   "Net Card Network Mode Invalid! Use Default!" );
				}
				
				// Hostname
				tmp_card.Set_Hostname( Second_Element.firstChildElement("Hostname").text() );
				
				// IP Address
				tmp_card.Set_IP_Address( Second_Element.firstChildElement("IP_Address").text() );
				
				// MAC Address
				tmp_card.Set_MAC_Address( Second_Element.firstChildElement("MAC_Address").text() );
				
				// Port
				tmp_card.Set_Port( Second_Element.firstChildElement("Port").text().toInt() );
				
				// VLAN
				tmp_card.Set_VLAN( Second_Element.firstChildElement("VLAN").text().toInt() );
				
				// Use TUN TAP Script
				tmp_card.Set_Use_TUN_TAP_Script( ! (Second_Element.firstChildElement("Use_TUN_TAP_Script").text() == "false") );
				
				// TUN TAP Script
				tmp_card.Set_TUN_TAP_Script( Second_Element.firstChildElement("TUN_TAP_Script").text() );
				
				// Interface Name
				tmp_card.Set_Interface_Name( Second_Element.firstChildElement("Interface_Name").text() );
				
				// File Descriptor
				tmp_card.Set_File_Descriptor( Second_Element.firstChildElement("File_Descriptor").text().toInt() );
				
				Add_Network_Card( tmp_card );
			}
			
			// Nativ Network Card
            // Use Native_Network
            Native_Network = (Child_Element.firstChildElement("Native_Network").text() == "true");
			
			// Network Cards Count
            int NetCardsNativCount = Child_Element.firstChildElement( "Network_Cards_Native_Count" ).text().toInt();
			Network_Cards_Nativ.clear();
			
			// Network Cards...
			for( int nx = 0; nx < NetCardsNativCount; nx++ )
			{
                Second_Element = Child_Element.firstChildElement( "Network_Card_Native_" + QString::number(nx) );
				
                VM_Net_Card_Native tmp_card;
				
				// Network_Type
                QString netCardNative_Type = Second_Element.firstChildElement( "Network_Type" ).text();
				
                if( netCardNative_Type == "NIC" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_NIC );
                else if( netCardNative_Type == "User" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_User );
                else if( netCardNative_Type == "Chanel" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_Chanel );
                else if( netCardNative_Type == "Bridge" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_Bridge );
                else if( netCardNative_Type == "TAP" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_TAP );
                else if( netCardNative_Type == "Socket" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_Socket );
                else if( netCardNative_Type == "MulticastSocket" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_MulticastSocket );
                else if( netCardNative_Type == "VDE" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_VDE );
                else if( netCardNative_Type == "Dump" )
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_Dump );
				else
				{
					AQWarning( "bool Virtual_Machine::Load_VM( const QString &file_name )",
							   "Invalid Network Type. Use Default: User" );
                    tmp_card.Set_Network_Type( VM::Net_Mode_Native_User );
				}
				
				tmp_card.Set_Card_Model( Second_Element.firstChildElement("Card_Model").text() );
				tmp_card.Use_MAC_Address( Second_Element.firstChildElement("Use_MAC_Address").text() == "yes" );
				
				tmp_card.Set_MAC_Address( Second_Element.firstChildElement("MAC_Address").text() );
				tmp_card.Use_VLAN( Second_Element.firstChildElement("Use_VLAN").text() == "yes" );
				
				tmp_card.Set_VLAN( Second_Element.firstChildElement("VLAN").text().toInt() );
				tmp_card.Use_Name( Second_Element.firstChildElement("Use_Name").text() == "yes" );
				
				tmp_card.Set_Name( Second_Element.firstChildElement("Name").text() );
				tmp_card.Use_Hostname( Second_Element.firstChildElement("Use_Hostname").text() == "yes" );
				
				tmp_card.Set_Hostname( Second_Element.firstChildElement("Hostname").text() );
				tmp_card.Set_PortDev( Second_Element.firstChildElement("PortDev").text() );
				
				tmp_card.Use_File_Descriptor( Second_Element.firstChildElement("Use_File_Descriptor").text() == "yes" );
				tmp_card.Set_File_Descriptor( Second_Element.firstChildElement("File_Descriptor").text().toInt() );
				
				tmp_card.Use_Interface_Name( Second_Element.firstChildElement("Use_Interface_Name").text() == "yes" );
				tmp_card.Set_Interface_Name( Second_Element.firstChildElement("Interface_Name").text() );
				
				tmp_card.Use_Bridge_Name( Second_Element.firstChildElement("Use_Bridge_Name").text() == "yes" );
				tmp_card.Set_Bridge_Name( Second_Element.firstChildElement("Bridge_Name").text() );

				tmp_card.Use_TUN_TAP_Script( Second_Element.firstChildElement("Use_TUN_TAP_Script").text() == "yes" );
				tmp_card.Set_TUN_TAP_Script( Second_Element.firstChildElement("TUN_TAP_Script").text() );
				
				tmp_card.Use_TUN_TAP_Down_Script( Second_Element.firstChildElement("Use_TUN_TAP_Down_Script").text() == "yes" );
				tmp_card.Set_TUN_TAP_Down_Script( Second_Element.firstChildElement("TUN_TAP_Down_Script").text() );
				
				tmp_card.Use_Bridge_Helper( Second_Element.firstChildElement("Use_Bridge_Helper").text() == "yes" );
				tmp_card.Set_Bridge_Helper( Second_Element.firstChildElement("Bridge_Helper").text() );

				tmp_card.Use_Listen( Second_Element.firstChildElement("Use_Listen").text() == "yes" );
				tmp_card.Set_Listen( Second_Element.firstChildElement("Listen").text() );
				
				tmp_card. Use_Connect( Second_Element.firstChildElement("Use_Connect").text() == "yes" );
				tmp_card.Set_Connect( Second_Element.firstChildElement("Connect").text() );
				
				tmp_card.Use_MCast( Second_Element.firstChildElement("Use_MCast").text() == "yes" );
				tmp_card.Set_MCast( Second_Element.firstChildElement("MCast").text() );
				
				tmp_card.Use_Sock( Second_Element.firstChildElement("Use_Sock").text() == "yes" );
				tmp_card.Set_Sock( Second_Element.firstChildElement("Sock").text() );
				
				tmp_card.Use_Port( Second_Element.firstChildElement("Use_Port").text() == "yes" );
				tmp_card.Set_Port( Second_Element.firstChildElement("Port").text().toInt() );
				
				tmp_card.Use_Group( Second_Element.firstChildElement("Use_Group").text() == "yes" );
				tmp_card.Set_Group( Second_Element.firstChildElement("Group").text() );
				
				tmp_card.Use_Mode( Second_Element.firstChildElement("Use_Mode").text() == "yes" );
				tmp_card.Set_Mode( Second_Element.firstChildElement("Mode").text() );
				
				tmp_card.Use_File( Second_Element.firstChildElement("Use_File").text() == "yes" );
				tmp_card.Set_File( Second_Element.firstChildElement("File").text() );
				
				tmp_card.Use_Len( Second_Element.firstChildElement("Use_Len").text() == "yes" );
				tmp_card.Set_Len( Second_Element.firstChildElement("Len").text().toInt() );
				
				tmp_card.Use_Addr( Second_Element.firstChildElement("Use_Addr").text() == "yes" );
				tmp_card.Set_Addr( Second_Element.firstChildElement("Addr").text() );
				
				tmp_card.Use_Vectors( Second_Element.firstChildElement("Use_Vectors").text() == "yes" );
				tmp_card.Set_Vectors( Second_Element.firstChildElement("Vectors").text().toInt() );
				
				tmp_card.Use_Net( Second_Element.firstChildElement("Use_Net").text() == "yes" );
				tmp_card.Set_Net( Second_Element.firstChildElement("Net").text() );
				
				tmp_card.Use_Host( Second_Element.firstChildElement("Use_Host").text() == "yes" );
				tmp_card.Set_Host( Second_Element.firstChildElement("Host").text() );
				
				tmp_card.Use_Restrict( Second_Element.firstChildElement("Use_Restrict").text() == "yes" );
				tmp_card.Set_Restrict( Second_Element.firstChildElement("Restrict").text() == "yes" );
				
				tmp_card.Use_DHCPstart( Second_Element.firstChildElement("Use_DHCPstart").text() == "yes" );
				tmp_card.Set_DHCPstart( Second_Element.firstChildElement("DHCPstart").text() );
				
				tmp_card.Use_DNS( Second_Element.firstChildElement("Use_DNS").text() == "yes" );
				tmp_card.Set_DNS( Second_Element.firstChildElement("DNS").text() );
				
				tmp_card.Use_Tftp( Second_Element.firstChildElement("Use_Tftp").text() == "yes" );
				tmp_card.Set_Tftp( Second_Element.firstChildElement("Tftp").text() );
				
				tmp_card.Use_Bootfile( Second_Element.firstChildElement("Use_Bootfile").text() == "yes" );
				tmp_card.Set_Bootfile( Second_Element.firstChildElement("Bootfile").text() );
				
				tmp_card.Use_HostFwd( Second_Element.firstChildElement("Use_HostFwd").text() == "yes" );
				tmp_card.Set_HostFwd( Second_Element.firstChildElement("HostFwd").text() );
				
				tmp_card.Use_GuestFwd( Second_Element.firstChildElement("Use_GuestFwd").text() == "yes" );
				tmp_card.Set_GuestFwd( Second_Element.firstChildElement("GuestFwd").text() );
				
				tmp_card.Use_SMB( Second_Element.firstChildElement("Use_SMB").text() == "yes" );
				tmp_card.Set_SMB( Second_Element.firstChildElement("SMB").text() );
				
				tmp_card.Use_SMBserver( Second_Element.firstChildElement("Use_SMBserver").text() == "yes" );
				tmp_card.Set_SMBserver( Second_Element.firstChildElement("SMBserver").text() );
				
				tmp_card.Use_Sndbuf( Second_Element.firstChildElement("Use_Sndbuf").text() == "yes" );
				tmp_card.Set_Sndbuf( Second_Element.firstChildElement("Sndbuf").text().toInt() );
				
				tmp_card.Use_VNet_hdr( Second_Element.firstChildElement("Use_VNet_hdr").text() == "yes" );
				tmp_card.Set_VNet_hdr( Second_Element.firstChildElement("VNet_hdr").text() == "yes" );

				tmp_card.Use_VHost( Second_Element.firstChildElement("Use_VHost").text() == "yes" );
				tmp_card.Set_VHost( Second_Element.firstChildElement("VHost").text() == "yes" );
				
				tmp_card.Use_VHostFd( Second_Element.firstChildElement("Use_VHostFd").text() == "yes" );
				tmp_card.Set_VHostFd( Second_Element.firstChildElement("VHostFd").text().toInt() );
				
				Network_Cards_Nativ << tmp_card;
			}
			
			// Redirections count
			int RedirsCount = Child_Element.firstChildElement( "Redirections_Count" ).text().toInt();
			
			// Network Redirections...
			for( int rx = 0; rx < RedirsCount; rx++ )
			{
				Second_Element = Child_Element.firstChildElement( "Network_Redirections_" + QString::number(rx) );
				
				VM_Redirection tmp_redir;
				
				tmp_redir.Set_Protocol( Second_Element.firstChildElement("Protocol").text() );
				tmp_redir.Set_Host_Port( (Second_Element.firstChildElement("Host_Port").text().toInt()) );
				tmp_redir.Set_Guest_IP( Second_Element.firstChildElement("Guest_IP").text() );
				tmp_redir.Set_Guest_Port( Second_Element.firstChildElement("Guest_Port").text().toInt() );
				
				Add_Network_Redirection( tmp_redir );
			}
			
			// TFTP Prefix
			TFTP_Prefix = Child_Element.firstChildElement( "TFTP_Prefix" ).text();
			
			// SMB Directory
			SMB_Directory = Child_Element.firstChildElement( "SMB_Directory" ).text();
			
			// Serial Ports Count
			int serial_count = Child_Element.firstChildElement( "Serial_Port_Count" ).text().toInt();
			
			// Serial Port
			for( int px = 0; px < serial_count; ++px )
			{
				VM_Port tmp_port;
				
				Second_Element = Child_Element.firstChildElement( "Serial_Port_" + QString::number(px) );
				
                QString tmp_str = Second_Element.firstChildElement( "Port_Redirection" ).text();
				
				if( tmp_str == "vc" )
					tmp_port.Set_Port_Redirection( VM::PR_vc );
				else if( tmp_str == "pty" )
					tmp_port.Set_Port_Redirection( VM::PR_pty );
				else if( tmp_str == "none" )
					tmp_port.Set_Port_Redirection( VM::PR_none );
				else if( tmp_str == "null" )
					tmp_port.Set_Port_Redirection( VM::PR_null );
				else if( tmp_str == "dev" )
					tmp_port.Set_Port_Redirection( VM::PR_dev );
				else if( tmp_str == "file" )
					tmp_port.Set_Port_Redirection( VM::PR_file );
				else if( tmp_str == "stdio" )
					tmp_port.Set_Port_Redirection( VM::PR_stdio );
				else if( tmp_str == "pipe" )
					tmp_port.Set_Port_Redirection( VM::PR_pipe );
				else if( tmp_str == "udp" )
					tmp_port.Set_Port_Redirection( VM::PR_udp );
				else if( tmp_str == "tcp" )
					tmp_port.Set_Port_Redirection( VM::PR_tcp );
				else if( tmp_str == "telnet" )
					tmp_port.Set_Port_Redirection( VM::PR_telnet );
				else if( tmp_str == "unix" )
					tmp_port.Set_Port_Redirection( VM::PR_unix );
				else if( tmp_str == "com" )
					tmp_port.Set_Port_Redirection( VM::PR_com );
				else if( tmp_str == "msmouse" )
					tmp_port.Set_Port_Redirection( VM::PR_msmouse );
				else if( tmp_str == "mon" )
					tmp_port.Set_Port_Redirection( VM::PR_mon );
				else if( tmp_str == "braille" )
					tmp_port.Set_Port_Redirection( VM::PR_braille );
				else
					tmp_port.Set_Port_Redirection( VM::PR_Default );
				
				// Parametrs Line
				tmp_port.Set_Parametrs_Line( Second_Element.firstChildElement("Parametrs").text() );
				
				Serial_Ports << tmp_port;
			}
			
			// Parallel Ports Count
			int parallel_count = Child_Element.firstChildElement( "Parallel_Port_Count" ).text().toInt();
			
			// Parallel Port
			for( int px = 0; px < parallel_count; ++px )
			{
				VM_Port tmp_port;
				
				Second_Element = Child_Element.firstChildElement( "Parallel_Port_" + QString::number(px) );
			
                QString tmp_str = Second_Element.firstChildElement( "Port_Redirection" ).text();
				
				if( tmp_str == "vc" )
					tmp_port.Set_Port_Redirection( VM::PR_vc );
				else if( tmp_str == "pty" )
					tmp_port.Set_Port_Redirection( VM::PR_pty );
				else if( tmp_str == "none" )
					tmp_port.Set_Port_Redirection( VM::PR_none );
				else if( tmp_str == "null" )
					tmp_port.Set_Port_Redirection( VM::PR_null );
				else if( tmp_str == "dev" )
					tmp_port.Set_Port_Redirection( VM::PR_dev );
				else if( tmp_str == "file" )
					tmp_port.Set_Port_Redirection( VM::PR_file );
				else if( tmp_str == "stdio" )
					tmp_port.Set_Port_Redirection( VM::PR_stdio );
				else if( tmp_str == "pipe" )
					tmp_port.Set_Port_Redirection( VM::PR_pipe );
				else if( tmp_str == "udp" )
					tmp_port.Set_Port_Redirection( VM::PR_udp );
				else if( tmp_str == "tcp" )
					tmp_port.Set_Port_Redirection( VM::PR_tcp );
				else if( tmp_str == "telnet" )
					tmp_port.Set_Port_Redirection( VM::PR_telnet );
				else if( tmp_str == "unix" )
					tmp_port.Set_Port_Redirection( VM::PR_unix );
				else if( tmp_str == "com" )
					tmp_port.Set_Port_Redirection( VM::PR_com );
				else if( tmp_str == "msmouse" )
					tmp_port.Set_Port_Redirection( VM::PR_msmouse );
				else if( tmp_str == "mon" )
					tmp_port.Set_Port_Redirection( VM::PR_mon );
				else if( tmp_str == "braille" )
					tmp_port.Set_Port_Redirection( VM::PR_braille );
				else
					tmp_port.Set_Port_Redirection( VM::PR_Default );
				
				// Parametrs Line
				tmp_port.Set_Parametrs_Line( Second_Element.firstChildElement("Parametrs").text() );
				
				Parallel_Ports << tmp_port;
			}
			
			// USB Count
			int usb_count = Child_Element.firstChildElement( "USB_Ports_Count" ).text().toInt();
			
			// USB Devices
			for( int ux = 0; ux < usb_count; ++ux )
			{
				Second_Element = Child_Element.firstChildElement( "USB_Port_" + QString::number(ux) );
				
				VM_USB tmp_usb;
				
				tmp_usb.Set_Use_Host_Device( Second_Element.firstChildElement("Use_Host_Device").text() == "true" );
				tmp_usb.Set_Manufacturer_Name( Second_Element.firstChildElement("Manufacturer_Name").text() );
				tmp_usb.Set_Product_Name( Second_Element.firstChildElement("Product_Name").text() );
				tmp_usb.Set_Vendor_ID( Second_Element.firstChildElement("Vendor_ID").text() );
				tmp_usb.Set_Product_ID( Second_Element.firstChildElement("Product_ID").text() );
				tmp_usb.Set_Serial_Number( Second_Element.firstChildElement("Serial_Number").text() );
				tmp_usb.Set_Speed( Second_Element.firstChildElement("Speed").text() );
				
				// For AQEMU VM files version 0.8.2 and oldest
				if( ! Second_Element.firstChildElement("BusAddr").text().isEmpty() )
				{
					QStringList busAddrList = Second_Element.firstChildElement("BusAddr").text().split( ':', QString::SkipEmptyParts );
					if( busAddrList.count() != 2 )
					{
						AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )",
								 "Cannot parse obsolete USB Bus.Addr string! Data: " + Second_Element.firstChildElement("BusAddr").text() );
					}
					else
					{
						tmp_usb.Set_Bus( busAddrList[0] );
						tmp_usb.Set_Addr( busAddrList[1] );
					}
				}
				else // AQEMU Version > 0.8.2
				{
					tmp_usb.Set_Bus( Second_Element.firstChildElement("Bus").text() );
					tmp_usb.Set_Addr( Second_Element.firstChildElement("Addr").text() );
					tmp_usb.Set_DevPath( Second_Element.firstChildElement("Path").text() );
				}
				
				// QEMU USB Devices
				bool usb_k, usb_m, usb_t, usb_wt, usb_b;
				usb_k = usb_m = usb_t = usb_wt = usb_b = false;
				
				// Keyboard
				usb_k = (Child_Element.firstChildElement("USB_Keyboard").text() == "true" );
				
				// Mouse
				usb_m = (Child_Element.firstChildElement("USB_Mouse").text() == "true" );
				
				// Tablet
				usb_t = (Child_Element.firstChildElement("USB_Tablet").text() == "true" );
				
				// Wacom Tablet
				usb_wt = (Child_Element.firstChildElement("USB_WacomTablet").text() == "true" );
				
				// Braille
				usb_b = (Child_Element.firstChildElement("USB_Braille").text() == "true" );
				
				// Set
				tmp_usb.Set_USB_QEMU_Devices( usb_k, usb_m, usb_t, usb_wt, usb_b );
				
				// Next code for compatibity old versions
				if( usb_k == false &&
					usb_m == false &&
					usb_t == false &&
					usb_wt == false &&
					usb_b == false ) tmp_usb.Set_Use_Host_Device( true );
				
				// Add device
				USB_Ports << tmp_usb;
			}
			
			// Linux Boot
			Linux_Boot = (Child_Element.firstChildElement("Linux_Boot").text() == "true" );
			
			// bzImage Path
			bzImage_Path = Child_Element.firstChildElement( "bzImage_Path" ).text();
			
			// Initrd Path
			Initrd_Path = Child_Element.firstChildElement( "Initrd_Path" ).text();
			
			// Kernel Command Line Arguments
			Kernel_ComLine = Child_Element.firstChildElement( "Kernel_ComLine" ).text();
			
			// Use ROM File
			Use_ROM_File = (Child_Element.firstChildElement("Use_ROM_File").text() == "true");
			
			// ROM File Path
			ROM_File =  Child_Element.firstChildElement( "ROM_File" ).text();
			
			// Use MTDBlock File
			MTDBlock = (Child_Element.firstChildElement("Use_MTDBlock_File").text() == "true" );
			
			// MTDBlock File
			MTDBlock_File = Child_Element.firstChildElement( "MTDBlock_File" ).text();
			
			// Use SecureDigital File
			SecureDigital = (Child_Element.firstChildElement("Use_SecureDigital_File").text() == "true" );
			
			// SecureDigital File
			SecureDigital_File = Child_Element.firstChildElement( "SecureDigital_File" ).text();
			
			// Use PFlash File
			PFlash = (Child_Element.firstChildElement("Use_PFlash_File").text() == "true" );
			
			// PFlash File
			PFlash_File = Child_Element.firstChildElement( "PFlash_File" ).text();
			
			// Enable KVM
			Enable_KVM = ! (Child_Element.firstChildElement("Enable_KVM").text() == "false" );
			
			// Use KVM IRQChip
			KVM_IRQChip = (Child_Element.firstChildElement("Use_KVM_IRQChip").text() == "true" );
			
			// Use No KVM Pit
			No_KVM_Pit = (Child_Element.firstChildElement("Use_No_KVM_Pit").text() == "true" );
			
			// KVM_No_Pit_Reinjection
			KVM_No_Pit_Reinjection = (Child_Element.firstChildElement("Use_KVM_No_Pit_Reinjection").text() == "true" );
			
			// Use_No_KVM_Pit
			KVM_Nesting = (Child_Element.firstChildElement("Use_KVM_Nesting").text() == "true" );
			
			// Use KVM Shadow Memory
			KVM_Shadow_Memory = (Child_Element.firstChildElement("Use_KVM_Shadow_Memory").text() == "true" );
			
			// KVM_Shadow_Memory_Size
			KVM_Shadow_Memory_Size = (uint)Child_Element.firstChildElement( "KVM_Shadow_Memory_Size" ).text().toInt();
			
			// Init Graphic Mode
			Second_Element = Child_Element.firstChildElement( "Init_Graphic_Mode" );
			
			// Enabled
			Init_Graphic_Mode.Set_Enabled( Second_Element.firstChildElement("Enabled").text() == "true" );
			
			// Width
			Init_Graphic_Mode.Set_Width( Second_Element.firstChildElement("Width").text().toInt() );
			
			// Height
			Init_Graphic_Mode.Set_Height( Second_Element.firstChildElement("Height").text().toInt() );
			
			// Depth
			Init_Graphic_Mode.Set_Depth( Second_Element.firstChildElement("Depth").text().toInt() );
			
			// Use No Frame
			No_Frame = (Child_Element.firstChildElement("Use_No_Frame").text() == "true" );
			
			// Use Alt Grab
			Alt_Grab = (Child_Element.firstChildElement("Use_Alt_Grab").text() == "true" );
			
			// Use No Quit
			No_Quit = (Child_Element.firstChildElement("Use_No_Quit").text() == "true");
			
			// Use Portrait
			Portrait = (Child_Element.firstChildElement("Use_Portrait").text() == "true");
			
			// Show_Cursor
			Show_Cursor = (Child_Element.firstChildElement("Show_Cursor").text() == "true");
			
			// Curses
			Curses = (Child_Element.firstChildElement("Curses").text() == "true");
			
			// RTC_TD_Hack
			RTC_TD_Hack = (Child_Element.firstChildElement("RTC_TD_Hack").text() == "true");
			
			// Start Date
			Start_Date = (Child_Element.firstChildElement("Use_Start_Date").text() == "true");
			
			// Start DateTime
			Start_DateTime = QDateTime::fromString( Child_Element.firstChildElement("Start_Date").text(), "dd.MM.yyyy HH:mm:ss" );
			
			// SPICE
			Second_Element = Child_Element.firstChildElement( "SPICE" );
			
			if( ! Second_Element.isNull() )
			{
				bool ok = false;
				
				SPICE.Use_SPICE( Second_Element.firstChildElement("Use_SPICE").text() == "true" );
				
				SPICE.Set_GXL_Devices_Count( Second_Element.firstChildElement("GXL_Devices_Count").text().toUInt(&ok) );
				if( ! ok ) AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE GXL_Devices_Count invalid!" );
				
				SPICE.Set_RAM_Size( Second_Element.firstChildElement("RAM_Size").text().toInt(&ok) );
				if( ! ok ) AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE RAM_Size invalid!" );
				
				SPICE.Set_Port( Second_Element.firstChildElement("Port").text().toUInt(&ok) );
				if( ! ok ) AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE Port invalid" );
				
				SPICE.Use_SPort( Second_Element.firstChildElement( "Use_SPort" ).text() == "true" );
				
				SPICE.Set_SPort( Second_Element.firstChildElement("SPort").text().toUInt(&ok) );
				if( ! ok ) AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE SPort invalid!" );
				
				SPICE.Use_Hostname( Second_Element.firstChildElement( "Use_Hostname" ).text() == "true" );
				
				SPICE.Set_Hostname( Second_Element.firstChildElement( "Hostname" ).text() );
				
				SPICE.Use_Image_Compression( Second_Element.firstChildElement( "Use_Image_Compression" ).text() == "true" );
				
				QString spiceIC = Second_Element.firstChildElement( "Image_Compression_Type" ).text();
				if( spiceIC == "on" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_on );
				else if( spiceIC == "auto_glz" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_auto_glz );
				else if( spiceIC == "auto_lz" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_auto_lz );
				else if( spiceIC == "quic" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_quic );
				else if( spiceIC == "glz" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_glz );
				else if( spiceIC == "lz" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_lz );
				else if( spiceIC == "off" )
					SPICE.Set_Image_Compression( VM::SPICE_IC_Type_off );
				else
					AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE image compression type invalid!" );
				
				SPICE.Use_Video_Stream_Compression( Second_Element.firstChildElement( "Use_Video_Stream_Compression" ).text() == "true" );
				
				SPICE.Use_Renderer( Second_Element.firstChildElement( "Use_Renderer" ).text() == "true" );
				
				QDomElement rendererListElement = Second_Element.firstChildElement( "Renderers_List" );
				if( rendererListElement.isNull() )
					AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE renderer list is empty!" );
				else
				{
					QList<VM::SPICE_Renderer> rendererList;
					QList<QDomElement> renderNodes = rendererListElement.childNodes();
                    QListIterator<QDomElement> it(renderNodes);					

					while( it.hasNext() )
					{
                        QDomElement el = it.next();
						QString rendererTypeStr = el.tagName();
						
						if( rendererTypeStr == "oglpbuf" )
							rendererList << VM::SPICE_Renderer_oglpbuf;
						else if( rendererTypeStr == "oglpixmap" )
							rendererList << VM::SPICE_Renderer_oglpixmap;
						else if( rendererTypeStr == "cairo" )
							rendererList << VM::SPICE_Renderer_cairo;
						else
							AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE renderer type invalid!" );
					}
					
					if( rendererList.isEmpty() )
						AQError( "bool Virtual_Machine::Load_VM( const QString &file_name )", "SPICE renderers list is empty!" );
					else
						SPICE.Set_Renderer_List( rendererList );
				}
				
				SPICE.Use_Playback_Compression( Second_Element.firstChildElement( "Use_Playback_Compression" ).text() == "true" );
				
				SPICE.Use_Password( Second_Element.firstChildElement( "Use_Password" ).text() == "true" );
				
				SPICE.Set_Password( Second_Element.firstChildElement( "Password" ).text() );
			}
			
			// Use VNC
			VNC = (Child_Element.firstChildElement("Use_VNC").text() == "true");
			
			// VNC Socket Mode
			VNC_Socket_Mode = (Child_Element.firstChildElement("VNC_Socket_Mode").text() == "true");
			
			// VNC Unix Socket Path
			VNC_Unix_Socket_Path = Child_Element.firstChildElement( "VNC_Unix_Socket_Path" ).text();
			
			// VNC Display Number
			VNC_Display_Number = (uint)Child_Element.firstChildElement( "VNC_Display_Number" ).text().toInt();
			
			// Use VNC Password
			VNC_Password = (Child_Element.firstChildElement("Use_VNC_Password").text() == "true");
			
			// Use VNC TLS
			VNC_TLS = (Child_Element.firstChildElement("Use_VNC_TLS").text() == "true");
			
			// Use VNC x509
			VNC_x509 = (Child_Element.firstChildElement("Use_VNC_x509").text() == "true");
			
			// VNC x509 Folder Path
			VNC_x509_Folder_Path = Child_Element.firstChildElement( "VNC_x509_Folder_Path" ).text();
			
			// Use VNC x509verify
			VNC_x509verify = (Child_Element.firstChildElement("Use_VNC_x509verify").text() == "true");
			
			// VNC x509verify Folder Path
			VNC_x509verify_Folder_Path = Child_Element.firstChildElement( "VNC_x509verify_Folder_Path" ).text();
			
			// Additional Arguments
			Additional_Args = Child_Element.firstChildElement( "Additional_Args" ).text();
			
			// Only_User_Args
			Only_User_Args = (Child_Element.firstChildElement("Only_User_Args").text() == "true");
			
			// Use_User_Emulator_Binary
			Use_User_Emulator_Binary = (Child_Element.firstChildElement("Use_User_Emulator_Binary").text() == "true");
			
			return true;
		}
	}
}

VM_Native_Storage_Device Virtual_Machine::Load_VM_Native_Storage_Device( const QDomElement &Second_Element ) const
{
    VM_Native_Storage_Device tmp_device;
	
	// Use File Path
	tmp_device.Use_File_Path( Second_Element.firstChildElement("Use_File_Path").text() == "true" );
	
	// File Path
	tmp_device.Set_File_Path( Second_Element.firstChildElement("File_Path").text() );
	
	// Use Interface
	tmp_device.Use_Interface( Second_Element.firstChildElement("Use_Interface").text() == "true" );
	
	// Interface
	QString interface_str = Second_Element.firstChildElement( "Interface" ).text();
	
	if( interface_str == "IDE" )
		tmp_device.Set_Interface( VM::DI_IDE );
	else if( interface_str == "SCSI" )
		tmp_device.Set_Interface( VM::DI_SCSI );
	else if( interface_str == "SD" )
		tmp_device.Set_Interface( VM::DI_SD );
	else if( interface_str == "MTD" )
		tmp_device.Set_Interface( VM::DI_MTD );
	else if( interface_str == "Floppy" )
		tmp_device.Set_Interface( VM::DI_Floppy );
	else if( interface_str == "PFlash" )
		tmp_device.Set_Interface( VM::DI_PFlash );
	else if( interface_str == "Virtio" )
		tmp_device.Set_Interface( VM::DI_Virtio );
	else if( interface_str == "Virtio_SCSI" )
		tmp_device.Set_Interface( VM::DI_Virtio_SCSI );
	else if( interface_str == "" ) ; // No Value
	else
	{
        AQError( "VM_Native_Storage_Device Virtual_Machine::Load_VM_Native_Storage_Device( const QDomElement &Second_Element ) const",
				 "Storage Device Interface Default Section! Value: \"" + interface_str + "\"" );
	}
	
	// Use Bus Unit
	tmp_device.Use_Bus_Unit( Second_Element.firstChildElement("Use_Bus_Unit").text() == "true" );
	
	// Bus
	tmp_device.Set_Bus( Second_Element.firstChildElement("Bus").text().toInt() );
	
	// Unit
	tmp_device.Set_Bus( Second_Element.firstChildElement("Unit").text().toInt() );
	
	// Use Index
	tmp_device.Use_Index( Second_Element.firstChildElement("Use_Index").text() == "true" );
	
	// Index
	tmp_device.Set_Index( Second_Element.firstChildElement("Index").text().toInt() );
	
	// Use Media
	tmp_device.Use_Media( Second_Element.firstChildElement("Use_Media").text() == "true" );
	
	// Media
	QString media_str = Second_Element.firstChildElement( "Media" ).text();
	
	if( media_str == "Disk" )
		tmp_device.Set_Index( VM::DM_Disk );
	else if( media_str == "CD_ROM" )
		tmp_device.Set_Index( VM::DM_CD_ROM );
	else if( media_str == "" ) ; // No value
	else
	{
        AQError( "VM_Native_Storage_Device Virtual_Machine::Load_VM_Native_Storage_Device( const QDomElement &Second_Element ) const",
				 "Storage Device Media Default Section!" );
	}
	
	// Use hdachs
	tmp_device.Use_hdachs( Second_Element.firstChildElement("Use_hdachs").text() == "true" );
	
	// Cyls
	tmp_device.Set_Cyls( Second_Element.firstChildElement("Cyls").text().toULongLong() );
	
	// Heads
	tmp_device.Set_Heads( Second_Element.firstChildElement("Heads").text().toULongLong() );
	
	// Secs
	tmp_device.Set_Secs( Second_Element.firstChildElement("Secs").text().toULongLong() );
	
	// Trans
	tmp_device.Set_Cyls( Second_Element.firstChildElement("Trans").text().toULongLong() );
	
	// Use Snapshot
	tmp_device.Use_Snapshot( Second_Element.firstChildElement("Use_Snapshot").text() == "true" );
	
	// Snapshot
	tmp_device.Set_Snapshot( Second_Element.firstChildElement("Snapshot").text() == "true" );
	
	// Use Cache
	tmp_device.Use_Cache( Second_Element.firstChildElement("Use_Cache").text() == "true" );
	
	// Cache
	tmp_device.Set_Cache( Second_Element.firstChildElement("Cache").text() );
	
	// Use AIO
	tmp_device.Use_AIO( Second_Element.firstChildElement("Use_AIO").text() == "true" );
	
	// AIO
	tmp_device.Set_AIO( Second_Element.firstChildElement("AIO").text() );
	
	// Use Boot
	tmp_device.Use_Boot( Second_Element.firstChildElement("Use_Boot").text() == "true" );

	// Boot
	tmp_device.Set_Boot( Second_Element.firstChildElement("Boot").text() == "true" );

	// Use Discard
	tmp_device.Use_Discard( Second_Element.firstChildElement("Use_Discard").text() == "true" );
	
	// Discard
	tmp_device.Set_Discard( Second_Element.firstChildElement("Discard").text() == "true" );


	return tmp_device;
}


VM_Shared_Folder Virtual_Machine::Load_VM_Shared_Folder( const QDomElement &Second_Element ) const
{
	VM_Shared_Folder tmp_device;
	
	// Folder Path
	tmp_device.Set_Folder( Second_Element.firstChildElement("Path").text() );
	
	return tmp_device;
}


void Virtual_Machine::Save_VM_Native_Storage_Device( QDomDocument &New_Dom_Document, QDomElement &Dom_Element,
                                                    const VM_Native_Storage_Device &device ) const
{
	// Use File Path
	QDomElement Sec_Element = New_Dom_Document.createElement( "Use_File_Path" );
	Dom_Element.appendChild( Sec_Element );
	QDomText Dom_Text;
	
	if( device.Use_File_Path() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// File Path
	Sec_Element = New_Dom_Document.createElement( "File_Path" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( device.Get_File_Path() );
	Sec_Element.appendChild( Dom_Text );
	
	// Use Interface
	Sec_Element = New_Dom_Document.createElement( "Use_Interface" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Interface() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Interface
	switch( device.Get_Interface() )
	{
		case VM::DI_IDE:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "IDE" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DI_SCSI:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "SCSI" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DI_SD:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "SD" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DI_MTD:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "MTD" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DI_Floppy:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "Floppy" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DI_PFlash:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "PFlash" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DI_Virtio:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "Virtio" );
			Sec_Element.appendChild( Dom_Text );
			break;

		case VM::DI_Virtio_SCSI:
			Sec_Element = New_Dom_Document.createElement( "Interface" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "Virtio_SCSI" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		default:
			AQError( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
					 "Storage Device Interface Default Section!" );
			break;
	}
	
	// Use Bus Unit
	Sec_Element = New_Dom_Document.createElement( "Use_Bus_Unit" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Bus_Unit() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Bus
	Sec_Element = New_Dom_Document.createElement( "Bus" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Bus()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Unit
	Sec_Element = New_Dom_Document.createElement( "Unit" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Unit()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Use Index
	Sec_Element = New_Dom_Document.createElement( "Use_Index" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Index() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Index
	Sec_Element = New_Dom_Document.createElement( "Index" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Index()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Use Media
	Sec_Element = New_Dom_Document.createElement( "Use_Media" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Media() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Media
	switch( device.Get_Media() )
	{
		case VM::DM_Disk:
			Sec_Element = New_Dom_Document.createElement( "Media" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "Disk" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		case VM::DM_CD_ROM:
			Sec_Element = New_Dom_Document.createElement( "Media" );
			Dom_Element.appendChild( Sec_Element );
			Dom_Text = New_Dom_Document.createTextNode( "CD_ROM" );
			Sec_Element.appendChild( Dom_Text );
			break;
			
		default:
			AQError( "bool Virtual_Machine::Create_VM_File( const QString &file_name, bool template_mode )",
						"Storage Device Media Default Section!" );
			break;
	}
	
	// Use hdachs
	Sec_Element = New_Dom_Document.createElement( "Use_hdachs" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_hdachs() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Cyls
	Sec_Element = New_Dom_Document.createElement( "Cyls" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Cyls()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Heads
	Sec_Element = New_Dom_Document.createElement( "Heads" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Heads()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Secs
	Sec_Element = New_Dom_Document.createElement( "Secs" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Secs()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Trans
	Sec_Element = New_Dom_Document.createElement( "Trans" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( QString::number(device.Get_Trans()) );
	Sec_Element.appendChild( Dom_Text );
	
	// Use Snapshot
	Sec_Element = New_Dom_Document.createElement( "Use_Snapshot" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Snapshot() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Snapshot
	Sec_Element = New_Dom_Document.createElement( "Snapshot" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Get_Snapshot() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Use Cache
	Sec_Element = New_Dom_Document.createElement( "Use_Cache" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Cache() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Cache
	Sec_Element = New_Dom_Document.createElement( "Cache" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( device.Get_Cache() );
	Sec_Element.appendChild( Dom_Text );
	
	// Use AIO
	Sec_Element = New_Dom_Document.createElement( "Use_AIO" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_AIO() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// AIO
	Sec_Element = New_Dom_Document.createElement( "AIO" );
	Dom_Element.appendChild( Sec_Element );
	Dom_Text = New_Dom_Document.createTextNode( device.Get_AIO() );
	Sec_Element.appendChild( Dom_Text );
	
	// Use Boot
	Sec_Element = New_Dom_Document.createElement( "Use_Boot" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Use_Boot() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	
	// Boot
	Sec_Element = New_Dom_Document.createElement( "Boot" );
	Dom_Element.appendChild( Sec_Element );
	
	if( device.Get_Boot() )
		Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
		Dom_Text = New_Dom_Document.createTextNode( "false" );
	
	Sec_Element.appendChild( Dom_Text );
	// Use Discard
	Sec_Element = New_Dom_Document.createElement( "Use_Discard" );
	Dom_Element.appendChild( Sec_Element );

	if( device.Use_Discard() )
	        Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
	        Dom_Text = New_Dom_Document.createTextNode( "false" );

	Sec_Element.appendChild( Dom_Text );

	// Discard
	Sec_Element = New_Dom_Document.createElement( "Discard" );
	Dom_Element.appendChild( Sec_Element );

	if ( device.Get_Discard() )
	        Dom_Text = New_Dom_Document.createTextNode( "true" );
	else
	        Dom_Text = New_Dom_Document.createTextNode( "false" );

	Sec_Element.appendChild( Dom_Text );
}

void Virtual_Machine::Save_VM_Shared_Folder( QDomDocument &New_Dom_Document, QDomElement &Dom_Element,
													const VM_Shared_Folder &shared_folder ) const
{
	// Folder Path
	QDomElement Sec_Element = New_Dom_Document.createElement( "Path" );
	Dom_Element.appendChild( Sec_Element );
	QDomText Dom_Text = New_Dom_Document.createTextNode( shared_folder.Get_Folder() );	
	
	Sec_Element.appendChild( Dom_Text );
}


bool Virtual_Machine::Save_VM()
{
	return Create_VM_File( VM_XML_File_Path, false );
}

bool Virtual_Machine::Save_VM( const QString &file_name )
{
	return Create_VM_File( file_name, false );
}

QStringList Virtual_Machine::Build_QEMU_Args()
{
	QStringList Args;

	// Introducing a second argument collector for the Storage-part,
	// because the new 'Virtio_SCSI' generated argument list requires the
	// VirtIO SCSI storage adapter leading any storage device in the
	// argument list. Because i can only detect the requirement of this
	// additional arg after each storage device has been examined, these
	// storage arguments must be collected in a separate list and then
	// appended to Args *after* the "-device" "virtio-scsi-pci,id=aq-vscsi"
	// arguments where appended ...
	QStringList StorageArgs;

	#ifdef Q_OS_WIN32
	Args << "-monitor" << QString("tcp:%1:%2,server,nowait")
						  .arg(Settings.value("Emulator_Monitor_Hostname", "localhost").toString() )
						  .arg(Settings.value("Emulator_MonGitor_Port", 6000).toInt() + Embedded_Display_Port);

	Monitor_Hostname = Settings.value("Emulator_Monitor_Hostname", "localhost").toString();
	Monitor_Port = (unsigned int)Settings.value("Emulator_MonGitor_Port", 6000).toInt() + Embedded_Display_Port;
	#else
	if( Settings.value("Emulator_Monitor_Type", "stdio").toString() == "tcp" )
	{
		Args << "-monitor" << QString("tcp:%1:%2,server,nowait")
							  .arg(Settings.value("Emulator_Monitor_Hostname", "localhost").toString() )
							  .arg(Settings.value("Emulator_MonGitor_Port", 6000).toInt() + Embedded_Display_Port);

		Monitor_Hostname = Settings.value("Emulator_Monitor_Hostname", "localhost").toString();
		Monitor_Port = (unsigned int)Settings.value("Emulator_MonGitor_Port", 6000).toInt() + Embedded_Display_Port;
	}
	else
	{
		Args << "-monitor" << "stdio";
	}
	#endif
	
	// Saved?
	if( ! Start_Snapshot_Tag.isEmpty() )
	{
		Args << "-loadvm" << Start_Snapshot_Tag;
		Load_Mode = true;
	}
	else if( State == VM::VMS_Saved )
	{
		Args << "-loadvm" << "aqemu_save";
		Load_Mode = true;
	}
	else
	{
		Load_Mode = false;
	}
	
	if( Current_Emulator_Devices.System.QEMU_Name.isEmpty() )
	{
		AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
				 "Current_Emulator_Devices is empty!" );
		Update_Current_Emulator_Devices();
		
		if( Current_Emulator_Devices.System.QEMU_Name.isEmpty() )
		{
			AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
					 "Current_Emulator_Devices is empty! AQEMU Break :(" );
			return Args;
		}
	}
	
	// SMP Mode
	if( SMP.SMP_Count <= Current_Emulator_Devices.PSO_SMP_Count )
	{
		if( Current_Emulator_Devices.PSO_SMP_Cores ||
			Current_Emulator_Devices.PSO_SMP_Threads ||
			Current_Emulator_Devices.PSO_SMP_Sockets ||
			Current_Emulator_Devices.PSO_SMP_MaxCPUs )
		{
			if( SMP.SMP_Count > 1 ||
				SMP.SMP_Cores > 0 ||
				SMP.SMP_Threads > 0 ||
				SMP.SMP_Sockets > 0 ||
				SMP.SMP_MaxCPUs > 0 )
			{
				QString smp_args = QString::number( SMP.SMP_Count );
				
				if( Current_Emulator_Devices.PSO_SMP_Cores && SMP.SMP_Cores > 0 )
					smp_args += ",cores=" + QString::number( SMP.SMP_Cores );
				
				if( Current_Emulator_Devices.PSO_SMP_Threads && SMP.SMP_Threads > 0 )
					smp_args += ",threads=" + QString::number( SMP.SMP_Threads );
				
				if( Current_Emulator_Devices.PSO_SMP_Sockets && SMP.SMP_Sockets > 0 )
					smp_args += ",sockets=" + QString::number( SMP.SMP_Sockets );
				
				if( Current_Emulator_Devices.PSO_SMP_MaxCPUs && SMP.SMP_MaxCPUs > 0 )
					smp_args += ",maxcpus=" + QString::number( SMP.SMP_MaxCPUs );
				
				Args << "-smp" << smp_args;
			}
		}
		else
		{
			if( SMP.SMP_Count > 1 )
				Args << "-smp" << QString::number( SMP.SMP_Count );
		}
	}
	
	// CPU Model
	if( Current_Emulator_Devices.CPU_List.count() > 1 &&
		CPU_Type.isEmpty() == false )
	{
		Args << "-cpu" << CPU_Type;
	}
	
	// Audio
	QStringList audio_list;
	
	if( Audio_Card.Audio_sb16 &&  Current_Emulator_Devices.Audio_Card_List.Audio_sb16 ) audio_list << "sb16";
	if( Audio_Card.Audio_es1370 && Current_Emulator_Devices.Audio_Card_List.Audio_es1370 ) audio_list << "es1370";
	if( Audio_Card.Audio_Adlib && Current_Emulator_Devices.Audio_Card_List.Audio_Adlib ) audio_list << "adlib";
	if( Audio_Card.Audio_PC_Speaker && Current_Emulator_Devices.Audio_Card_List.Audio_PC_Speaker ) audio_list << "pcspk";
	if( Audio_Card.Audio_GUS && Current_Emulator_Devices.Audio_Card_List.Audio_GUS ) audio_list << "gus";
	if( Audio_Card.Audio_AC97 && Current_Emulator_Devices.Audio_Card_List.Audio_AC97 ) audio_list << "ac97";
	if( Audio_Card.Audio_HDA && Current_Emulator_Devices.Audio_Card_List.Audio_HDA ) audio_list << "hda";
	if( Audio_Card.Audio_cs4231a && Current_Emulator_Devices.Audio_Card_List.Audio_cs4231a ) audio_list << "cs4231a";
	
	if( audio_list.count() > 0 )
	{
		Args << "-soundhw";
		
		QString all_cards = "";
		
		for( int ax = 0; ax < audio_list.count(); ++ax )
		{
			// Next card end?
			if( ax != audio_list.count()-1 )
				all_cards += audio_list[ ax ] + ",";
			else
				all_cards += audio_list[ ax ];
		}
		
		Args << all_cards;
	}
	
	// Machine Type
	if( ! Machine_Type.isEmpty() )
		Args << "-M" << Machine_Type;
	
	// Keyboard Layout (language)
	if( Keyboard_Layout != "Default" )
		Args << "-k" << Get_Keyboard_Layout();
	
	// Video
	if( Current_Emulator_Devices.PSO_Std_VGA ) // QEMU before 0.10 style
		Args << Video_Card;
	else if( ! Video_Card.isEmpty() ) // QEMU 0.10 style
		Args << "-vga" << Video_Card;

    // Accelerator
    Args << "-machine";

    QStringList props;
    props << "accel="+VM::Accel_To_String( Machine_Accelerator );

	if( Current_Emulator_Devices.PSO_KVM_Shadow_Memory && KVM_Shadow_Memory )
		props << "kvm_shadow_mem=" + QString::number( KVM_Shadow_Memory_Size * 1024 );

    Args << props.join(",");
	
	// KVM Options
	//if( Current_Emulator_Devices.PSO_Enable_KVM && Enable_KVM )
	//	Args << "-enable-kvm";
	
	if( Current_Emulator_Devices.PSO_No_KVM_IRQChip && KVM_IRQChip )
		Args << "-no-kvm-irqchip";
	
	if( Current_Emulator_Devices.PSO_No_KVM_Pit && No_KVM_Pit )
		Args << "-no-kvm-pit";
	
	if( Current_Emulator_Devices.PSO_No_KVM_Pit_Reinjection && KVM_No_Pit_Reinjection )
		Args << "-no-kvm-pit-reinjection";
	
	if( Current_Emulator_Devices.PSO_Enable_Nesting && KVM_Nesting )
		Args << "-enable-nesting";
	
	// Memory
	Args << "-m" << QString::number( Memory_Size, 10 );
	
	// full screen
	if( Fullscreen )
		Args << "-full-screen";
	
	// Win2000 Hack
	if( Current_Emulator_Devices.PSO_Win2K_Hack &&
		Win2K_Hack ) Args << "-win2k-hack";
	
	// No Check FDD boot sector
	if( Current_Emulator_Devices.PSO_No_FB_Boot_Check &&
		Check_FDD_Boot_Sector == false ) Args << "-no-fd-bootchk";
	
	// No ACPI
	if( Current_Emulator_Devices.PSO_No_ACPI &&
		ACPI == false ) Args << "-no-acpi";
	
	// Snapshot
	if( Snapshot_Mode )
		Args << "-snapshot";
	
	// Do not start CPU at startup
	if( ! Start_CPU )
		Args << "-S";
	
	// Exit instead of rebooting
	if( No_Reboot )
		Args << "-no-reboot";
	
	if( Current_Emulator_Devices.PSO_Show_Cursor && Show_Cursor )
		Args << "-show-cursor";
	
	if( Current_Emulator_Devices.PSO_Curses && Curses )
		Args << "-curses";
	
	if( Current_Emulator_Devices.PSO_No_Shutdown && No_Shutdown )
		Args << "-no-shutdown";

	// This flag is used for the decision if the 'virtio-scsi-pci' storage
	// adapter must be inserted into the argument list. It is set if only
	// one of the storage devices was defined with the 'virtio-scsi'
	// (Virtio_SCSI) interface type ...
	bool has_virt_scsi = false;
    native_device_count = 0;

	// FD0
	if( FD0.Get_Enabled() )
	{
        if( FD0.Get_Native_Mode() )
		{
			// Testing for 'virtio-scsi' interface type
            VM::Device_Interface iftype = FD0.Get_Native_Device().Get_Interface();
			if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( FD0.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
		else
		{
			if( QFile::exists(FD0.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-fda" << "\"" + FD0.Get_File_Name() + "\"";
				else
					StorageArgs << "-fda" << FD0.Get_File_Name();
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
							QString("Image \"%1\" doesn't exists!").arg(FD0.Get_File_Name()) );
			}
		}
	}
	
	// FD1
	if( FD1.Get_Enabled() )
	{
        if( FD1.Get_Native_Mode() )
        {
            // Testing for the interface type 'virtio-scsi'
            VM::Device_Interface iftype = FD1.Get_Native_Device().Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( FD1.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
        else
		{
			if( QFile::exists(FD1.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-fdb" << "\"" + FD1.Get_File_Name() + "\"";
				else
					StorageArgs << "-fdb" << FD1.Get_File_Name();
            }
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
							QString("Image \"%1\" doesn't exist!").arg(FD1.Get_File_Name()) );
			}
		}
	}
	
	// CD-ROM
	if( CD_ROM.Get_Enabled() )
    {
        if( CD_ROM.Get_Native_Mode() )
        {
            // Testing for the interface type 'virtio-scsi'
            VM::Device_Interface iftype = CD_ROM.Get_Native_Device().Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( CD_ROM.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
		else
		{
			if( QFile::exists(CD_ROM.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-cdrom" << "\"" + CD_ROM.Get_File_Name() + "\"";
				else
					StorageArgs << "-cdrom" << CD_ROM.Get_File_Name();
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
							QString("Image \"%1\" doesn't exist!").arg(CD_ROM.Get_File_Name()) );
			}
		}
	}
	
	// HDA
	if( HDA.Get_Enabled() )
	{
        if( HDA.Get_Native_Mode() )
        {
            // Testing for the interface type 'virtio-scsi'
            VM::Device_Interface iftype = HDA.Get_Native_Device().Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( HDA.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
		else
		{
			if( QFile::exists(HDA.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-hda" << "\"" + HDA.Get_File_Name() + "\"";
				else
					StorageArgs << "-hda" << HDA.Get_File_Name();
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
						 QString("Image \"%1\" doesn't exist!").arg(HDA.Get_File_Name()) );
			}
		}
	}
	
	// HDB
	if( HDB.Get_Enabled() )
	{
        if( HDB.Get_Native_Mode() )
        {
            // Testing for the interface type 'virtio-scsi'
            VM::Device_Interface iftype = HDB.Get_Native_Device().Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( HDB.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
		else
		{
			if( QFile::exists(HDB.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-hdb" << "\"" + HDB.Get_File_Name() + "\"";
				else
					StorageArgs << "-hdb" << HDB.Get_File_Name();
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
						 QString("Image \"%1\" doesn't exist!").arg(HDB.Get_File_Name()) );
			}
		}
	}
	
	// HDC
	if( HDC.Get_Enabled() )
	{
        if( HDC.Get_Native_Mode() )
        {
            // Testing for the interface type 'virtio-scsi'
            VM::Device_Interface iftype = HDC.Get_Native_Device().Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( HDC.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
		else
		{
			if( QFile::exists(HDC.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-hdc" << "\"" + HDC.Get_File_Name() + "\"";
				else
					StorageArgs << "-hdc" << HDC.Get_File_Name();
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
						 QString("Image \"%1\" doesn't exist!").arg(HDC.Get_File_Name()) );
			}
		}
	}
	
	// HDD
	if( HDD.Get_Enabled() )
	{
        if( HDD.Get_Native_Mode() )
        {
            // Testing for the interface type 'virtio-scsi'
            VM::Device_Interface iftype = HDD.Get_Native_Device().Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
            }
            StorageArgs << Build_Native_Device_Args( HDD.Get_Native_Device(), Build_QEMU_Args_for_Tab_Info );
		}
		else
		{
			if( QFile::exists(HDD.Get_File_Name()) || Build_QEMU_Args_for_Tab_Info )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					StorageArgs << "-hdd" << "\"" + HDD.Get_File_Name() + "\"";
				else
					StorageArgs << "-hdd" << HDD.Get_File_Name();
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
						 QString("Image \"%1\" doesn't exist!").arg(HDD.Get_File_Name()) );
			}
		}
	}
	
	// Storage Devices
	if( Current_Emulator_Devices.PSO_Drive &&
		Storage_Devices.count() > 0 )
	{
		for( int ix = 0; ix < Storage_Devices.count(); ++ix )
        {
            // Testing for the 'virtio-scsi' interface type
            VM::Device_Interface iftype = Storage_Devices[ix].Get_Interface();
            if (iftype == VM::DI_Virtio_SCSI)
            {
				has_virt_scsi = true;
			}
            StorageArgs << Build_Native_Device_Args( Storage_Devices[ix], Build_QEMU_Args_for_Tab_Info );
		}
	}
	
    // Shared Folders //TODO
	if( Shared_Folders.count() > 0 )
	{
		for( int ix = 0; ix < Shared_Folders.count(); ++ix )
		{
			Args << Build_Shared_Folder_Args( Shared_Folders[ix], ix,  Build_QEMU_Args_for_Tab_Info );
		}
	}

	//      AQEMU's advanced device settings. If one of the detected storage
	//      devices is defined with this interface type, the required storage
	//      adapter must be added to the argument list *before* any
	//      of the storage devices ...
	//
    if (has_virt_scsi)
    {
		Args << "-device" << "virtio-scsi-pci,id=aq-vscsi";
	}

	//      After the storage adapter has been added to the argument list,
	//      the storage device arguments can be appended here without any
	//      (further) problems ...
	Args << StorageArgs;

	// Boot Device
	if( Current_Emulator_Devices.PSO_Boot_Order )
	{
		int bootDevCount = 0;
		int onceBootDeviceIndex = -1;
		for( int ix = 0; ix < Boot_Order_List.count(); ix++ )
		{
			if( Boot_Order_List[ix].Enabled )
			{
				bootDevCount++;
				onceBootDeviceIndex = ix;
			}
		}
		
		if( bootDevCount > 1 )
		{
			QString bootStr = "";
			
			for( int ix = 0; ix < Boot_Order_List.count(); ix++ )
			{
				if( Boot_Order_List[ix].Enabled )
				{
					     if( Boot_Order_List[ix].Type == VM::Boot_From_FDA ) bootStr += "a";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_FDB ) bootStr += "b";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_HDD ) bootStr += "c";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_CDROM ) bootStr += "d";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_Network1 ) bootStr += "n-1";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_Network2 ) bootStr += "n-2";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_Network3 ) bootStr += "n-3";
					else if( Boot_Order_List[ix].Type == VM::Boot_From_Network4 ) bootStr += "n-4";
				}
			}
			
			bootStr.prepend( (bootStr.isEmpty() ? "" : "order=") );
			bootStr += QString(bootStr.isEmpty() ? "" : ",") + "menu=" + QString(Show_Boot_Menu ? "on" : "off");
			
			Args << "-boot" << bootStr;
		}
		else if( onceBootDeviceIndex != -1 )
		{
			QString bootStr = "";
			
			     if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_FDA ) bootStr = "a";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_FDB ) bootStr = "b";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_HDD ) bootStr = "c";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_CDROM ) bootStr = "d";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_Network1 ) bootStr = "n-1";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_Network2 ) bootStr = "n-2";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_Network3 ) bootStr = "n-3";
			else if( Boot_Order_List[onceBootDeviceIndex].Type == VM::Boot_From_Network4 ) bootStr = "n-4";
			
			bootStr.prepend( (bootStr.isEmpty() ? "" : "once=") );
			bootStr += QString(bootStr.isEmpty() ? "" : ",") + "menu=" + QString(Show_Boot_Menu ? "on" : "off");
			
			Args << "-boot" << bootStr;
		}
		else
		{
			AQWarning( "QStringList Virtual_Machine::Build_QEMU_Args()",
					   "No boot devices? Use empty boot settings..." );
		}
	}
	
	// Network Cards
    if( (Use_Native_Network() == false && Network_Cards.count() < 1) ||
        (Use_Native_Network() == true  && Network_Cards_Nativ.count() < 1) ||
		Use_Network == false )
	{
		Args << "-net" << "none";
	}
	else
	{
        if( Use_Native_Network() )
		{
			for( int nc = 0; nc < Network_Cards_Nativ.count(); nc++ )
			{
				QString nic_str = "";
				bool u_vlan, u_macaddr, u_model, u_name, u_hostname, u_port_dev, u_fd, u_ifname, u_script,
					 u_downscript, u_bridge, u_helper, u_listen, u_connect, u_mcast, u_sock, u_port, u_group,
					 u_mode, u_file, u_len, u_addr, u_vectors, u_net, u_host, u_restrict, u_dhcpstart, u_dns,
					 u_tftp, u_bootfile, u_hostfwd, u_guestfwd, u_smb, u_smbserver, u_sndbuf, u_vnet_hdr,
					 u_vhost, u_vhostfd;
				
				u_vlan = u_macaddr = u_model = u_name = u_hostname = u_port_dev = u_fd = u_ifname = u_script =
				u_downscript = u_bridge = u_helper = u_listen = u_connect = u_mcast = u_sock = u_port = u_group =
				u_mode = u_file = u_len = u_addr = u_vectors = u_net = u_host = u_restrict = u_dhcpstart = u_dns =
				u_tftp = u_bootfile = u_hostfwd = u_guestfwd = u_smb = u_smbserver = u_sndbuf = u_vnet_hdr =
				u_vhost = u_vhostfd = false;
				
				switch( Network_Cards_Nativ[nc].Get_Network_Type() )
				{
					// -net nic[,vlan=n][,macaddr=mac][,model=type][,name=str][,addr=str][,vectors=v]
                    case VM::Net_Mode_Native_NIC:
						nic_str += "nic";
						u_vlan = u_macaddr = u_model = u_name = u_addr = u_vectors = true;
						break;
						
					// -net user[,vlan=n][,name=str][,net=addr[/mask]][,host=addr][,restrict=y|n]
					//			[,hostname=host][,dhcpstart=addr][,dns=addr][,tftp=dir][,bootfile=f]
					//			[,hostfwd=rule][,guestfwd=rule][,smb=dir[,smbserver=addr]]
                    case VM::Net_Mode_Native_User:
						nic_str += "user";
						u_vlan = u_name = u_net = u_host = u_restrict = u_hostname = u_dhcpstart = u_dns =
							     u_tftp = u_bootfile = u_hostfwd = u_guestfwd = u_smb = u_smbserver = true;
						break;
						
					// -net channel,port:dev
                    case VM::Net_Mode_Native_Chanel:
						nic_str += "channel";
						u_port_dev = true;
						break;
						
					// -net tap[,vlan=n][,name=str][,fd=h][,ifname=name][,script=file][,downscript=dfile]
                    //                 [,helper=helper][,sndbuf=nbytes][,vnet_hdr=on|off][,vhost=on|off][,vhostfd=h]
                    case VM::Net_Mode_Native_TAP:
						nic_str += "tap";
                        u_vlan = u_name = u_fd = u_ifname = u_script = u_downscript = u_helper = u_sndbuf =
                        u_vnet_hdr = u_vhost = u_vhostfd = true;
						break;
						
					// -net socket[,vlan=n][,name=str][,fd=h][,listen=[host]:port][,connect=host:port]
                    case VM::Net_Mode_Native_Socket:
						nic_str += "socket";
						u_vlan = u_name = u_fd = u_listen = u_connect = true;
						break;
						
					// -net socket[,vlan=n][,name=str][,fd=h][,mcast=maddr:port]
                    case VM::Net_Mode_Native_MulticastSocket:
						nic_str += "socket";
						u_vlan = u_name = u_fd = u_mcast = true;
						break;
						
					// -net vde[,vlan=n][,name=str][,sock=socketpath][,port=n][,group=groupname][,mode=octalmode]
                    case VM::Net_Mode_Native_VDE:
						if( ! Current_Emulator_Devices.PSO_Net_type_vde ) continue;
						nic_str += "vde";
						u_vlan = u_name = u_sock = u_port = u_group = u_mode = true;
						break;
						
					// -net dump[,vlan=n][,file=file][,len=len]
                    case VM::Net_Mode_Native_Dump:
						if( ! Current_Emulator_Devices.PSO_Net_type_dump ) continue;
						nic_str += "dump";
						u_vlan = u_file = u_len = true;
						break;
						
					default:
						break;
				}
				
				// Create String
				if( Network_Cards_Nativ[nc].Use_VLAN() && u_vlan )
					nic_str += ",vlan=" + QString::number( Network_Cards_Nativ[nc].Get_VLAN() );
				
				if( Network_Cards_Nativ[nc].Use_MAC_Address() && u_macaddr )
					nic_str += ",macaddr=" + Network_Cards_Nativ[ nc ].Get_MAC_Address();
				
				if( Network_Cards_Nativ[ nc ].Get_Card_Model().isEmpty() == false && u_model )
					nic_str += ",model=" + Network_Cards_Nativ[ nc ].Get_Card_Model();
				
				if( Network_Cards_Nativ[nc].Use_Name() && u_name && Current_Emulator_Devices.PSO_Net_name )
					nic_str += ",name=\"" + Network_Cards_Nativ[ nc ].Get_Name() + "\"";
				
				if( Network_Cards_Nativ[nc].Use_Hostname() && u_hostname )
					nic_str += ",hostname=" + Network_Cards_Nativ[ nc ].Get_Hostname();
				
				if( u_port_dev )
					nic_str += ",port:" + Network_Cards_Nativ[ nc ].Get_PortDev();
				
				if( Network_Cards_Nativ[nc].Use_File_Descriptor() && u_fd )
					nic_str += ",fd=" + QString::number( Network_Cards_Nativ[nc].Get_File_Descriptor() );
				
				if( Network_Cards_Nativ[nc].Use_Interface_Name() && u_ifname && Current_Emulator_Devices.PSO_Net_ifname )
					nic_str += ",ifname=" + Network_Cards_Nativ[ nc ].Get_Interface_Name();
				
				if( Network_Cards_Nativ[nc].Use_Bridge_Name() && u_bridge && Current_Emulator_Devices.PSO_Net_bridge )
					nic_str += ",br=" + Network_Cards_Nativ[ nc ].Get_Bridge_Name();

				if( u_script && Current_Emulator_Devices.PSO_Net_script )
				{
					QString s_script;
					
					if( Network_Cards_Nativ[nc].Use_TUN_TAP_Script() )
						s_script = Network_Cards_Nativ[ nc ].Get_TUN_TAP_Script();
					else
						s_script = "no";
					
					if( Build_QEMU_Args_for_Script_Mode )
						nic_str += ",script=\"" + s_script + "\"";
					else
						nic_str += ",script=" + s_script;
				}
				
				if( u_downscript && Current_Emulator_Devices.PSO_Net_downscript )
				{
					QString s_downscript;
					
					if( Network_Cards_Nativ[nc].Use_TUN_TAP_Down_Script() )
						s_downscript = Network_Cards_Nativ[ nc ].Get_TUN_TAP_Down_Script();
					else
						s_downscript = "no";
					
					if( Build_QEMU_Args_for_Script_Mode )
						nic_str += ",downscript=\"" + s_downscript + "\"";
					else
						nic_str += ",downscript=" + s_downscript;
				}
				
				if( u_helper && Current_Emulator_Devices.PSO_Net_helper )
				{
					QString s_helper;

					if( Network_Cards_Nativ[nc].Use_Bridge_Helper() )
					{
						s_helper = Network_Cards_Nativ[ nc ].Get_Bridge_Helper();

						if( Build_QEMU_Args_for_Script_Mode )
							nic_str += ",helper=\"" + s_helper + "\"";
						else
							nic_str += ",helper=" + s_helper;
					}
				}

				if( Network_Cards_Nativ[nc].Use_Listen() && u_listen && Current_Emulator_Devices.PSO_Net_listen )
					nic_str += ",listen=" + Network_Cards_Nativ[ nc ].Get_Listen();
				
				if( Network_Cards_Nativ[nc].Use_Connect() && u_connect && Current_Emulator_Devices.PSO_Net_connect )
					nic_str += ",connect=" + Network_Cards_Nativ[ nc ].Get_Connect();
				
				if( Network_Cards_Nativ[nc].Use_MCast() && u_mcast && Current_Emulator_Devices.PSO_Net_mcast )
					nic_str += ",mcast=" + Network_Cards_Nativ[ nc ].Get_MCast();
				
				if( Network_Cards_Nativ[nc].Use_Sock() && u_sock && Current_Emulator_Devices.PSO_Net_sock )
					nic_str += ",sock=" + Network_Cards_Nativ[ nc ].Get_Sock();
				
				if( Network_Cards_Nativ[nc].Use_Port() && u_port && Current_Emulator_Devices.PSO_Net_port )
					nic_str += ",port=" + QString::number( Network_Cards_Nativ[ nc ].Get_Port() );
				
				if( Network_Cards_Nativ[nc].Use_Group() && u_group && Current_Emulator_Devices.PSO_Net_group )
					nic_str += ",group=" + Network_Cards_Nativ[ nc ].Get_Group();
				
				if( Network_Cards_Nativ[nc].Use_Mode() && u_mode && Current_Emulator_Devices.PSO_Net_mode )
					nic_str += ",mode=" + Network_Cards_Nativ[ nc ].Get_Mode();
				
				if( Network_Cards_Nativ[nc].Use_File() && u_file && Current_Emulator_Devices.PSO_Net_file )
				{
					if( Build_QEMU_Args_for_Script_Mode )
						nic_str += ",file=\"" + Network_Cards_Nativ[ nc ].Get_File() + "\"";
					else
						nic_str += ",file=" + Network_Cards_Nativ[ nc ].Get_File();
				}
				
				if( Network_Cards_Nativ[nc].Use_Len() && u_len && Current_Emulator_Devices.PSO_Net_len )
					nic_str += ",len=" + QString::number( Network_Cards_Nativ[nc].Get_Len() );
				
				if( Network_Cards_Nativ[nc].Use_Addr() && u_addr && Current_Emulator_Devices.PSO_Net_addr )
					nic_str += ",addr=" + Network_Cards_Nativ[ nc ].Get_Addr();
				
				if( Network_Cards_Nativ[nc].Use_Vectors() && u_vectors && Current_Emulator_Devices.PSO_Net_vectors )
					nic_str += ",vectors=" + QString::number( Network_Cards_Nativ[ nc ].Get_Vectors() );
				
				if( Network_Cards_Nativ[nc].Use_Net() && u_net && Current_Emulator_Devices.PSO_Net_net )
					nic_str += ",net=" + Network_Cards_Nativ[ nc ].Get_Net();
				
				if( Network_Cards_Nativ[nc].Use_Host() && u_host && Current_Emulator_Devices.PSO_Net_host )
					nic_str += ",host=" + Network_Cards_Nativ[ nc ].Get_Host();
				
				if( Network_Cards_Nativ[nc].Use_Restrict() && u_restrict && Current_Emulator_Devices.PSO_Net_restrict )
					nic_str += ",restrict=" + Network_Cards_Nativ[nc].Get_Restrict() ? "y" : "n";
				
				if( Network_Cards_Nativ[nc].Use_DHCPstart() && u_dhcpstart && Current_Emulator_Devices.PSO_Net_dhcpstart )
					nic_str += ",dhcpstart=" + Network_Cards_Nativ[ nc ].Get_DHCPstart();
				
				if( Network_Cards_Nativ[nc].Use_DNS() && u_dns && Current_Emulator_Devices.PSO_Net_dns )
					nic_str += ",dns=" + Network_Cards_Nativ[ nc ].Get_DNS();
				
				if( Network_Cards_Nativ[nc].Use_Tftp() && u_tftp && Current_Emulator_Devices.PSO_Net_tftp )
				{
					if( Build_QEMU_Args_for_Script_Mode )
						nic_str += ",tftp=\"" + Network_Cards_Nativ[ nc ].Get_Tftp() + "\"";
					else
						nic_str += ",tftp=" + Network_Cards_Nativ[ nc ].Get_Tftp();
				}
				
				if( Network_Cards_Nativ[nc].Use_Bootfile() && u_bootfile && Current_Emulator_Devices.PSO_Net_bootfile )
				{
					if( Build_QEMU_Args_for_Script_Mode )
						nic_str += ",bootfile=\"" + Network_Cards_Nativ[ nc ].Get_Bootfile() + "\"";
					else
						nic_str += ",bootfile=" + Network_Cards_Nativ[ nc ].Get_Bootfile();
				}
				
				if( Network_Cards_Nativ[nc].Use_HostFwd() && u_hostfwd && Current_Emulator_Devices.PSO_Net_hostfwd )
					nic_str += ",hostfwd=" + Network_Cards_Nativ[ nc ].Get_HostFwd();
				
				if( Network_Cards_Nativ[nc].Use_GuestFwd() && u_guestfwd && Current_Emulator_Devices.PSO_Net_guestfwd )
					nic_str += ",guestfwd=" + Network_Cards_Nativ[ nc ].Get_GuestFwd();
				
				if( Network_Cards_Nativ[nc].Use_SMB() && u_smb && Current_Emulator_Devices.PSO_Net_smb )
				{
					if( Build_QEMU_Args_for_Script_Mode )
						nic_str += ",smb=\"" + Network_Cards_Nativ[ nc ].Get_SMB() + "\"";
					else
						nic_str += ",smb=" + Network_Cards_Nativ[ nc ].Get_SMB();
					
					if( Network_Cards_Nativ[nc].Use_SMBserver() && u_smbserver )
						nic_str += ",smbserver=" + Network_Cards_Nativ[ nc ].Get_SMBserver();
				}
				
				if( Network_Cards_Nativ[nc].Use_Sndbuf() && u_sndbuf && Current_Emulator_Devices.PSO_Net_sndbuf )
					nic_str += ",sndbuf=" + QString::number( Network_Cards_Nativ[nc].Get_Sndbuf() );
				
				if( Network_Cards_Nativ[nc].Use_VNet_hdr() && u_vnet_hdr && Current_Emulator_Devices.PSO_Net_vnet_hdr )
					nic_str += ",vnet_hdr=" + Network_Cards_Nativ[ nc ].Get_VNet_hdr() ? "on" : "off";
				
				if( Network_Cards_Nativ[nc].Get_VHost() && u_vhost && Current_Emulator_Devices.PSO_Net_vhost )
					nic_str += ",vhost=" + Network_Cards_Nativ[ nc ].Get_VHost() ? "on" : "off";
				
				if( Network_Cards_Nativ[nc].Use_VHostFd() && u_vhostfd && Current_Emulator_Devices.PSO_Net_vhostfd )
					nic_str += ",vhostfd=" + QString::number( Network_Cards_Nativ[nc].Get_VHostFd() );
				
				// Add to Args
				Args << "-net";
				Args << nic_str;
			}
		}
		else
		{
			for( int nc = 0; nc < Network_Cards.count(); nc++ )
			{
				Args << "-net";
				QString nic_str = "nic,vlan=" + QString::number( Network_Cards[nc].Get_VLAN() );
				
				if( ! Network_Cards[nc].Get_MAC_Address().isEmpty() ) // Use MAC?
					nic_str += ",macaddr=" + Network_Cards[nc].Get_MAC_Address();
				
				if( ! Network_Cards[nc].Get_Card_Model().isEmpty() )
					nic_str += ",model=" + Network_Cards[nc].Get_Card_Model();
				
				Args << nic_str;
				
				// Net Modes
				QString tap_tmp;
				
				switch( Network_Cards[nc].Get_Net_Mode() )
				{
					case VM::Net_Mode_None:
						break;
					
					case VM::Net_Mode_Usermode:
						if( Network_Cards[nc].Get_Hostname().isEmpty() )
							Args << "-net" << QString( "user,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) );
						else
							Args << "-net" << QString( "user,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",hostname=" + Network_Cards[nc].Get_Hostname() );
						break;
					
					case VM::Net_Mode_Tuntap:
						Args << "-net" ;
						
						tap_tmp = QString( "tap,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) );
						
						if( ! Network_Cards[nc].Get_Interface_Name().isEmpty() )
							tap_tmp += QString( ",ifname=" + Network_Cards[nc].Get_Interface_Name() );
						
						if( ! Network_Cards[nc].Get_Use_TUN_TAP_Script() )
						{
							tap_tmp += QString( ",script=no" );
						}
						else
						{
							if( ! Network_Cards[nc].Get_TUN_TAP_Script().isEmpty() )
							{
								if( Build_QEMU_Args_for_Script_Mode )
									tap_tmp += QString( ",script=\"%1\"" ).arg( Network_Cards[nc].Get_TUN_TAP_Script() );
								else
									tap_tmp += ",script=" + Network_Cards[nc].Get_TUN_TAP_Script();
							}
						}
						
						Args << tap_tmp;
						break;
						
					case VM::Net_Mode_Tuntapfd:
						Args << "-net" << QString( "tap,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
												   ((Network_Cards[nc].Get_File_Descriptor() > 0) ? ",fd=" + QString::number(Network_Cards[nc].Get_File_Descriptor()) : "") +
												   ((Network_Cards[nc].Get_Interface_Name().isEmpty() == false) ? ",ifname=" + Network_Cards[nc].Get_Interface_Name() : "") );
						break;
						
					case VM::Net_Mode_Tcplisten:
						if( Network_Cards[nc].Get_IP_Address().isEmpty() )
						{
							Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",listen=:" + QString::number(Network_Cards[nc].Get_Port()) );
						}
						else
						{
							Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",listen=" + Network_Cards[nc].Get_IP_Address() + ":" +
									QString::number(Network_Cards[nc].Get_Port()) );
						}
						break;
						
					case VM::Net_Mode_Tcpfd:
						Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
								",fd=" + QString::number(Network_Cards[nc].Get_File_Descriptor()) );
						break;
						
					case VM::Net_Mode_Tcpconnect:
						if( Network_Cards[nc].Get_IP_Address().isEmpty() )
						{
							Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",connect=:" + QString::number(Network_Cards[nc].Get_Port()) );
						}
						else
						{
							Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",connect=" + Network_Cards[nc].Get_IP_Address() +
									":" + QString::number(Network_Cards[nc].Get_Port()) );
						}
						break;
						
					case VM::Net_Mode_Multicast:
						if( Network_Cards[nc].Get_IP_Address().isEmpty() )
						{
							Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",mcast=:" + QString::number(Network_Cards[nc].Get_Port()) );
						}
						else
						{
							Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
									",mcast=" + Network_Cards[nc].Get_IP_Address() +
									":" + QString::number(Network_Cards[nc].Get_Port()) );
						}
						break;
						
					case VM::Net_Mode_Multicastfd:
						Args << "-net" << QString( "socket,vlan=" + QString::number(Network_Cards[nc].Get_VLAN()) +
								",fd=" + QString::number(Network_Cards[nc].Get_File_Descriptor()) );
						break;
						
					default:
						Args << "-net" << "none";
						break;
				}
			}
		}
		
		// Network Tab. Redirections
		if( Use_Redirections )
		{
			QString redir_str = "";
			Args << "-net";

            QStringList hostfwds;

			for( int rx = 0; rx < Get_Network_Redirections_Count(); rx++ )
			{
				redir_str = "hostfwd=";
				
				if( Get_Network_Redirection(rx).Get_Protocol() == "TCP" )
					redir_str += "tcp:";
				else if( Get_Network_Redirection(rx).Get_Protocol() == "UDP" )
					redir_str += "udp:";
				else
					redir_str += ":"; //qemu uses TCP by default


                //FIXME/TODO: host_address doesn't get taken into account
				
				redir_str += ":" + QString::number( Get_Network_Redirection(rx).Get_Host_Port() ) + "-";
				redir_str += Get_Network_Redirection(rx).Get_Guest_IP() + ":";
				redir_str += QString::number( Get_Network_Redirection(rx).Get_Guest_Port() );
				
				hostfwds << redir_str;
			}

            Args << "user," + hostfwds.join(",");
		}
	}
	
	// TFTP Prefix
	if( ! TFTP_Prefix.isEmpty() )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			Args << "-net" << "user,tftp=" + QString("\"") + TFTP_Prefix + "\"";
		else
			Args << "-net" << "user,tftp=" + TFTP_Prefix;
	}
	
	// SMB Dir
	if( ! SMB_Directory.isEmpty() )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			Args << "-net" << "user,smb=" + QString("\"") + SMB_Directory + "\"";
		else
			Args << "-net" << "user,smb=" + SMB_Directory;
	}
	
	// Ports Tabs
	for( int ix = 0; ix < Serial_Ports.count(); ix++ )
	{
		if( Serial_Ports[ix].Get_Port_Redirection() == VM::PR_Default ) continue;
		
		Args << "-serial";
		
		switch( Serial_Ports[ix].Get_Port_Redirection() )
		{
			case VM::PR_vc:
				Args << "vc:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_pty:
				Args << "pty";
				break;
				
			case VM::PR_none:
				Args << "none";
				break;
				
			case VM::PR_null:
				Args << "null";
				break;
				
			case VM::PR_dev:
				Args << Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_file:
				Args << "file:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_stdio:
				Args << "stdio";
				break;
				
			case VM::PR_pipe:
				Args << "pipe:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_udp:
				Args << "udp:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_tcp:
				Args << "tcp:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_telnet:
				Args << "telnet:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_unix:
				Args << "unix:" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_com:
				Args << "COM" + Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_msmouse:
				Args << "msmouse";
				break;
				
			case VM::PR_mon:
				Args << "mon:" << Serial_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_braille:
				Args << "braille";
				break;
				
			default:
				break;
		}
	}
	
	// parallel port
	for( int ix = 0; ix < Parallel_Ports.count(); ix++ )
	{
		if( Parallel_Ports[ix].Get_Port_Redirection() == VM::PR_Default ) continue;
		
		Args << "-parallel";
		
		switch( Parallel_Ports[ix].Get_Port_Redirection() )
		{
			case VM::PR_vc:
				Args << "vc:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_pty:
				Args << "pty";
				break;
				
			case VM::PR_none:
				Args << "none";
				break;
				
			case VM::PR_null:
				Args << "null";
				break;
				
			case VM::PR_dev:
				Args << Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_file:
				Args << "file:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_stdio:
				Args << "stdio";
				break;
				
			case VM::PR_pipe:
				Args << "pipe:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_udp:
				Args << "udp:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_tcp:
				Args << "tcp:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_telnet:
				Args << "telnet:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_unix:
				Args << "unix:" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_com:
				Args << "COM" + Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_msmouse:
				Args << "msmouse";
				break;
				
			case VM::PR_mon:
				Args << "mon:" << Parallel_Ports[ix].Get_Parametrs_Line();
				break;
				
			case VM::PR_braille:
				Args << "braille";
				break;
				
			default:
				break;
		}
	}
	
	// USB
	if( ! System_Info::Update_Host_USB() ) // check USB support
	{
		AQDebug( "QStringList Virtual_Machine::Build_QEMU_Args()",
				 "No USB Support in This System!" );
	}
	else
	{
		if( USB_Ports.count() > 0 )
		{
			bool usb_ehci_arg_added = false; // USB 2.0 controller
			bool usb_xhci_arg_added = false; // USB 3.0 controller
			
			Args << "-usb";
			
			if( Build_QEMU_Args_for_Tab_Info == false ) System_Info::Update_Host_USB();
			QList<VM_USB> all_usb = System_Info::Get_All_Host_USB();
			
			// Add usb
			for( int ux = 0; ux < USB_Ports.count(); ux++ )
			{
				if( USB_Ports[ux].Get_Use_Host_Device() )
				{
					// Compare VM USB device and Host USB Device
					// Find device by Vendor and Product ID's
					bool usb_cmpr = false;
					VM_USB current_USB_Device;
					
					for( int ix = 0; ix < all_usb.count(); ix++ )
					{
						if( all_usb[ix].Get_Vendor_ID() == USB_Ports[ux].Get_Vendor_ID() &&
							all_usb[ix].Get_Product_ID() == USB_Ports[ux].Get_Product_ID() &&
							all_usb[ix].Get_Serial_Number() == USB_Ports[ux].Get_Serial_Number() )
						{
							usb_cmpr = true;
							current_USB_Device = all_usb[ ix ];
							all_usb.removeAt( ix );
							break;
						}
					}
					
					// Error! Not Found
					if( Build_QEMU_Args_for_Tab_Info == false && usb_cmpr == false )
					{
						AQGraphic_Warning( tr("Warning!"),
										   tr("USB Device %1 %2 (%3 %4) Not Found!").arg(USB_Ports[ux].Get_Manufacturer_Name())
																					.arg(USB_Ports[ux].Get_Product_Name())
																					.arg(USB_Ports[ux].Get_Vendor_ID())
																					.arg(USB_Ports[ux].Get_Product_ID()) );
						
						continue;
					}
					
					/*
							ui.RB_USB_Style_device->isChecked() )
								Settings.setValue( "USB_Style", "device" );
							else
								Settings.setValue( "USB_Style", "usbdevice" );
							
							if( ui.RB_USB_ID_BusAddr->isChecked() )
								Settings.setValue( "USB_ID_Style", "BusAddr" );
							else if( ui.RB_USB_ID_BusPath->isChecked() )
								Settings.setValue( "USB_ID_Style", "BusPath" );
							else if( ui.RB_USB_ID_VendorProduct->isChecked() )
								Settings.setValue( "USB_ID_Style", "VendorProduct" );
					 */
					
					if( Current_Emulator_Devices.PSO_Device == false ||
						Settings.value("USB_Style", "device").toString() == "usbdevice" )
					{
						if( Settings.value("USB_ID_Style","").toString() == "BusAddr" )
							Args << "-usbdevice" << QString( "host:%1.%2" ).arg( current_USB_Device.Get_Bus() ).arg( current_USB_Device.Get_Addr() );
						else if( Settings.value("USB_ID_Style","").toString() == "VendorProduct" )
							Args << "-usbdevice" << QString( "host:%1:%2" ).arg( current_USB_Device.Get_Vendor_ID() ).arg( current_USB_Device.Get_Product_ID() );
						else
							Args << "-usbdevice" << QString( "host:%1.%2" ).arg( current_USB_Device.Get_Bus() ).arg( current_USB_Device.Get_DevPath() );
					}
					else
					{
						QString usbControllerID = "";
						
						// Add USB 3.0 controller if need
						if( Current_Emulator_Devices.PSO_Device_USB_EHCI &&
							current_USB_Device.Get_Speed().toInt() == 5000 )
						{
							if( ! usb_xhci_arg_added )
							{
								// Only needed once
								Args << "-device" << "nec-usb-xhci,id=xhci";
								usb_xhci_arg_added = true;
							}
							
							usbControllerID = "xhci.0";
						}
						// Add USB 2.0 controller if need
						else if( Current_Emulator_Devices.PSO_Device_USB_EHCI &&
								 current_USB_Device.Get_Speed().toInt() >= 480 )
						{
							if( ! usb_ehci_arg_added )
							{
								// Only needed once
								Args << "-device" << "usb-ehci,id=ehci";
								usb_ehci_arg_added = true;
							}
							
							usbControllerID = "ehci.0";
						}
						else // USB 1.1
						{
							usbControllerID = "usb.0";
						}
						
						
						// Add USB devices
						if( Settings.value("USB_ID_Style","").toString() == "BusAddr" )
						{
							Args << "-device"
								 << QString( "usb-host,bus=%1,hostbus=%2,hostaddr=%3" )
									.arg( usbControllerID )
									.arg( current_USB_Device.Get_Bus() )
									.arg( current_USB_Device.Get_Addr() );
						}
						else if( Settings.value("USB_ID_Style","").toString() == "VendorProduct" )
						{
							Args << "-device"
								 << QString( "usb-host,bus=%1,vendorid=%2,productid=%3" )
									.arg( usbControllerID )
									.arg( current_USB_Device.Get_Vendor_ID() )
									.arg( current_USB_Device.Get_Product_ID() );
						}
						else // Bus.Path
						{
							Args << "-device"
								 << QString( "usb-host,bus=%1,hostbus=%2,hostport=%3" )
									.arg( usbControllerID )
									.arg( current_USB_Device.Get_Bus() )
									.arg( current_USB_Device.Get_DevPath() );
						}
					}
				}
				else
				{
					// QEMU USB Devices
					bool usb_k, usb_m, usb_t, usb_wt, usb_b;
					usb_k = usb_m = usb_t = usb_wt = usb_b = false;
					USB_Ports[ux].Get_USB_QEMU_Devices( usb_k, usb_m, usb_t, usb_wt, usb_b );
					
					if( usb_k ) Args << "-usbdevice" << "keyboard";
					else if( usb_m ) Args << "-usbdevice" << "mouse";
					else if( usb_t ) Args << "-usbdevice" << "tablet";
					else if( usb_wt ) Args << "-usbdevice" << "wacom-tablet";
					else if( usb_b ) Args << "-usbdevice" << "braille";
					else
					{
						AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
								 "Incorrcect USB Device!" );
					}
				}
			}
		}
	}
	
	// Other Tab
	if( Linux_Boot )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			Args << "-kernel" << "\"" + bzImage_Path + "\"";
		else
			Args << "-kernel" << bzImage_Path;
		
		if( ! Initrd_Path.isEmpty() )
		{
			if( Build_QEMU_Args_for_Script_Mode )
				Args << "-initrd" << "\"" + Initrd_Path + "\"";
			else
				Args << "-initrd" << Initrd_Path;
		}
		
		if( ! Kernel_ComLine.isEmpty() )
		{
			if( Build_QEMU_Args_for_Script_Mode )
				Args << "-append" << "\"" + Kernel_ComLine + "\"";
			else
				Args << "-append" << Kernel_ComLine;
		}
	}
	
	// Use ROM File
	if( Use_ROM_File )
	{
		if( QFile::exists(ROM_File) == false &&
			Build_QEMU_Args_for_Tab_Info == false )
		{
			AQGraphic_Warning( tr("Error!"), tr("ROM File doesn't Exists! Can't run emulator!") );
		}
		else
		{
			if( Build_QEMU_Args_for_Script_Mode )
				Args << "-option-rom" << "\"" + ROM_File + "\"";
			else
				Args << "-option-rom" << ROM_File;
		}
	}
	
	// Start Date
	// Localtime
    QStringList rtc_list;

    rtc_list << "-rtc";
	if ( Local_Time )
		rtc_list << "base=localtime";
	else if ( Start_Date )
		rtc_list << "base=" + Start_DateTime.toString( "yyyy-MM-ddTHH:mm:ss" ); // QEMU Format
    else
        rtc_list << "base=utc";
	if( Current_Emulator_Devices.PSO_RTC_TD_Hack && RTC_TD_Hack )
	    rtc_list.last() += ",driftfix=slew";

    Args << rtc_list;

	// QEMU 0.9.1 Options
	// on-board Flash memory image
	if( Current_Emulator_Devices.PSO_MTDBlock && MTDBlock )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			Args << "-mtdblock" << "\"" + MTDBlock_File + "\"";
		else
			Args << "-mtdblock" << MTDBlock_File;
	}
	
	// SecureDigital card image
	if( Current_Emulator_Devices.PSO_SD && SecureDigital )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			Args << "-sd" << "\"" + SecureDigital_File + "\"";
		else
			Args << "-sd" << SecureDigital_File;
	}
	
	// parallel flash image
	if( Current_Emulator_Devices.PSO_PFlash && PFlash )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			Args << "-pflash" << "\"" + PFlash_File + "\"";
		else
			Args << "-pflash" << PFlash_File;
	}
	
	// Set the initial graphical resolution and depth
	if( Current_Emulator_Devices.PSO_Initial_Graphic_Mode &&
		Init_Graphic_Mode.Get_Enabled() )
	{
		Args << "-g" << QString::number( Init_Graphic_Mode.Get_Width() ) + "x" +
						QString::number( Init_Graphic_Mode.Get_Height() ) + "x" +
						QString::number( Init_Graphic_Mode.Get_Depth() );
	}
	
	// open SDL window without a frame and window decorations
	if( Current_Emulator_Devices.PSO_No_Frame && No_Frame )
		Args << "-no-frame";
	
	// use Ctrl-Alt-Shift to grab mouse (instead of Ctrl-Alt)
	if( Current_Emulator_Devices.PSO_Alt_Grab && Alt_Grab )
		Args << "-alt-grab";
	
	// disable SDL window close capability
	if( Current_Emulator_Devices.PSO_No_Quit && No_Quit )
		Args << "-no-quit";
	
	// rotate graphical output 90 deg left (only PXA LCD)
	if( Current_Emulator_Devices.PSO_Portrait && Portrait )
		Args << "-portrait";
	
	// VM Name
	if( Current_Emulator_Devices.PSO_Name )
		Args << "-name" << "\"" + Machine_Name + "\"";
	
	// SPICE
	// FIXME. VNC and SPICE together?
	if( SPICE.Use_SPICE() )
	{
		// QLX devices count and RAM size
		if( Current_Emulator_Devices.PSO_QXL )
			Args << "-qxl" << QString( "%1,ram=%2" ).arg( SPICE.Get_GXL_Devices_Count() ).arg( SPICE.Get_RAM_Size() );
		
		// Basic SPICE options
		QStringList spiceArgs;
		
		spiceArgs << QString( "port=%1" ).arg( SPICE.Get_Port() );
		
		if( SPICE.Use_SPort() )
			spiceArgs << QString( "tls-port=%1" ).arg( SPICE.Get_SPort() );
		
		if( SPICE.Use_Hostname() )
			spiceArgs << QString( "addr=%1" ).arg( SPICE.Get_Hostname() );
		
		// Image, video & audio options
		if( SPICE.Use_Image_Compression() )
		{
			switch( SPICE.Get_Image_Compression() )
			{
				case VM::SPICE_IC_Type_on:
					spiceArgs << "image-compression=on";
					break;
					
				case VM::SPICE_IC_Type_auto_glz:
					spiceArgs << "image-compression=auto_glz";
					break;
					
				case VM::SPICE_IC_Type_auto_lz:
					spiceArgs << "image-compressionc=auto_lz";
					break;
					
				case VM::SPICE_IC_Type_quic:
					spiceArgs << "image-compression=quic";
					break;
					
				case VM::SPICE_IC_Type_glz:
					spiceArgs << "image-compression=glz";
					break;
					
				case VM::SPICE_IC_Type_lz:
					spiceArgs << "image-compression=lz";
					break;
					
				case VM::SPICE_IC_Type_off:
					spiceArgs << "image-compression=off";
					break;
					
				default:
					AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
							 "SPICE image compression type invalid!" );
					break;
			}
		}
		
		// Set video streams detection and (lossy) compression (default=on)
		if( ! SPICE.Use_Video_Stream_Compression() )
			spiceArgs << "streaming-video=off";
		
		// Select renderers. Multiple choice prioritized by order (default=cairo)		
		if( SPICE.Use_Renderer() )
		{
			QString spiceRendersStr = "";
			for( int ix = 0; ix < SPICE.Get_Renderer_List().count(); ++ix )
			{
				switch( SPICE.Get_Renderer_List()[ix] )
				{
					case VM::SPICE_Renderer_oglpbuf:
						spiceRendersStr += "oglpbuf";
						break;
						
					case VM::SPICE_Renderer_oglpixmap:
						spiceRendersStr += "oglpixmap";
						break;
						
					case VM::SPICE_Renderer_cairo:
						spiceRendersStr += "cairo";
						break;
						
					default:
						AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
								 "SPICE render type invalid!" );
						break;
				}
				
				if( ! spiceRendersStr.isEmpty() &&
					ix != SPICE.Get_Renderer_List().count() -1 )
				{
					spiceRendersStr += "+";
				}
			}
			
			if( spiceRendersStr.isEmpty() )
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
						 "SPICE render type order list is empty!" );
			}
			else
			{
				spiceArgs << "renderer=" + spiceRendersStr;
			}
		}
		
		// Set playback compression, using the CELT algorithm (default=on)
		if( ! SPICE.Use_Playback_Compression() )
			spiceArgs << "playback-compression=off";
		
		// Security options
		if( SPICE.Use_Password() )
		{
			spiceArgs << "password=" + SPICE.Get_Password();
		}
		else
		{
			spiceArgs << "disable-ticketing";
		}
		
		// Add to args
		Args << "-spice" << spiceArgs.join( "," );
	}
	
	// VNC
	if( VNC )
	{
		Args << "-vnc";
		
		QString vnc_args = "";
		
		if( VNC_Socket_Mode )
			vnc_args += "unix:" + VNC_Unix_Socket_Path;
		else
			vnc_args += ":" + QString::number( VNC_Display_Number );
		
		if( VNC_Password )
			vnc_args += ",password";
		
		if( VNC_TLS )
		{
			vnc_args += ",tls";
			
			if( VNC_x509 )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					vnc_args += QString( ",x509=\"%1\"" ).arg( VNC_x509_Folder_Path );
				else
					vnc_args += ",x509=" + VNC_x509_Folder_Path;
			}
			
			if( VNC_x509verify )
			{
				if( Build_QEMU_Args_for_Script_Mode )
					vnc_args += QString( ",x509verify=\"%1\"" ).arg( VNC_x509verify_Folder_Path );
				else
					vnc_args += ",x509verify=" + VNC_x509verify_Folder_Path;
			}
		}
		
		Args << vnc_args;
	}
	else
	{
		#ifdef VNC_DISPLAY
		if( Settings.value("Use_VNC_Display", "no").toString() == "yes" )
		{
			if( Embedded_Display_Port >= 0 )
			{
				int port = (Settings.value( "First_VNC_Port", "5910" ).toString().toInt() - 5900) + Embedded_Display_Port;
				
				if( port >= 1 && port < 59636 )
					Args << "-vnc" << ":" + QString::number( port );
				else
					AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
							 "Port for Embedded Display > 1 or < 59636" );
			}
			else
			{
				AQError( "QStringList Virtual_Machine::Build_QEMU_Args()",
						 "Embedded_Display_Port < 0" );
			}
		}
		#endif
	}
	
	// Additional Args...
	if( Only_User_Args )
	{
		Args.clear();
		
		if( Additional_Args.isEmpty() && Build_QEMU_Args_for_Tab_Info == false )
			AQGraphic_Warning( tr("Error!"), tr("Additional Arguments is Empty! Cannot Use User Arguments!") );
	}
	
	if( ! Additional_Args.isEmpty() )
	{
		QString tmp_str = Additional_Args;
		tmp_str.replace( "\n", " " );
		QStringList ad_args = tmp_str.split( " ", QString::SkipEmptyParts );
		
		for( int ix = 0; ix < ad_args.count(); ix++ )
			Args << ad_args[ ix ];
	}
	
	// Set All Special Modes Off
	Build_QEMU_Args_for_Script_Mode = false;
	Build_QEMU_Args_for_Tab_Info = false;
	
	return Args;
}

QStringList Virtual_Machine::Build_QEMU_Args_For_Tab_Info()
{
    Build_QEMU_Args_for_Script_Mode = true;
	Build_QEMU_Args_for_Tab_Info = true;
	return Build_QEMU_Args();
}

QStringList Virtual_Machine::Build_QEMU_Args_For_Script()
{
	Build_QEMU_Args_for_Script_Mode = true;
	return Build_QEMU_Args();
}

//      Build_Native_Device_Args now has an additional variable - an index.
//      This variable is (together with a short string) concatenated to the
//      device-id of the storage device image. The resulting name (aqcd<N> or
//      aqhd<N>) is used for concatenating the device-definition arguments
//      with the drive-image arguments, e.g.
//         -device scsi-hd,driv=aqhd3
//      with
//         -drive file=/path/to/image,if=none,id=aqhd3
// ...
QStringList Virtual_Machine::Build_Native_Device_Args( VM_Native_Storage_Device device, bool Build_QEMU_Args_for_Script_Mode )
{
	QStringList opt;

	QString vsname = (device.Get_Media() == VM::DM_CD_ROM ? "aqcd" : "aqhd")
               + QString::number (native_device_count++);

	// File
	if( device.Use_File_Path() )
	{
		if( Build_QEMU_Args_for_Script_Mode )
			opt << "file=\"" + device.Get_File_Path() + "\"";
		else
			opt << "file=" + device.Get_File_Path();
	}
	
	// Interface
	if( device.Use_Interface() )
	{
		switch( device.Get_Interface() )
		{
			case VM::DI_IDE:
				opt << "if=ide";
				break;
			
			case VM::DI_SCSI:
				opt << "if=scsi";
				break;
				
			case VM::DI_SD:
				opt << "if=sd";
				break;
				
			case VM::DI_MTD:
				opt << "if=mtd";
				break;
				
			case VM::DI_Floppy:
				opt << "if=floppy";
				break;
				
			case VM::DI_PFlash:
				opt << "if=pflash";
				break;
				
			case VM::DI_Virtio:
				opt << "if=virtio";
				break;

			case VM::DI_Virtio_SCSI:
				opt << "if=none,id=" + vsname;
				break;
				
			default:
                AQError( "QStringList Virtual_Machine::Build_Native_Device_Args( VM_Native_Storage_Device device, bool Build_QEMU_Args_for_Script_Mode )",
						 "Storage Device Interface Default Section!" );
				break;
		}
	}
	
	// Bus, Unit
	if( device.Use_Bus_Unit() )
	{
		opt << "bus=" + QString::number( device.Get_Bus() );
		opt << "unit=" + QString::number( device.Get_Unit() );
	}
	
	// Index
	if( device.Use_Index() )
		opt << "index=" + QString::number( device.Get_Index() );
	
	// Media
	if( device.Use_Media() )
	{
		switch( device.Get_Media() )
		{
			case VM::DM_Disk:
				opt << "media=disk";
				break;
				
			case VM::DM_CD_ROM:
				opt << "media=cdrom";
				break;
				
			default:
                AQError( "QStringList Virtual_Machine::Build_Native_Device_Args( VM_Native_Storage_Device device, bool Build_QEMU_Args_for_Script_Mode )",
						 "Storage Device Media Default Section!" );
				break;
		}
	}
	
	// hdachs
	if( device.Use_hdachs() )
	{
		opt << "cyls=" + QString::number( device.Get_Cyls() );
		opt << "heads=" + QString::number( device.Get_Heads() );
		opt << "secs=" + QString::number( device.Get_Secs() );
		opt << "trans=" + QString::number( device.Get_Trans() );
	}
	
	// Snapshot
	if( device.Use_Snapshot() )
	{
		if( device.Get_Snapshot() ) opt << "snapshot=on";
		else opt << "snapshot=off";
	}
	
	// Cache
	if( device.Use_Cache() )
		opt << "cache=" + device.Get_Cache();
	
	// AIO
	if( device.Use_AIO() )
		opt << "aio=" + device.Get_AIO();
	
	// Boot
	if( device.Use_Boot() )
	{
		if( device.Get_Boot() ) opt << "boot=on";
		else opt << "boot=off";
	}
	

	// Discard
	if( device.Use_Discard() )
	{
	        if( device.Get_Discard() ) opt << "discard=unmap";
	        else opt << "discard=ignore";
	}

	// Create complete drive string
	QString driveStr = "";
	for( int ox = 0; ox < opt.count(); ++ox )
	{
		driveStr += opt[ox] + ((ox < opt.count() -1) ? "," : "");
	}
	
	// return
	QStringList args;
	if (device.Get_Interface() == VM::DI_Virtio_SCSI) {
	    QString devtype =
		(device.Get_Media() == VM::DM_CD_ROM ? "scsi-cd" : "scsi-hd");
	    args << "-device" << devtype + ",drive=" + vsname;
	}
	args << "-drive" << driveStr;
	return args;
}

QStringList Virtual_Machine::Build_Shared_Folder_Args( VM_Shared_Folder folder, int id, bool Build_QEMU_Args_for_Script_Mode )
{
	QStringList opt;
	
    QString path = folder.Get_Folder();

    //    -virtfs fsdriver,id=[id],path=[path to share],security_model=[mapped|passthrough|none][,writeout=writeout][,readonly]
    // [,socket=socket|sock_fd=sock_fd],mount_tag=[mount tag]

    opt << "-virtfs";
 
    QStringList virtfs;

    QString driver = "local";    

    virtfs << driver+",id="+QString("shared_folder_dev_")+QString::number(id);

    virtfs << "path="+path;

    virtfs << "security_model=none";

    virtfs << "mount_tag=shared"+QString::number(id);

    opt << virtfs.join(",");

    return opt;
}

bool Virtual_Machine::Start_impl()
{

    delete QEMU_Error_Win;
    QEMU_Error_Win = new Error_Log_Window();

    // Check KVM
    if( (Current_Emulator_Devices.PSO_KVM || Current_Emulator_Devices.PSO_Enable_KVM ) &&
        Settings.value("Disable_KVM_Module_Check", "no").toString() != "yes" )
    {
        QProcess lsmod;

        lsmod.start( "lsmod" );

        if( ! lsmod.waitForFinished(1000) )
        {
            AQError( "bool Virtual_Machine::Start()", "lsmod not finished!" );
        }
        else
        {
            QString all_mod = lsmod.readAll();

            if( all_mod.isEmpty() )
            {
                AQError( "bool Virtual_Machine::Start()", "all_mod is empty!" );
            }
            else
            {
                bool kvm_ok = false;

                // Version Using -
                QRegExp kvm_intel_mod = QRegExp( "*kvm-intel*" );
                kvm_intel_mod.setPatternSyntax( QRegExp::Wildcard );

                QRegExp kvm_amd_mod = QRegExp( "*kvm-amd*" );
                kvm_amd_mod.setPatternSyntax( QRegExp::Wildcard );

                if( kvm_intel_mod.exactMatch(all_mod) ) kvm_ok = true;
                else if( kvm_amd_mod.exactMatch(all_mod) ) kvm_ok = true;

                // Version Using _
                kvm_intel_mod = QRegExp( "*kvm_intel*" );
                kvm_intel_mod.setPatternSyntax( QRegExp::Wildcard );

                kvm_amd_mod = QRegExp( "*kvm_amd*" );
                kvm_amd_mod.setPatternSyntax( QRegExp::Wildcard );

                if( kvm_intel_mod.exactMatch(all_mod) ) kvm_ok = true;
                else if( kvm_amd_mod.exactMatch(all_mod) ) kvm_ok = true;

                if( ! kvm_ok )
                {
                    // Module not found... KVM compiled into kernel?
                    QProcess zcat;

                    zcat.start( "zcat", QStringList("/proc/config.gz") );

                    if( ! zcat.waitForFinished(1000) )
                    {
                        AQError( "bool Virtual_Machine::Start()", "zcat not finished!" );
                    }
                    else
                    {
                        QString all_config = zcat.readAll();

                        if( all_config.isEmpty() )
                        {
                            AQError( "bool Virtual_Machine::Start()", "all_config is empty!" );
                        }
                        else
                        {
                            QRegExp kvm_intel_conf = QRegExp( "*CONFIG_KVM_INTEL=y*" );
                            kvm_intel_conf.setPatternSyntax( QRegExp::Wildcard );
                            if( kvm_intel_conf.exactMatch(all_config) ) kvm_ok = true;
                            else
                            {
                                QRegExp kvm_amd_conf = QRegExp( "*CONFIG_KVM_AMD=y*" );
                                kvm_amd_conf.setPatternSyntax( QRegExp::Wildcard );
                                if( kvm_amd_conf.exactMatch(all_config) ) kvm_ok = true;
                            }
                        }
                    }
                }

                if( ! kvm_ok )
                {
                    int retVal = QMessageBox::question( NULL, tr("Error!"),
                                           tr("KVM Kernel Module Not Loaded!\n"
                                           "To load this Module, Enter in Terminal with root privileges: \"modprobe kvm-intel\". "
                                           "Or If Use AMD Processor: \"modprobe kvm-amd\".\nIgnore This Error?"),
                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll,
                                           QMessageBox::No );

                    if( retVal == QMessageBox::No )
                    {
                        Start_Snapshot_Tag = "";
                        return false;
                    }
                    else if( retVal == QMessageBox::YesToAll )
                    {
                        Settings.setValue( "Disable_KVM_Module_Check", "yes" );
                    }
                }
                else
                {
                    AQDebug( "bool Virtual_Machine::Start()", "OK. KVM Kernel Module Loaded" );
                }
            }
        }
    }

    // QEMU Audio Environment
    if( Settings.value("QEMU_AUDIO/Use_Default_Driver", "yes").toString() == "no" )
    {
        QStringList tmp_env = QProcess::systemEnvironment();
        tmp_env << "QEMU_AUDIO_DRV=" + Settings.value("QEMU_AUDIO/QEMU_AUDIO_DRV", "alsa").toString();
        QEMU_Process->setEnvironment( tmp_env );
    }

    // User Args Only
    if( Use_User_Emulator_Binary && Only_User_Args )
    {
        QStringList tmp_list = this->Build_QEMU_Args();

        if( tmp_list.count() < 1 )
        {
            AQError( "bool Virtual_Machine::Start()", "Cannot Start! Args is Empty!" );
        }
        else
        {
            QString bin_name = tmp_list.takeAt( 0 );
            QEMU_Process->start( bin_name, tmp_list );
        }
    }
    else
    {
        // Get bin path
        QMap<QString, QString> bin_list = Current_Emulator.Get_Binary_Files();
        QString find_name = Current_Emulator_Devices.System.QEMU_Name;
        QString bin_path = "";

        for( QMap<QString, QString>::const_iterator iter = bin_list.constBegin(); iter != bin_list.constEnd(); iter++ )
        {
            if( iter.key() == find_name ||
                (find_name == "qemu-system-x86" && iter.key() == "qemu")) // FIXME
            {
                bin_path = iter.value();
                break;
            }
        }

        // Check path
        if( bin_path.isEmpty() )
        {
            AQGraphic_Error( "bool Virtual_Machine::Start()", tr("Error!"),
                             tr("Cannot start emulator! Binary path is empty!"), false );
            Start_Snapshot_Tag = "";
            return false;
        }

        if( ! QFile::exists(bin_path) )
        {
            AQGraphic_Error( "bool Virtual_Machine::Start()", tr("Error!"),
                             tr("Emulator binary not exists! Check path: %1").arg(bin_path), false );
            Start_Snapshot_Tag = "";
            return false;
        }

        // Add VM USB devices to used USB list
        if( USB_Ports.count() > 0 )
        {
            foreach( VM_USB usb_dev, USB_Ports )
                System_Info::Add_To_Used_USB_List( usb_dev );
        }

        QEMU_Process->start( bin_path, this->Build_QEMU_Args() );
    }

    // Do NOT Start CPU
    if( ! Start_CPU )
        Set_State( VM::VMS_Pause );

    // VNC Password
    if( VNC && VNC_Password )
    {
        Execute_Emu_Ctl_Command( "change vnc password" );
    }

    if( Load_Mode )
    {
        connect( this, SIGNAL(Ready_StdOut(const QString&)),
                 this, SLOT(Started_Booting(const QString&)) );

        Send_Emulator_Command( "info vnc\n" );
    }

    // Init Emulator Control Window
    if( Emu_Ctl->First_Start == false )
    {
        delete Emu_Ctl;

        Emu_Ctl = new Emulator_Control_Window();

        QObject::connect( Emu_Ctl, SIGNAL(Ready_Read_Command(QString)),
                          this, SLOT(Execute_Emu_Ctl_Command(QString)) );
    }

    Emu_Ctl->Use_Minimal_Size( true );
    Emu_Ctl->Set_Current_VM( this );
    Emu_Ctl->Init();

    // Show vm loading window?
    if( Start_Snapshot_Tag.isEmpty() == false ||
        State == VM::VMS_Saved )
    {
        Show_VM_Load_Window();
    }

    return true;
}

bool Virtual_Machine::Start()
{
    if ( Start_impl() )
    {
        // VNC Password
        if( Use_VNC_Password() )
        {
            VNC_Password_Window vnc_pas_win;

            if( vnc_pas_win.exec() == QDialog::Accepted )
                Set_VNC_Password( vnc_pas_win.Get_Password() );
        }
        return true;
    }

    return false;
}

void Virtual_Machine::Pause()
{
	switch( State )
	{
		case VM::VMS_Pause:
			Send_Emulator_Command( "c\n" );
			Set_State( VM::VMS_Running );
			break;
			
		case VM::VMS_Running:
			Send_Emulator_Command( "stop\n" );
			Set_State( VM::VMS_Pause );
			break;
			
		default:
			AQDebug( "void Virtual_Machine::Pause()",
					 "Default Section!" );
			break;
	}
}

void Virtual_Machine::Stop()
{
	if( State == VM::VMS_Saved )
		Set_State( VM::VMS_Power_Off, true );
	else
		Send_Emulator_Command( "quit\n" );
}

void Virtual_Machine::Shutdown()
{
    /*if( State == VM::VMS_Saved )
        Set_State( VM::VMS_Power_Off, true );
    else*/
    Send_Emulator_Command( "system_powerdown\n" );
}

void Virtual_Machine::Reset()
{
	if( State == VM::VMS_Saved )
	{
		Start_Snapshot_Tag = "";
		Set_State( VM::VMS_Power_Off, true );
		Start();
	}
	else
	{
		Send_Emulator_Command( "system_reset\n" );
	}
}

void Virtual_Machine::Save_VM_State()
{
	Save_VM_State( "aqemu_save", true );
}

void Virtual_Machine::Save_VM_State( const QString &tag, bool quit )
{
	Show_VM_Save_Window();
	Quit_Before_Save = quit;
	
	if( Settings.value("Use_Screenshot_for_OS_Logo", "yes").toString() == "yes" )
	{
		QString scrn_file = Settings.value( "VM_Directory", "~" ).toString() + Get_FS_Compatible_VM_Name( Machine_Name );
		if( Take_Screenshot(scrn_file, 64, 64) ) Screenshot_Path = scrn_file;
	}
	
	Send_Emulator_Command( qPrintable("savevm " + tag + "\n") );
	
	connect( this, SIGNAL(Ready_StdOut(const QString&)),
			 this, SLOT(Suspend_Finished(const QString&)) );
}

void Virtual_Machine::Load_VM_State( const QString &tag )
{
	Show_VM_Load_Window();
	
	connect( this, SIGNAL(Ready_StdOut(const QString&)),
			 this, SLOT(Resume_Finished(const QString&)) );
	
	Send_Emulator_Command( qPrintable("loadvm " + tag + "\n") );
}

bool Virtual_Machine::Start_Snapshot( const QString &tag )
{
	Start_Snapshot_Tag = tag;
	
	return Start();
}

void Virtual_Machine::Delete_Snapshot( const QString &tag )
{
	Send_Emulator_Command( qPrintable("delvm " + tag + "\n") );
}

const QString &Virtual_Machine::Get_UID() const
{
	return UID;
}

void Virtual_Machine::Set_UID( const QString &uid )
{
	UID = uid;
}

void Virtual_Machine::Show_Emu_Ctl_Win()
{
	Emu_Ctl->show();
	Emu_Ctl->raise();
	Emu_Ctl->activateWindow();
}

void Virtual_Machine::Show_Monitor_Window()
{
    Emu_Ctl->Show_Monitor();
}

void Virtual_Machine::Hide_Emu_Ctl_Win()
{
	Emu_Ctl->Set_Show_Close_Warning( false );
	Emu_Ctl->close();
}

void Virtual_Machine::Update_Removable_Devices_List()
{
	Removable_Devices_List.clear();
	Update_Removable_Devices_Mode = true;
	
	Send_Emulator_Command( "info block\n" );
}

const QString &Virtual_Machine::Get_Removable_Devices_List() const
{
	return Removable_Devices_List;
}

void Virtual_Machine::Show_Error_Log_Window()
{
    if ( ! QEMU_Error_Win )
        QEMU_Error_Win = new Error_Log_Window();
        
	QEMU_Error_Win->setWindowTitle( tr("QEMU Error Log") + " (" + Machine_Name + ")" );
	QEMU_Error_Win->show();
}

void Virtual_Machine::Show_QEMU_Error( const QString &err_str )
{
    if ( err_str.simplified().isEmpty() )
        return;

    if ( ! QEMU_Error_Win )
        QEMU_Error_Win = new Error_Log_Window();

	QEMU_Error_Win->Add_to_Log( err_str );
	
	if( Settings.value( "No_Show_Error_Log_Forever", "no" ).toString() == "yes" ||
		QEMU_Error_Win->No_Show_Before_AQEMU_Restart() == true ||
		QEMU_Error_Win->No_Show_Before_VM_Restart() == true ||
        QEMU_Error_Win->No_Show_Deprecated_Options_Error() == true )
	{
		AQDebug( "void Virtual_Machine::Show_QEMU_Error( const QString &err_str )",
				 "Error Log Window Not Shown" );
		return;
	}
	else
	{
		Show_Error_Log_Window();
	}
}

void Virtual_Machine::Hide_QEMU_Error_Log()
{
    if ( QEMU_Error_Win )
    {
    	QEMU_Error_Win->close();
    }
}

void Virtual_Machine::Show_VM_Load_Window()
{
	QDesktopWidget *des_widget = new QDesktopWidget();
	QRect re = des_widget->screenGeometry( des_widget->primaryScreen() );
	
	Load_VM_Window = new QWidget();
	
	QLabel *load_label;
	
	Load_VM_Window->setWindowTitle( tr("Loading...") );
	load_label = new QLabel( tr("Loading Virtual Machine. Please wait...") );
	
	QHBoxLayout *h_layout = new QHBoxLayout();
	h_layout->addWidget( load_label );
	Load_VM_Window->setLayout( h_layout );
	
	Load_VM_Window->move( re.height() / 2, re.width() / 2 );
	Load_VM_Window->show();
}

void Virtual_Machine::Hide_VM_Load_Window()
{
	if( ! Dont_Reinit )
	{
		AQDebug( "void Virtual_Machine::Hide_VM_Load_Window()",
				 "emit Loading_Complete()" );
		//emit Loading_Complete();
	}
	
    if ( Load_VM_Window )
    {
    	Load_VM_Window->close();
        delete Load_VM_Window;
    }
}

void Virtual_Machine::Show_VM_Save_Window()
{
	QDesktopWidget *des_widget = new QDesktopWidget();
	QRect re = des_widget->screenGeometry( des_widget->primaryScreen() );
	
	//Load_VM_Window = new QWidget();
	Save_VM_Window = new QWidget();
	
	QLabel *save_label;
	
	Save_VM_Window->setWindowTitle( tr("Saving...") );
	save_label = new QLabel( tr("Saving Virtual Machine. Please wait...") );
	
	QHBoxLayout *h_layout = new QHBoxLayout();
	h_layout->addWidget( save_label );
	Save_VM_Window->setLayout( h_layout );
	
	Save_VM_Window->move( re.height() / 2, re.width() / 2 );
	Save_VM_Window->show();
}

void Virtual_Machine::Hide_VM_Save_Window()
{
    if ( Save_VM_Window )
    {
    	Save_VM_Window->close();
        delete Save_VM_Window;
    }
}

bool Virtual_Machine::Take_Screenshot( const QString &file_name, int width, int height )
{
	if( ! (State == VM::VMS_Running || State == VM::VMS_Pause) )
	{
		AQError( "bool Virtual_Machine::Take_Screenshot( const QString &file_name, int width, int height )",
				 "VM Not Running, Cannot Create Screenshot!" );
		return false;
	}
	
	Execute_Emu_Ctl_Command( "screendump \"" + file_name + "\"" );
	
	#ifdef Q_OS_WIN32
	Sleep( 100 );
	#else
	QTest::qWait( 100 );
	#endif

	QImage im = QImage();
	
	bool load_ok = false;
	
	// Time for creating screenshot
	for( int wx = 0; wx < 10; ++wx )
	{
		if( im.load(file_name) )
		{
			load_ok = true;
			break;
		}
		
		#ifdef Q_OS_WIN32
		Sleep( 100 );
		#else
		QTest::qWait( 100 );
		#endif
	}
	
	// Loading Complete?
	if( load_ok == false)
	{
		AQError( "bool Virtual_Machine::Take_Screenshot( const QString &file_name, int width, int height )",
				 "Cannot Load Screenshot File!" );
		return false;
	}
	
	// delete tmp file
	if( ! QFile::remove(file_name) )
	{
		AQError( "bool Virtual_Machine::Take_Screenshot( const QString &file_name, int width, int height )",
				 "Cannot Remove Temp Screenshot File!" );
		return false;
	}
	
	// Default save format
	QString fmt = Settings.value( "Screenshot_Save_Format", "PNG" ).toString();
	
	// OS Icon Mode?
	if( width > 0 && height > 0 )
	{
		im = im.scaled( width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation );
		fmt = "PNG";
		
		// 64x64 Mode
		QImage im2 = QImage( width, height, QImage::Format_ARGB32 );
		im2.fill( Qt::transparent );
		
		QSize ms = im2.size() - im.size();
		ms /= 2;
		
		QPainter myp;
		
		myp.begin( &im2 );
		myp.drawImage( QPoint(ms.width(),ms.height()), im );
		
		myp.end();
		
		im = im2;
	}
	
	bool save_ok = false;
	
	if( fmt == "JPEG" )
		save_ok = im.save( file_name, fmt.toLatin1(), Settings.value("Jpeg_Quality", 75).toInt() );
	else
		save_ok = im.save( file_name, fmt.toLatin1() );
	
	if( save_ok )
	{
		return true;
	}
	else
	{
		AQError( "bool Virtual_Machine::Take_Screenshot( const QString &file_name, int width, int height )",
				 "Cannot Save Image!" );
		return false;
	}
}

VM::Machine_Accelerator Virtual_Machine::Get_Machine_Accelerator() const
{
	return Machine_Accelerator;
}

void Virtual_Machine::Set_Machine_Accelerator( VM::Machine_Accelerator accel )
{
	Machine_Accelerator = accel;
}

const Emulator &Virtual_Machine::Get_Emulator() const
{
	return Current_Emulator;
}

void Virtual_Machine::Set_Emulator( const Emulator &emul )
{
	Current_Emulator = emul;
	Update_Current_Emulator_Devices();
}

void Virtual_Machine::Update_Current_Emulator_Devices()
{
	// Apply Emulator Devices List
	if( Current_Emulator.Get_Force_Version() || Current_Emulator.Get_Check_Version() )
	{
		switch( Current_Emulator.Get_Version() )
		{
			case VM::QEMU_2_0:
				Current_Emulator_Devices = System_Info::Emulator_QEMU_2_0[ Computer_Type ];
				break;
				
            /*
			case VM::KVM_2_0:
				Current_Emulator_Devices = System_Info::Emulator_KVM_2_0[ Computer_Type ];
				break; */ //tobgle //FIXME?
	
			default:
				AQError( "void Update_Current_Emulator_Devices()", "Emulator Version Invalid!" );
				break;
		}
	}
	else
	{
		Current_Emulator_Devices = Current_Emulator.Get_Devices()[ Computer_Type ];
	}
	
	// Loading Info Complete?
	if( Current_Emulator_Devices.System.QEMU_Name.isEmpty() )
	{
		AQError( "void Update_Current_Emulator_Devices()",
				 "Cannot Load Info About This Emulator! AQEMU Doesn't Work!" );
	}
}

const Available_Devices *Virtual_Machine::Get_Current_Emulator_Devices() const
{
	if( Current_Emulator_Devices.System.QEMU_Name.isEmpty() )
	{
		AQError( "const Virtual_Machine::Available_Devices *Get_Current_Emulator_Devices() const",
				 "Cannot get valid devices!" );
		return new Available_Devices();
	}
	
	return &Current_Emulator_Devices;
}

QString Virtual_Machine::Get_Current_Emulator_Binary_Path( const QString &names ) const
{
	QMap<QString, QString> bin_list = Current_Emulator.Get_Binary_Files();
	QStringList nl = names.split( " ", QString::SkipEmptyParts );
	
	if( bin_list.count() <= 0 || nl.count() <= 0 )
	{
		AQError( "QString Virtual_Machine::Get_Current_Emulator_Binary_Path( const QString &names ) const",
				 "bin_list.count() <= 0 || nl.count() <= 0" );
		return "";
	}
	
	for( QMap<QString, QString>::const_iterator iter = bin_list.constBegin(); iter != bin_list.constEnd(); iter++ )
	{
		for( int fx = 0; fx < nl.count(); fx++ )
		{
			if( iter.key() == nl[fx] ) return iter.value();
		}
	}
	
	return "";
}

VM::VM_State Virtual_Machine::Get_State() const
{
	return State;
}

QString Virtual_Machine::Get_State_Text() const
{
    switch (State)
	{
		case VM::VMS_Running:
			return tr("Running");
			
		case VM::VMS_Power_Off:
			return tr("Power Off");
			
		case VM::VMS_Pause:
			return tr("Pause");
			
		case VM::VMS_Saved:
			return tr("Saved");
			
		default:
			return tr("Error!");
	}	
}

void Virtual_Machine::Set_State( VM::VM_State s, bool real_poewer_off )
{
	if( State == VM::VMS_Saved && s == VM::VMS_Power_Off )
	{
		if( ! real_poewer_off ) return;
	}
	
	Old_State = State;
	
	State = s;
	emit State_Changed( this, State );
}

const QString &Virtual_Machine::Get_Machine_Name() const
{
	return Machine_Name;
}

void Virtual_Machine::Set_Machine_Name( const QString &name )
{
	if( name.isEmpty() )
		AQError( "void Virtual_Machine::Set_Machine_Name( const QString &name )",
				 "File name is empty!" );
	else Machine_Name = name;
}

const QString &Virtual_Machine::Get_Icon_Path() const
{
	return Icon_Path;
}

void Virtual_Machine::Set_Icon_Path( const QString &file_name )
{
	if( file_name.isEmpty() )
		AQError( "void Virtual_Machine::Set_Icon_Path( const QString &file_name )",
				 "File name is empty!" );
	else
		Icon_Path = file_name;
}

const QString &Virtual_Machine::Get_Screenshot_Path() const
{
	return Screenshot_Path;
}

void Virtual_Machine::Set_Screenshot_Path( const QString &file_name )
{
	Screenshot_Path = file_name;
}

bool Virtual_Machine::Get_Remove_RAM_Size_Limitation() const
{
	return Remove_RAM_Size_Limitation;
}

void Virtual_Machine::Set_Remove_RAM_Size_Limitation( bool on )
{
	Remove_RAM_Size_Limitation = on;
}

int Virtual_Machine::Get_Memory_Size() const
{
	return Memory_Size;
}

void Virtual_Machine::Set_Memory_Size( int megs )
{
    if( megs > 0 ) Memory_Size = megs;
	else
	{
		AQError( "void Virtual_Machine::Set_Memory_Size( int megs )",
				 "Memory size " + QString::number(megs) + " invalid!" );
	}
}

const QString &Virtual_Machine::Get_Computer_Type() const
{
	return Computer_Type;
}

void Virtual_Machine::Set_Computer_Type( const QString &type )
{
	Computer_Type = type;
}

const QString &Virtual_Machine::Get_Machine_Type() const
{
	return Machine_Type;
}

void Virtual_Machine::Set_Machine_Type( const QString &type )
{
	Machine_Type = type;
}

const QString &Virtual_Machine::Get_CPU_Type() const
{
	return CPU_Type;
}

void Virtual_Machine::Set_CPU_Type( const QString &type )
{
	CPU_Type = type;
}

VM::SMP_Options Virtual_Machine::Get_SMP() const
{
	return SMP;
}

void Virtual_Machine::Set_SMP( const VM::SMP_Options &smp )
{
	SMP = smp;
}

int Virtual_Machine::Get_SMP_CPU_Count() const
{
	return SMP.SMP_Count;
}

void Virtual_Machine::Set_SMP_CPU_Count( int count )
{
	SMP.SMP_Count = count;
}

const QString &Virtual_Machine::Get_Keyboard_Layout() const
{
	return Keyboard_Layout;
}

void Virtual_Machine::Set_Keyboard_Layout( const QString &kl )
{
	Keyboard_Layout = kl;
}

VM::Sound_Cards Virtual_Machine::Get_Audio_Cards() const
{
	return Audio_Card;
}

void Virtual_Machine::Set_Audio_Cards( VM::Sound_Cards card )
{
	Audio_Card = card;
}

const QString &Virtual_Machine::Get_Video_Card() const
{
	return Video_Card;
}

void Virtual_Machine::Set_Video_Card( const QString &card )
{
	Video_Card = card;
}

const QString &Virtual_Machine::Get_Additional_Args() const
{
	return Additional_Args;
}

void Virtual_Machine::Set_Additional_Args( const QString &aa )
{
	Additional_Args = aa;
}

bool Virtual_Machine::Get_Only_User_Args() const
{
	return Only_User_Args;
}

void Virtual_Machine::Set_Only_User_Args( bool use )
{
	Only_User_Args = use;
}

bool Virtual_Machine::Get_Use_User_Emulator_Binary() const
{
	return Use_User_Emulator_Binary;
}

void Virtual_Machine::Set_Use_User_Emulator_Binary( bool use )
{
	Use_User_Emulator_Binary = use;
}

const QList<VM::Boot_Order> &Virtual_Machine::Get_Boot_Order_List() const
{
	return Boot_Order_List;
}

void Virtual_Machine::Set_Boot_Order_List( QList<VM::Boot_Order> &list )
{
	Boot_Order_List = list;
}

bool Virtual_Machine::Get_Show_Boot_Menu() const
{
	return Show_Boot_Menu;
}

void Virtual_Machine::Set_Show_Boot_Menu( bool use )
{
	Show_Boot_Menu = use;
}

bool Virtual_Machine::Use_Fullscreen_Mode() const
{
	return Fullscreen;
}

void Virtual_Machine::Use_Fullscreen_Mode( bool use )
{
	Fullscreen = use;
}

bool Virtual_Machine::Use_ACPI() const
{
	return ACPI;
}

void Virtual_Machine::Use_ACPI( bool use )
{
	ACPI = use;
}

bool Virtual_Machine::Use_Local_Time() const
{
	return Local_Time;
}

void Virtual_Machine::Use_Local_Time( bool use )
{
	Local_Time = use;
}

bool Virtual_Machine::Use_Snapshot_Mode() const
{
	return Snapshot_Mode;
}

void Virtual_Machine::Use_Snapshot_Mode( bool use )
{
	Snapshot_Mode = use;
}

bool Virtual_Machine::Use_Win2K_Hack() const
{
	return Win2K_Hack;
}

void Virtual_Machine::Use_Win2K_Hack( bool use )
{
	Win2K_Hack = use;
}

bool Virtual_Machine::Use_Start_CPU() const
{
	return Start_CPU;
}

void Virtual_Machine::Use_Start_CPU( bool start )
{
	Start_CPU = start;
}

bool Virtual_Machine::Use_No_Reboot() const
{
	return No_Reboot;
}

void Virtual_Machine::Use_No_Reboot( bool use )
{
	No_Reboot = use;
}

bool Virtual_Machine::Use_No_Shutdown() const
{
	return No_Shutdown;
}

void Virtual_Machine::Use_No_Shutdown( bool use )
{
	No_Shutdown = use;
}

bool Virtual_Machine::Use_Check_FDD_Boot_Sector() const
{
	return Check_FDD_Boot_Sector;
}

void Virtual_Machine::Use_Check_FDD_Boot_Sector( bool use )
{
	Check_FDD_Boot_Sector = use;
}

const VM_Storage_Device &Virtual_Machine::Get_FD0() const
{
	return FD0;
}

void Virtual_Machine::Set_FD0( const VM_Storage_Device &floppy )
{
	FD0 = floppy;
}

const VM_Storage_Device &Virtual_Machine::Get_FD1() const
{
	return FD1;
}

void Virtual_Machine::Set_FD1( const VM_Storage_Device &floppy )
{
	FD1 = floppy;
}

const VM_Storage_Device &Virtual_Machine::Get_CD_ROM() const
{
	return CD_ROM;
}

void Virtual_Machine::Set_CD_ROM( const VM_Storage_Device &cdrom )
{
	CD_ROM = cdrom;
}

const VM_HDD &Virtual_Machine::Get_HDA() const
{
	return HDA;
}

void Virtual_Machine::Set_HDA( const VM_HDD &hdd )
{
	HDA = hdd;
}

const VM_HDD &Virtual_Machine::Get_HDB() const
{
	return HDB;
}

void Virtual_Machine::Set_HDB( const VM_HDD &hdd )
{
	HDB = hdd;
}

const VM_HDD &Virtual_Machine::Get_HDC() const
{
	return HDC;
}

void Virtual_Machine::Set_HDC( const VM_HDD &hdd )
{
	HDC = hdd;
}

const VM_HDD &Virtual_Machine::Get_HDD() const
{
	return HDD;
}

void Virtual_Machine::Set_HDD( const VM_HDD &hdd )
{
	HDD = hdd;
}

const QList<VM_Snapshot> &Virtual_Machine::Get_Snapshots() const
{
	return Snapshots;
}

void Virtual_Machine::Set_Snapshots( const QList<VM_Snapshot> &list )
{
	Snapshots.clear();
	
	for( int sx = 0; sx < list.count(); ++sx )
	{
		Snapshots.append( VM_Snapshot(list[sx]) );
	}
}

void Virtual_Machine::Add_Snapshot( const QString &tag, const QString &name, const QString &desc )
{
	VM_Snapshot tmp;
	
	tmp.Set_Tag( tag );
	tmp.Set_Name( name );
	tmp.Set_Description( desc );
	
	Snapshots.append( tmp );
}

void Virtual_Machine::Add_Snapshot( const VM_Snapshot &snapshot )
{
	Snapshots.append( VM_Snapshot(snapshot) );
}

void Virtual_Machine::Set_Snapshot( int index, const VM_Snapshot &s )
{
	if( index >= 0 && index < Snapshots.count() )
	{
		Snapshots[ index ] = VM_Snapshot( s );
	}
	else
	{
		AQError( "void Virtual_Machine::Set_Snapshot( int index, const VM_Snapshot &s )",
				 "Index == " + QString::number(index) );
	}
}

const QList<VM_Native_Storage_Device> &Virtual_Machine::Get_Storage_Devices_List() const
{
	return Storage_Devices;
}

const QList<VM_Shared_Folder> &Virtual_Machine::Get_Shared_Folders_List() const
{
	return Shared_Folders;
}

void Virtual_Machine::Set_Storage_Devices_List( const QList<VM_Native_Storage_Device> &list )
{
	Storage_Devices.clear();
	
	for( int ix = 0; ix < list.count(); ++ix )
	{
        Storage_Devices.append( VM_Native_Storage_Device(list[ix]) );
	}
}

void Virtual_Machine::Set_Shared_Folders_List( const QList<VM_Shared_Folder> &list )
{
	Shared_Folders.clear();
	
	for( int ix = 0; ix < list.count(); ++ix )
	{
		Shared_Folders.append( VM_Shared_Folder(list[ix]) );
	}
}

const VM_Native_Storage_Device &Virtual_Machine::Get_Storage_Device( int index ) const
{
	if( index < 0 || index > Storage_Devices.count() )
	{
		// FIXME message
        VM_Native_Storage_Device *empty_dev = new VM_Native_Storage_Device();
		return *empty_dev;
	}
	
	return Storage_Devices[ index ];
}

void Virtual_Machine::Set_Storage_Device( int index, const VM_Native_Storage_Device &device )
{
	if( index < 0 || index > Storage_Devices.count() )
	{
		// FIXME message
		return;
	}
	
	Storage_Devices[ index ] = device;
}

const QString &Virtual_Machine::Get_SMB_Directory() const
{
	return SMB_Directory;
}

void Virtual_Machine::Set_SMB_Directory( const QString &dir )
{
	SMB_Directory = dir;
}

const QString &Virtual_Machine::Get_TFTP_Prefix() const
{
	return TFTP_Prefix;
}

void Virtual_Machine::Set_TFTP_Prefix( const QString &p )
{
	TFTP_Prefix = p;
}

const QList<VM_Net_Card> &Virtual_Machine::Get_Network_Cards() const
{
	return Network_Cards;
}

void Virtual_Machine::Set_Network_Cards( const QList<VM_Net_Card> &cards )
{
	Network_Cards = cards;
}

const QList<VM_Net_Card_Native> &Virtual_Machine::Get_Network_Cards_Nativ() const
{
	return Network_Cards_Nativ;
}

void Virtual_Machine::Set_Network_Cards_Nativ( const QList<VM_Net_Card_Native> &cards )
{
	Network_Cards_Nativ = cards;
}

const VM_Net_Card &Virtual_Machine::Get_Network_Card( int index ) const
{
	if( index >= 0 && index < Network_Cards.count() )
	{
		return Network_Cards[ index ];
	}
	else
	{
		AQError( "VM_Net_Card *Virtual_Machine::Get_Network_Card( int index ) const",
				 "Invalid Index!" );
		return *(new VM_Net_Card());
	}
}

void Virtual_Machine::Set_VM_Network_Card( int index, const VM_Net_Card &nc )
{
	if( index >= 0 && index <= 8 && index < Network_Cards.count() )
	{
		Network_Cards[ index ] = nc;
	}
	else
	{
		AQError( "void Virtual_Machine::Set_VM_Network_Card( int index, VM_Net_Card *nc )",
				 "Invalid Index!" );
	}
}

bool Virtual_Machine::Add_Network_Card( const VM_Net_Card &nc )
{
	if( Network_Cards.count() < 9 )
	{
		Network_Cards.append( VM_Net_Card(nc) );
		return true;
	}
	else
	{
		return false;
	}
}

bool Virtual_Machine::Delete_Network_Card( int ix )
{
	if( ix >= 0 && ix <= 8 )
	{
		Network_Cards.removeAt( ix );
		return true;
	}
	else
	{
		return false;
	}
}

void Virtual_Machine::Clear_Network_Cards_List()
{
	Network_Cards.clear();
}

int Virtual_Machine::Get_Network_Cards_Count() const
{
	return Network_Cards.count();
}

const VM_Redirection &Virtual_Machine::Get_Network_Redirection( int index ) const
{
	if( index >= 0 && index < Network_Redirections.count() )
	{
		return Network_Redirections[ index ];
	}
	else
	{
		AQError( "VM_Redirection *Virtual_Machine::Get_Network_Redirection( int index ) const",
				 "Invalid Index!" );
		return *(new VM_Redirection());
	}
}

void Virtual_Machine::Set_Network_Redirection( int index, const VM_Redirection &r )
{
	if( index >= 0 && index < Network_Redirections.count() )
	{
		Network_Redirections[ index ] = r;
	}
	else
	{
		AQError( "void Virtual_Machine::Set_Network_Redirection( int index, VM_Redirection *r )",
				 "Invalid Index!" );
	}
}

void Virtual_Machine::Add_Network_Redirection( const VM_Redirection &r )
{
	Network_Redirections.append( VM_Redirection(r) );
}

bool Virtual_Machine::Delete_Network_Redirection( int ix )
{
	if( ix >= 0 && ix < Network_Redirections.count() )
	{
		Network_Redirections.removeAt( ix );
		return true;
	}
	else
	{
		return false;
	}
}

int Virtual_Machine::Get_Network_Redirections_Count() const
{
	return Network_Redirections.count();
}

bool Virtual_Machine::Get_Use_Network() const
{
	return Use_Network;
}

void Virtual_Machine::Set_Use_Network( bool use )
{
	Use_Network = use;
}

bool Virtual_Machine::Use_Native_Network() const
{
    return Native_Network;
}

void Virtual_Machine::Use_Native_Network( bool use )
{
    Native_Network = use;
}

bool Virtual_Machine::Get_Use_Redirections() const
{
	return Use_Redirections;
}

void Virtual_Machine::Set_Use_Redirections( bool r )
{
	Use_Redirections = r;
}

const QList<VM_Port> &Virtual_Machine::Get_Serial_Ports() const
{
	return Serial_Ports;
}

void Virtual_Machine::Set_Serial_Ports( const QList<VM_Port> &list )
{
	Serial_Ports = list;
}

const QList<VM_Port> &Virtual_Machine::Get_Parallel_Ports() const
{
	return Parallel_Ports;
}

void Virtual_Machine::Set_Parallel_Ports( const QList<VM_Port> &list )
{
	Parallel_Ports = list;
}
/*
// FIXME WARNING! THIS FUNCTION INCOMPLETE
QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )
{
	// Find Device Name by vendor_id:device_id
	Send_Emulator_Command( "info usbhost\n" );
	QTest::qSleep(200);
	
	QString info_usbhost_res = QEMU_Process->readAll();
	
	if( info_usbhost_res.isEmpty() )
	{
		AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
				 "info_usbhost_res.isEmpty()" );
		return "";
	}
	
	QStringList usbhost_dev_list = info_usbhost_res.split( "\n", QString::SkipEmptyParts );
	
	AQWarning( "IW", "Data: " + info_usbhost_res );
	AQWarning( "IW", "End" );
	
	if( usbhost_dev_list.count() <= 0 )
	{
		AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
				 "usbhost_dev_list.count() <= 0" );
		return "";
	}
	
	// Valid String is: "Class 00: USB device ab12:34cd, USB UPS"
	QRegExp usbhost_dev_reg = QRegExp( ".+([\\da-fA-F]{,4}:[\\da-fA-F]{,4}),\\s+(.+)" );
	
	QString dev_name = "";
	
	for( int ix = 0; ix < usbhost_dev_list.count(); ++ix )
	{
		if( ! usbhost_dev_reg.exactMatch(usbhost_dev_list[ix]) )
		{
			AQDebug( "CM1", usbhost_dev_list[ix] );
			continue;
		}
		
		QStringList info_lines = usbhost_dev_reg.capturedTexts();
		
		if( info_lines.count() <= 1 )
		{
			AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
					 "info_lines.count() <= 1" );
			return "";
		}
		
		// info_lines[1] == ab12:34cd
		// info_lines[2] == USB UPS
		if( info_lines[1] == id )
		{
			dev_name = info_lines[2];
			break;
		}
	}
	
	if( dev_name.isEmpty() )
	{
		AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
				 "dev_name.isEmpty()" );
		return "";
	}
	
	AQWarning( "id == " + id, "name == " + dev_name );
	
	// Find Bus.Address by Device Name
	Send_Emulator_Command( "info usb\n" );
	QTest::qSleep(200);
	QString info_usb_res = QEMU_Process->readAll();
	
	if( info_usb_res.isEmpty() )
	{
		AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
				 "info_usb_res is Empty" );
		return "";
	}
	
	QStringList usb_dev_list = info_usb_res.split( "\n", QString::SkipEmptyParts );
	
	if( usb_dev_list.count() <= 0 )
	{
		AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
				 "usb_dev_list.count() <= 0" );
		return "";
	}
	
	// Valid String is: "Device 0.0, Speed 1.5 Mb/s, Product USB UPS"
	QRegExp usb_dev_reg = QRegExp( "\\s*Device\\s+([\\d]+[.][\\d]+),\\s+Speed\\s+[\\d]+[.][\\d]+\\sMb/s,\\sProduct\\s+(.+)" );
	
	for( int ix = 0; ix < usb_dev_list.count(); ++ix )
	{
		if( ! usb_dev_reg.exactMatch(usb_dev_list[ix]) )
		{
			AQDebug( "CM2", usb_dev_list[ix] );
			continue;
		}
		
		QStringList info_lines = usb_dev_reg.capturedTexts();
		
		if( info_lines.count() <= 1 )
		{
			AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
					 "info_lines.count() <= 1" );
			return "";
		}
		
		// info_lines[1] == 0.0
		// info_lines[2] == USB UPS
		if( info_lines[2] == dev_name )
		{
			AQDebug( "Found: ", info_lines[ 1 ] );
			return info_lines[ 1 ]; // OK. This is Bus.Adr
		}
	}
	
	AQError( "QString Virtual_Machine::Get_USB_Bus_Address( const QString &id )",
			 "Not Found!" );
	
	return "";
}

bool Virtual_Machine::Use_USB_Hub() const
{
	return USB_Hub;
}

void Virtual_Machine::Use_USB_Hub( bool use )
{
	USB_Hub = use;
}

int Virtual_Machine::Get_USB_Ports_Count() const
{
	return USB_Ports.count();
}

const VM_USB &Virtual_Machine::Get_USB_Port( int index ) const
{
	if( index >= 0 && index < USB_Ports.count() )
	{
		return USB_Ports[ index ];
	}
	else
	{
		AQError( "VM_USB *Virtual_Machine::Get_USB_Port( int index ) const",
				 "Index Invalid!" );
		
		return *(new VM_USB());
	}
}

void Virtual_Machine::Set_USB_Port( int index, const VM_USB &u )
{
	if( index >= 0 && index < USB_Ports.count() )
	{
		USB_Ports[ index ] = VM_USB( u );
	}
}
*/
void Virtual_Machine::Add_USB_Port( const VM_USB &u )
{
	USB_Ports.append( VM_USB(u) );
}

bool Virtual_Machine::Delete_USB_Port( int index )
{
	if( index >= 0 && index < USB_Ports.count() )
	{
		USB_Ports.removeAt( index );
		return true;
	}
	else
	{
		return false;
	}
}

const QList<VM_USB> &Virtual_Machine::Get_USB_Ports() const
{
	return USB_Ports;
}

void Virtual_Machine::Set_USB_Ports( const QList<VM_USB> &list )
{
	USB_Ports.clear();
	
	for( int ix = 0; ix < list.count(); ++ix )
	{
		USB_Ports.append( VM_USB(list[ix]) );
	}
}

bool Virtual_Machine::Get_Use_Linux_Boot() const
{
	return Linux_Boot;
}

void Virtual_Machine::Set_Use_Linux_Boot( bool use )
{
	Linux_Boot = use;
}

const QString &Virtual_Machine::Get_bzImage_Path() const
{
	return bzImage_Path;
}

void Virtual_Machine::Set_bzImage_Path( const QString &path )
{
	bzImage_Path = path;
}

const QString &Virtual_Machine::Get_Initrd_Path() const
{
	return Initrd_Path;
}

void Virtual_Machine::Set_Initrd_Path( const QString &path )
{
	Initrd_Path = path;
}

const QString &Virtual_Machine::Get_Kernel_ComLine() const
{
	return Kernel_ComLine;
}

void Virtual_Machine::Set_Kernel_ComLine( const QString &cl )
{
	Kernel_ComLine = cl;
}

bool Virtual_Machine::Get_Use_ROM_File() const
{
	return Use_ROM_File;
}

void Virtual_Machine::Set_Use_ROM_File( bool use )
{
	Use_ROM_File = use;
}

const QString &Virtual_Machine::Get_ROM_File() const
{
	return ROM_File;
}

void Virtual_Machine::Set_ROM_File( const QString &path )
{
	ROM_File = path;
}

bool Virtual_Machine::Use_MTDBlock_File() const
{
	return MTDBlock;
}

void Virtual_Machine::Use_MTDBlock_File( bool use )
{
	MTDBlock = use;
}

const QString &Virtual_Machine::Get_MTDBlock_File() const
{
	return MTDBlock_File;
}

void Virtual_Machine::Set_MTDBlock_File( const QString &file )
{
	MTDBlock_File = file;
}

bool Virtual_Machine::Use_SecureDigital_File() const
{
	return SecureDigital;
}

void Virtual_Machine::Use_SecureDigital_File( bool use )
{
	SecureDigital = use;
}

const QString &Virtual_Machine::Get_SecureDigital_File() const
{
	return SecureDigital_File;
}

void Virtual_Machine::Set_SecureDigital_File( const QString &file )
{
	SecureDigital_File = file;
}

bool Virtual_Machine::Use_PFlash_File() const
{
	return PFlash;
}

void Virtual_Machine::Use_PFlash_File( bool use )
{
	PFlash = use;
}

const QString &Virtual_Machine::Get_PFlash_File() const
{
	return PFlash_File;
}

void Virtual_Machine::Set_PFlash_File( const QString &file )
{
	PFlash_File = file;
}

bool Virtual_Machine::Use_KVM() const
{
	return Enable_KVM;
}

void Virtual_Machine::Use_KVM( bool use )
{
	Enable_KVM = use;
}

bool Virtual_Machine::Use_KVM_IRQChip() const
{
	return KVM_IRQChip;
}

void Virtual_Machine::Use_KVM_IRQChip( bool use )
{
	KVM_IRQChip = use;
}

bool Virtual_Machine::Use_No_KVM_Pit() const
{
	return No_KVM_Pit;
}

void Virtual_Machine::Use_No_KVM_Pit( bool use )
{
	No_KVM_Pit = use;
}

bool Virtual_Machine::Use_KVM_No_Pit_Reinjection() const
{
	return KVM_No_Pit_Reinjection;
}

void Virtual_Machine::Use_KVM_No_Pit_Reinjection( bool use )
{
	KVM_No_Pit_Reinjection = use;
}

bool Virtual_Machine::Use_KVM_Nesting() const
{
	return KVM_Nesting;
}

void Virtual_Machine::Use_KVM_Nesting( bool use )
{
	KVM_Nesting = use;
}

bool Virtual_Machine::Use_KVM_Shadow_Memory() const
{
	return KVM_Shadow_Memory;
}

void Virtual_Machine::Use_KVM_Shadow_Memory( bool use )
{
	KVM_Shadow_Memory = use;
}

int Virtual_Machine::Get_KVM_Shadow_Memory_Size() const
{
	return KVM_Shadow_Memory_Size;
}

void Virtual_Machine::Set_KVM_Shadow_Memory_Size( int size )
{
	KVM_Shadow_Memory_Size = size;
}

const VM_Init_Graphic_Mode &Virtual_Machine::Get_Init_Graphic_Mode() const
{
	return Init_Graphic_Mode;
}

void Virtual_Machine::Set_Init_Graphic_Mode( const VM_Init_Graphic_Mode &mode )
{
	Init_Graphic_Mode = mode;
}

bool Virtual_Machine::Use_No_Frame() const
{
	return No_Frame;
}

void Virtual_Machine::Use_No_Frame( bool use )
{
	No_Frame = use;
}

bool Virtual_Machine::Use_Alt_Grab() const
{
	return Alt_Grab;
}

void Virtual_Machine::Use_Alt_Grab( bool use )
{
	Alt_Grab = use;
}

bool Virtual_Machine::Use_No_Quit() const
{
	return No_Quit;
}

void Virtual_Machine::Use_No_Quit( bool use )
{
	No_Quit = use;
}

bool Virtual_Machine::Use_Portrait() const
{
	return Portrait;
}

void Virtual_Machine::Use_Portrait( bool use )
{
	Portrait = use;
}

bool Virtual_Machine::Use_Show_Cursor() const
{
	return Show_Cursor;
}

void Virtual_Machine::Use_Show_Cursor( bool use )
{
	Show_Cursor = use;
}

bool Virtual_Machine::Use_Curses() const
{
	return Curses;
}

void Virtual_Machine::Use_Curses( bool use )
{
	Curses = use;
}

bool Virtual_Machine::Use_RTC_TD_Hack() const
{
	return RTC_TD_Hack;
}

void Virtual_Machine::Use_RTC_TD_Hack( bool use )
{
	RTC_TD_Hack = use;
}

bool Virtual_Machine::Use_Start_Date() const
{
	return Start_Date;
}

void Virtual_Machine::Use_Start_Date( bool use )
{
	Start_Date = use;
}

const QDateTime &Virtual_Machine::Get_Start_Date() const
{
	return Start_DateTime;
}

void Virtual_Machine::Set_Start_Date( const QDateTime &dt )
{
	Start_DateTime = dt;
}

const VM_SPICE &Virtual_Machine::Get_SPICE() const
{
	return SPICE;
}

void Virtual_Machine::Set_SPICE( const VM_SPICE &spice )
{
	SPICE = spice;
}

bool Virtual_Machine::Use_VNC() const
{
	return VNC;
}

void Virtual_Machine::Use_VNC( bool use )
{
	VNC = use;
}

bool Virtual_Machine::Get_VNC_Socket_Mode() const
{
	return VNC_Socket_Mode;
}

void Virtual_Machine::Set_VNC_Socket_Mode( bool use )
{
	VNC_Socket_Mode = use;
}

const QString &Virtual_Machine::Get_VNC_Unix_Socket_Path() const
{
	return VNC_Unix_Socket_Path;
}

void Virtual_Machine::Set_VNC_Unix_Socket_Path( const QString &path )
{
	VNC_Unix_Socket_Path = path;
}

int Virtual_Machine::Get_VNC_Display_Number() const
{
	return VNC_Display_Number;
}

void Virtual_Machine::Set_VNC_Display_Number( int num )
{
	VNC_Display_Number = num;
}

bool Virtual_Machine::Use_VNC_Password() const
{
	return VNC_Password;
}

void Virtual_Machine::Use_VNC_Password( bool use )
{
	VNC_Password = use;
}

void Virtual_Machine::Set_VNC_Password( const QString &pas )
{
	Execute_Emu_Ctl_Command( pas );
}

bool Virtual_Machine::Use_VNC_TLS() const
{
	return VNC_TLS;
}

void Virtual_Machine::Use_VNC_TLS( bool use )
{
	VNC_TLS = use;
}

bool Virtual_Machine::Use_VNC_x509() const
{
	return VNC_x509;
}

void Virtual_Machine::Use_VNC_x509( bool use )
{
	VNC_x509 = use;
}

const QString &Virtual_Machine::Get_VNC_x509_Folder_Path() const
{
	return VNC_x509_Folder_Path;
}

void Virtual_Machine::Set_VNC_x509_Folder_Path( const QString &path )
{
	VNC_x509_Folder_Path = path;
}

bool Virtual_Machine::Use_VNC_x509verify() const
{
	return VNC_x509verify;
}

void Virtual_Machine::Use_VNC_x509verify( bool use )
{
	VNC_x509verify = use;
}

const QString &Virtual_Machine::Get_VNC_x509verify_Folder_Path() const
{
	return VNC_x509verify_Folder_Path;
}

void Virtual_Machine::Set_VNC_x509verify_Folder_Path( const QString &path )
{
	VNC_x509verify_Folder_Path = path;
}

int Virtual_Machine::Get_Embedded_Display_Port() const
{
	return Embedded_Display_Port;
}

void Virtual_Machine::Set_Embedded_Display_Port( int port )
{
	Embedded_Display_Port = port;
}

void Virtual_Machine::Parse_StdOut()
{
	QString convOutput = "";

	#ifndef Q_OS_WIN32
	if( Use_Monitor_TCP == false )
		convOutput = QEMU_Process->readAllStandardOutput();
	else
	#endif
		convOutput = Monitor_Socket->readAll();

    // For whatever reason qemu doesn't write all errors to stderr,
    // which means we unfortunately need to filter output to stdout
    // for errors. This is extremely bad design by qemu.
    QRegularExpression re("Option .* not supported", QRegularExpression::CaseInsensitiveOption);
    if ( re.match(convOutput).hasMatch() )
    {
        Show_QEMU_Error( convOutput );
    }

	QStringList splitOutput = convOutput.split( "[K" );
	QString cleanOutput = splitOutput.last().remove( QRegExp("\[[KD].") );
	
	emit Clean_Console( cleanOutput );
	emit Ready_StdOut( cleanOutput );
	//Last_Output.append( convOutput );
	//Output_Parts = Last_Output.split( "(qemu)" );
	
	// Find devices list?
	if( Update_Removable_Devices_Mode )
	{
		if( cleanOutput.contains("(qemu) ") )
		{
			Update_Removable_Devices_Mode = false;
			Removable_Devices_List += cleanOutput;
			emit Ready_Removable_Devices_List();
		}
		else
		{
			Removable_Devices_List += cleanOutput;
		}
	}
}

void Virtual_Machine::Parse_StdErr()
{
	// FIXME in monitor tcp mode no possible get error strings
	QString convOutput = QEMU_Process->readAllStandardError();
	
	emit Clean_Console( convOutput );
	emit Ready_StdErr( convOutput );
	//Last_Output.append( convOutput );
	Show_QEMU_Error( convOutput );
	
	/* FIXME
	QStringList splitOutput = convOutput.split( "[K" );
	
	if( splitOutput.last() == splitOutput.first() )
	{
		emit Clean_Console( convOutput.trimmed() );
		emit Ready_StdErr( convOutput.simplified() );
		Last_Output.append( convOutput.simplified() );
	}
	else
	{
		if( ! splitOutput.last().isEmpty() )
		{
			QString cleanOutput = splitOutput.last().remove( QRegExp("\[[KD].") );
			emit Clean_Console( cleanOutput.trimmed() );
			emit Ready_StdErr( cleanOutput.simplified() );
			Last_Output.append( convOutput.simplified() );
		}
	}
	
	Output_Parts = Last_Output.split( "(qemu)" );*/
}

void Virtual_Machine::Send_Emulator_Command( const QString &text )
{
	#ifndef Q_OS_WIN32
	if( Use_Monitor_TCP == false )
	{
		QEMU_Process->write( qPrintable(text) );
	}
	else
	#endif
	{
		if( Monitor_Socket->state() != QAbstractSocket::ConnectedState )
		{
			AQError( "void Virtual_Machine::Send_Emulator_Command( const QString &text )",
					 "Monitor socket not connected!" );
		}
		else
		{
			Monitor_Socket->write( qPrintable(text) );
		}
	}
}

void Virtual_Machine::QEMU_Started()
{
	AQDebug( "void Virtual_Machine::QEMU_Started()",
			 "QEMU Start" );
	
	if( Start_CPU )
	{
		Set_State( VM::VMS_Running );
	}
	
	if( ! Load_Mode )
	{
		AQDebug( "void Virtual_Machine::QEMU_Started()",
				 "emit Loading_Complete()" );
		emit Loading_Complete();
	}
	
	if( ! Settings.value("Run_Before_QEMU", "").toString().isEmpty() )
	{
		QProcess *before_proc = new QProcess();
		before_proc->start( Settings.value("Run_Before_QEMU", "").toString() );
	}
	
	// Connect monitor?
	#ifndef Q_OS_WIN32
	if( Use_Monitor_TCP == true )
	#endif
	{
		Monitor_Socket->connectToHost( Monitor_Hostname, Monitor_Port, QIODevice::ReadWrite );
	}

	emit QEMU_Start();
}

void Virtual_Machine::QEMU_Finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	AQDebug( "void Virtual_Machine::QEMU_Finished( int exitCode, QProcess::ExitStatus exitStatus )" ,
			 "QEMU Finished" );
	
	emit QEMU_End();
	
	Start_Snapshot_Tag = "";
	Set_State( VM::VMS_Power_Off );
	
    if (exitStatus == QProcess::CrashExit)
	{
		AQError( "QEMU Crashed!", "QEMU Crashed!" );
	}
    else if ( (exitCode != 0) ) 
    {
        QString error = QEMU_Process->readAll();
        AQError( "QEMU return value != 0", error );

        Show_QEMU_Error( error );
    }
	else
	{
		AQDebug( "void Virtual_Machine::QEMU_Finished( int exitCode, QProcess::ExitStatus exitStatus )",
				 "QEMU Closed" );
	}

    // Add VM USB devices to used USB list
    if( USB_Ports.count() > 0 )
    {
	    foreach( VM_USB usb_dev, USB_Ports ) System_Info::Delete_From_Used_USB_List( usb_dev );
    }

	if( ! Settings.value("Run_After_QEMU", "").toString().isEmpty() )
	{
		QProcess *after_proc = new QProcess();
		after_proc->start( Settings.value("Run_After_QEMU", "").toString() );
	}
}

void Virtual_Machine::Resume_Finished( const QString &returned_text )
{
	AQDebug( "void Virtual_Machine::Resume_Finished( const QString &returned_text )",
			 QString("Data: \"%1\"").arg(returned_text) );
	
	if( returned_text.contains("(qemu)") )
	{
		disconnect( this, SIGNAL(Ready_StdOut(const QString&)),
					this, SLOT(Resume_Finished(const QString&)) );
		
		Dont_Reinit = true;
		
		Hide_VM_Load_Window();
	}
	
    // might need to reconnect the usb tablet here...
}

void Virtual_Machine::Suspend_Finished( const QString &returned_text )
{
	AQDebug( "void Virtual_Machine::Suspend_Finished( const QString &returned_text )",
			 QString("Data: \"%1\"").arg(returned_text) );
	
	if( returned_text.simplified() == "(qemu)" )
	{
		disconnect( this, SIGNAL(Ready_StdOut(const QString&)),
					this,SLOT(Suspend_Finished(const QString&)) );
		
		Hide_VM_Save_Window();
		
		if( Quit_Before_Save )
		{
			Send_Emulator_Command( "quit\n" );
			Hide_Emu_Ctl_Win();
			Set_State( VM::VMS_Saved );
		}
		
		if( ! Save_VM() )
			AQError( "void Virtual_Machine::Save_VM_State( const QString &tag )",
					 "Saving Not Complete!" );
	}
	else if( returned_text.contains("No block device can accept snapshots") )
	{
		disconnect( this, SIGNAL(Ready_StdOut(const QString&)),
					this,SLOT(Suspend_Finished(const QString&)) );
		
		Hide_VM_Save_Window();
		AQGraphic_Warning( tr("Error!"),
						   tr("You must add HDD image in QCOW2 format to save virtual machine!") );
	}
}

void Virtual_Machine::Started_Booting( const QString &text )
{
	if( text.contains("VNC") || text.contains("Server") )
	{
		disconnect( this, SIGNAL(Ready_StdOut(const QString&) ),
				    this, SLOT(Started_Booting(const QString&)) );
		
		AQDebug( "void Virtual_Machine::Started_Booting( const QString &text )",
				 "emit Loading_Complete()" );
		emit Loading_Complete();
		Hide_VM_Load_Window();
	}
}

void Virtual_Machine::Execute_Emu_Ctl_Command( const QString &com )
{
	Send_Emulator_Command( qPrintable(com + "\n") );
	
	AQDebug( "void Virtual_Machine::Execute_Emu_Ctl_Command( const QString &com )",
			 "Run: " + com );
}

class Setting_Or_Preview_Helper
{
    public:
        Setting_Or_Preview_Helper(bool _preview)
        {
            preview = _preview;
        }

        bool either(bool settings)
        {
            if ( preview )
                return true;

            if ( settings )
                return true;

            return false;
        }

        bool setting(bool settings)
        {
            if ( preview )
                return false;

            if ( settings )
                return true;

            return false;
        }

        bool preview;
};

QString Virtual_Machine::GenerateHTMLInfoText(int info_mode)
{
    QTextEdit textedit;

    #define INFO_TEXT_INDENT 25

    if( info_mode != 0 && info_mode < 3)
    {
        // This is for the Tab Info background color
        QPalette qpal;
        textedit.setHtml( "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head>"
                                         "<body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">"
                                         "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>" );

        QTextCursor cursor = textedit.textCursor();
        QTextCharFormat format = QTextCharFormat();

        QTextCharFormat bold_format = format;
        bold_format.setFontWeight( QFont::Bold );

        if ( calculateContrast(qApp->palette().color(QPalette::Window),qApp->palette().color(QPalette::Link)) > 3.0 )
            bold_format.setForeground( qpal.color(QPalette::Link) );
        else
            bold_format.setForeground( qpal.color(QPalette::WindowText) );

        QTextTableFormat table_format;

        table_format.setAlignment( Qt::AlignLeft );
        table_format.setCellPadding( 0 );
        table_format.setCellSpacing( 0 );

        QVector<QTextLength> constraints;
        constraints << QTextLength( QTextLength::FixedLength, INFO_TEXT_INDENT ) << QTextLength( QTextLength::FixedLength, 220 );
        table_format.setColumnWidthConstraints(constraints);

        if( info_mode == 1 ) cursor.insertText( tr("You must create a new virtual machine"), bold_format );
        else if( info_mode == 2 ) cursor.insertText( tr("This VM uses the emulator \"%1\" that is not installed.\n"
                                                        "The VM cannot work!").arg( (Get_Machine_Accelerator() == VM::TCG) ? "TCG" : "KVM" ), bold_format );

        cursor.insertBlock();

        return textedit.toHtml();
    }

    Setting_Or_Preview_Helper soph(info_mode == 3 );

    // This is for the Tab Info background color
    QPalette qpal;
    textedit.setHtml( "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head>"
                                     "<body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\">"
                                     "<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"></p></body></html>" );

    QTextCursor cursor = textedit.textCursor();
    QTextCharFormat format = QTextCharFormat();
    QTextFrame *topFrame = cursor.currentFrame();

    // Use Scrennshot in Save Mode
    if( Settings.value("Info/Show_Screenshot_in_Save_Mode", "no").toString() == "yes" )
    {
        // Find Full Size Screenshot
        QString img_path = QDir::toNativeSeparators( Settings.value("VM_Directory", "~").toString() +
                Get_FS_Compatible_VM_Name(Get_Machine_Name()) + "_screenshot" );

        if( ! QFile::exists(img_path) )
        {
            AQDebug( "void Main_Window::Update_Info_Text( int info_mode )",
                     "Screenshot Path is Empty!" );
        }
        else
        {
            /*cursor.insertHtml( "<img width=\"" +
                               QString::number(width() - ui.Machines_List->width() -50) +
                               "\" src=\"" + img_path + "\">" );*/
            cursor.insertHtml( "<img width=\"100%\" src=\"" + img_path + "\">" );

            return textedit.toHtml();
        }
    }

    QTextCharFormat bold_format = format;
    bold_format.setFontWeight( QFont::Bold );
    if ( calculateContrast(qApp->palette().color(QPalette::Window),qApp->palette().color(QPalette::Link)) > 3.0 )
        bold_format.setForeground( qpal.color(QPalette::Link) );
    else
        bold_format.setForeground( qpal.color(QPalette::WindowText) );

    QTextTableFormat table_format;

    table_format.setAlignment( Qt::AlignLeft );
    table_format.setCellPadding( 0 );
    table_format.setCellSpacing( 0 );

    QVector<QTextLength> constraints;
    constraints << QTextLength( QTextLength::FixedLength, INFO_TEXT_INDENT )
                << QTextLength( QTextLength::FixedLength, 220 )
                << QTextLength( QTextLength::VariableLength, 250 );
    table_format.setColumnWidthConstraints(constraints);

    // Vairables
    QTextTable *table;
    QTextFrame *frame;
    QTextFrameFormat frame_format;
    QTextTableCell cell;
    QTextCursor cell_cursor;

    // Machine State
    if( soph.either( Settings.value("Info/Machine_Details", "yes").toString() == "yes" ) )
    {

        cursor.insertText( tr("Machine"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );
        frame = cursor.currentFrame();

        frame_format = frame->frameFormat();
        frame_format.setBorder( 0 );
        frame->setFrameFormat( frame_format );

        if( soph.either ( Settings.value("Info/Machine_Name", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Name:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Get_Machine_Name(), format );
            table->insertRows( table->rows(), 1 );
        }

        if ( ! soph.preview )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("State:"), format );

            QString state_text = Get_State_Text();

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( state_text, format );
            table->insertRows( table->rows(), 1 );
        }
    }
    else
    {
        // this code is mainly here to prevent a segfault
        // it's a temporary hack, since all of the code in this method
        // should really be refactored/rewritten.

        table = cursor.insertTable( 1, 3, table_format );
        frame = cursor.currentFrame();

        frame_format = frame->frameFormat();
        frame_format.setBorder( 0 );
        frame->setFrameFormat( frame_format );
    }

    // General Tab
    if( Settings.value("Info/Machine_Accelerator", "yes").toString() == "yes" ||
        Settings.value("Info/Emulator_Version", "no").toString() == "yes" ||
        Settings.value("Info/Computer_Type", "yes").toString() == "yes" ||
        Settings.value("Info/Machine_Type", "no").toString() == "yes" ||
        Settings.value("Info/Boot_Priority", "yes").toString() == "yes" ||
        Settings.value("Info/CPU_Type", "no").toString() == "yes" ||
        Settings.value("Info/Number_of_CPU", "yes").toString() == "yes" ||
        Settings.value("Info/Video_Card", "yes").toString() == "yes" ||
        Settings.value("Info/Keyboard_Layout", "no").toString() == "yes" ||
        Settings.value("Info/Memory_Size", "yes").toString() == "yes" ||
        Settings.value("Info/Use_Sound", "yes").toString() == "yes" ||
        Settings.value("Info/Fullscreen", "yes").toString() == "yes" ||
        Settings.value("Info/Snapshot", "yes").toString() == "yes" ||
        Settings.value("Info/Localtime", "yes").toString() == "yes" ||
        soph.preview )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("General"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );

        frame = cursor.currentFrame();
        frame->setFrameFormat( frame_format );

        if( soph.either( Settings.value("Info/Machine_Accelerator", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Accelerator:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( VM::Accel_To_String(Get_Machine_Accelerator()).toUpper(), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either( Settings.value("Info/Computer_Type", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Architecture:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            QString name;
            if ( Get_Current_Emulator_Devices()->System.QEMU_Name == Get_Computer_Type() &&
                 ! Get_Current_Emulator_Devices()->System.Caption.isEmpty() )
            {
                name = Get_Current_Emulator_Devices()->System.Caption;
            }
            if ( Get_Computer_Type().isEmpty() )
                name = tr("Default");

            cell_cursor.insertText( name, format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either( Settings.value("Info/Machine_Type", "no").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Machine Type:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            QString name;
            for ( int i = 0; i < Get_Current_Emulator_Devices()->Machine_List.count(); i++ )
            {
                if ( Get_Current_Emulator_Devices()->Machine_List.at(i).QEMU_Name == Get_Machine_Type() &&
                     ! Get_Current_Emulator_Devices()->Machine_List.at(i).Caption.isEmpty() )
                {
                    name = Get_Current_Emulator_Devices()->Machine_List.at(i).Caption;
                }
            }
            if ( Get_Machine_Type().isEmpty() )
                name = tr("Default");

            cell_cursor.insertText( name, format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either ( Settings.value("Info/Boot_Priority", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Boot Priority:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( VM::Boot_Order_To_String_List(Get_Boot_Order_List()).join("/"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either ( Settings.value("Info/CPU_Type", "no").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("CPU Type:"), format );

            QString name;
            for ( int i = 0; i < Get_Current_Emulator_Devices()->CPU_List.count(); i++ )
            {
                if ( Get_Current_Emulator_Devices()->CPU_List.at(i).QEMU_Name == Get_CPU_Type() &&
                     ! Get_Current_Emulator_Devices()->CPU_List.at(i).Caption.isEmpty() )
                {
                    name = Get_Current_Emulator_Devices()->CPU_List.at(i).Caption;
                }
            }
            if ( Get_CPU_Type().isEmpty() )
                name = tr("Default");

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( name, format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either ( Settings.value("Info/Number_of_CPU", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Number of CPU:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( QString::number(Get_SMP_CPU_Count()), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either ( Settings.value("Info/Video_Card", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Video Card:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            QString name;
            for ( int i = 0; i < Get_Current_Emulator_Devices()->Video_Card_List.count(); i++ )
            {
                if ( Get_Current_Emulator_Devices()->Video_Card_List.at(i).QEMU_Name == Get_Video_Card() &&
                     ! Get_Current_Emulator_Devices()->Video_Card_List.at(i).Caption.isEmpty() )
                {
                    name = Get_Current_Emulator_Devices()->Video_Card_List.at(i).Caption;
                }
            }
            if ( Get_Video_Card().isEmpty() )
                name = tr("Default");

            cell_cursor.insertText( name, format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either( Settings.value("Info/Keyboard_Layout", "no").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Keyboard Layout:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Get_Keyboard_Layout(), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either( Settings.value("Info/Memory_Size", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Memory Size:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( QString::number(Get_Memory_Size()), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.either ( Settings.value("Info/Use_Sound", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Use Sound:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            if( Get_Audio_Cards().isEnabled() )
            {
                cell_cursor.insertText( tr("Yes"), format );
            }
            else
            {
                cell_cursor.insertText( tr("No"), format );
            }

            table->insertRows( table->rows(), 1 );
        }

        if( soph.setting( Settings.value("Info/Fullscreen", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Fullscreen Mode:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Fullscreen_Mode() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.setting( Settings.value("Info/Snapshot", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Use Snapshot Mode:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Snapshot_Mode() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( soph.setting( Settings.value("Info/Localtime", "yes").toString() == "yes" ) )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Use Local Time:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Local_Time() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }
    }

    // FDD/CD/HDD
    if( ( soph.either( Settings.value("Info/Show_FDD", "yes").toString() == "yes" ) &&
            (Get_FD0().Get_Enabled() ||
             Get_FD1().Get_Enabled() ||
             Get_Storage_Devices_List().count() > 0)) ||

        ( soph.either( Settings.value("Info/Show_CD", "yes").toString() == "yes" ) &&
            (Get_CD_ROM().Get_Enabled() ||
             Get_Storage_Devices_List().count() > 0)) ||

        ( soph.either( Settings.value("Info/Show_HDD", "yes").toString() == "yes" ) &&
            (Get_HDA().Get_Enabled() ||
             Get_HDB().Get_Enabled() ||
             Get_HDC().Get_Enabled() ||
             Get_HDD().Get_Enabled() ||
             Get_Storage_Devices_List().count() > 0)) )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("Devices"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );

        frame = cursor.currentFrame();
        frame->setFrameFormat( frame_format );

        if( true /*Settings.value("Use_Device_Manager", "no").toString() == "yes"*/ )
        {
            QFileInfo fi;

            if( soph.either ( Settings.value("Info/Show_FDD", "yes").toString() == "yes" ) )
            {
                if( Get_FD0().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Floppy 1:"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();

                    fi = QFileInfo( Get_FD0().Get_File_Name() );

                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }

                if( Get_FD1().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Floppy 2:"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    fi = QFileInfo( Get_FD1().Get_File_Name() );

                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }
            }

            if( soph.either ( Settings.value("Info/Show_CD", "yes").toString() == "yes" ) )
            {
                if( Get_CD_ROM().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("CD/DVD-ROM:"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    fi = QFileInfo( Get_CD_ROM().Get_File_Name() );

                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }
            }

            if( soph.either ( Settings.value("Info/Show_HDD", "yes").toString() == "yes" ) )
            {
                if( Get_HDA().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Primary Master (HDA):"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    fi = QFileInfo( Get_HDA().Get_File_Name() );
                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }

                if( Get_HDB().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Primary Slave (HDB):"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    fi = QFileInfo( Get_HDB().Get_File_Name() );
                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }

                if( Get_HDC().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Secondary Master (HDC):"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    fi = QFileInfo( Get_HDC().Get_File_Name() );
                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }

                if( Get_HDD().Get_Enabled() )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Secondary Slave (HDD):"), format );

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    fi = QFileInfo( Get_HDD().Get_File_Name() );
                    cell_cursor.insertText( fi.fileName(), format );
                    table->insertRows( table->rows(), 1 );
                }
            }
        }
    }

    // Network
    if( soph.either( Settings.value("Info/Network_Cards", "yes").toString() == "yes" ) &&
        ((Use_Native_Network() == false && Get_Network_Cards_Count() > 0) ||
         (Use_Native_Network() == true  && Get_Network_Cards_Nativ().count() > 0)) )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("Network"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );

        frame = cursor.currentFrame();
        frame->setFrameFormat( frame_format );

        if( Get_Use_Network() == false )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Use Network:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }
        else
        {
            if( Use_Native_Network() == false )
            {
                for( int nx = 0; nx < Get_Network_Cards_Count(); ++nx )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Card: ") + QString::number(nx+1), format );

                    QString con_mode = "";

                    switch( Get_Network_Card(nx).Get_Net_Mode() )
                    {
                        case VM::Net_Mode_Usermode:
                            con_mode = tr("User mode network stack");
                            break;

                        case VM::Net_Mode_Tuntap:
                            con_mode = tr("TUN/TAP Interface");
                            break;

                        case VM::Net_Mode_Tuntapfd:
                            con_mode = tr("TUN/TAP Interface");
                            break;

                        case VM::Net_Mode_Tcplisten:
                            con_mode = tr("TCP Socket");
                            break;

                        case VM::Net_Mode_Tcpfd:
                            con_mode = tr("TCP Socket");
                            break;

                        case VM::Net_Mode_Tcpconnect:
                            con_mode = tr("VLAN");
                            break;

                        case VM::Net_Mode_Multicast:
                            con_mode = tr("UDP multicast socket");
                            break;

                        case VM::Net_Mode_Multicastfd:
                            con_mode = tr("UDP multicast socket");
                            break;

                        default:
                            con_mode = tr("No Connection");
                            break;
                    }

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( con_mode, format );
                    table->insertRows( table->rows(), 1 );
                }
            }
            else
            {
                QList<VM_Net_Card_Native> Native_Cards = Get_Network_Cards_Nativ();

                for( int ix = 0; ix < Native_Cards.count(); ix++ )
                {
                    cell = table->cellAt( table->rows()-1, 1 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( tr("Item %1").arg(ix+1), format );

                    QString con_mode = "";

                    switch( Native_Cards[ix].Get_Network_Type() )
                    {
                        case VM::Net_Mode_Native_NIC:
                            con_mode = tr( "NIC" );
                            break;

                        case VM::Net_Mode_Native_User:
                            con_mode = tr( "User" );
                            break;

                        case VM::Net_Mode_Native_Chanel:
                            con_mode = tr( "Channel" );
                            break;

                        case VM::Net_Mode_Native_TAP:
                            con_mode = tr( "TAP" );
                            break;

                        case VM::Net_Mode_Native_Socket:
                            con_mode = tr( "Socket" );
                            break;

                        case VM::Net_Mode_Native_MulticastSocket:
                            con_mode = tr( "Multicast Socket" );
                            break;

                        case VM::Net_Mode_Native_VDE:
                            con_mode = tr( "VDE" );
                            break;

                        case VM::Net_Mode_Native_Dump:
                            con_mode = tr( "Dump" );
                            break;

                        default:
                            con_mode = tr( "Unknown Type Item" );
                            break;
                    }

                    cell = table->cellAt( table->rows()-1, 2 );
                    cell_cursor = cell.firstCursorPosition();
                    cell_cursor.insertText( con_mode, format );
                    table->insertRows( table->rows(), 1 );
                }
            }
        }
    }

    // Network Redirections
    if( soph.setting ( Settings.value("Info/Redirections", "no").toString() == "yes" ) )
    {
        if( Get_Use_Redirections() ||
            Get_Network_Redirections_Count() < 1 )
        {
            for( int rx = 0; rx < Get_Network_Redirections_Count(); ++rx )
            {
                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("Redirection ") + QString::number(rx+1) + ":", format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText(
                        Get_Network_Redirection(rx).Get_Protocol() + " " +
                        QString::number(Get_Network_Redirection(rx).Get_Host_Port()) + ":" +
                        Get_Network_Redirection(rx).Get_Guest_IP() + ":" +
                        QString::number(Get_Network_Redirection(rx).Get_Guest_Port()), format );
                table->insertRows( table->rows(), 1 );
            }
        }
        else
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Use Redirections:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }
    }

    // Ports Tab
    if( ( soph.setting( Settings.value("Info/Serial_Port", "yes").toString() == "yes" ) && Get_Serial_Ports().count() > 0) ||
        ( soph.setting( Settings.value("Info/Parallel_Port", "yes").toString() == "yes" ) && Get_Parallel_Ports().count() > 0) ||
        ( soph.setting( Settings.value("Info/USB_Port", "yes").toString() == "yes" ) && Get_USB_Ports().count() > 0) )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("Ports"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );

        frame = cursor.currentFrame();
        frame->setFrameFormat( frame_format );

        if( Settings.value("Info/Serial_Port", "yes").toString() == "yes" )
        {
            for( int ix = 0; ix < Get_Serial_Ports().count(); ix++ )
            {
                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("Serial Port %1:").arg(ix), format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();

                switch( Get_Serial_Ports()[ix].Get_Port_Redirection() )
                {
                    case VM::PR_Default:
                        cell_cursor.insertText( tr("Default"), format );
                        break;

                    case VM::PR_vc:
                        cell_cursor.insertText( tr("vc"), format );
                        break;

                    case VM::PR_pty:
                        cell_cursor.insertText( tr("pty"), format );
                        break;

                    case VM::PR_none:
                        cell_cursor.insertText( tr("none"), format );
                        break;

                    case VM::PR_null:
                        cell_cursor.insertText( tr("null"), format );
                        break;

                    case VM::PR_dev:
                        cell_cursor.insertText( tr("dev"), format );
                        break;

                    case VM::PR_host_port:
                        cell_cursor.insertText( tr("host_port"), format );
                        break;

                    case VM::PR_file:
                        cell_cursor.insertText( tr("file"), format );
                        break;

                    case VM::PR_stdio:
                        cell_cursor.insertText( tr("stdio"), format );
                        break;

                    case VM::PR_pipe:
                        cell_cursor.insertText( tr("pipe"), format );
                        break;

                    case VM::PR_udp:
                        cell_cursor.insertText( tr("udp"), format );
                        break;

                    case VM::PR_tcp:
                        cell_cursor.insertText( tr("tcp"), format );
                        break;

                    case VM::PR_telnet:
                        cell_cursor.insertText( tr("telnet"), format );
                        break;

                    case VM::PR_unix:
                        cell_cursor.insertText( tr("unix"), format );
                        break;

                    default:
                        cell_cursor.insertText( tr("Default"), format );
                        break;
                }

                table->insertRows( table->rows(), 1 );
            }
        }

        if( Settings.value("Info/Parallel_Port", "yes").toString() == "yes" )
        {
            for( int ix = 0; ix < Get_Parallel_Ports().count(); ix++ )
            {
                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("Parallel Port %1:").arg(ix), format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();

                switch( Get_Parallel_Ports()[ix].Get_Port_Redirection() )
                {
                case VM::PR_Default:
                    cell_cursor.insertText( tr("Default"), format );
                    break;

                case VM::PR_vc:
                    cell_cursor.insertText( tr("vc"), format );
                    break;

                case VM::PR_pty:
                    cell_cursor.insertText( tr("pty"), format );
                    break;

                case VM::PR_none:
                    cell_cursor.insertText( tr("none"), format );
                    break;

                case VM::PR_null:
                    cell_cursor.insertText( tr("null"), format );
                    break;

                case VM::PR_dev:
                    cell_cursor.insertText( tr("dev"), format );
                    break;

                case VM::PR_host_port:
                    cell_cursor.insertText( tr("host_port"), format );
                    break;

                case VM::PR_file:
                    cell_cursor.insertText( tr("file"), format );
                    break;

                case VM::PR_stdio:
                    cell_cursor.insertText( tr("stdio"), format );
                    break;

                case VM::PR_pipe:
                    cell_cursor.insertText( tr("pipe"), format );
                    break;

                case VM::PR_udp:
                    cell_cursor.insertText( tr("udp"), format );
                    break;

                case VM::PR_tcp:
                    cell_cursor.insertText( tr("tcp"), format );
                    break;

                case VM::PR_telnet:
                    cell_cursor.insertText( tr("telnet"), format );
                    break;

                case VM::PR_unix:
                    cell_cursor.insertText( tr("unix"), format );
                    break;

                default:
                    cell_cursor.insertText( tr("Default"), format );
                    break;
                }

                table->insertRows( table->rows(), 1 );
            }
        }

        if( Settings.value("Info/USB_Port", "yes").toString() == "yes" )
        {
            for( int ix = 0; ix < Get_USB_Ports().count(); ix++ )
            {
                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("USB Port %1:").arg(ix), format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( Get_USB_Ports()[ix].Get_Manufacturer_Name() + " " +
                                        Get_USB_Ports()[ix].Get_Product_Name(), format );
                table->insertRows( table->rows(), 1 );
            }
        }
    }

    // Other Tab
    if( ( Settings.value("Info/Linux_Boot", "no").toString() == "yes" ||
        Settings.value("Info/ROM_File", "no").toString() == "yes" ||
        Settings.value("Info/MTDBlock", "no").toString() == "yes" ||
        Settings.value("Info/SD_Image", "no").toString() == "yes" ||
        Settings.value("Info/PFlash", "no").toString() == "yes" ||
        Settings.value("Info/VNC", "no").toString() == "yes" ||
        Settings.value("Info/SPICE", "no").toString() == "yes" ||
        Settings.value("Info/Acceleration", "no").toString() == "yes" ) && ! soph.preview )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("Other"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );

        frame = cursor.currentFrame();
        frame->setFrameFormat( frame_format );

        // Acceleration
        if( Settings.value("Info/Acceleration", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Acceleration:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            //FIXME //or remove?
            cell_cursor.insertText( tr("Use If Possible"), format );

            table->insertRows( table->rows(), 1 );
        }

        // VNC
        if( Settings.value("Info/VNC", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("VNC Port:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            if ( Use_VNC() )
                cell_cursor.insertText( QString::number( Get_VNC_Display_Number() + 5900 ) , format );
            else
                cell_cursor.insertText( QString::number( Settings.value("First_VNC_Port", "5910").toInt() + Get_Embedded_Display_Port() ) , format );

            table->insertRows( table->rows(), 1 );
        }

        // SPICE
        if( Settings.value("Info/SPICE", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("SPICE Port:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( QString::number(Get_SPICE().Get_Port()) , format );
            table->insertRows( table->rows(), 1 );
        }

        // Linux Boot
        if( Settings.value("Info/Linux_Boot", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Use Linux Boot:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Get_Use_Linux_Boot() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );

            if( Get_Use_Linux_Boot() )
            {
                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("Kernel bzImage:"), format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();
                QFileInfo lb_tmp = QFileInfo( Get_bzImage_Path() );
                cell_cursor.insertText( lb_tmp.fileName(), format );
                table->insertRows( table->rows(), 1 );

                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("Initrd File:"), format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();
                lb_tmp = QFileInfo( Get_Initrd_Path() );
                cell_cursor.insertText( lb_tmp.fileName(), format );
                table->insertRows( table->rows(), 1 );

                cell = table->cellAt( table->rows()-1, 1 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( tr("Kernel ComLine:"), format );

                cell = table->cellAt( table->rows()-1, 2 );
                cell_cursor = cell.firstCursorPosition();
                cell_cursor.insertText( Get_Kernel_ComLine() , format );
                table->insertRows( table->rows(), 1 );
            }
        }

        QFileInfo im_info;

        // ROM File
        if( Settings.value("Info/ROM_File", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("ROM File:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            if( Get_Use_ROM_File() )
            {
                im_info = QFileInfo( Get_ROM_File() );
                cell_cursor.insertText( im_info.fileName(), format );
            }
            else
            {
                cell_cursor.insertText( tr("No"), format );
            }

            table->insertRows( table->rows(), 1 );
        }

        // On-Board Flash Image
        if( Settings.value("Info/MTDBlock", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("On-Board Flash Image:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            if( Use_MTDBlock_File() )
            {
                im_info = QFileInfo( Get_MTDBlock_File() );
                cell_cursor.insertText( im_info.fileName(), format );
            }
            else
            {
                cell_cursor.insertText( tr("No"), format );
            }

            table->insertRows( table->rows(), 1 );
        }

        // SD Card Image
        if( Settings.value("Info/SD_Image", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("SD Card Image:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            if( Use_SecureDigital_File() )
            {
                im_info = QFileInfo( Get_SecureDigital_File() );
                cell_cursor.insertText( im_info.fileName(), format );
            }
            else
            {
                cell_cursor.insertText( tr("No"), format );
            }

            table->insertRows( table->rows(), 1 );
        }

        // Parallel Flash Image
        if( Settings.value("Info/PFlash", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Parallel Flash Image:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();

            if( Use_PFlash_File() )
            {
                im_info = QFileInfo( Get_PFlash_File() );
                cell_cursor.insertText( im_info.fileName(), format );
            }
            else
            {
                cell_cursor.insertText( tr("No"), format );
            }

            table->insertRows( table->rows(), 1 );
        }
    }

    // Advanced Tab
    if( ( Settings.value("Info/RTC_TD_Hack", "no").toString() == "yes" ||
        Settings.value("Info/Win2K_Hack", "no").toString() == "yes" ||
        Settings.value("Info/No_Shutdown", "no").toString() == "yes" ||
        Settings.value("Info/No_Reboot", "no").toString() == "yes" ||
        Settings.value("Info/Start_CPU", "no").toString() == "yes" ||
        Settings.value("Info/Check_Boot_on_FDD", "no").toString() == "yes" ||
        Settings.value("Info/ACPI", "no").toString() == "yes" ||
        (Settings.value("Info/Start_Date", "no").toString() == "yes" && Use_Start_Date()) ||
        Settings.value("Info/No_Frame", "no").toString() == "yes" ||
        Settings.value("Info/Alt_Grab", "no").toString() == "yes" ||
        Settings.value("Info/No_Quit", "no").toString() == "yes" ||
        Settings.value("Info/Portrait", "no").toString() == "yes" ||
        Settings.value("Info/Curses", "no").toString() == "yes" ||
        Settings.value("Info/Show_Cursor", "no").toString() == "yes" ||
        (Settings.value("Info/Init_Graphical_Mode", "no").toString() == "yes" && Get_Init_Graphic_Mode().Get_Enabled() ) )
        && ! soph.preview )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("Advanced"), bold_format );
        cursor.insertBlock();

        table = cursor.insertTable( 1, 3, table_format );

        frame = cursor.currentFrame();
        frame->setFrameFormat( frame_format );

        if( Settings.value("Info/RTC_TD_Hack", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("RTC TD Hack:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_RTC_TD_Hack() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Win2K_Hack", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Windows 2000 Hack:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Win2K_Hack() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/No_Shutdown", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("No Shutdown:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_No_Shutdown() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/No_Reboot", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("No Reboot:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_No_Reboot() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Start_CPU", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Start CPU at Startup:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Start_CPU() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Check_Boot_on_FDD", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Check Boot Sector on FDD:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Check_FDD_Boot_Sector() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/ACPI", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Enable ACPI:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_ACPI() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Start_Date", "no").toString() == "yes" &&
            Use_Start_Date() )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Start Date:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Get_Start_Date().toString(), format );
            table->insertRows( table->rows(), 1 );
        }

        // Advanced -> QEMU Window Options
        if( Settings.value("Info/No_Frame", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("No Frame:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_No_Frame() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Alt_Grab", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Alt Grab:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Alt_Grab() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/No_Quit", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("No Quit:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_No_Quit() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Portrait", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Portrait Mode:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Portrait() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Curses", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Curses:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Curses() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Show_Cursor", "no").toString() == "yes" )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Show Cursor:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( Use_Show_Cursor() ? tr("Yes") : tr("No"), format );
            table->insertRows( table->rows(), 1 );
        }

        if( Settings.value("Info/Init_Graphical_Mode", "no").toString() == "yes" &&
            Get_Init_Graphic_Mode().Get_Enabled() )
        {
            cell = table->cellAt( table->rows()-1, 1 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( tr("Initial Graphical Mode:"), format );

            cell = table->cellAt( table->rows()-1, 2 );
            cell_cursor = cell.firstCursorPosition();
            cell_cursor.insertText( QString("%1x%2x%3").arg(Get_Init_Graphic_Mode().Get_Width())
                                                       .arg(Get_Init_Graphic_Mode().Get_Height())
                                                       .arg(Get_Init_Graphic_Mode().Get_Depth()) , format );
            table->insertRows( table->rows(), 1 );
        }
    }

    // Show_QEMU_Arguments
    if( soph.setting ( Settings.value("Info/Show_QEMU_Args", "no").toString() == "yes" ) )
    {
        cursor.setPosition( topFrame->lastPosition() );
        cursor.insertText( tr("QEMU Arguments"), bold_format );
        cursor.insertBlock();

        QTextTableFormat table_format2;

        table_format2.setAlignment( Qt::AlignLeft );
        table_format2.setCellPadding( 0 );
        table_format2.setCellSpacing( 0 );

        QVector<QTextLength> constraints2;
        constraints2 << QTextLength( QTextLength::FixedLength, INFO_TEXT_INDENT )
                     << QTextLength( QTextLength::VariableLength, 500 )
                     << QTextLength( QTextLength::FixedLength, 10 );
        table_format2.setColumnWidthConstraints( constraints2 );

        QTextTable *table2 = cursor.insertTable( 1, 3, table_format2 );

        frame = cursor.currentFrame();
        QTextFrameFormat frame_format2 = frame->frameFormat();
        frame_format2.setBorder( 0 );
        frame->setFrameFormat( frame_format2 );

        cell = table2->cellAt( table2->rows()-1, 1 );
        cell_cursor = cell.firstCursorPosition();
        cell_cursor.insertText( Build_QEMU_Args_For_Tab_Info().join(" ").replace(" -"," \\\n    -"), format );
    }

    // Move the cursor to the top
    textedit.moveCursor(QTextCursor::Start);
    textedit.ensureCursorVisible();

    return textedit.toHtml();
}

//===========================================================================
