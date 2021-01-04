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

#ifndef VM_DEVICES_H
#define VM_DEVICES_H

#include <QObject>
#include <QList>
#include <QString>
#include <QFlags>
#include <QStringList>
#include <QMap>

class VM
{
	public:
		// Emulator type
		enum Machine_Accelerator { KVM, XEN, TCG };
		
		// Emulators Version
		enum Emulator_Version { Obsolete,
								QEMU_2_0, QEMU_2_1, QEMU_2_2, QEMU_2_3, QEMU_2_4, QEMU_2_5, QEMU_2_6 };
		
		// Virtual Machine State
		enum VM_State { VMS_Running, VMS_Power_Off, VMS_Pause, VMS_Saved, VMS_In_Error };

        static QString Accel_To_String(Machine_Accelerator accel)
        {
            switch(accel)
            {
                case KVM:
                    return "kvm";
                    break;
                case XEN:
                    return "xen";
                    break;
                case TCG:
                    return "tcg";
                    break;
                default:
                    return "";
            }
        }

        static Machine_Accelerator String_To_Accel(QString accel)
        {
            if (accel.toLower() == "qemu")
                return VM::TCG;
            else if (accel.toLower() == "tcg")
                return VM::TCG;
            else if (accel.toLower() == "kvm")
                return VM::KVM;
            else if (accel.toLower() == "qemu-kvm")
                return VM::KVM;
            else if (accel.toLower() == "xen")
                return VM::XEN;
            else
                return VM::KVM; //default //FIXME? is this a good default?
        }
		
		// x86 audio cards ( one or more )
		class Sound_Cards
		{
			public:
				bool Audio_sb16;
				bool Audio_es1370;
				bool Audio_Adlib;
				bool Audio_PC_Speaker;
				bool Audio_GUS;
				bool Audio_AC97;
				bool Audio_HDA;
				bool Audio_cs4231a;
				
				Sound_Cards()
				{
					Audio_sb16 = Audio_es1370 = Audio_Adlib = Audio_PC_Speaker = Audio_GUS = Audio_AC97 = Audio_HDA = Audio_cs4231a = false;
				}

                bool isEnabled()
                {
                    return ( Audio_sb16 ||
                             Audio_es1370 ||
                             Audio_Adlib ||
                             Audio_PC_Speaker ||
                             Audio_GUS ||
                             Audio_AC97 ||
                             Audio_HDA ||
                             Audio_cs4231a );
                }
				
				bool operator==( const Sound_Cards &v ) const
				{
					if( Audio_sb16 == v.Audio_sb16 &&
						Audio_es1370 == v.Audio_es1370 &&
						Audio_Adlib == v.Audio_Adlib &&
						Audio_PC_Speaker == v.Audio_PC_Speaker &&
						Audio_GUS == v.Audio_GUS &&
						Audio_AC97 == v.Audio_AC97 &&
						Audio_HDA == v.Audio_HDA &&
						Audio_cs4231a == v.Audio_cs4231a )
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				
				bool operator!=( const Sound_Cards &v ) const
				{
					return ! operator==(v);
				}
				
		};
		
		// SMP Settings
		class SMP_Options
		{
			public:
				unsigned short SMP_Count;
				unsigned short SMP_Cores;
				unsigned short SMP_Threads;
				unsigned short SMP_Sockets;
				unsigned short SMP_MaxCPUs;
				
				SMP_Options()
				{
					SMP_Count = 1;
					SMP_Cores = 0;
					SMP_Threads = 0;
					SMP_Sockets = 0;
					SMP_MaxCPUs = 0;
				}
				
				bool operator==( const SMP_Options &v ) const
				{
					if( SMP_Count == v.SMP_Count &&
						SMP_Cores == v.SMP_Cores &&
						SMP_Threads == v.SMP_Threads &&
						SMP_Sockets == v.SMP_Sockets &&
						SMP_MaxCPUs == v.SMP_MaxCPUs )
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				
				bool operator!=( const SMP_Options &v ) const
				{
					return ! operator==(v);
				}
		};
		
		// Boot Priority
		enum Boot_Device { Boot_From_FDA, Boot_From_FDB, Boot_From_CDROM, Boot_From_HDD,
						   Boot_From_Network1, Boot_From_Network2,
						   Boot_From_Network3, Boot_From_Network4, Boot_None };
		
		// Boot Order
		struct Boot_Order
		{
			bool Enabled;
			Boot_Device Type;
		};
		
        static QStringList Boot_Order_To_String_List(QList<Boot_Order> list)
        {
            QStringList bootStr;
            for( int ix = 0; ix < list.count(); ix++ )
            {
                if( list[ix].Enabled )
                {
                    switch( list[ix].Type )
                    {
                        case VM::Boot_From_FDA:
                            bootStr << "FDA";
                            break;

                        case VM::Boot_From_FDB:
                            bootStr << "FDB";
                            break;

                        case VM::Boot_From_CDROM:
                            bootStr << "CDROM";
                            break;

                        case VM::Boot_From_HDD:
                            bootStr << "HDD";
                            break;

                        case VM::Boot_From_Network1:
                            bootStr << "Net1";
                            break;

                        case VM::Boot_From_Network2:
                            bootStr << "Net2";
                            break;

                        case VM::Boot_From_Network3:
                            bootStr << "Net3";
                            break;

                        case VM::Boot_From_Network4:
                            bootStr << "Net4";
                            break;

                        default:
                            break;
                    }
                }
            }
            return bootStr;
        }

		// Kilobyte, Megabyte, Gigabyte
		enum Size_Suffix { Size_Suf_Kb, Size_Suf_Mb, Size_Suf_Gb };
		
		// Size for Storage Devices
		struct Device_Size
		{
			double Size;
			Size_Suffix Suffix;
		};
		
		// All HDD Image Disk Information
		struct Disk_Info
		{
			QString Image_File_Name, Disk_Format;
			VM::Device_Size Virtual_Size, Disk_Size;
			int Cluster_Size;
		};
		
		// Storage Device Interfaces
		enum Device_Interface { DI_IDE, DI_SCSI, DI_SD, DI_MTD, DI_Floppy, DI_PFlash, DI_Virtio, DI_Virtio_SCSI };
		
		// Storage Device Media
		enum Device_Media { DM_Disk, DM_CD_ROM };
		
		// Serial/Parallel Port Redirections
		enum Port_Redirection { PR_Default, PR_vc, PR_pty, PR_none, PR_null, PR_dev,
								PR_host_port, PR_file, PR_stdio, PR_pipe, PR_udp,
								PR_tcp, PR_telnet, PR_unix, PR_com, PR_msmouse, PR_mon, PR_braille };
		
		// QEMU Network Modes
		enum Network_Mode { Net_Mode_None, Net_Mode_Usermode, Net_Mode_Tuntap,
							Net_Mode_Tuntapfd, Net_Mode_Tcplisten, Net_Mode_Tcpfd,
							Net_Mode_Tcpconnect, Net_Mode_Multicast, Net_Mode_Multicastfd,
							Net_Mode_Dummy };
		
		// New Network Modes
		enum Network_Mode_Native { Net_Mode_Native_NIC, Net_Mode_Native_User, Net_Mode_Native_Chanel,
								  Net_Mode_Native_Bridge, Net_Mode_Native_TAP, Net_Mode_Native_Socket,
								  Net_Mode_Native_MulticastSocket, Net_Mode_Native_VDE, Net_Mode_Native_Dump };
		
		// Image Compression Type (Used in SPICE)
		enum SPICE_Image_Compression_Type { SPICE_IC_Type_on, SPICE_IC_Type_auto_glz, SPICE_IC_Type_auto_lz,
											SPICE_IC_Type_quic, SPICE_IC_Type_glz, SPICE_IC_Type_lz, SPICE_IC_Type_off };
		// Renderer Type (Used in SPICE)
		enum SPICE_Renderer { SPICE_Renderer_oglpbuf, SPICE_Renderer_oglpixmap, SPICE_Renderer_cairo };
};

class Device_Map
{
	public:
		Device_Map() { Caption = ""; QEMU_Name = ""; };
		Device_Map( QString cp, QString nm ) : Caption(cp), QEMU_Name(nm) {};
		
		QString Caption;
		QString QEMU_Name;
};

class Available_Devices
{
	public:
		// Constructor for init values
		Available_Devices();
		
		// QEMU System
		Device_Map System;
		
		// QEMU CPUs
		QList<Device_Map> CPU_List;
		
		// QEMU Machines
		QList<Device_Map> Machine_List;
		
		// QEMU Network Cards
		QList<Device_Map> Network_Card_List;
		
		// QEMU Audio Cards
		VM::Sound_Cards Audio_Card_List;
		
		// QEMU Video Cards
		QList<Device_Map> Video_Card_List;
		
		// Platform Specific Options
		unsigned short PSO_SMP_Count;
		bool PSO_SMP_Cores;
		bool PSO_SMP_Threads;
		bool PSO_SMP_Sockets;
		bool PSO_SMP_MaxCPUs;
		
		bool PSO_Device;
		bool PSO_Device_USB_EHCI;
		bool PSO_Device_USB_XHCI;
		
		bool PSO_Drive;
		bool PSO_Drive_File;
		bool PSO_Drive_If;
		bool PSO_Drive_Bus_Unit;
		bool PSO_Drive_Index;
		bool PSO_Drive_Media;
		bool PSO_Drive_Cyls_Heads_Secs_Trans;
		bool PSO_Drive_Snapshot;
		bool PSO_Drive_Cache;
		bool PSO_Drive_AIO;
		bool PSO_Drive_Format;
		bool PSO_Drive_Serial;
		bool PSO_Drive_ADDR;
		bool PSO_Drive_Boot;
		
		bool PSO_Boot_Order;
		bool PSO_Initial_Graphic_Mode;
		bool PSO_No_FB_Boot_Check;
		bool PSO_Win2K_Hack;
		bool PSO_Kernel_KQEMU;
		bool PSO_No_ACPI;
		bool PSO_KVM;
		bool PSO_RTC_TD_Hack;
		
		bool PSO_MTDBlock;
		bool PSO_SD;
		bool PSO_PFlash;
		bool PSO_Name;
		bool PSO_Curses;
		bool PSO_No_Frame;
		bool PSO_Alt_Grab;
		bool PSO_No_Quit;
		bool PSO_SDL;
		bool PSO_Portrait;
		bool PSO_No_Shutdown;
		bool PSO_Startdate;
		bool PSO_Show_Cursor;
		bool PSO_Bootp;
		
		bool PSO_Net_type_vde;
		bool PSO_Net_type_dump;
		
		bool PSO_Net_name;
		bool PSO_Net_addr;
		bool PSO_Net_vectors;
		
		bool PSO_Net_net;
		bool PSO_Net_host;
		bool PSO_Net_restrict;
		bool PSO_Net_dhcpstart;
		bool PSO_Net_dns;
		bool PSO_Net_tftp;
		bool PSO_Net_bootfile;
		bool PSO_Net_smb;
		bool PSO_Net_hostfwd;
		bool PSO_Net_guestfwd;
		
		bool PSO_Net_ifname;
		bool PSO_Net_bridge;
		bool PSO_Net_script;
		bool PSO_Net_downscript;
		bool PSO_Net_helper;
		bool PSO_Net_sndbuf;
		bool PSO_Net_vnet_hdr;
		bool PSO_Net_vhost;
		bool PSO_Net_vhostfd;
		
		bool PSO_Net_listen;
		bool PSO_Net_connect;
		
		bool PSO_Net_mcast;
		
		bool PSO_Net_sock;
		bool PSO_Net_port;
		bool PSO_Net_group;
		bool PSO_Net_mode;
		
		bool PSO_Net_file;
		bool PSO_Net_len;
		
		bool PSO_Enable_KVM;
		bool PSO_No_KVM;
		bool PSO_No_KVM_IRQChip;
		bool PSO_No_KVM_Pit;
		bool PSO_No_KVM_Pit_Reinjection;
		bool PSO_Enable_Nesting;
		bool PSO_KVM_Shadow_Memory;
		
		bool PSO_TFTP;
		bool PSO_SMB;
		bool PSO_Std_VGA;
		
		bool PSO_SPICE;
		bool PSO_QXL;
};

class Emulator
{
	public:
		Emulator();
		Emulator( const Emulator &emul );
		
		bool operator==( const Emulator &emul ) const;
		bool operator!=( const Emulator &emul ) const;
		
		bool Load( const QString &path );
		bool Save() const;
		QString Get_Emulator_File_Path() const;
		
		const QString &Get_Name() const;
		void Set_Name( const QString &name );
		
		bool Get_Default() const;
		void Set_Default( bool on );
		
		const QString &Get_Path() const;
		void Set_Path( const QString &path );
		
		bool Get_Check_Version() const;
		void Set_Check_Version( bool check );
		
		bool Get_Check_Available_Options() const;
		void Set_Check_Available_Options( bool check );
		
		bool Get_Force_Version() const;
		void Set_Force_Version( bool on );
		
		VM::Emulator_Version Get_Version() const;
		void Set_Version( VM::Emulator_Version ver );
		
		const QMap<QString, QString> &Get_Binary_Files() const;
		void Set_Binary_Files( const QMap<QString, QString> &files );
		
		const QMap<QString, Available_Devices> &Get_Devices() const;
		void Set_Devices( const QMap<QString, Available_Devices> &devices );
		
	private:
		QString Name;
		QString Path;
		bool Default;
		
		bool Check_Version;
		bool Check_Available_Options;
		bool Force_Version;
		VM::Emulator_Version Version;
		
		// KEY for next maps is emulator exeutable id (like qemu-system-arm)
		QMap<QString, QString> Binary_Files; // VALUE is path to executable
		QMap<QString, Available_Devices> Devices; // VALUE is Available_Devices class
};

// Native Storage Device (QEMU >= 0.9.1 Device Style)
class VM_Native_Storage_Device
{
	public:
		VM_Native_Storage_Device();
		VM_Native_Storage_Device( const VM_Native_Storage_Device &sd );
		
		bool Get_Native_Mode() const;
		
		QString Get_QEMU_Device_Name() const;
		
		bool operator==( const VM_Native_Storage_Device &sd ) const;
		bool operator!=( const VM_Native_Storage_Device &sd ) const;
		
		bool Use_File_Path() const;
		void Use_File_Path( bool use );
		
		const QString &Get_File_Path() const;
		void Set_File_Path( const QString &path );
		
		bool Use_Interface() const;
		void Use_Interface( bool use );
		
		VM::Device_Interface Get_Interface() const;
		void Set_Interface( VM::Device_Interface di );
		
		bool Use_Bus_Unit() const;
		void Use_Bus_Unit( bool use );
		
		int Get_Bus() const;
		void Set_Bus( int bus );
		
		int Get_Unit() const;
		void Set_Unit( int unit );
		
		bool Use_Index() const;
		void Use_Index( bool use );
		
		int Get_Index() const;
		void Set_Index( int index );
		
		bool Use_Media() const;
		void Use_Media( bool use );
		
		VM::Device_Media Get_Media() const;
		void Set_Media( VM::Device_Media media );
		
		bool Use_hdachs() const;
		void Use_hdachs( bool use );
		
		unsigned long Get_Cyls() const;
		void Set_Cyls( unsigned long cyls );
		
		unsigned long Get_Heads() const;
		void Set_Heads( unsigned long heads );
		
		unsigned long Get_Secs() const;
		void Set_Secs( unsigned long secs );
		
		unsigned long Get_Trans() const;
		void Set_Trans( unsigned long trans );
		
		bool Use_Snapshot() const;
		void Use_Snapshot( bool use );
		
		bool Get_Snapshot() const;
		void Set_Snapshot( bool use );
		
		bool Use_Cache() const;
		void Use_Cache( bool use );
		
		const QString &Get_Cache() const;
		void Set_Cache( const QString &cache );
		
		bool Use_AIO() const;
		void Use_AIO( bool use );
		
		const QString &Get_AIO() const;
		void Set_AIO( const QString &aio );
		
		bool Use_Boot() const;
		void Use_Boot( bool use );
		
		bool Get_Boot() const;
		void Set_Boot( bool use );
		bool Use_Discard() const;
		void Use_Discard( bool use );
		
		bool Get_Discard() const;
		void Set_Discard( bool discard );
	private:
		bool UFile_Path;
		QString File_Path;
		
		bool UInterface;
		VM::Device_Interface Interface; // ide, scsi, sd, mtd, floppy, pflash
		
		bool UBus_Unit;
		int Bus, Unit; // For SCSI
		
		bool UIndex;
		int Index; // For IDE, Floppy
		
		bool UMedia;
		VM::Device_Media Media; // disk or cdrom
		
		bool Uhdachs;
		unsigned long Cyls, Heads, Secs, Trans; // For -hdachs
		
		bool USnapshot;
		bool Snapshot;
		
		bool UCache;
		QString Cache;
		
		bool UAIO;
		QString AIO;
		
		bool UBoot;
		bool Boot;
		bool UDiscard;
		bool Discard;
};

// Virtual Machine Storage Device (FDD, CD, HDD)
class VM_Storage_Device
{
	public:
		VM_Storage_Device();
		VM_Storage_Device( const VM_Storage_Device &device );
		VM_Storage_Device( bool enabled, const QString &file_name );
		VM_Storage_Device( bool enabled, const QString &file_name, bool Native_mode, const VM_Native_Storage_Device &device ); // FIXME test const VM_Native_Storage_Device &device IN OLD version without const
		
		bool operator==( const VM_Storage_Device &device ) const;
		bool operator!=( const VM_Storage_Device &device ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool enabled );
		
		const QString &Get_File_Name() const;
		void Set_File_Name( const QString &file_name );
		
		bool Get_Native_Mode() const;
		
		const VM_Native_Storage_Device &Get_Native_Device() const;
		void Set_Native_Device( const VM_Native_Storage_Device &device );
		
	protected:
		bool Enabled;
		QString File_Name;
		VM_Native_Storage_Device Native_Device;
};


// Virtual Machine Shared Folder (shared between host and guest)
class VM_Shared_Folder
{
	public:
		VM_Shared_Folder();
		VM_Shared_Folder( const VM_Shared_Folder &device );
		VM_Shared_Folder( bool enabled, const QString &folder );
		
		bool operator==( const VM_Shared_Folder &device ) const;
		bool operator!=( const VM_Shared_Folder &device ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool enabled );
		
		const QString &Get_Folder() const;
		void Set_Folder( const QString &folder );

	protected:
		bool Enabled;
		QString Folder;
};

// Virtual Machine Hard Drive Device
class VM_HDD: public VM_Storage_Device
{
	public:
		VM_HDD();
		VM_HDD( const VM_HDD &hd );
		VM_HDD( bool enabled, const QString &im_pach );
		
		bool operator==( const VM_HDD &v ) const;
		bool operator!=( const VM_HDD &v ) const;
		
		void Set_File_Name( const QString &file_name );
		
		void Set_Disk_Info( VM::Disk_Info info );
		const QString &Get_Image_Format() const;
		const VM::Device_Size &Get_Virtual_Size() const;
		const VM::Device_Size &Get_Disk_Size() const;
		int Get_Cluster_Size() const;
		
		double Get_Virtual_Size_in_GB() const;
		QString Get_Complete_Virtual_Size() const;
		QString Get_Complete_Disk_Size() const;
		
		void Set_Virtual_Size( const VM::Device_Size &size );
		void Set_Image_Format( const QString &format );
		
		QString Size_Suffix_to_String( VM::Size_Suffix suf ) const;
		VM::Device_Size String_to_Device_Size( const QString &size ) const;
	
	private:
		void Reset_Info();
		
		QString Disk_Format;
		VM::Device_Size Virtual_Size, Disk_Size;
		int Cluster_Size;
};

// VM Snapshot
class VM_Snapshot
{
	public:
		VM_Snapshot();
		VM_Snapshot( const VM_Snapshot &s );
		
		bool operator==( const VM_Snapshot &s ) const;
		bool operator!=( const VM_Snapshot &s ) const;
		
		const QString &Get_Tag() const;
		void Set_Tag( const QString &tag );
		
		const QString &Get_ID() const;
		void Set_ID( const QString &id );
		
		const QString &Get_Size() const;
		void Set_Size( const QString &size );
		
		const QString &Get_Date() const;
		void Set_Date( const QString &date );
		
		const QString &Get_VM_Clock() const;
		void Set_VM_Clock( const QString &vm_clock );
		
		const QString &Get_Name() const;
		void Set_Name( const QString &name );
		
		const QString &Get_Description() const;
		void Set_Description( const QString &desc );
		
	private:
		QString Tag; // Snapshot Name
		QString ID;
		QString Size;
		QString Date;
		QString VM_Clock;
		
		QString Name; // This name overwrite Tag in GUI
		QString Description;
};

// Network Card
class VM_Net_Card
{
	public:
		VM_Net_Card();
		VM_Net_Card( const VM_Net_Card &nc );
		
		bool operator==( const VM_Net_Card &nc ) const;
		bool operator!=( const VM_Net_Card &nc ) const;
		
		QString Generate_MAC() const;
		
		const QString &Get_Card_Model() const;
		void Set_Card_Model( const QString &m );
		
		VM::Network_Mode Get_Net_Mode() const;
		void Set_Net_Mode( VM::Network_Mode mode );
		
		const QString &Get_Hostname() const;
		void Set_Hostname( const QString &hn );
		
		const QString &Get_IP_Address() const;
		void Set_IP_Address( const QString &ia );
		
		const QString &Get_MAC_Address() const;
		void Set_MAC_Address( const QString &ma );
		
		int Get_Port() const;
		void Set_Port( int p );
		
		int Get_VLAN() const;
		void Set_VLAN( int vl );

		bool Get_Use_TUN_TAP_Script() const;
		void Set_Use_TUN_TAP_Script( bool use );
		
		const QString &Get_TUN_TAP_Script() const;
		void Set_TUN_TAP_Script( const QString &s );
		
		const QString &Get_Interface_Name() const;
		void Set_Interface_Name( const QString &n );
		
		bool Use_Bridge_Name() const;
		void Use_Bridge_Name( bool use );
		const QString &Get_Bridge_Name() const;
		void Set_Bridge_Name( const QString &n );

		int Get_File_Descriptor() const;
		void Set_File_Descriptor( int f );
		
	private:
		QString Card_Model;
		VM::Network_Mode Net_Mode;
		QString Hostname;
		QString IP;
		QString MAC;
		int Port;
		int VLAN;
		bool Use_TUN_TAP_Script;
		QString TUN_TAP_Script;
		QString Interface_Name;
		int File_Descriptor;
};

// Network Card Native
class VM_Net_Card_Native
{
	public:
		VM_Net_Card_Native();
		VM_Net_Card_Native( const VM_Net_Card_Native &nc );
		
		bool operator==( const VM_Net_Card_Native &nc ) const;
		bool operator!=( const VM_Net_Card_Native &nc ) const;
		
		QString Generate_MAC() const;
		
		VM::Network_Mode_Native Get_Network_Type() const;
		void Set_Network_Type( VM::Network_Mode_Native mode );
		
		const QString &Get_Card_Model() const;
		void Set_Card_Model( const QString &m );
		
		bool Use_MAC_Address() const;
		void Use_MAC_Address( bool use );
		const QString &Get_MAC_Address() const;
		void Set_MAC_Address( const QString &ma );
		
		bool Use_VLAN() const;
		void Use_VLAN( bool use );
		int Get_VLAN() const;
		void Set_VLAN( int vl );
		
		bool Use_Name() const;
		void Use_Name( bool use );
		const QString &Get_Name() const;
		void Set_Name( const QString &name );
		
		bool Use_Hostname() const;
		void Use_Hostname( bool use );
		const QString &Get_Hostname() const;
		void Set_Hostname( const QString &hostname );
		
		const QString &Get_PortDev() const;
		void Set_PortDev( const QString &portDev );
		
		bool Use_File_Descriptor() const;
		void Use_File_Descriptor( bool use );
		int Get_File_Descriptor() const;
		void Set_File_Descriptor( int f );
		
		bool Use_Interface_Name() const;
		void Use_Interface_Name( bool use );
		const QString &Get_Interface_Name() const;
		void Set_Interface_Name( const QString &n );
		
		bool Use_Bridge_Name() const;
		void Use_Bridge_Name( bool use );
		const QString &Get_Bridge_Name() const;
		void Set_Bridge_Name( const QString &n );

		bool Use_TUN_TAP_Script() const;
		void Use_TUN_TAP_Script( bool use );
		const QString &Get_TUN_TAP_Script() const;
		void Set_TUN_TAP_Script( const QString &s );
		
		bool Use_TUN_TAP_Down_Script() const;
		void Use_TUN_TAP_Down_Script( bool use );
		const QString &Get_TUN_TAP_Down_Script() const;
		void Set_TUN_TAP_Down_Script( const QString &s );
		
		bool Use_Bridge_Helper() const;
		void Use_Bridge_Helper( bool use );
		const QString &Get_Bridge_Helper() const;
		void Set_Bridge_Helper( const QString &s );

		bool Use_Listen() const;
		void Use_Listen( bool use );
		const QString &Get_Listen() const;
		void Set_Listen( const QString &listen );
		
		bool Use_Connect() const;
		void Use_Connect( bool use );
		const QString &Get_Connect() const;
		void Set_Connect( const QString &con );
		
		bool Use_MCast() const;
		void Use_MCast( bool use );
		const QString &Get_MCast() const;
		void Set_MCast( const QString &mcast );
		
		bool Use_Sock() const;
		void Use_Sock( bool use );
		const QString &Get_Sock() const;
		void Set_Sock( const QString &sock );
		
		bool Use_Port() const;
		void Use_Port( bool use );
		int Get_Port() const;
		void Set_Port( int p );
		
		bool Use_Group() const;
		void Use_Group( bool use );
		const QString &Get_Group() const;
		void Set_Group( const QString &group );
		
		bool Use_Mode() const;
		void Use_Mode( bool use );
		const QString &Get_Mode() const;
		void Set_Mode( const QString &mode );
		
		bool Use_File() const;
		void Use_File( bool use );
		const QString &Get_File() const;
		void Set_File( const QString &file );
		
		bool Use_Len() const;
		void Use_Len( bool use );
		int Get_Len() const;
		void Set_Len( int kb );
		
		bool Use_Addr() const;
		void Use_Addr( bool use );
		const QString &Get_Addr() const;
		void Set_Addr( const QString &addr );
		
		bool Use_Vectors() const;
		void Use_Vectors( bool use );
		int Get_Vectors() const;
		void Set_Vectors( int vectors );
		
		bool Use_Net() const;
		void Use_Net( bool use );
		const QString &Get_Net() const;
		void Set_Net( const QString &net );
		
		bool Use_Host() const;
		void Use_Host( bool use );
		const QString &Get_Host() const;
		void Set_Host( const QString &host );
		
		bool Use_Restrict() const;
		void Use_Restrict( bool use );
		bool Get_Restrict() const;
		void Set_Restrict( bool use );
		
		bool Use_DHCPstart() const;
		void Use_DHCPstart( bool use );
		const QString &Get_DHCPstart() const;
		void Set_DHCPstart( const QString &addr );
		
		bool Use_DNS() const;
		void Use_DNS( bool use );
		const QString &Get_DNS() const;
		void Set_DNS( const QString &addr );
		
		bool Use_Tftp() const;
		void Use_Tftp( bool use );
		const QString &Get_Tftp() const;
		void Set_Tftp( const QString &dir );
		
		bool Use_Bootfile() const;
		void Use_Bootfile( bool use );
		const QString &Get_Bootfile() const;
		void Set_Bootfile( const QString &file );
		
		bool Use_HostFwd() const;
		void Use_HostFwd( bool use );
		const QString &Get_HostFwd() const;
		void Set_HostFwd( const QString &rule );
		
		bool Use_GuestFwd() const;
		void Use_GuestFwd( bool use );
		const QString &Get_GuestFwd() const;
		void Set_GuestFwd( const QString &rule );
		
		bool Use_SMB() const;
		void Use_SMB( bool use );
		const QString &Get_SMB() const;
		void Set_SMB( const QString &dir );
		
		bool Use_SMBserver() const;
		void Use_SMBserver( bool use );
		const QString &Get_SMBserver() const;
		void Set_SMBserver( const QString &addr );
		
		bool Use_Sndbuf() const;
		void Use_Sndbuf( bool use );
		int Get_Sndbuf() const;
		void Set_Sndbuf( int bytes );
		
		bool Use_VNet_hdr() const;
		void Use_VNet_hdr( bool use );
		bool Get_VNet_hdr() const;
		void Set_VNet_hdr( bool use );
		
		bool Use_VHost() const;
		void Use_VHost( bool use );
		bool Get_VHost() const;
		void Set_VHost( bool use );
		
		bool Use_VHostFd() const;
		void Use_VHostFd( bool use );
		int Get_VHostFd() const;
		void Set_VHostFd( int port );
		
	private:
		VM::Network_Mode_Native Network_Mode;
		QString Card_Model;
		QString MAC;
		int VLAN;
		QString Name;
		QString Hostname;
		QString PortDev;
		int File_Descriptor;
		QString Interface_Name;
		QString Bridge_Name;
		QString TUN_TAP_Script;
		QString TUN_TAP_Down_Script;
		QString Bridge_Helper;
		QString Listen;
		QString Connect;
		QString MCast;
		QString Sock;
		int Port;
		QString Group;
		QString Mode;
		QString File;
		int Len;
		QString Addr;
		int Vectors;
		QString Net;
		QString Host;
		bool Restrict;
		QString DHCPstart;
		QString DNS;
		QString Tftp;
		QString Bootfile;
		QString HostFwd;
		QString GuestFwd;
		QString SMB;
		QString SMBserver;
		int Sndbuf;
		bool VNet_hdr;
		bool VHost;
		int VHostFd;
		
		bool _Use_MAC, _Use_VLAN, _Use_Name, _Use_Hostname, _Use_File_Descriptor,
			 _Use_Interface_Name, _Use_Bridge_Name, _Use_TUN_TAP_Script, _Use_TUN_TAP_Down_Script,
			 _Use_Bridge_Helper, _Use_Listen, _Use_Connect, _Use_MCast, _Use_Sock, _Use_Port,
			 _Use_Group, _Use_Mode, _Use_File, _Use_Len, _Use_Addr, _Use_Vectors, _Use_Net,
			 _Use_Host, _Use_Restrict, _Use_DHCPstart, _Use_DNS, _Use_Tftp, _Use_Bootfile,
			 _Use_HostFwd, _Use_GuestFwd, _Use_SMB, _Use_SMBserver, _Use_Sndbuf,
			 _Use_VNet_hdr, _Use_VHost, _Use_VHostFd;
};

class VM_Redirection
{
	public:
		VM_Redirection();
		VM_Redirection( const VM_Redirection &r );
		
		bool operator==( const VM_Redirection &r ) const;
		bool operator!=( const VM_Redirection &r ) const;
		
		const QString &Get_Protocol() const;
		void Set_Protocol( const QString &p );
		
		int Get_Host_Port() const;
		void Set_Host_Port( int hp );
		
		const QString &Get_Guest_IP() const;
		void Set_Guest_IP( const QString &ip );
		
		int Get_Guest_Port() const;
		void Set_Guest_Port( int p );
		
	private:
		QString Protocol;
		int Host_Port;
		QString Guest_IP;
		int Guest_Port;
};

// Port (Serial or Parallel)
class VM_Port
{
	public:
		VM_Port();
		VM_Port( const VM_Port &p );
		
		bool operator==( const VM_Port &p ) const;
		bool operator!=( const VM_Port &p ) const;
		
		VM::Port_Redirection Get_Port_Redirection() const;
		void Set_Port_Redirection( VM::Port_Redirection pr );
		
		const QString &Get_Parametrs_Line() const;
		void Set_Parametrs_Line( const QString &pl );
		
	private:
		VM::Port_Redirection Port_Redir;
		QString Param_Line;
};

// USB Port
class VM_USB
{
	public:
		VM_USB();
		VM_USB( const VM_USB &u );
		
		bool operator==( const VM_USB &u ) const;
		bool operator!=( const VM_USB &u ) const;
		
		bool Get_Use_Host_Device() const;
		void Set_Use_Host_Device( bool use );
		
		const QString &Get_Manufacturer_Name() const;
		void Set_Manufacturer_Name( const QString &name );
		
		const QString &Get_Product_Name() const;
		void Set_Product_Name( const QString &name );
		
		const QString &Get_Vendor_ID() const;
		void Set_Vendor_ID( const QString &dev );
		
		const QString &Get_Product_ID() const;
		void Set_Product_ID( const QString &dev );
		
		QString Get_ID_Line() const;
		void Set_ID_Line( const QString &line );
		
		QString Get_Bus() const;
		void Set_Bus( const QString &bus );
		
		QString Get_Addr() const;
		void Set_Addr( const QString &addr );
		
		QString Get_DevPath() const;
		void Set_DevPath( const QString &devPath );
		
		QString Get_Serial_Number() const;
		void Set_Serial_Number( const QString &serial );
		
		QString Get_Speed() const;
		void Set_Speed( const QString &speed );
		
		void Get_USB_QEMU_Devices( bool &keyboard, bool &mouse, bool &tablet, bool &wacomTablet, bool &braille ) const;
		void Set_USB_QEMU_Devices( bool keyboard, bool mouse, bool tablet, bool wacomTablet, bool braille );
		
	private:
		bool Use_Host_Device;
		QString Manufacturer_Name;
		QString Product_Name;
		QString Vendor_ID;
		QString Product_ID;
		QString Bus;
		QString Addr;
		QString DevPath;
		QString Serial_Number;
		QString Speed;
		bool USB_Keyboard, USB_Mouse, USB_Tablet, USB_WacomTablet, USB_Braille;
};

// Initial Graphical Mode
class VM_Init_Graphic_Mode
{
	public:
		VM_Init_Graphic_Mode();
		VM_Init_Graphic_Mode( const VM_Init_Graphic_Mode &mode );
		
		bool operator==( const VM_Init_Graphic_Mode &mode ) const;
		bool operator!=( const VM_Init_Graphic_Mode &mode ) const;
		
		bool Get_Enabled() const;
		void Set_Enabled( bool use );
		
		int Get_Width() const;
		void Set_Width( int w );
		
		int Get_Height() const;
		void Set_Height( int h );
		
		int Get_Depth() const;
		void Set_Depth( int d );
		
	private:
		bool Enabled;
		int Width;
		int Height;
		int Depth;
};

// SPICE
class VM_SPICE
{
	public:
		VM_SPICE();
		VM_SPICE( const VM_SPICE &vm_spice );
		
		bool operator==( const VM_SPICE &vm_spice ) const;
		bool operator!=( const VM_SPICE &vm_spice ) const;
		
		bool Use_SPICE() const;
		void Use_SPICE( bool use );
		
		// qxl options
		unsigned int Get_GXL_Devices_Count() const;
		void Set_GXL_Devices_Count( unsigned int count );
		
		int Get_RAM_Size() const;
		void Set_RAM_Size( int size );
		
		// Basic options
		unsigned int Get_Port() const;
		void Set_Port( unsigned int port );
		
		bool Use_SPort() const;
		void Use_SPort( bool use );
		
		unsigned int Get_SPort() const;
		void Set_SPort( unsigned int sport );
		
		bool Use_Hostname() const;
		void Use_Hostname( bool use );
		
		QString Get_Hostname() const;
		void Set_Hostname( QString hostname );
		
		// Image, video & audio options
		bool Use_Image_Compression() const;
		void Use_Image_Compression( bool use );
		
		VM::SPICE_Image_Compression_Type Get_Image_Compression() const;
		void Set_Image_Compression( VM::SPICE_Image_Compression_Type type );
		
		bool Use_Video_Stream_Compression() const;
		void Use_Video_Stream_Compression( bool use );
		
		bool Use_Renderer() const;
		void Use_Renderer( bool use );
		
		const QList<VM::SPICE_Renderer> &Get_Renderer_List() const;
		void Set_Renderer_List( const QList<VM::SPICE_Renderer> &list );
		
		bool Use_Playback_Compression() const;
		void Use_Playback_Compression( bool use );
		
		// Security options
		bool Use_Password() const;
		void Use_Password( bool use );
		
		QString Get_Password() const;
		void Set_Password( QString password );
	
	private:
		bool Enable_SPICE;
		
		unsigned int GXL_Devices_Count;
		int RAM_Size;
		
		unsigned int Port;
		bool _Use_SPort;
		unsigned int SPort;
		bool _Use_Hostname;
		QString Hostname;
		
		bool _Use_Image_Compression;
		VM::SPICE_Image_Compression_Type Image_Compression;
		bool _Use_Video_Stream_Compression;
		bool _Use_Renderer;
		QList<VM::SPICE_Renderer> Renderer_List;
		bool _Use_Playback_Compression;
		
		bool _Use_Password;
		QString Password;
};

#endif
