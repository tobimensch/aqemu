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

#ifndef VM_H
#define VM_H

#include <QDateTime>
#include <QProcess>
#include <QTcpSocket>
#include <QFile>

#include "VM_Devices.h"
#include "Error_Log_Window.h"
#include "Create_Template_Window.h"
#include "QDom.h"

class AQEMU_Service;
class Emulator_Control_Window;

// Vitrual Machine
class Virtual_Machine: public QObject
{
    friend class AQEMU_Service;

	Q_OBJECT

    // the following methods are private to give
    // AQEMU_Service exclusive access
    private:
        bool Start();
        void Pause(); // qemu command stop
        void Stop();  // qemu command quit
        void Shutdown();  // qemu command system_powerdown
        void Reset();
        void Save_VM_State(); // Save default snapshot
        void Save_VM_State( const QString &tag, bool quit );
        void Load_VM_State( const QString &tag );

	public:
		// Constructors...
		Virtual_Machine();
		Virtual_Machine( const QString &name );
		Virtual_Machine( const Virtual_Machine &vm );
		~Virtual_Machine();
		void Shared_Constructor();
		
		// oprators
		bool operator==( const Virtual_Machine &v1 ) const;
		bool operator!=( const Virtual_Machine &v1 ) const;
		Virtual_Machine &operator=( const Virtual_Machine &vm );
		
		// Create, Read, Save VM XML Files
		const QString &Get_VM_XML_File_Path() const;
		void Set_VM_XML_File_Path( const QString &path );
		bool Create_VM_File( const QString &file_name, bool template_mode );
		
		bool Create_Template( const QString &vm_path, const QString &template_name,
							  Create_Template_Window::Template_Options opts );
		
		bool Load_VM( const QString &file_name );
		bool Save_VM();
		bool Save_VM( const QString &file_name );

        QString GenerateHTMLInfoText( int info_mode = 0 );
		
		QStringList Build_QEMU_Args(); // build all args for QEMU
		QStringList Build_QEMU_Args_For_Tab_Info();
		QStringList Build_QEMU_Args_For_Script();
		
        QStringList Build_Native_Device_Args( VM_Native_Storage_Device device, bool Build_QEMU_Args_for_Script_Mode );
		QStringList Build_Shared_Folder_Args( VM_Shared_Folder folder, int id, bool Build_QEMU_Args_for_Script_Mode );

		bool Start_Snapshot( const QString &tag );
		void Delete_Snapshot( const QString &tag );
		
		const QString &Get_UID() const;
		void Set_UID( const QString &uid );
		
		// Emu_Ctl
		void Show_Emu_Ctl_Win();
		void Hide_Emu_Ctl_Win();
		void Update_Removable_Devices_List();
		const QString &Get_Removable_Devices_List() const;
        void Show_Monitor_Window();
		
		// Error Log Window
		void Show_Error_Log_Window();
		
		// QEMU Log
		void Show_QEMU_Error( const QString &err_str );
		void Hide_QEMU_Error_Log();
		
		// Plase wait win
		void Show_VM_Load_Window();
		void Hide_VM_Load_Window();
		
		void Show_VM_Save_Window();
		void Hide_VM_Save_Window();
		
		// Screenshot
		bool Take_Screenshot( const QString &file_name, int width=-1, int height=-1 );
		
		VM::Machine_Accelerator Get_Machine_Accelerator() const;
		void Set_Machine_Accelerator( VM::Machine_Accelerator accel );
		
		const Emulator &Get_Emulator() const;
		void Set_Emulator( const Emulator &emul );
		
		void Update_Current_Emulator_Devices();
		const Available_Devices *Get_Current_Emulator_Devices() const;
		
		QString Get_Current_Emulator_Binary_Path( const QString &names ) const;
		
        QString Get_State_Text() const;
		VM::VM_State Get_State() const;
		void Set_State( VM::VM_State s, bool real_poewer_off = false );
		
		const QString &Get_Icon_Path() const;
		void Set_Icon_Path( const QString &file_name );
		
		const QString &Get_Screenshot_Path() const;
		void Set_Screenshot_Path( const QString &file_name );
		
		const QString &Get_Machine_Name() const;
		void Set_Machine_Name( const QString &name );
		
		const QString &Get_Computer_Type() const;
		void Set_Computer_Type( const QString &type );
		
		const QString &Get_Machine_Type() const;
		void Set_Machine_Type( const QString &type );
		
		const QString &Get_CPU_Type() const;
		void Set_CPU_Type( const QString &type );
		
		VM::SMP_Options Get_SMP() const;
		void Set_SMP( const VM::SMP_Options &smp );
		
		int Get_SMP_CPU_Count() const;
		void Set_SMP_CPU_Count( int count );
		
		const QString &Get_Keyboard_Layout() const;
		void Set_Keyboard_Layout( const QString &kl );
		
		const QList<VM::Boot_Order> &Get_Boot_Order_List() const;
		void Set_Boot_Order_List( QList<VM::Boot_Order> &list );
		
		bool Get_Show_Boot_Menu() const;
		void Set_Show_Boot_Menu( bool use );
		
		const QString &Get_Video_Card() const;
		void Set_Video_Card( const QString &card );
		
		VM::Sound_Cards Get_Audio_Cards() const;
		void Set_Audio_Cards( VM::Sound_Cards card );
		
		int Get_Memory_Size() const;
		void Set_Memory_Size( int megs );
		
		bool Get_Remove_RAM_Size_Limitation() const;
		void Set_Remove_RAM_Size_Limitation( bool on );
		
		bool Use_Fullscreen_Mode() const;
		void Use_Fullscreen_Mode( bool use );
		
		bool Use_Win2K_Hack() const;
		void Use_Win2K_Hack( bool use );
		
		bool Use_Local_Time() const;
		void Use_Local_Time( bool use );
		
		bool Use_Check_FDD_Boot_Sector() const;
		void Use_Check_FDD_Boot_Sector( bool use );
		
		bool Use_ACPI() const;
		void Use_ACPI( bool use );
		
		bool Use_Snapshot_Mode() const;
		void Use_Snapshot_Mode( bool use );
		
		bool Use_Start_CPU() const;
		void Use_Start_CPU( bool start );
		
		bool Use_No_Reboot() const;
		void Use_No_Reboot( bool use );
		
		bool Use_No_Shutdown() const;
		void Use_No_Shutdown( bool use );
		
		const VM_Storage_Device &Get_FD0() const;
		void Set_FD0( const VM_Storage_Device &floppy );
		
		const VM_Storage_Device &Get_FD1() const;
		void Set_FD1( const VM_Storage_Device &floppy );
		
		const VM_Storage_Device &Get_CD_ROM() const;
		void Set_CD_ROM( const VM_Storage_Device &cdrom );
		
		const VM_HDD &Get_HDA() const;
		void Set_HDA( const VM_HDD &hdd );
		
		const VM_HDD &Get_HDB() const;
		void Set_HDB( const VM_HDD &hdd );
		
		const VM_HDD &Get_HDC() const;
		void Set_HDC( const VM_HDD &hdd );
		
		const VM_HDD &Get_HDD() const;
		void Set_HDD( const VM_HDD &hdd );
		
		const QList<VM_Snapshot> &Get_Snapshots() const;
		void Set_Snapshots( const QList<VM_Snapshot> &list );
		
		void Add_Snapshot( const QString &tag, const QString &name, const QString &desc );
		void Add_Snapshot( const VM_Snapshot &snapshot );
		
		void Set_Snapshot( int index, const VM_Snapshot &s );
		
        const QList<VM_Native_Storage_Device> &Get_Storage_Devices_List() const;
        void Set_Storage_Devices_List( const QList<VM_Native_Storage_Device> &list );
		 		
        const VM_Native_Storage_Device &Get_Storage_Device( int index ) const;
        void Set_Storage_Device( int index, const VM_Native_Storage_Device &device );
		
		const QList<VM_Shared_Folder> &Get_Shared_Folders_List() const;
		void Set_Shared_Folders_List( const QList<VM_Shared_Folder> &list );

		bool Get_Use_Network() const;
		void Set_Use_Network( bool use );
		
        bool Use_Native_Network() const;
        void Use_Native_Network( bool use );
		
		bool Get_Use_Redirections() const;
		void Set_Use_Redirections( bool r );
		
		const QString &Get_TFTP_Prefix() const;
		void Set_TFTP_Prefix( const QString &p );
		
		const QString &Get_SMB_Directory() const;
		void Set_SMB_Directory( const QString &dir );
		
		const QList<VM_Net_Card> &Get_Network_Cards() const;
		void Set_Network_Cards( const QList<VM_Net_Card> &cards );
		
		const QList<VM_Net_Card_Native> &Get_Network_Cards_Native() const;
		void Set_Network_Cards_Native( const QList<VM_Net_Card_Native> &cards );
		
		const VM_Net_Card &Get_Network_Card( int index ) const;
		void Set_VM_Network_Card( int index, const VM_Net_Card &nc );
		
		bool Add_Network_Card( const VM_Net_Card &nc );
		bool Delete_Network_Card( int ix );
		
		void Clear_Network_Cards_List();
		
		int Get_Network_Cards_Count() const;
		
		const VM_Redirection &Get_Network_Redirection( int index ) const;
		void Set_Network_Redirection( int index, const VM_Redirection &r );
		
		void Add_Network_Redirection( const VM_Redirection &r );
		bool Delete_Network_Redirection( int ix );
		
		int Get_Network_Redirections_Count() const;
		
		const QList<VM_Port> &Get_Serial_Ports() const;
		void Set_Serial_Ports( const QList<VM_Port> &list );
		
		const QList<VM_Port> &Get_Parallel_Ports() const;
		void Set_Parallel_Ports( const QList<VM_Port> &list );
		
		void Add_USB_Port( const VM_USB &u );
		bool Delete_USB_Port( int index );
		
		const QList<VM_USB> &Get_USB_Ports() const;
		void Set_USB_Ports( const QList<VM_USB> &list );
		
		bool Get_Use_Linux_Boot() const;
		void Set_Use_Linux_Boot( bool use );
		
		const QString &Get_bzImage_Path() const;
		void Set_bzImage_Path( const QString &path );
		
		const QString &Get_Initrd_Path() const;
		void Set_Initrd_Path( const QString &path );
		
		const QString &Get_Kernel_ComLine() const;
		void Set_Kernel_ComLine( const QString &cl );
		
		const QString &Get_Additional_Args() const;
		void Set_Additional_Args( const QString &aa );
		
		bool Get_Only_User_Args() const;
		void Set_Only_User_Args( bool use );
		
		bool Get_Use_User_Emulator_Binary() const;
		void Set_Use_User_Emulator_Binary( bool use );
		
		bool Get_Use_ROM_File() const;
		void Set_Use_ROM_File( bool use );
		
		const QString &Get_ROM_File() const;
		void Set_ROM_File( const QString &path );
		
		bool Use_MTDBlock_File() const;
		void Use_MTDBlock_File( bool use );
		
		const QString &Get_MTDBlock_File() const;
		void Set_MTDBlock_File( const QString &file );
		
		bool Use_SecureDigital_File() const;
		void Use_SecureDigital_File( bool use );
		
		const QString &Get_SecureDigital_File() const;
		void Set_SecureDigital_File( const QString &file );
		
		bool Use_PFlash_File() const;
		void Use_PFlash_File( bool use );
		
		const QString &Get_PFlash_File() const;
		void Set_PFlash_File( const QString &file );
		
		bool Use_KVM() const;
		void Use_KVM( bool use );
		
		bool Use_KVM_IRQChip() const;
		void Use_KVM_IRQChip( bool use );
		
		bool Use_No_KVM_Pit() const;
		void Use_No_KVM_Pit( bool use );
		
		bool Use_KVM_No_Pit_Reinjection() const;
		void Use_KVM_No_Pit_Reinjection( bool use );
		
		bool Use_KVM_Nesting() const;
		void Use_KVM_Nesting( bool use );
		
		bool Use_KVM_Shadow_Memory() const;
		void Use_KVM_Shadow_Memory( bool use );
		
		int Get_KVM_Shadow_Memory_Size() const;
		void Set_KVM_Shadow_Memory_Size( int size );
		
		const VM_Init_Graphic_Mode &Get_Init_Graphic_Mode() const;
		void Set_Init_Graphic_Mode( const VM_Init_Graphic_Mode &mode );
		
		bool Use_No_Frame() const;
		void Use_No_Frame( bool use );
		
		bool Use_Alt_Grab() const;
		void Use_Alt_Grab( bool use );
		
		bool Use_No_Quit() const;
		void Use_No_Quit( bool use );
		
		bool Use_Portrait() const;
		void Use_Portrait( bool use );
		
		bool Use_Show_Cursor() const;
		void Use_Show_Cursor( bool use );
		
		bool Use_Curses() const;
		void Use_Curses( bool use );
		
		bool Use_RTC_TD_Hack() const;
		void Use_RTC_TD_Hack( bool use );
		
		bool Use_Start_Date() const;
		void Use_Start_Date( bool use );
		
		const QDateTime &Get_Start_Date() const;
		void Set_Start_Date( const QDateTime &dt );
		
		const VM_SPICE &Get_SPICE() const;
		void Set_SPICE( const VM_SPICE &spice );
		
		bool Use_VNC() const;
		void Use_VNC( bool use );
		
		bool Get_VNC_Socket_Mode() const;
		void Set_VNC_Socket_Mode( bool use );
		
		const QString &Get_VNC_Unix_Socket_Path() const;
		void Set_VNC_Unix_Socket_Path( const QString &path );
		
		int Get_VNC_Display_Number() const;
		void Set_VNC_Display_Number( int num );
		
		bool Use_VNC_Password() const;
		void Use_VNC_Password( bool use );
		
		void Set_VNC_Password( const QString &pas );
		
		bool Use_VNC_TLS() const;
		void Use_VNC_TLS( bool use );
		
		bool Use_VNC_x509() const;
		void Use_VNC_x509( bool use );
		
		const QString &Get_VNC_x509_Folder_Path() const;
		void Set_VNC_x509_Folder_Path( const QString &path );
		
		bool Use_VNC_x509verify() const;
		void Use_VNC_x509verify( bool use );
		
		const QString &Get_VNC_x509verify_Folder_Path() const;
		void Set_VNC_x509verify_Folder_Path( const QString &path );
		
		int Get_Embedded_Display_Port() const;
		void Set_Embedded_Display_Port( int port );
		
		// Window for control qemu/kvm
		Emulator_Control_Window *Emu_Ctl;
		
	signals:
		void State_Changed( Virtual_Machine *vm, VM::VM_State s );
		
		void Ready_StdIn( const QString &text );
		void Ready_StdOut( const QString &text );
		void Ready_StdErr( const QString &text );
		
		void QEMU_Start();
		void Loading_Complete();
		void QEMU_End();
		
		void Clean_Console( const QString &text );
		
		void Ready_Removable_Devices_List();
		
	private slots:
		void Parse_StdErr();
		void Parse_StdOut();
		
		void Send_Emulator_Command( const QString &text );

		void QEMU_Started();
		void QEMU_Finished( int exitCode, QProcess::ExitStatus exitStatus );
		
		void Resume_Finished( const QString &neturned_text );
		void Suspend_Finished( const QString &neturned_text );
		void Started_Booting( const QString &text );
		
		// for Emulator_Control_Window
		void Execute_Emu_Ctl_Command( const QString &com );
		
        VM_Native_Storage_Device Load_VM_Native_Storage_Device( const TXML2QDOM::QDomElement &Second_Element ) const;
		VM_Shared_Folder Load_VM_Shared_Folder( const TXML2QDOM::QDomElement &Second_Element ) const;
        void Save_VM_Native_Storage_Device( TXML2QDOM::QDomDocument &New_Dom_Document, TXML2QDOM::QDomElement &Dom_Element,
                                           const VM_Native_Storage_Device &device ) const;
		
		void Save_VM_Shared_Folder( TXML2QDOM::QDomDocument &New_Dom_Document, TXML2QDOM::QDomElement &Dom_Element,
										   const VM_Shared_Folder &device ) const;
	private:
        bool Start_impl();

		QProcess *QEMU_Process;

		QTcpSocket *Monitor_Socket; // Used for "-monitor tcp" connection type
		bool Use_Monitor_TCP; // This value set in VM start time and no changes after
		QString Monitor_Hostname;
		unsigned int Monitor_Port;

		VM::VM_State State; // Saved, Running, etc...
		VM::VM_State Old_State;
		QString VM_XML_File_Path; // for load and save
		QString Start_Snapshot_Tag;
		bool Load_Mode;
		bool Dont_Reinit;
		bool Build_QEMU_Args_for_Tab_Info;
		bool Build_QEMU_Args_for_Script_Mode;
		
		QString UID;
		
		VM::Machine_Accelerator Machine_Accelerator;
		Emulator Current_Emulator;
		Available_Devices Current_Emulator_Devices;
		
		QString Snapshot_Name_String;
		QString Last_Output;
		QStringList Output_Parts;
		
		QString Icon_Path; // OS Logo Icon
		QString Screenshot_Path; // Screenshot in save mode
		
		QString Computer_Type; // PC, MAC, ARM...
		QString Machine_Name; // this machine name
		QString Machine_Type; // All QEMU Mach Types
		QString CPU_Type; // x86, ppc, sparc...
		VM::SMP_Options SMP; // All SMP Settings
		QString Keyboard_Layout; // language en, ru, jp...
		QList<VM::Boot_Order> Boot_Order_List; // New boot order
		bool Show_Boot_Menu; // Enable interactive boot menu
        QString Video_Card; // std vga, cirus logic
		VM::Sound_Cards Audio_Card; // sb16, es1370
		bool Remove_RAM_Size_Limitation; // true - limitation off
		int Memory_Size; // RAM Size
		
		// General Tab Options
		bool Fullscreen;
		bool Win2K_Hack;
		bool Local_Time;
		bool Check_FDD_Boot_Sector;
		bool ACPI;
		bool Snapshot_Mode;
		bool Start_CPU;
		bool No_Reboot;
		bool No_Shutdown;
		
		// storage devices
		VM_Storage_Device FD0; // floppy 0
		VM_Storage_Device FD1; // floppy 1
		VM_Storage_Device CD_ROM; // cdrom device ide2
		VM_HDD HDA; // ide0
		VM_HDD HDB; // ide1
		VM_HDD HDC; // ide2 if cdrom is enabled, hdc be disabled
		VM_HDD HDD; // ide3
		QList<VM_Snapshot> Snapshots; // VM State Snapshots
        QList<VM_Native_Storage_Device> Storage_Devices; // For QEMU 0.8.2 Device Style
        int native_device_count; // to keep track of the number of native devices when building
                                 // the argument list

        // shared folders
        QList<VM_Shared_Folder> Shared_Folders;
		
		// Network
		bool Use_Network;
        bool Native_Network;
		bool Use_Redirections; // 1 or more net cards be avirable
		
		QList<VM_Net_Card> Network_Cards; // Network Cards. Max 8
		QList<VM_Net_Card_Native> Network_Cards_Native; // Native QEMU Network Cards. Max 8
		QList<VM_Redirection> Network_Redirections; // List Redirections
		
		QString TFTP_Prefix; // Buildin TFTP Server
		QString SMB_Directory; // SAMBA most be Installed on host!
		
		// Ports
		QList<VM_Port> Serial_Ports;
		QList<VM_Port> Parallel_Ports;
		QList<VM_USB> USB_Ports;
		
		// Other page
		bool Linux_Boot;
		QString bzImage_Path;
		QString Initrd_Path;
		QString Kernel_ComLine;
		
		QString Additional_Args; // user arguments
		bool Only_User_Args;
		bool Use_User_Emulator_Binary;
		
		bool Use_ROM_File;
		QString ROM_File; // ROM File for EtherBoot
		
		// QEMU 0.9.1 Options
		bool MTDBlock;
		QString MTDBlock_File;
		
		bool SecureDigital;
		QString SecureDigital_File;
		
		bool PFlash;
		QString PFlash_File;
		
		bool Enable_KVM;
		bool KVM_IRQChip;
		bool No_KVM_Pit;
		bool KVM_No_Pit_Reinjection;
		bool KVM_Nesting;
		bool KVM_Shadow_Memory;
		int KVM_Shadow_Memory_Size;
		
		VM_Init_Graphic_Mode Init_Graphic_Mode;
		
		bool No_Frame;
		bool Alt_Grab;
		bool No_Quit;
		bool Portrait;
		bool Show_Cursor;
		bool Curses;
		bool RTC_TD_Hack;
		
		// DateTime
		bool Start_Date;
		QDateTime Start_DateTime;
		
		// SPICE
		VM_SPICE SPICE;
		
		// VNC
		bool VNC;
		bool VNC_Socket_Mode;
		QString VNC_Unix_Socket_Path;
		int VNC_Display_Number;
		bool VNC_Password;
		bool VNC_TLS;
		bool VNC_x509;
		QString VNC_x509_Folder_Path;
		bool VNC_x509verify;
		QString VNC_x509verify_Folder_Path;
		
		// VNC Port for Embedded Display
		int Embedded_Display_Port;
		
		// Additional Windows
		QWidget *Load_VM_Window;
		QWidget *Save_VM_Window;
		Error_Log_Window* QEMU_Error_Win;
		bool Quit_Before_Save;
		bool Update_Removable_Devices_Mode;
		QString Removable_Devices_List;
		
		// For Create Templates
		QString Template_Name;
		Create_Template_Window::Template_Options Template_Opts;
		
		QSettings Settings;
};

#endif
