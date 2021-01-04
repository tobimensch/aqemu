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

#include <QRegExp>
#include <QUuid>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QStringList>
#include <QTextStream>

#include "VM.h"
#include "VM_Devices.h"
#include "Utils.h"
#include "HDD_Image_Info.h"
#include "System_Info.h"

using namespace TinyXML2QDomWrapper;

// Available_Devices ---------------------------------------------------------

Available_Devices::Available_Devices()
{
	CPU_List << Device_Map( QObject::tr("Default"), "" );
	Machine_List << Device_Map( QObject::tr("Default"), "" );
	Network_Card_List << Device_Map( QObject::tr("Default"), "" );
	Video_Card_List << Device_Map( QObject::tr("Default"), "" );
	
	PSO_SMP_Count = 1;
	PSO_SMP_Cores = false;
	PSO_SMP_Threads = false;
	PSO_SMP_Sockets = false;
	PSO_SMP_MaxCPUs = false;
	
	PSO_Device = false;
	PSO_Device_USB_EHCI = false;
	
	PSO_Drive = false;
	PSO_Drive_File = false;
	PSO_Drive_If = false;
	PSO_Drive_Bus_Unit = false;
	PSO_Drive_Index = false;
	PSO_Drive_Media = false;
	PSO_Drive_Cyls_Heads_Secs_Trans = false;
	PSO_Drive_Snapshot = false;
	PSO_Drive_Cache = false;
	PSO_Drive_AIO = false;
	PSO_Drive_Format = false;
	PSO_Drive_Serial = false;
	PSO_Drive_ADDR = false;
	PSO_Drive_Boot = false;
	
	PSO_Boot_Order = false;
	PSO_Initial_Graphic_Mode = false;
	PSO_No_FB_Boot_Check = false;
	PSO_Win2K_Hack = false;
	PSO_Kernel_KQEMU = false;
	PSO_No_ACPI = false;
	PSO_KVM = false;
	PSO_RTC_TD_Hack = false;
	
	PSO_MTDBlock = false;
	PSO_SD = false;
	PSO_PFlash = false;
	PSO_Name = false;
	PSO_Curses = false;
	PSO_No_Frame = false;
	PSO_Alt_Grab = false;
	PSO_No_Quit = false;
	PSO_SDL = false;
	PSO_Portrait = false;
	PSO_No_Shutdown = false;
	PSO_Startdate = false;
	PSO_Show_Cursor = false;
	PSO_Bootp = false;
	
	PSO_Net_type_vde = false;
	PSO_Net_type_dump = false;
	
	PSO_Net_name = false;
	PSO_Net_addr = false;
	PSO_Net_vectors = false;
	
	PSO_Net_net = false;
	PSO_Net_host = false;
	PSO_Net_restrict = false;
	PSO_Net_dhcpstart = false;
	PSO_Net_dns = false;
	PSO_Net_tftp = false;
	PSO_Net_bootfile = false;
	PSO_Net_smb = false;
	PSO_Net_hostfwd = false;
	PSO_Net_guestfwd = false;
	
	PSO_Net_ifname = false;
	PSO_Net_bridge = false;
	PSO_Net_script = false;
	PSO_Net_downscript = false;
	PSO_Net_helper = false;
	PSO_Net_sndbuf = false;
	PSO_Net_vnet_hdr = false;
	PSO_Net_vhost = false;
	PSO_Net_vhostfd = false;
	
	PSO_Net_listen = false;
	PSO_Net_connect = false;
	
	PSO_Net_mcast = false;
	
	PSO_Net_sock = false;
	PSO_Net_port = false;
	PSO_Net_group = false;
	PSO_Net_mode = false;
	
	PSO_Net_file = false;
	PSO_Net_len = false;
	
	PSO_Enable_KVM = false;
	PSO_No_KVM = false;
	PSO_No_KVM_IRQChip = false;
	PSO_No_KVM_Pit = false;
	PSO_No_KVM_Pit_Reinjection = false;
	PSO_Enable_Nesting = false;
	PSO_KVM_Shadow_Memory = false;
	
	PSO_TFTP = false;
	PSO_SMB = false;
	PSO_Std_VGA = false;
	
	PSO_SPICE = false;
	PSO_QXL = false;
}

//===========================================================================

// VM_Native_Storage_Device --------------------------------------------------

Emulator::Emulator()
{
	Name = "";
	Path = "";
	Default = false;
	Check_Version = false;
	Check_Available_Options = false;
	Force_Version = false;
	Version = VM::Obsolete;
	Binary_Files = QMap<QString, QString>();
	Devices = QMap<QString, Available_Devices>();
}

Emulator::Emulator( const Emulator &emul )
{
	Name = emul.Get_Name();
	Path = emul.Get_Path();
	Default = emul.Get_Default();
	Check_Version = emul.Get_Check_Version();
	Check_Available_Options = emul.Get_Check_Available_Options();
	Force_Version = emul.Get_Force_Version();
	Version = emul.Get_Version();
	Binary_Files = emul.Get_Binary_Files();
	Devices = emul.Get_Devices();
}

bool Emulator::operator==( const Emulator &emul ) const
{
	if( Name == emul.Get_Name() &&
		Path == emul.Get_Path() &&
		Default == emul.Get_Default() &&
		Check_Version == emul.Get_Check_Version() &&
		Check_Available_Options == emul.Get_Check_Available_Options() &&
		Force_Version == emul.Get_Force_Version() &&
		Version == emul.Get_Version() &&
		Binary_Files == emul.Get_Binary_Files() ) // compare devices?
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Emulator::operator!=( const Emulator &emul ) const
{
	return ! ( operator==(emul) );
}

bool Emulator::Load( const QString &path )
{
	// Load file
	QDomDocument doc;
	QFile inFile( path );
	if( ! inFile.open(QIODevice::ReadOnly) )
	{
		AQError( "bool Emulator::Load( const QString &path )",
				 QString("Cannot open file \"%1\"!").arg(path) );
		return false;
	}
	
	QString errorString = "";
	int errorLine = 0, errorColumn = 0;
	if( ! doc.setContent(&inFile, true, &errorString, &errorLine, &errorColumn) )
	{
		AQError( "bool Emulator::Load( const QString &path )",
				 QString("Cannot set content form file \"%1\"!\n\nLine: %2\nColumn: %3\nString: %4" )
				 .arg(path).arg(errorLine).arg(errorColumn).arg(errorString) );
		inFile.close();
		return false;
	}
	
	inFile.close();
	
	QDomElement rootElement = doc.documentElement();
	if( rootElement.tagName() != "AQEMU_Emulator" )
	{
		AQError( "bool Emulator::Load( const QString &path )",
				 QString("File \"%1\" is not AQEMU emulator file!").arg(path) );
		return false;
	}
	
	if( rootElement.attribute("version") == "0.8" )
	{
		AQDebug( "bool Emulator::Load( const QString &path )",
				 "Loading emulator file version 0.8" );
	}
	else
	{
		AQWarning( "bool Emulator::Load( const QString &path )",
				   QString("Emulator version %1 not defined!").arg(rootElement.attribute("version")) );
	}
	
	// Create vairables
	QDomElement childElement = rootElement.firstChildElement( "Emulator" );
	if( childElement.isNull() )
	{
		AQError( "bool Emulator::Load( const QString &path )",
				 "No \"Emulator\" element!" );
		return false;
	}
	
	// Load
	Name = childElement.firstChildElement( "Name" ).text();
	Default = childElement.firstChildElement( "Default" ).text() == "yes";
	Path = childElement.firstChildElement( "Path" ).text();
	Check_Version = childElement.firstChildElement( "Check_Version" ).text() == "yes";
	Check_Available_Options = childElement.firstChildElement( "Check_Available_Options" ).text() == "yes";
	Force_Version = childElement.firstChildElement( "Force_Version" ).text() == "yes";
	Version = String_To_Emulator_Version( childElement.firstChildElement("Version").text() );
	
	// Load emulators executable path's
	Binary_Files.clear();
	
	childElement = childElement.firstChildElement( "Executable_Paths" );
	if( childElement.isNull() )
	{
		AQError( "bool Emulator::Load( const QString &path )",
				 "No \"Executable_Paths\" element!" );
		return false;
	}
	
	for( QDomNode node = childElement.firstChild(); node.isNull() == false; node = node.nextSibling() )
	{
		QDomElement elem = node.toElement();
		if( ! elem.isNull() ) Binary_Files[ elem.tagName() ] = elem.text();
	}
	
	// Load devices
	QMapIterator<QString, QString> iter( Binary_Files );
	while( iter.hasNext() )
	{
		iter.next();
		
		childElement = rootElement.firstChildElement( "Emulator" );
		
		childElement = childElement.firstChildElement( iter.key() );
		if( childElement.isNull() )
		{
			AQWarning( "bool Emulator::Load( const QString &path )",
					   QString("No \"%1\" element!").arg(iter.key()) );
			continue;
		}
		else
		{
			Available_Devices tmpDev;
			
			// System
			tmpDev.System.Caption = childElement.firstChildElement( "System_Caption" ).text();
			tmpDev.System.QEMU_Name = childElement.firstChildElement( "System_QEMU_Name" ).text();
			
			// CPUs
			tmpDev.CPU_List.clear();
			
			QDomElement thirdElement = childElement.firstChildElement( "CPU_List" );
			
			if( thirdElement.isNull() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No \"CPU_List\" element!" );
				return false;
			}
			
			QDomNode node = thirdElement.firstChild();
			Device_Map tmpDevMap;
			for( int ix = 1; node.isNull() == false; node = node.nextSibling(), ix++ )
			{
				QDomElement elem = node.toElement();
				if( ! elem.isNull() )
				{
					if( ix % 2 != 0 ) tmpDevMap.Caption = elem.text();
					else
					{
						tmpDevMap.QEMU_Name = elem.text();
						tmpDev.CPU_List << tmpDevMap;
					}
				}
			}
			
			if( tmpDev.CPU_List.isEmpty() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No values on \"CPU_List\" element! Add default CPU element." );
				tmpDev.CPU_List << Device_Map( QObject::tr("Default"), "" );
			}
			
			// Machines
			tmpDev.Machine_List.clear();
			
			thirdElement = childElement.firstChildElement( "Machine_List" );
			
			if( thirdElement.isNull() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No \"Machine_List\" element!" );
				return false;
			}
			
			node = thirdElement.firstChild();
			for( int ix = 1; node.isNull() == false; node = node.nextSibling(), ix++ )
			{
				QDomElement elem = node.toElement();
				if( ! elem.isNull() )
				{
					if( ix % 2 != 0 ) tmpDevMap.Caption = elem.text();
					else
					{
						tmpDevMap.QEMU_Name = elem.text();
						tmpDev.Machine_List << tmpDevMap;
					}
				}
			}
			
			if( tmpDev.Machine_List.isEmpty() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No values on \"Machine_List\" element! Add default Machine element." );
				tmpDev.Machine_List << Device_Map( QObject::tr("Default"), "" );
			}
			
			// Network Cards
			tmpDev.Network_Card_List.clear();
			
			thirdElement = childElement.firstChildElement( "Network_Card_List" );
			
			if( thirdElement.isNull() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No \"Network_Card_List\" element!" );
				return false;
			}
			
			node = thirdElement.firstChild();
			for( int ix = 1; node.isNull() == false; node = node.nextSibling(), ix++ )
			{
				QDomElement elem = node.toElement();
				if( ! elem.isNull() )
				{
					if( ix % 2 != 0 ) tmpDevMap.Caption = elem.text();
					else
					{
						tmpDevMap.QEMU_Name = elem.text();
						tmpDev.Network_Card_List << tmpDevMap;
					}
				}
			}
			
			if( tmpDev.Network_Card_List.isEmpty() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No values on \"Network_Card_List\" element! Add default Network Card element." );
				tmpDev.Network_Card_List << Device_Map( QObject::tr("Default"), "" );
			}
			
			// Audio Cards
			thirdElement = childElement.firstChildElement( "Audio_Card_List" );
			
			if( thirdElement.isNull() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No \"Audio_Card_List\" element!" );
				return false;
			}
			
			tmpDev.Audio_Card_List.Audio_sb16 = (thirdElement.firstChildElement("sb16").text() == "yes");
			tmpDev.Audio_Card_List.Audio_es1370 = (thirdElement.firstChildElement("es1370").text() == "yes");
			tmpDev.Audio_Card_List.Audio_Adlib = (thirdElement.firstChildElement("Adlib").text() == "yes");
			tmpDev.Audio_Card_List.Audio_PC_Speaker = (thirdElement.firstChildElement("PC_Speaker").text() == "yes");
			tmpDev.Audio_Card_List.Audio_GUS = (thirdElement.firstChildElement("GUS").text() == "yes");
			tmpDev.Audio_Card_List.Audio_AC97 = (thirdElement.firstChildElement("AC97").text() == "yes");
			tmpDev.Audio_Card_List.Audio_HDA = (thirdElement.firstChildElement("HDA").text() == "yes");
			tmpDev.Audio_Card_List.Audio_cs4231a = (thirdElement.firstChildElement("cs4231a").text() == "yes");
			
			// Video Cards
			tmpDev.Video_Card_List.clear();
			
			thirdElement = childElement.firstChildElement( "Video_Card_List" );
			
			if( thirdElement.isNull() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No \"Video_Card_List\" element!" );
				return false;
			}
			
			node = thirdElement.firstChild();
			for( int ix = 1; node.isNull() == false; node = node.nextSibling(), ix++ )
			{
				QDomElement elem = node.toElement();
				if( ! elem.isNull() )
				{
					if( ix % 2 != 0 ) tmpDevMap.Caption = elem.text();
					else
					{
						tmpDevMap.QEMU_Name = elem.text();
						tmpDev.Video_Card_List << tmpDevMap;
					}
				}
			}
			
			if( tmpDev.Video_Card_List.isEmpty() )
			{
				AQError( "bool Emulator::Load( const QString &path )",
						 "No values on \"Video_Card_List\" element! Add default Video Card element." );
				tmpDev.Video_Card_List << Device_Map( QObject::tr("Default"), "" );
			}
			
			// Platform Specific Options
			bool ok = false;
			tmpDev.PSO_SMP_Count = childElement.firstChildElement( "SMP_Count" ).text().toUInt( &ok );
			if( ! ok )
			{
				AQWarning( "bool Emulator::Load( const QString &path )",
						   "SMP_Count value not uint type! Use default value: 1" );
				tmpDev.PSO_SMP_Count = 1;
			}
			
			tmpDev.PSO_SMP_Cores = (childElement.firstChildElement("SMP_Cores").text() == "yes" );
			tmpDev.PSO_SMP_Threads = (childElement.firstChildElement("SMP_Threads").text() == "yes" );
			tmpDev.PSO_SMP_Sockets = (childElement.firstChildElement("SMP_Sockets").text() == "yes" );
			tmpDev.PSO_SMP_MaxCPUs = (childElement.firstChildElement("SMP_MaxCPUs").text() == "yes" );
			
			tmpDev.PSO_Device = (childElement.firstChildElement("Device").text() == "yes" );
			tmpDev.PSO_Device_USB_EHCI = (childElement.firstChildElement("Device_USB_EHCI").text() == "yes" );
			
			tmpDev.PSO_Drive = (childElement.firstChildElement("Drive").text() == "yes" );
			tmpDev.PSO_Drive_File = (childElement.firstChildElement("Drive_File").text() == "yes" );
			tmpDev.PSO_Drive_If = (childElement.firstChildElement("Drive_If").text() == "yes" );
			tmpDev.PSO_Drive_Bus_Unit = (childElement.firstChildElement("Drive_Bus_Unit").text() == "yes" );
			tmpDev.PSO_Drive_Index = (childElement.firstChildElement("Drive_Index").text() == "yes" );
			tmpDev.PSO_Drive_Media = (childElement.firstChildElement("Drive_Media").text() == "yes" );
			tmpDev.PSO_Drive_Cyls_Heads_Secs_Trans = (childElement.firstChildElement("Drive_Cyls_Heads_Secs_Trans").text() == "yes" );
			tmpDev.PSO_Drive_Snapshot = (childElement.firstChildElement("Drive_Snapshot").text() == "yes" );
			tmpDev.PSO_Drive_Cache = (childElement.firstChildElement("Drive_Cache").text() == "yes" );
			tmpDev.PSO_Drive_AIO = (childElement.firstChildElement("Drive_AIO").text() == "yes" );
			tmpDev.PSO_Drive_Format = (childElement.firstChildElement("Drive_Format").text() == "yes" );
			tmpDev.PSO_Drive_Serial = (childElement.firstChildElement("Drive_Serial").text() == "yes" );
			tmpDev.PSO_Drive_ADDR = (childElement.firstChildElement("Drive_ADDR").text() == "yes" );
			tmpDev.PSO_Drive_Boot = (childElement.firstChildElement("Drive_Boot").text() == "yes" );
			
			tmpDev.PSO_Boot_Order = (childElement.firstChildElement("Boot_Order").text() == "yes" );
			tmpDev.PSO_Initial_Graphic_Mode = (childElement.firstChildElement("Initial_Graphic_Mode").text() == "yes" );
			tmpDev.PSO_No_FB_Boot_Check = (childElement.firstChildElement("No_FB_Boot_Check").text() == "yes" );
			tmpDev.PSO_Win2K_Hack = (childElement.firstChildElement("Win2K_Hack").text() == "yes" );
			tmpDev.PSO_Kernel_KQEMU = (childElement.firstChildElement("Kernel_KQEMU").text() == "yes" );
			tmpDev.PSO_No_ACPI = (childElement.firstChildElement("No_ACPI").text() == "yes" );
			tmpDev.PSO_KVM = (childElement.firstChildElement("KVM").text() == "yes" );
			tmpDev.PSO_RTC_TD_Hack = (childElement.firstChildElement("RTC_TD_Hack").text() == "yes" );
			
			tmpDev.PSO_MTDBlock = (childElement.firstChildElement("MTDBlock").text() == "yes" );
			tmpDev.PSO_SD = (childElement.firstChildElement("SD").text() == "yes" );
			tmpDev.PSO_PFlash = (childElement.firstChildElement("PFlash").text() == "yes" );
			tmpDev.PSO_Name = (childElement.firstChildElement("Name").text() == "yes" );
			tmpDev.PSO_Curses = (childElement.firstChildElement("Curses").text() == "yes" );
			tmpDev.PSO_No_Frame = (childElement.firstChildElement("No_Frame").text() == "yes" );
			tmpDev.PSO_Alt_Grab = (childElement.firstChildElement("Alt_Grab").text() == "yes" );
			tmpDev.PSO_No_Quit = (childElement.firstChildElement("No_Quit").text() == "yes" );
			tmpDev.PSO_SDL = (childElement.firstChildElement("SDL").text() == "yes" );
			tmpDev.PSO_Portrait = (childElement.firstChildElement("Portrait").text() == "yes" );
			tmpDev.PSO_No_Shutdown = (childElement.firstChildElement("No_Shutdown").text() == "yes" );
			tmpDev.PSO_Startdate = (childElement.firstChildElement("Startdate").text() == "yes" );
			tmpDev.PSO_Show_Cursor = (childElement.firstChildElement("Show_Cursor").text() == "yes" );
			tmpDev.PSO_Bootp = (childElement.firstChildElement("Bootp").text() == "yes" );
			
			tmpDev.PSO_Net_type_vde = (childElement.firstChildElement("Net_type_vde").text() == "yes" );
			tmpDev.PSO_Net_type_dump = (childElement.firstChildElement("Net_type_dump").text() == "yes" );
			
			tmpDev.PSO_Net_name = (childElement.firstChildElement("Net_name").text() == "yes" );
			tmpDev.PSO_Net_addr = (childElement.firstChildElement("Net_addr").text() == "yes" );
			tmpDev.PSO_Net_vectors = (childElement.firstChildElement("Net_vectors").text() == "yes" );
			
			tmpDev.PSO_Net_net = (childElement.firstChildElement("Net_net").text() == "yes" );
			tmpDev.PSO_Net_host = (childElement.firstChildElement("Net_host").text() == "yes" );
			tmpDev.PSO_Net_restrict = (childElement.firstChildElement("Net_restrict").text() == "yes" );
			tmpDev.PSO_Net_dhcpstart = (childElement.firstChildElement("Net_dhcpstart").text() == "yes" );
			tmpDev.PSO_Net_dns = (childElement.firstChildElement("Net_dns").text() == "yes" );
			tmpDev.PSO_Net_tftp = (childElement.firstChildElement("Net_tftp").text() == "yes" );
			tmpDev.PSO_Net_bootfile = (childElement.firstChildElement("Net_bootfile").text() == "yes" );
			tmpDev.PSO_Net_smb = (childElement.firstChildElement("Net_smb").text() == "yes" );
			tmpDev.PSO_Net_hostfwd = (childElement.firstChildElement("Net_hostfwd").text() == "yes" );
			tmpDev.PSO_Net_guestfwd = (childElement.firstChildElement("Net_guestfwd").text() == "yes" );
			
			tmpDev.PSO_Net_ifname = (childElement.firstChildElement("Net_ifname").text() == "yes" );
			tmpDev.PSO_Net_bridge = (childElement.firstChildElement("Net_bridge").text() == "yes" );
			tmpDev.PSO_Net_script = (childElement.firstChildElement("Net_script").text() == "yes" );
			tmpDev.PSO_Net_downscript = (childElement.firstChildElement("Net_downscript").text() == "yes" );
			tmpDev.PSO_Net_helper = (childElement.firstChildElement("Net_helper").text() == "yes" );
			tmpDev.PSO_Net_sndbuf = (childElement.firstChildElement("Net_sndbuf").text() == "yes" );
			tmpDev.PSO_Net_vnet_hdr = (childElement.firstChildElement("Net_vnet_hdr").text() == "yes" );
			tmpDev.PSO_Net_vhost = (childElement.firstChildElement("Net_vhost").text() == "yes" );
			tmpDev.PSO_Net_vhostfd = (childElement.firstChildElement("Net_vhostfd").text() == "yes" );
			
			tmpDev.PSO_Net_listen = (childElement.firstChildElement("Net_listen").text() == "yes" );
			tmpDev.PSO_Net_connect = (childElement.firstChildElement("Net_connect").text() == "yes" );
			
			tmpDev.PSO_Net_mcast = (childElement.firstChildElement("Net_mcast").text() == "yes" );
			
			tmpDev.PSO_Net_sock = (childElement.firstChildElement("Net_sock").text() == "yes" );
			tmpDev.PSO_Net_port = (childElement.firstChildElement("Net_port").text() == "yes" );
			tmpDev.PSO_Net_group = (childElement.firstChildElement("Net_group").text() == "yes" );
			tmpDev.PSO_Net_mode = (childElement.firstChildElement("Net_mode").text() == "yes" );
			
			tmpDev.PSO_Net_file = (childElement.firstChildElement("Net_file").text() == "yes" );
			tmpDev.PSO_Net_len = (childElement.firstChildElement("Net_len").text() == "yes" );
			
			tmpDev.PSO_Enable_KVM = (childElement.firstChildElement("Enable_KVM").text() == "yes" );
			tmpDev.PSO_No_KVM = (childElement.firstChildElement("No_KVM").text() == "yes" );
			tmpDev.PSO_No_KVM_IRQChip = (childElement.firstChildElement("No_KVM_IRQChip").text() == "yes" );
			tmpDev.PSO_No_KVM_Pit = (childElement.firstChildElement("No_KVM_Pit").text() == "yes" );
			tmpDev.PSO_No_KVM_Pit_Reinjection = (childElement.firstChildElement("No_KVM_Pit_Reinjection").text() == "yes" );
			tmpDev.PSO_Enable_Nesting = (childElement.firstChildElement("Enable_Nesting").text() == "yes" );
			tmpDev.PSO_KVM_Shadow_Memory = (childElement.firstChildElement("KVM_Shadow_Memory").text() == "yes" );
			
			tmpDev.PSO_SPICE = (childElement.firstChildElement("SPICE").text() == "yes" );
			tmpDev.PSO_QXL = (childElement.firstChildElement("QXL").text() == "yes" );
			
			tmpDev.PSO_TFTP = (childElement.firstChildElement("TFTP").text() == "yes" );
			tmpDev.PSO_SMB = (childElement.firstChildElement("SMB").text() == "yes" );
			tmpDev.PSO_Std_VGA = (childElement.firstChildElement("Std_VGA").text() == "yes" );
			
			// Add devices
			Devices[ iter.key() ] = tmpDev;
		}
	}
	
	return true;
}

bool Emulator::Save() const
{
	QString path = Get_Emulator_File_Path();
	if( path.isEmpty() ) return false;
	
	// Document type
	QDomDocument domDocument( "AQEMU" );
	domDocument.appendChild( domDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"") );
	
	// Document version
	QDomElement rootElement = domDocument.createElement( "AQEMU_Emulator" );
	rootElement.setAttribute( "version", "0.8" );
	domDocument.appendChild( rootElement );
	
	// Emulator - general section
	QDomElement emulatorElement = domDocument.createElement( "Emulator" );
	rootElement.appendChild( emulatorElement );
	
	// Save emulator data
	QDomElement domElement;
	QDomText domText;
	
	// Name
	domElement = domDocument.createElement( "Name" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( Name );
	domElement.appendChild( domText );
	
	// Default
	domElement = domDocument.createElement( "Default" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( (Default ? "yes" : "no") );
	domElement.appendChild( domText );
	
	// Path
	domElement = domDocument.createElement( "Path" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( Path );
	domElement.appendChild( domText );
	
	// Check_Version
	domElement = domDocument.createElement( "Check_Version" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( (Check_Version ? "yes" : "no") );
	domElement.appendChild( domText );
	
	// Check_Available_Options
	domElement = domDocument.createElement( "Check_Available_Options" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( (Check_Available_Options ? "yes" : "no") );
	domElement.appendChild( domText );
	
	// Check_Available_Options
	domElement = domDocument.createElement( "Check_Available_Options" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( (Check_Available_Options ? "yes" : "no") );
	domElement.appendChild( domText );
	
	// Force_Version
	domElement = domDocument.createElement( "Force_Version" );
	emulatorElement.appendChild( domElement );
	domText = domDocument.createTextNode( (Force_Version ? "yes" : "no") );
	domElement.appendChild( domText );
	
	// Version
	domElement = domDocument.createElement( "Version" );
	emulatorElement.appendChild( domElement );
	
	QString emulatorVersion = Emulator_Version_To_String( Version );
	if( emulatorVersion.isEmpty() )
	{
		AQError( "bool Emulator::Save() const",
				 "Emulator version invalid! Use version: Obsolete" );
		domText = domDocument.createTextNode( "Obsolete" );
	}
	else domText = domDocument.createTextNode( emulatorVersion );
	
	domElement.appendChild( domText );
	
	// Save emulators executable path's
	domElement = domDocument.createElement( "Executable_Paths" );
	
	QMap<QString, QString>::const_iterator binFilesIter = Binary_Files.constBegin();
	while( binFilesIter != Binary_Files.constEnd() )
	{
		QDomElement binElement = domDocument.createElement( binFilesIter.key() );
		domText = domDocument.createTextNode( binFilesIter.value() );
		binElement.appendChild( domText );
		domElement.appendChild( binElement );
		
		++binFilesIter;
	}
	
	emulatorElement.appendChild( domElement );
	
	// Save devices
	QMap<QString, Available_Devices>::const_iterator devicesIter = Devices.constBegin();
	while( devicesIter != Devices.constEnd() )
	{
		const Available_Devices &tmpDev = devicesIter.value();
		domElement = domDocument.createElement( devicesIter.key() );
		
		QDomElement deviceElement;
		QDomElement thirdElement;
		
		// System
		deviceElement = domDocument.createElement( "System_Caption" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( tmpDev.System.Caption );
		deviceElement.appendChild( domText );
		
		deviceElement = domDocument.createElement( "System_QEMU_Name" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( tmpDev.System.QEMU_Name );
		deviceElement.appendChild( domText );
		
		// CPUs
		deviceElement = domDocument.createElement( "CPU_List" );
		for( int ix = 0; ix < tmpDev.CPU_List.count(); ix++ )
		{
			thirdElement = domDocument.createElement( QString("Caption%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.CPU_List[ix].Caption );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
			
			thirdElement = domDocument.createElement( QString("Name%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.CPU_List[ix].QEMU_Name );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
		}
		domElement.appendChild( deviceElement );
		
		// Machines
		deviceElement = domDocument.createElement( "Machine_List" );
		for( int ix = 0; ix < tmpDev.Machine_List.count(); ix++ )
		{
			thirdElement = domDocument.createElement( QString("Caption%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.Machine_List[ix].Caption );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
			
			thirdElement = domDocument.createElement( QString("Name%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.Machine_List[ix].QEMU_Name );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
		}
		domElement.appendChild( deviceElement );
		
		// Network Cards
		deviceElement = domDocument.createElement( "Network_Card_List" );
		for( int ix = 0; ix < tmpDev.Network_Card_List.count(); ix++ )
		{
			thirdElement = domDocument.createElement( QString("Caption%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.Network_Card_List[ix].Caption );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
			
			thirdElement = domDocument.createElement( QString("Name%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.Network_Card_List[ix].QEMU_Name );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
		}
		domElement.appendChild( deviceElement );
		
		// Audio Cards
		deviceElement = domDocument.createElement( "Audio_Card_List" );
		
		thirdElement = domDocument.createElement( "sb16" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_sb16 ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "es1370" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_es1370 ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "Adlib" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_Adlib ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "PC_Speaker" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_PC_Speaker ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "GUS" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_GUS ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "AC97" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_AC97 ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "HDA" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_HDA ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		thirdElement = domDocument.createElement( "cs4231a" );
		domText = domDocument.createTextNode( (tmpDev.Audio_Card_List.Audio_cs4231a ? "yes" : "no") );
		thirdElement.appendChild( domText );
		deviceElement.appendChild( thirdElement );
		
		domElement.appendChild( deviceElement );
		
		// Video Cards
		deviceElement = domDocument.createElement( "Video_Card_List" );
		for( int ix = 0; ix < tmpDev.Video_Card_List.count(); ix++ )
		{
			thirdElement = domDocument.createElement( QString("Caption%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.Video_Card_List[ix].Caption );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
			
			thirdElement = domDocument.createElement( QString("Name%1").arg(ix) );
			domText = domDocument.createTextNode( tmpDev.Video_Card_List[ix].QEMU_Name );
			thirdElement.appendChild( domText );
			deviceElement.appendChild( thirdElement );
		}
		domElement.appendChild( deviceElement );
		
		// Platform Specific Options
		// PSO_SMP_Count
		deviceElement = domDocument.createElement( "SMP_Count" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( QString::number(tmpDev.PSO_SMP_Count) );
		deviceElement.appendChild( domText );
		
		// PSO_SMP_Cores
		deviceElement = domDocument.createElement( "SMP_Cores" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SMP_Cores ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SMP_Threads
		deviceElement = domDocument.createElement( "SMP_Threads" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SMP_Threads ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SMP_Sockets
		deviceElement = domDocument.createElement( "SMP_Sockets" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SMP_Sockets ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SMP_MaxCPUs
		deviceElement = domDocument.createElement( "SMP_MaxCPUs" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SMP_MaxCPUs ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Device
		deviceElement = domDocument.createElement( "Device" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Device ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Device_USB_EHCI
		deviceElement = domDocument.createElement( "Device_USB_EHCI" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Device_USB_EHCI ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive
		deviceElement = domDocument.createElement( "Drive" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_File
		deviceElement = domDocument.createElement( "Drive_File" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_File ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_If
		deviceElement = domDocument.createElement( "Drive_If" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_If ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Bus_Unit
		deviceElement = domDocument.createElement( "Drive_Bus_Unit" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Bus_Unit ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Index
		deviceElement = domDocument.createElement( "Drive_Index" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Index ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Media
		deviceElement = domDocument.createElement( "Drive_Media" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Media ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Cyls_Heads_Secs_Trans
		deviceElement = domDocument.createElement( "Drive_Cyls_Heads_Secs_Trans" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Cyls_Heads_Secs_Trans ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Snapshot
		deviceElement = domDocument.createElement( "Drive_Snapshot" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Snapshot ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Cache
		deviceElement = domDocument.createElement( "Drive_Cache" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Cache ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_AIO
		deviceElement = domDocument.createElement( "Drive_AIO" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_AIO ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Format
		deviceElement = domDocument.createElement( "Drive_Format" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Format ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Serial
		deviceElement = domDocument.createElement( "Drive_Serial" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Serial ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_ADDR
		deviceElement = domDocument.createElement( "Drive_ADDR" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_ADDR ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Drive_Boot
		deviceElement = domDocument.createElement( "Drive_Boot" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Drive_Boot ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Boot_Order
		deviceElement = domDocument.createElement( "Boot_Order" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Boot_Order ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Initial_Graphic_Mode
		deviceElement = domDocument.createElement( "Initial_Graphic_Mode" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Initial_Graphic_Mode ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_FB_Boot_Check
		deviceElement = domDocument.createElement( "No_FB_Boot_Check" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_FB_Boot_Check ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Win2K_Hack
		deviceElement = domDocument.createElement( "Win2K_Hack" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Win2K_Hack ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Kernel_KQEMU
		deviceElement = domDocument.createElement( "Kernel_KQEMU" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Kernel_KQEMU ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_ACPI
		deviceElement = domDocument.createElement( "No_ACPI" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_ACPI ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_KVM
		deviceElement = domDocument.createElement( "KVM" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_KVM ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_RTC_TD_Hack
		deviceElement = domDocument.createElement( "RTC_TD_Hack" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_RTC_TD_Hack ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_MTDBlock
		deviceElement = domDocument.createElement( "MTDBlock" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_MTDBlock ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SD
		deviceElement = domDocument.createElement( "SD" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SD ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_PFlash
		deviceElement = domDocument.createElement( "PFlash" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_PFlash ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Name
		deviceElement = domDocument.createElement( "Name" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Name ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Curses
		deviceElement = domDocument.createElement( "Curses" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Curses ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_Frame
		deviceElement = domDocument.createElement( "No_Frame" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_Frame ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Alt_Grab
		deviceElement = domDocument.createElement( "Alt_Grab" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Alt_Grab ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_Quit
		deviceElement = domDocument.createElement( "No_Quit" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_Quit ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SDL
		deviceElement = domDocument.createElement( "SDL" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SDL ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Portrait
		deviceElement = domDocument.createElement( "Portrait" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Portrait ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_Shutdown
		deviceElement = domDocument.createElement( "No_Shutdown" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_Shutdown ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Startdate
		deviceElement = domDocument.createElement( "Startdate" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Startdate ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Show_Cursor
		deviceElement = domDocument.createElement( "Show_Cursor" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Show_Cursor ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Bootp
		deviceElement = domDocument.createElement( "Bootp" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Bootp ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_type_vde
		deviceElement = domDocument.createElement( "Net_type_vde" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_type_vde ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_type_dump
		deviceElement = domDocument.createElement( "Net_type_dump" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_type_dump ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_name
		deviceElement = domDocument.createElement( "Net_name" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_name ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_addr
		deviceElement = domDocument.createElement( "Net_addr" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_addr ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_vectors
		deviceElement = domDocument.createElement( "Net_vectors" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_vectors ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_net
		deviceElement = domDocument.createElement( "Net_net" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_net ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_host
		deviceElement = domDocument.createElement( "Net_host" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_host ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_restrict
		deviceElement = domDocument.createElement( "Net_restrict" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_restrict ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_dhcpstart
		deviceElement = domDocument.createElement( "Net_dhcpstart" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_dhcpstart ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_dns
		deviceElement = domDocument.createElement( "Net_dns" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_dns ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_tftp
		deviceElement = domDocument.createElement( "Net_tftp" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_tftp ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_bootfile
		deviceElement = domDocument.createElement( "Net_bootfile" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_bootfile ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_smb
		deviceElement = domDocument.createElement( "Net_smb" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_smb ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_hostfwd
		deviceElement = domDocument.createElement( "Net_hostfwd" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_hostfwd ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_guestfwd
		deviceElement = domDocument.createElement( "Net_guestfwd" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_guestfwd ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_ifname
		deviceElement = domDocument.createElement( "Net_ifname" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_ifname ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_bridge
		deviceElement = domDocument.createElement( "Net_bridge" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_bridge ? "yes" : "no") );
		deviceElement.appendChild( domText );

		// PSO_Net_script
		deviceElement = domDocument.createElement( "Net_script" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_script ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_downscript
		deviceElement = domDocument.createElement( "Net_downscript" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_downscript ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_helper
		deviceElement = domDocument.createElement( "Net_helper" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_helper ? "yes" : "no") );
		deviceElement.appendChild( domText );

		// PSO_Net_sndbuf
		deviceElement = domDocument.createElement( "Net_sndbuf" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_sndbuf ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_vnet_hdr
		deviceElement = domDocument.createElement( "Net_vnet_hdr" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_vnet_hdr ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_vhost
		deviceElement = domDocument.createElement( "Net_vhost" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_vhost ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_Net_vhostfd
		deviceElement = domDocument.createElement( "Net_vhostfd" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_vhostfd ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_listen
		deviceElement = domDocument.createElement( "Net_listen" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_listen ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_connect
		deviceElement = domDocument.createElement( "Net_connect" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_connect ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_mcast
		deviceElement = domDocument.createElement( "Net_mcast" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_mcast ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_sock
		deviceElement = domDocument.createElement( "Net_sock" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_sock ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_port
		deviceElement = domDocument.createElement( "Net_port" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_port ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_group
		deviceElement = domDocument.createElement( "Net_group" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_group ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_mode
		deviceElement = domDocument.createElement( "Net_mode" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_mode ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Net_file
		deviceElement = domDocument.createElement( "Net_file" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_file ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Net_len
		deviceElement = domDocument.createElement( "Net_len" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Net_len ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		// PSO_Enable_KVM
		deviceElement = domDocument.createElement( "Enable_KVM" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Enable_KVM ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_KVM
		deviceElement = domDocument.createElement( "No_KVM" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_KVM ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_KVM_IRQChip
		deviceElement = domDocument.createElement( "No_KVM_IRQChip" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_KVM_IRQChip ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_KVM_Pit
		deviceElement = domDocument.createElement( "No_KVM_Pit" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_KVM_Pit ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_No_KVM_Pit_Reinjection
		deviceElement = domDocument.createElement( "No_KVM_Pit_Reinjection" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_No_KVM_Pit_Reinjection ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Enable_Nesting
		deviceElement = domDocument.createElement( "Enable_Nesting" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Enable_Nesting ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_KVM_Shadow_Memory
		deviceElement = domDocument.createElement( "KVM_Shadow_Memory" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_KVM_Shadow_Memory ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SPICE
		deviceElement = domDocument.createElement( "SPICE" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SPICE ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_QXL
		deviceElement = domDocument.createElement( "QXL" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_QXL ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_TFTP
		deviceElement = domDocument.createElement( "TFTP" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_TFTP ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_SMB
		deviceElement = domDocument.createElement( "SMB" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_SMB ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		// PSO_Std_VGA
		deviceElement = domDocument.createElement( "Std_VGA" );
		domElement.appendChild( deviceElement );
		domText = domDocument.createTextNode( (tmpDev.PSO_Std_VGA ? "yes" : "no") );
		deviceElement.appendChild( domText );
		
		
		emulatorElement.appendChild( domElement );
		
		// Next
		++devicesIter;
	}
	
	// Save to file
	QFile outFile( path );
	
	if( ! outFile.open(QFile::WriteOnly | QFile::Truncate) )
	{
		AQError( "bool Emulator::Save() const",
				 QString("Cannot create emulator file \"%1\"").arg(path) );
		return false;
	}
	
	QTextStream outStream( &outFile );
	domDocument.save( outStream, 4 );
	
	return true;
}

QString Emulator::Get_Emulator_File_Path() const
{
	if( Name.isEmpty() )
	{
		AQError( "QString Emulator::Get_Emulator_File_Path() const",
				 "Name is empty" );
		return "";
	}

	QSettings settigs;
	QFileInfo info( settigs.fileName() );
	QString aqemuSettingsFolder = info.absoluteDir().absolutePath();

	if( ! QFile::exists(aqemuSettingsFolder) )
	{
		AQError( "QString Emulator::Get_Emulator_File_Path() const",
				 QString("Cannot get path for save emulator! Folder \"%1\" not exists!").arg(aqemuSettingsFolder) );
		return "";
	}

	// All ok. Return result
	return QDir::toNativeSeparators( aqemuSettingsFolder +
									 (aqemuSettingsFolder.endsWith("/") || aqemuSettingsFolder.endsWith("\\") ? "" : "/") +
									 Name + ".emulator" );
}

/*
void Emulator::Set_Type( VM::Emulator_Type type )
{
	Type = type;
	 // FIXME
	if( Type != type )
	{
		// bin names
		QStringList QEMU_Binary_Names;
		QEMU_Binary_Names << "qemu";
		QEMU_Binary_Names << "qemu-system-x86_64";
		QEMU_Binary_Names << "qemu-system-arm";
		QEMU_Binary_Names << "qemu-system-cris";
		QEMU_Binary_Names << "qemu-system-m68k";
		QEMU_Binary_Names << "qemu-system-mips";
		QEMU_Binary_Names << "qemu-system-mipsel";
		QEMU_Binary_Names << "qemu-system-mips64";
		QEMU_Binary_Names << "qemu-system-mips64el";
		QEMU_Binary_Names << "qemu-system-ppc";
		QEMU_Binary_Names << "qemu-system-ppc64";
		QEMU_Binary_Names << "qemu-system-ppcemb";
		QEMU_Binary_Names << "qemu-system-sh4";
		QEMU_Binary_Names << "qemu-system-sh4eb";
		QEMU_Binary_Names << "qemu-system-sparc";
		
		QStringList KVM_Binary_Names;
		KVM_Binary_Names << "kvm";
		
		if( type == VM::QEMU )
		{
			Binary_Files.clear();
			for( int ix = 0; ix < QEMU_Binary_Names.count(); ++ix ) Binary_Files[ QEMU_Binary_Names[ix] ] = "";
		}
		else if( type == VM::KVM )
		{
			Binary_Files.clear();
			for( int ix = 0; ix < KVM_Binary_Names.count(); ++ix ) Binary_Files[ KVM_Binary_Names[ix] ] = "";
		}
		else
		{
			AQError( "void Emulator::Set_Type( const QString &type )",
					 "Type Incorrect! Use Default: QEMU" );
			
			Binary_Files.clear();
			for( int ix = 0; ix < QEMU_Binary_Names.count(); ++ix ) Binary_Files[ QEMU_Binary_Names[ix] ] = "";
		}
		
		Type = type;
	}
}*/

const QString &Emulator::Get_Name() const
{
	return Name;
}

void Emulator::Set_Name( const QString &name )
{
	Name = name;
}

bool Emulator::Get_Default() const
{
	return Default;
}

void Emulator::Set_Default( bool on )
{
	Default = on;
}

const QString &Emulator::Get_Path() const
{
	return Path;
}

void Emulator::Set_Path( const QString &path )
{
	Path = path;
}

bool Emulator::Get_Check_Version() const
{
	return Check_Version;
}

void Emulator::Set_Check_Version( bool check )
{
	Check_Version = check;
}

bool Emulator::Get_Check_Available_Options() const
{
	return Check_Available_Options;
}

void Emulator::Set_Check_Available_Options( bool check )
{
	Check_Available_Options = check;
}

bool Emulator::Get_Force_Version() const
{
	return Force_Version;
}

void Emulator::Set_Force_Version( bool on )
{
	Force_Version = on;
}

VM::Emulator_Version Emulator::Get_Version() const
{
	return Version;
}

void Emulator::Set_Version( VM::Emulator_Version ver )
{
	Version = ver;
}

const QMap<QString, QString> &Emulator::Get_Binary_Files() const
{
	return Binary_Files;
}

void Emulator::Set_Binary_Files( const QMap<QString, QString> &files )
{
	Binary_Files = files;
}

const QMap<QString, Available_Devices> &Emulator::Get_Devices() const
{
	if( Force_Version || Check_Version )
	{
		switch( Version )
		{
			
			case VM::QEMU_2_0:
				return System_Info::Emulator_QEMU_2_0;
				
			/*case VM::KVM_2_0:
				return System_Info::Emulator_KVM_2_0; */ //tobgle //FIXME?
				
			default:
				AQError( "const QMap<QString, Available_Devices> &Emulator::Get_Devices() const",
						 "Emulator Version Incorrect!" );
				static QMap<QString, Available_Devices> emptyMap;
				return emptyMap;
		}		
	}
	else // Check Options or Save Options
	{
		return Devices;
	}
}

void Emulator::Set_Devices( const QMap<QString, Available_Devices> &devices )
{
	Devices = devices;
}

//---------------------------------------------------------------------------

// VM_Native_Storage_Device --------------------------------------------------

VM_Native_Storage_Device::VM_Native_Storage_Device()
{
	UFile_Path = false;
	File_Path = "";
	UInterface = false;
	Interface = VM::DI_IDE;
	UBus_Unit = false;
	Bus = 0;
	Unit = 0;
	UIndex = false;
	Index = 0;
	UMedia = false;
	Media = VM::DM_Disk;
	Uhdachs = false;
	Cyls = 0;
	Heads = 0;
	Secs = 0;
	Trans = 0;
	USnapshot = false;
	Snapshot = false;
	UCache = false;
	Cache = "none";
	UAIO = false;
	AIO = "threads";
	UBoot = false;
	Boot = false;
}

VM_Native_Storage_Device::VM_Native_Storage_Device( const VM_Native_Storage_Device &sd )
{
	UFile_Path = sd.Use_File_Path();
	File_Path = sd.Get_File_Path();
	UInterface = sd.Use_Interface();
	Interface = sd.Get_Interface();
	UBus_Unit = sd.Use_Bus_Unit();
	Bus = sd.Get_Bus();
	Unit = sd.Get_Unit();
	UIndex = sd.Use_Index();
	Index = sd.Get_Index();
	UMedia = sd.Use_Media();
	Media = sd.Get_Media();
	Uhdachs = sd.Use_hdachs();
	Cyls = sd.Get_Cyls();
	Heads = sd.Get_Heads();
	Secs = sd.Get_Secs();
	Trans = sd.Get_Trans();
	USnapshot = sd.Use_Snapshot();
	Snapshot = sd.Get_Snapshot();
	UCache = sd.Use_Cache();
	Cache = sd.Get_Cache();
	UAIO = sd.Use_AIO();
	AIO = sd.Get_AIO();
	UBoot = sd.Use_Boot();
	Boot = sd.Get_Boot();
	UDiscard = sd.Use_Discard();
	Discard = sd.Get_Discard();
}

bool VM_Native_Storage_Device::Get_Native_Mode() const
{
	if( UAIO ) return true;
	if( UBoot ) return true;
	if( UBus_Unit ) return true;
	if( UCache ) return true;
	if( UFile_Path ) return true;
	if( Uhdachs ) return true;
	if( UIndex ) return true;
	if( UInterface ) return true;
	if( UMedia ) return true;
	if( USnapshot ) return true;
	
	// Native device options not used
	return false;
}

QString VM_Native_Storage_Device::Get_QEMU_Device_Name() const
{
	// FIXME Get_QEMU_Device_Name()
	return "ide0-hd1";
}

bool VM_Native_Storage_Device::operator==( const VM_Native_Storage_Device &sd ) const
{
	if( UFile_Path == sd.Use_File_Path() &&
		File_Path == sd.Get_File_Path() &&
		UInterface == sd.Use_Interface() &&
		Interface == sd.Get_Interface() &&
		UBus_Unit == sd.Use_Bus_Unit() &&
		Bus == sd.Get_Bus() &&
		Unit == sd.Get_Unit() &&
		UIndex == sd.Use_Index() &&
		Index == sd.Get_Index() &&
		UMedia == sd.Use_Media() &&
		Media == sd.Get_Media() &&
		Uhdachs == sd.Use_hdachs() &&
		Cyls == sd.Get_Cyls() &&
		Heads == sd.Get_Heads() &&
		Secs == sd.Get_Secs() &&
		Trans == sd.Get_Trans() &&
		USnapshot == sd.Use_Snapshot() &&
		Snapshot == sd.Get_Snapshot() &&
		UCache == sd.Use_Cache() &&
		Cache == sd.Get_Cache() &&
		UAIO == sd.Use_AIO() &&
		AIO == sd.Get_AIO() &&
		UBoot == sd.Use_Boot() &&
		Boot == sd.Get_Boot() &&
		UDiscard == sd.Use_Discard() &&
		Discard == sd.Get_Discard() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Native_Storage_Device::operator!=( const VM_Native_Storage_Device &sd ) const
{
	return ! ( operator==(sd) );
}

bool VM_Native_Storage_Device::Use_File_Path() const
{
	return UFile_Path;
}

void VM_Native_Storage_Device::Use_File_Path( bool use )
{
	UFile_Path = use;
}

const QString &VM_Native_Storage_Device::Get_File_Path() const
{
	return File_Path;
}

void VM_Native_Storage_Device::Set_File_Path( const QString &path )
{
	File_Path = path;
}

bool VM_Native_Storage_Device::Use_Interface() const
{
	return UInterface;
}

void VM_Native_Storage_Device::Use_Interface( bool use )
{
	UInterface = use;
}

VM::Device_Interface VM_Native_Storage_Device::Get_Interface() const
{
	return Interface;
}

void VM_Native_Storage_Device::Set_Interface( VM::Device_Interface di )
{
	Interface = di;
}

bool VM_Native_Storage_Device::Use_Bus_Unit() const
{
	return UBus_Unit;
}

void VM_Native_Storage_Device::Use_Bus_Unit( bool use )
{
	UBus_Unit = use;
}

int VM_Native_Storage_Device::Get_Bus() const
{
	return Bus;
}

void VM_Native_Storage_Device::Set_Bus( int bus )
{
	Bus = bus;
}

int VM_Native_Storage_Device::Get_Unit() const
{
	return Unit;
}

void VM_Native_Storage_Device::Set_Unit( int unit )
{
	Unit = unit;
}

bool VM_Native_Storage_Device::Use_Index() const
{
	return UIndex;
}

void VM_Native_Storage_Device::Use_Index( bool use )
{
	UIndex = use;
}

int VM_Native_Storage_Device::Get_Index() const
{
	return Index;
}

void VM_Native_Storage_Device::Set_Index( int index )
{
	Index = index;
}

bool VM_Native_Storage_Device::Use_Media() const
{
	return UMedia;
}

void VM_Native_Storage_Device::Use_Media( bool use )
{
	UMedia = use;
}

VM::Device_Media VM_Native_Storage_Device::Get_Media() const
{
	return Media;
}

void VM_Native_Storage_Device::Set_Media( VM::Device_Media media )
{
	Media = media;
}

bool VM_Native_Storage_Device::Use_hdachs() const
{
	return Uhdachs;
}

void VM_Native_Storage_Device::Use_hdachs( bool use )
{
	Uhdachs = use;
}

unsigned long VM_Native_Storage_Device::Get_Cyls() const
{
	return Cyls;
}

void VM_Native_Storage_Device::Set_Cyls( unsigned long cyls )
{
	Cyls = cyls;
}

unsigned long VM_Native_Storage_Device::Get_Heads() const
{
	return Heads;
}

void VM_Native_Storage_Device::Set_Heads( unsigned long heads )
{
	Heads = heads;
}

unsigned long VM_Native_Storage_Device::Get_Secs() const
{
	return Secs;
}

void VM_Native_Storage_Device::Set_Secs( unsigned long secs )
{
	Secs = secs;
}

unsigned long VM_Native_Storage_Device::Get_Trans() const
{
	return Trans;
}

void VM_Native_Storage_Device::Set_Trans( unsigned long trans )
{
	Trans = trans;
}

bool VM_Native_Storage_Device::Use_Snapshot() const
{
	return USnapshot;
}

void VM_Native_Storage_Device::Use_Snapshot( bool use )
{
	USnapshot = use;
}

bool VM_Native_Storage_Device::Get_Snapshot() const
{
	return Snapshot;
}

void VM_Native_Storage_Device::Set_Snapshot( bool snapshot )
{
	Snapshot = snapshot;
}

bool VM_Native_Storage_Device::Use_Cache() const
{
	return UCache;
}

void VM_Native_Storage_Device::Use_Cache( bool use )
{
	UCache = use;
}

const QString &VM_Native_Storage_Device::Get_Cache() const
{
	return Cache;
}

void VM_Native_Storage_Device::Set_Cache( const QString &cache )
{
	Cache = cache;
}

bool VM_Native_Storage_Device::Use_AIO() const
{
	return UAIO;
}

void VM_Native_Storage_Device::Use_AIO( bool use )
{
	UAIO = use;
}

const QString &VM_Native_Storage_Device::Get_AIO() const
{
	return AIO;
}

void VM_Native_Storage_Device::Set_AIO( const QString &aio )
{
	AIO = aio;
}

bool VM_Native_Storage_Device::Use_Boot() const
{
	return UBoot;
}

void VM_Native_Storage_Device::Use_Boot( bool use )
{
	UBoot = use;
}

bool VM_Native_Storage_Device::Get_Boot() const
{
	return Boot;
}

void VM_Native_Storage_Device::Set_Boot( bool boot )
{
	Boot = boot;
}


bool VM_Native_Storage_Device::Use_Discard() const
{
	return UDiscard;
}

void VM_Native_Storage_Device::Use_Discard( bool use )
{
	UDiscard = use;
}

bool VM_Native_Storage_Device::Get_Discard() const
{
	return Discard;
}

void VM_Native_Storage_Device::Set_Discard( bool discard )
{
	Discard = discard;
}

//===========================================================================

// VM_Storage_Device Class --------------------------------------------------

VM_Storage_Device::VM_Storage_Device()
{
	Enabled = false;
	File_Name = "";
    Native_Device = VM_Native_Storage_Device();
}

VM_Storage_Device::VM_Storage_Device( const VM_Storage_Device &device )
{
	Enabled = device.Get_Enabled();
	File_Name = device.Get_File_Name();
    Native_Device = device.Get_Native_Device();
}

VM_Storage_Device::VM_Storage_Device( bool enabled, const QString &file_name )
{
	Enabled = enabled;
	File_Name = file_name;
    Native_Device = VM_Native_Storage_Device();
    Native_Device.Set_File_Path( file_name );
}

VM_Storage_Device::VM_Storage_Device( bool enabled, const QString &file_name, bool Native_mode, const VM_Native_Storage_Device &device )
{
	Enabled = enabled;
	File_Name = file_name;
    Native_Device = VM_Native_Storage_Device( device );
    Native_Device.Set_File_Path( file_name );
}

bool VM_Storage_Device::operator==( const VM_Storage_Device &device ) const
{
	if( Enabled == device.Get_Enabled() &&
		File_Name == device.Get_File_Name() &&
        Native_Device == device.Get_Native_Device() )
	{
		return true;
	}
	else return false;
}

bool VM_Storage_Device::operator!=( const VM_Storage_Device &device ) const
{
	return ! operator==( device );
}

bool VM_Storage_Device::Get_Enabled() const
{
	return Enabled;
}

void VM_Storage_Device::Set_Enabled( bool enabled )
{
	Enabled = enabled;
}

const QString &VM_Storage_Device::Get_File_Name() const
{
	return File_Name;
}

void VM_Storage_Device::Set_File_Name( const QString &file_name )
{
	File_Name = file_name;
    Native_Device.Set_File_Path( file_name );
}

bool VM_Storage_Device::Get_Native_Mode() const
{
    return Native_Device.Get_Native_Mode();
}

const VM_Native_Storage_Device &VM_Storage_Device::Get_Native_Device() const
{
    return Native_Device;
}

void VM_Storage_Device::Set_Native_Device( const VM_Native_Storage_Device &device )
{
    Native_Device = device;
}

//===========================================================================


// VM_Shared_Folder Class --------------------------------------------------

VM_Shared_Folder::VM_Shared_Folder()
{
	Enabled = false;
	Folder = "";
}

VM_Shared_Folder::VM_Shared_Folder( const VM_Shared_Folder &device )
{
	Enabled = device.Get_Enabled();
	Folder = device.Get_Folder();
}

VM_Shared_Folder::VM_Shared_Folder( bool enabled, const QString &folder )
{
	Enabled = enabled;
	Folder = folder;
}

bool VM_Shared_Folder::operator==( const VM_Shared_Folder &folder ) const
{
	if( Enabled == folder.Get_Enabled() &&
		Folder == folder.Get_Folder() )
	{
		return true;
	}
	else return false;
}

bool VM_Shared_Folder::operator!=( const VM_Shared_Folder &folder ) const
{
	return ! operator==( folder );
}

bool VM_Shared_Folder::Get_Enabled() const
{
	return Enabled;
}

void VM_Shared_Folder::Set_Enabled( bool enabled )
{
	Enabled = enabled;
}

const QString &VM_Shared_Folder::Get_Folder() const
{
	return Folder;
}

void VM_Shared_Folder::Set_Folder( const QString &folder )
{
	Folder = folder;
}

//===========================================================================


// VM_HDD Class -------------------------------------------------------------

VM_HDD::VM_HDD() : VM_Storage_Device()
{
	Reset_Info();
}

VM_HDD::VM_HDD( const VM_HDD &hd ) : VM_Storage_Device( hd )
{
	if( File_Name.isEmpty() ) Reset_Info();
}

VM_HDD::VM_HDD( bool enabled, const QString &im_pach )
{
	Set_Enabled( enabled );
	Set_File_Name( im_pach );
	
	if( im_pach.isEmpty() ) Reset_Info();
}

bool VM_HDD::operator==( const VM_HDD &v ) const
{
	return VM_Storage_Device::operator==( v );
}

bool VM_HDD::operator!=( const VM_HDD &v ) const
{
	return ! ( operator==(v) );
}

void VM_HDD::Set_File_Name( const QString &file_name )
{
	if( file_name.isEmpty() ) Reset_Info();
	
	VM_Storage_Device::Set_File_Name( file_name );
}

void VM_HDD::Set_Disk_Info( VM::Disk_Info info )
{
	Disk_Format = info.Disk_Format;
	Virtual_Size = info.Virtual_Size;
	Disk_Size = info.Disk_Size;
	Cluster_Size = info.Cluster_Size;
}

const QString &VM_HDD::Get_Image_Format() const
{
	return Disk_Format;
}

const VM::Device_Size &VM_HDD::Get_Virtual_Size() const
{
	return Virtual_Size;
}

const VM::Device_Size &VM_HDD::Get_Disk_Size() const
{
	return Disk_Size;
}

int VM_HDD::Get_Cluster_Size() const
{
	return Cluster_Size;
}

double VM_HDD::Get_Virtual_Size_in_GB() const
{
	if( Virtual_Size.Size < 1 ) return 0.0;
	
	int div = 0;
	
	switch( Virtual_Size.Suffix )
	{
		case VM::Size_Suf_Kb:
			div = 1048576;
			break;
			
		case VM::Size_Suf_Mb:
			div = 1024;
			break;
			
		case VM::Size_Suf_Gb:
			div = 1;
			break;
			
		default:
			AQError( "double VM_HDD::Get_Virtual_Size_in_GB() const",
					 "Default Section!" );
			return 0.0;
	}
	
	return Virtual_Size.Size / div;
}

QString VM_HDD::Get_Complete_Virtual_Size() const
{
	return QString::number(Virtual_Size.Size) + Size_Suffix_to_String(Virtual_Size.Suffix);
}

QString VM_HDD::Get_Complete_Disk_Size() const
{
	return QString::number(Disk_Size.Size) + Size_Suffix_to_String(Disk_Size.Suffix);
}

void VM_HDD::Set_Virtual_Size( const VM::Device_Size &size )
{
	Virtual_Size = size;
}

void VM_HDD::Set_Image_Format( const QString &format )
{
	Disk_Format = format;
}

QString VM_HDD::Size_Suffix_to_String( VM::Size_Suffix suf ) const
{
	switch( suf )
	{
		case VM::Size_Suf_Kb:
			return "K";
			
		case VM::Size_Suf_Mb:
			return "M";
			
		case VM::Size_Suf_Gb:
			return "G";
			
		default:
			AQError( "QString VM_HDD::Size_Suffix_to_String( VM::Size_Suffix suf ) const",
					 "Size Suffix Incorrect!" );
			return "";
	}
}

VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const
{
	VM::Device_Size zero_size, hd_size;
	zero_size.Size = 0.0;
	zero_size.Suffix = VM::Size_Suf_Gb;
	
	if( size.isEmpty() )
	{
		AQDebug( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "Size String is Empty..." );
		return zero_size;
	}
	
	QRegExp RegInfo = QRegExp( "([\\d]+|[\\d]+[.,][\\d]+).*([KMG]+)" );
	
	if( ! RegInfo.exactMatch(size) )
	{
		AQError( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "Cannot Match RegExp!" );
		return zero_size;
	}
	
	QStringList info_lines = RegInfo.capturedTexts();
	
	bool ok = false;
	hd_size.Size = info_lines[1].toDouble( &ok );
	
	if( ! ok )
	{
		AQError( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "Cannot Convert String to Double!" );
		return zero_size;
	}
	
	if( info_lines[2] == "K" )
		hd_size.Suffix = VM::Size_Suf_Kb;
	else if( info_lines[2] == "M" )
		hd_size.Suffix = VM::Size_Suf_Mb;
	else if( info_lines[2] == "G" )
		hd_size.Suffix = VM::Size_Suf_Gb;
	else
	{
		AQError( "VM::Device_Size VM_HDD::String_to_Device_Size( const QString &size ) const",
				 "HDD Size Suffix Not K,M,G!" );
		return zero_size;
	}
	
	return hd_size;
}

void VM_HDD::Reset_Info()
{
	File_Name = "";
	Disk_Format = "";

	VM::Device_Size zero_size;
	zero_size.Size = 0.0;
	zero_size.Suffix = VM::Size_Suf_Gb;

	Virtual_Size = zero_size;
	Disk_Size = zero_size;

	Cluster_Size = 0;
}

//===========================================================================

// VM_Snapshot --------------------------------------------------------------

VM_Snapshot::VM_Snapshot()
{
	Tag = "";
	ID = "";
	Size = "";
	Date = "";
	VM_Clock = "";
	Name = "";
	Description = "";
}

VM_Snapshot::VM_Snapshot( const VM_Snapshot &s )
{
	Tag = s.Get_Tag();
	ID = s.Get_ID();
	Size = s.Get_Size();
	Date = s.Get_Date();
	VM_Clock = s.Get_VM_Clock();
	Name = s.Get_Name();
	Description = s.Get_Description();
}

bool VM_Snapshot::operator==( const VM_Snapshot &s ) const
{
	if( Tag == s.Get_Tag() &&
		ID == s.Get_ID() &&
		Size == s.Get_Size() &&
		Date == s.Get_Date() &&
		VM_Clock == s.Get_VM_Clock() &&
		Name == s.Get_Name() &&
		Description == s.Get_Description() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Snapshot::operator!=( const VM_Snapshot &s ) const
{
	return ! ( operator==(s) );
}

const QString &VM_Snapshot::Get_Tag() const
{
	return Tag;
}

void VM_Snapshot::Set_Tag( const QString &tag )
{
	Tag = tag;
}

const QString &VM_Snapshot::Get_ID() const
{
	return ID;
}

void VM_Snapshot::Set_ID( const QString &id )
{
	ID = id;
}

const QString &VM_Snapshot::Get_Size() const
{
	return Size;
}

void VM_Snapshot::Set_Size( const QString &size )
{
	Size = size;
}

const QString &VM_Snapshot::Get_Date() const
{
	return Date;
}

void VM_Snapshot::Set_Date( const QString &date )
{
	Date = date;
}

const QString &VM_Snapshot::Get_VM_Clock() const
{
	return VM_Clock;
}

void VM_Snapshot::Set_VM_Clock( const QString &vm_clock )
{
	VM_Clock = vm_clock;
}

const QString &VM_Snapshot::Get_Name() const
{
	return Name;
}

void VM_Snapshot::Set_Name( const QString &name )
{
	Name = name;
}

const QString &VM_Snapshot::Get_Description() const
{
	return Description;
}

void VM_Snapshot::Set_Description( const QString &desc )
{
	Description = desc;
}

//===========================================================================

// VM_Net_Card_Native --------------------------------------------------------

VM_Net_Card_Native::VM_Net_Card_Native()
{
    Network_Mode = VM::Net_Mode_Native_NIC;
	Card_Model = "";
	MAC = "";
	VLAN = 0;
	Name = "";
	Hostname = "";
	PortDev = "";
	File_Descriptor = 0;
	Interface_Name = "";
	Bridge_Name = "";
	TUN_TAP_Script = "";
	TUN_TAP_Down_Script = "";
	Bridge_Helper = "";
	Listen = "";
	Connect = "";
	MCast = "";
	Sock = "";
	Port = 0;
	Group = "";
	Mode = "";
	File = "";
	Len = 0;
	Addr = "";
	Vectors = 0;
	Net = "";
	Host = "";
	Restrict = false;
	DHCPstart = "";
	DNS = "";
	Tftp = "";
	Bootfile = "";
	HostFwd = "";
	GuestFwd = "";
	SMB = "";
	SMBserver = "";
	Sndbuf = 0;
	VNet_hdr = false;
	VHost = false;
	VHostFd = false;
	
	_Use_MAC = false;
	_Use_VLAN = false;
	_Use_Name = false;
	_Use_Hostname = false;
	_Use_File_Descriptor = false;
	_Use_Interface_Name = false;
	_Use_Bridge_Name = false;
	_Use_TUN_TAP_Script = false;
	_Use_TUN_TAP_Down_Script = false;
	_Use_Bridge_Helper = false;
	_Use_Listen = false;
	_Use_Connect = false;
	_Use_MCast = false;
	_Use_Sock = false;
	_Use_Port = false;
	_Use_Group = false;
	_Use_Mode = false;
	_Use_File = false;
	_Use_Len = false;
	_Use_Addr = false;
	_Use_Vectors = false;
	_Use_Net = false; 
	_Use_Host = false;
	_Use_Restrict = false;
	_Use_DHCPstart = false;
	_Use_DNS = false;
	_Use_Tftp = false;
	_Use_Bootfile = false;
	_Use_HostFwd = false;
	_Use_GuestFwd = false;
	_Use_SMB = false;
	_Use_SMBserver = false;
	_Use_Sndbuf = false;
	_Use_VNet_hdr = false;
	_Use_VHost = false;
	_Use_VHostFd = false;
}

VM_Net_Card_Native::VM_Net_Card_Native( const VM_Net_Card_Native &nc )
{
	Network_Mode = nc.Get_Network_Type();
	Card_Model = nc.Get_Card_Model();
	MAC = nc.Get_MAC_Address();
	VLAN = nc.Get_VLAN();
	Name = nc.Get_Name();
	Hostname = nc.Get_Hostname();
	PortDev = nc.Get_PortDev();
	File_Descriptor = nc.Get_File_Descriptor();
	Interface_Name = nc.Get_Interface_Name();
	Bridge_Name = nc.Get_Bridge_Name();
	TUN_TAP_Script = nc.Get_TUN_TAP_Script();
	TUN_TAP_Down_Script = nc.Get_TUN_TAP_Down_Script();
	Bridge_Helper = nc.Get_Bridge_Helper();
	Listen = nc.Get_Listen();
	Connect = nc.Get_Connect();
	MCast = nc.Get_MCast();
	Sock = nc.Get_Sock();
	Port = nc.Get_Port();
	Group = nc.Get_Group();
	Mode = nc.Get_Mode();
	File = nc.Get_File();
	Len = nc.Get_Len();
	Addr = nc.Get_Addr();
	Vectors = nc.Get_Vectors();
	Net = nc.Get_Net();
	Host = nc.Get_Host();
	Restrict = nc.Get_Restrict();
	DHCPstart = nc.Get_DHCPstart();
	DNS = nc.Get_DNS();
	Tftp = nc.Get_Tftp();
	Bootfile = nc.Get_Bootfile();
	HostFwd = nc.Get_HostFwd();
	GuestFwd = nc.Get_GuestFwd();
	SMB = nc.Get_SMB();
	SMBserver = nc.Get_SMBserver();
	Sndbuf = nc.Get_Sndbuf();
	VNet_hdr = nc.Get_VNet_hdr();
	VHost = nc.Get_VHost();
	VHostFd = nc.Get_VHostFd();
	
	_Use_MAC = nc.Use_MAC_Address();
	_Use_VLAN = nc.Use_VLAN();
	_Use_Name = nc.Use_Name();
	_Use_Hostname = nc.Use_Hostname();
	_Use_File_Descriptor = nc.Use_File_Descriptor();
	_Use_Interface_Name = nc.Use_Interface_Name();
	_Use_Bridge_Name = nc.Use_Bridge_Name();
	_Use_TUN_TAP_Script = nc.Use_TUN_TAP_Script();
	_Use_TUN_TAP_Down_Script = nc.Use_TUN_TAP_Down_Script();
	_Use_Bridge_Helper = nc.Use_Bridge_Helper();
	_Use_Listen = nc.Use_Listen();
	_Use_Connect = nc.Use_Connect();
	_Use_MCast = nc.Use_MCast();
	_Use_Sock = nc.Use_Sock();
	_Use_Port = nc.Use_Port();
	_Use_Group = nc.Use_Group();
	_Use_Mode = nc.Use_Mode();
	_Use_File = nc.Use_File();
	_Use_Len = nc.Use_Len();
	_Use_Addr = nc.Use_Addr();
	_Use_Vectors = nc.Use_Vectors();
	_Use_Net = nc.Use_Net(); 
	_Use_Host = nc.Use_Host();
	_Use_Restrict = nc.Use_Restrict();
	_Use_DHCPstart = nc.Use_DHCPstart();
	_Use_DNS = nc.Use_DNS();
	_Use_Tftp = nc.Use_Tftp();
	_Use_Bootfile = nc.Use_Bootfile();
	_Use_HostFwd = nc.Use_HostFwd();
	_Use_GuestFwd = nc.Use_GuestFwd();
	_Use_SMB = nc.Use_SMB();
	_Use_SMBserver = nc.Use_SMBserver();
	_Use_Sndbuf = nc.Use_Sndbuf();
	_Use_VNet_hdr = nc.Use_VNet_hdr();
	_Use_VHost = nc.Use_VHost();
	_Use_VHostFd = nc.Use_VHostFd();
}

bool VM_Net_Card_Native::operator==( const VM_Net_Card_Native &nc ) const
{
	if( Network_Mode == nc.Get_Network_Type() &&
		Card_Model == nc.Get_Card_Model() &&
		MAC == nc.Get_MAC_Address() &&
		VLAN == nc.Get_VLAN() &&
		Name == nc.Get_Name() &&
		Hostname == nc.Get_Hostname() &&
		PortDev == nc.Get_PortDev() &&
		File_Descriptor == nc.Get_File_Descriptor() &&
		Interface_Name == nc.Get_Interface_Name() &&
		Bridge_Name == nc.Get_Bridge_Name() &&
		TUN_TAP_Script == nc.Get_TUN_TAP_Script() &&
		TUN_TAP_Down_Script == nc.Get_TUN_TAP_Down_Script() &&
		Bridge_Helper == nc.Get_Bridge_Helper() &&
		Listen == nc.Get_Listen() &&
		Connect == nc.Get_Connect() &&
		MCast == nc.Get_MCast() &&
		Sock == nc.Get_Sock() &&
		Port == nc.Get_Port() &&
		Group == nc.Get_Group() &&
		Mode == nc.Get_Mode() &&
		File == nc.Get_File() &&
		Len == nc.Get_Len() &&
		Addr == nc.Get_Addr() &&
		Vectors == nc.Get_Vectors() &&
		Net == nc.Get_Net() &&
		Host == nc.Get_Host() &&
		Restrict == nc.Get_Restrict() &&
		DHCPstart == nc.Get_DHCPstart() &&
		DNS == nc.Get_DNS() &&
		Tftp == nc.Get_Tftp() &&
		Bootfile == nc.Get_Bootfile() &&
		HostFwd == nc.Get_HostFwd() &&
		GuestFwd == nc.Get_GuestFwd() &&
		SMB == nc.Get_SMB() &&
		SMBserver == nc.Get_SMBserver() &&
		Sndbuf == nc.Get_Sndbuf() &&
		VNet_hdr == nc.Get_VNet_hdr() &&
		VHost == nc.Get_VHost() &&
		VHostFd == nc.Get_VHostFd() &&
		_Use_MAC == nc.Use_MAC_Address() &&
		_Use_VLAN == nc.Use_VLAN() &&
		_Use_Name == nc.Use_Name() &&
		_Use_Hostname == nc.Use_Hostname() &&
		_Use_File_Descriptor == nc.Use_File_Descriptor() &&
		_Use_Interface_Name == nc.Use_Interface_Name() &&
		_Use_Bridge_Name == nc.Use_Bridge_Name() &&
		_Use_TUN_TAP_Script == nc.Use_TUN_TAP_Script() &&
		_Use_TUN_TAP_Down_Script == nc.Use_TUN_TAP_Down_Script() &&
		_Use_Bridge_Helper == nc.Use_Bridge_Helper() &&
		_Use_Listen == nc.Use_Listen() &&
		_Use_Connect == nc.Use_Connect() &&
		_Use_MCast == nc.Use_MCast() &&
		_Use_Sock == nc.Use_Sock() &&
		_Use_Port == nc.Use_Port() &&
		_Use_Group == nc.Use_Group() &&
		_Use_Mode == nc.Use_Mode() &&
		_Use_File == nc.Use_File() &&
		_Use_Len == nc.Use_Len() &&
		_Use_Addr == nc.Use_Addr() &&
		_Use_Vectors == nc.Use_Vectors() &&
		_Use_Net == nc.Use_Net() && 
		_Use_Host == nc.Use_Host() &&
		_Use_Restrict == nc.Use_Restrict() &&
		_Use_DHCPstart == nc.Use_DHCPstart() &&
		_Use_DNS == nc.Use_DNS() &&
		_Use_Tftp == nc.Use_Tftp() &&
		_Use_Bootfile == nc.Use_Bootfile() &&
		_Use_HostFwd == nc.Use_HostFwd() &&
		_Use_GuestFwd == nc.Use_GuestFwd() &&
		_Use_SMB == nc.Use_SMB() &&
		_Use_SMBserver == nc.Use_SMBserver() &&
		_Use_Sndbuf == nc.Use_Sndbuf() &&
		_Use_VNet_hdr == nc.Use_VNet_hdr() &&
		_Use_VHost == nc.Use_VHost() &&
		_Use_VHostFd == nc.Use_VHostFd() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Net_Card_Native::operator!=( const VM_Net_Card_Native &nc ) const
{
	return ! operator ==( nc );
}

QString VM_Net_Card_Native::Generate_MAC() const
{
	VM_Net_Card tmp_card;
	tmp_card.Set_Card_Model( Card_Model );
	return tmp_card.Generate_MAC();
}

VM::Network_Mode_Native VM_Net_Card_Native::Get_Network_Type() const
{
	return Network_Mode;
}

void VM_Net_Card_Native::Set_Network_Type( VM::Network_Mode_Native mode )
{
	Network_Mode = mode;
}

const QString &VM_Net_Card_Native::Get_Card_Model() const
{
	return Card_Model;
}

void VM_Net_Card_Native::Set_Card_Model( const QString &m )
{
	Card_Model = m;
}

bool VM_Net_Card_Native::Use_MAC_Address() const
{
	return _Use_MAC;
}

void VM_Net_Card_Native::Use_MAC_Address( bool use )
{
	_Use_MAC = use;
}

const QString &VM_Net_Card_Native::Get_MAC_Address() const
{
	return MAC;
}

void VM_Net_Card_Native::Set_MAC_Address( const QString &ma )
{
	MAC = ma;
}

bool VM_Net_Card_Native::Use_VLAN() const
{
	return _Use_VLAN;
}

void VM_Net_Card_Native::Use_VLAN( bool use )
{
	_Use_VLAN = use;
}

int VM_Net_Card_Native::Get_VLAN() const
{
	return VLAN;
}

void VM_Net_Card_Native::Set_VLAN( int vl )
{
	VLAN = vl;
}

bool VM_Net_Card_Native::Use_Name() const
{
	return _Use_Name;
}

void VM_Net_Card_Native::Use_Name( bool use )
{
	_Use_Name = use;
}

const QString &VM_Net_Card_Native::Get_Name() const
{
	return Name;
}

void VM_Net_Card_Native::Set_Name( const QString &name )
{
	Name = name;
}

bool VM_Net_Card_Native::Use_Hostname() const
{
	return _Use_Hostname;
}

void VM_Net_Card_Native::Use_Hostname( bool use )
{
	_Use_Hostname = use;
}

const QString &VM_Net_Card_Native::Get_Hostname() const
{
	return Hostname;
}

void VM_Net_Card_Native::Set_Hostname( const QString &hostname )
{
	Hostname = hostname;
}

const QString &VM_Net_Card_Native::Get_PortDev() const
{
	return PortDev;
}

void VM_Net_Card_Native::Set_PortDev( const QString &portDev )
{
	PortDev = portDev;
}

bool VM_Net_Card_Native::Use_File_Descriptor() const
{
	return _Use_File_Descriptor;
}

void VM_Net_Card_Native::Use_File_Descriptor( bool use )
{
	_Use_File_Descriptor = use;
}

int VM_Net_Card_Native::Get_File_Descriptor() const
{
	return File_Descriptor;
}

void VM_Net_Card_Native::Set_File_Descriptor( int f )
{
	File_Descriptor = f;
}

bool VM_Net_Card_Native::Use_Interface_Name() const
{
	return _Use_Interface_Name;
}

void VM_Net_Card_Native::Use_Interface_Name( bool use )
{
	_Use_Interface_Name = use;
}

const QString &VM_Net_Card_Native::Get_Interface_Name() const
{
	return Interface_Name;
}

void VM_Net_Card_Native::Set_Interface_Name( const QString &n )
{
	Interface_Name = n;
}

bool VM_Net_Card_Native::Use_Bridge_Name() const
{
	return _Use_Bridge_Name;
}

void VM_Net_Card_Native::Use_Bridge_Name( bool use )
{
	_Use_Bridge_Name = use;
}

const QString &VM_Net_Card_Native::Get_Bridge_Name() const
{
	return Bridge_Name;
}

void VM_Net_Card_Native::Set_Bridge_Name( const QString &n )
{
	Bridge_Name = n;
}

bool VM_Net_Card_Native::Use_TUN_TAP_Script() const
{
	return _Use_TUN_TAP_Script;
}

void VM_Net_Card_Native::Use_TUN_TAP_Script( bool use )
{
	_Use_TUN_TAP_Script = use;
}

const QString &VM_Net_Card_Native::Get_TUN_TAP_Script() const
{
	return TUN_TAP_Script;
}

void VM_Net_Card_Native::Set_TUN_TAP_Script( const QString &s )
{
	TUN_TAP_Script = s;
}

bool VM_Net_Card_Native::Use_TUN_TAP_Down_Script() const
{
	return _Use_TUN_TAP_Down_Script;
}

void VM_Net_Card_Native::Use_TUN_TAP_Down_Script( bool use )
{
	_Use_TUN_TAP_Down_Script = use;
}

const QString &VM_Net_Card_Native::Get_TUN_TAP_Down_Script() const
{
	return TUN_TAP_Down_Script;
}

void VM_Net_Card_Native::Set_TUN_TAP_Down_Script( const QString &s )
{
	TUN_TAP_Down_Script = s;
}

bool VM_Net_Card_Native::Use_Bridge_Helper() const
{
	return _Use_Bridge_Helper;
}

void VM_Net_Card_Native::Use_Bridge_Helper( bool use )
{
	_Use_Bridge_Helper = use;
}

const QString &VM_Net_Card_Native::Get_Bridge_Helper() const
{
	return Bridge_Helper;
}

void VM_Net_Card_Native::Set_Bridge_Helper( const QString &s )
{
	Bridge_Helper = s;
}

bool VM_Net_Card_Native::Use_Listen() const
{
	return _Use_Listen;
}

void VM_Net_Card_Native::Use_Listen( bool use )
{
	_Use_Listen = use;
}

const QString &VM_Net_Card_Native::Get_Listen() const
{
	return Listen;
}

void VM_Net_Card_Native::Set_Listen( const QString &listen )
{
	Listen = listen;
}

bool VM_Net_Card_Native::Use_Connect() const
{
	return _Use_Connect;
}

void VM_Net_Card_Native::Use_Connect( bool use )
{
	_Use_Connect = use;
}

const QString &VM_Net_Card_Native::Get_Connect() const
{
	return Connect;
}

void VM_Net_Card_Native::Set_Connect( const QString &con )
{
	Connect = con;
}

bool VM_Net_Card_Native::Use_MCast() const
{
	return _Use_MCast;
}

void VM_Net_Card_Native::Use_MCast( bool use )
{
	_Use_MCast = use;
}

const QString &VM_Net_Card_Native::Get_MCast() const
{
	return MCast;
}

void VM_Net_Card_Native::Set_MCast( const QString &mcast )
{
	MCast = mcast;
}

bool VM_Net_Card_Native::Use_Sock() const
{
	return _Use_Sock;
}

void VM_Net_Card_Native::Use_Sock( bool use )
{
	_Use_Sock = use;
}

const QString &VM_Net_Card_Native::Get_Sock() const
{
	return Sock;
}

void VM_Net_Card_Native::Set_Sock( const QString &sock )
{
	Sock = sock;
}

bool VM_Net_Card_Native::Use_Port() const
{
	return _Use_Port;
}

void VM_Net_Card_Native::Use_Port( bool use )
{
	_Use_Port = use;
}

int VM_Net_Card_Native::Get_Port() const
{
	return Port;
}

void VM_Net_Card_Native::Set_Port( int p )
{
	Port = p;
}

bool VM_Net_Card_Native::Use_Group() const
{
	return _Use_Group;
}

void VM_Net_Card_Native::Use_Group( bool use )
{
	_Use_Group = use;
}

const QString &VM_Net_Card_Native::Get_Group() const
{
	return Group;
}

void VM_Net_Card_Native::Set_Group( const QString &group )
{
	Group = group;
}

bool VM_Net_Card_Native::Use_Mode() const
{
	return _Use_Mode;
}

void VM_Net_Card_Native::Use_Mode( bool use )
{
	_Use_Mode = use;
}

const QString &VM_Net_Card_Native::Get_Mode() const
{
	return Mode;
}

void VM_Net_Card_Native::Set_Mode( const QString &mode )
{
	Mode = mode;
}

bool VM_Net_Card_Native::Use_File() const
{
	return _Use_File;
}

void VM_Net_Card_Native::Use_File( bool use )
{
	_Use_File = use;
}

const QString &VM_Net_Card_Native::Get_File() const
{
	return File;
}

void VM_Net_Card_Native::Set_File( const QString &file )
{
	File = file;
}

bool VM_Net_Card_Native::Use_Len() const
{
	return _Use_Len;
}

void VM_Net_Card_Native::Use_Len( bool use )
{
	_Use_Len = use;
}

int VM_Net_Card_Native::Get_Len() const
{
	return Len;
}

void VM_Net_Card_Native::Set_Len( int kb )
{
	Len = kb;
}

bool VM_Net_Card_Native::Use_Addr() const
{
	return _Use_Addr;
}

void VM_Net_Card_Native::Use_Addr( bool use )
{
	_Use_Addr = use;
}

const QString &VM_Net_Card_Native::Get_Addr() const
{
	return Addr;
}

void VM_Net_Card_Native::Set_Addr( const QString &addr )
{
	Addr = addr;
}


bool VM_Net_Card_Native::Use_Vectors() const
{
	return _Use_Vectors;
}

void VM_Net_Card_Native::Use_Vectors( bool use )
{
	_Use_Vectors = use;
}

int VM_Net_Card_Native::Get_Vectors() const
{
	return Vectors;
}

void VM_Net_Card_Native::Set_Vectors( int vectors )
{
	Vectors = vectors;
}

bool VM_Net_Card_Native::Use_Net() const
{
	return _Use_Net;
}

void VM_Net_Card_Native::Use_Net( bool use )
{
	_Use_Net = use;
}

const QString &VM_Net_Card_Native::Get_Net() const
{
	return Net;
}

void VM_Net_Card_Native::Set_Net( const QString &net )
{
	Net = net;
}

bool VM_Net_Card_Native::Use_Host() const
{
	return _Use_Host;
}

void VM_Net_Card_Native::Use_Host( bool use )
{
	_Use_Host = use;
}

const QString &VM_Net_Card_Native::Get_Host() const
{
	return Host;
}

void VM_Net_Card_Native::Set_Host( const QString &host )
{
	Host = host;
}

bool VM_Net_Card_Native::Use_Restrict() const
{
	return _Use_Restrict;
}

void VM_Net_Card_Native::Use_Restrict( bool use )
{
	_Use_Restrict = use;
}

bool VM_Net_Card_Native::Get_Restrict() const
{
	return Restrict;
}

void VM_Net_Card_Native::Set_Restrict( bool use )
{
	Restrict = use;
}

bool VM_Net_Card_Native::Use_DHCPstart() const
{
	return _Use_DHCPstart;
}

void VM_Net_Card_Native::Use_DHCPstart( bool use )
{
	_Use_DHCPstart = use;
}

const QString &VM_Net_Card_Native::Get_DHCPstart() const
{
	return DHCPstart;
}

void VM_Net_Card_Native::Set_DHCPstart( const QString &addr )
{
	DHCPstart = addr;
}

bool VM_Net_Card_Native::Use_DNS() const
{
	return _Use_DNS;
}

void VM_Net_Card_Native::Use_DNS( bool use )
{
	_Use_DNS = use;
}

const QString &VM_Net_Card_Native::Get_DNS() const
{
	return DNS;
}

void VM_Net_Card_Native::Set_DNS( const QString &addr )
{
	DNS = addr;
}

bool VM_Net_Card_Native::Use_Tftp() const
{
	return _Use_Tftp;
}

void VM_Net_Card_Native::Use_Tftp( bool use )
{
	_Use_Tftp = use;
}

const QString &VM_Net_Card_Native::Get_Tftp() const
{
	return Tftp;
}

void VM_Net_Card_Native::Set_Tftp( const QString &dir )
{
	Tftp = dir;
}

bool VM_Net_Card_Native::Use_Bootfile() const
{
	return _Use_Bootfile;
}

void VM_Net_Card_Native::Use_Bootfile( bool use )
{
	_Use_Bootfile = use;
}

const QString &VM_Net_Card_Native::Get_Bootfile() const
{
	return Bootfile;
}

void VM_Net_Card_Native::Set_Bootfile( const QString &file )
{
	Bootfile = file;
}

bool VM_Net_Card_Native::Use_HostFwd() const
{
	return _Use_HostFwd;
}

void VM_Net_Card_Native::Use_HostFwd( bool use )
{
	_Use_HostFwd = use;
}

const QString &VM_Net_Card_Native::Get_HostFwd() const
{
	return HostFwd;
}

void VM_Net_Card_Native::Set_HostFwd( const QString &rule )
{
	HostFwd = rule;
}

bool VM_Net_Card_Native::Use_GuestFwd() const
{
	return _Use_GuestFwd;
}

void VM_Net_Card_Native::Use_GuestFwd( bool use )
{
	_Use_GuestFwd = use;
}

const QString &VM_Net_Card_Native::Get_GuestFwd() const
{
	return GuestFwd;
}

void VM_Net_Card_Native::Set_GuestFwd( const QString &rule )
{
	GuestFwd = rule;
}

bool VM_Net_Card_Native::Use_SMB() const
{
	return _Use_SMB;
}

void VM_Net_Card_Native::Use_SMB( bool use )
{
	_Use_SMB = use;
}

const QString &VM_Net_Card_Native::Get_SMB() const
{
	return SMB;
}

void VM_Net_Card_Native::Set_SMB( const QString &dir )
{
	SMB = dir;
}

bool VM_Net_Card_Native::Use_SMBserver() const
{
	return _Use_SMBserver;
}

void VM_Net_Card_Native::Use_SMBserver( bool use )
{
	_Use_SMBserver = use;
}

const QString &VM_Net_Card_Native::Get_SMBserver() const
{
	return SMBserver;
}

void VM_Net_Card_Native::Set_SMBserver( const QString &addr )
{
	SMBserver = addr;
}

bool VM_Net_Card_Native::Use_Sndbuf() const
{
	return _Use_Sndbuf;
}

void VM_Net_Card_Native::Use_Sndbuf( bool use )
{
	_Use_Sndbuf = use;
}

int VM_Net_Card_Native::Get_Sndbuf() const
{
	return Sndbuf;
}

void VM_Net_Card_Native::Set_Sndbuf( int bytes )
{
	Sndbuf = bytes;
}

bool VM_Net_Card_Native::Use_VNet_hdr() const
{
	return _Use_VNet_hdr;
}

void VM_Net_Card_Native::Use_VNet_hdr( bool use )
{
	_Use_VNet_hdr = use;
}

bool VM_Net_Card_Native::Get_VNet_hdr() const
{
	return VNet_hdr;
}

void VM_Net_Card_Native::Set_VNet_hdr( bool use )
{
	VNet_hdr = use;
}

bool VM_Net_Card_Native::Use_VHost() const
{
	return _Use_VHost;
}

void VM_Net_Card_Native::Use_VHost( bool use )
{
	_Use_VHost = use;
}

bool VM_Net_Card_Native::Get_VHost() const
{
	return VHost;
}

void VM_Net_Card_Native::Set_VHost( bool use )
{
	VHost = use;
}

bool VM_Net_Card_Native::Use_VHostFd() const
{
	return _Use_VHostFd;
}

void VM_Net_Card_Native::Use_VHostFd( bool use )
{
	_Use_VHostFd = use;
}

int VM_Net_Card_Native::Get_VHostFd() const
{
	return VHostFd;
}

void VM_Net_Card_Native::Set_VHostFd( int port )
{
	VHostFd = port;
}


//===========================================================================

// VM_Net_Card --------------------------------------------------------------

VM_Net_Card::VM_Net_Card()
{
	Card_Model = "";
	Net_Mode = VM::Net_Mode_None;
	IP = "";
	MAC = "";
	Hostname = "";
	Port = 0;
	VLAN = 0;
	Use_TUN_TAP_Script = true;
	TUN_TAP_Script = "";
	Interface_Name = "";
	File_Descriptor = 0;
}

VM_Net_Card::VM_Net_Card( const VM_Net_Card &nc )
{
	Card_Model = nc.Get_Card_Model();
	Net_Mode = nc.Get_Net_Mode();
	Hostname = nc.Get_Hostname();
	IP = nc.Get_IP_Address();
	MAC = nc.Get_MAC_Address();
	Port = nc.Get_Port();
	VLAN = nc.Get_VLAN();
	Use_TUN_TAP_Script = nc.Get_Use_TUN_TAP_Script();
	TUN_TAP_Script = nc.Get_TUN_TAP_Script();
	Interface_Name = nc.Get_Interface_Name();
	File_Descriptor = nc.Get_File_Descriptor();
}

bool VM_Net_Card::operator==( const VM_Net_Card &nc ) const
{
	if( Card_Model == nc.Get_Card_Model() &&
		Net_Mode == nc.Get_Net_Mode() &&
		Hostname == nc.Get_Hostname() &&
		IP == nc.Get_IP_Address() &&
		MAC == nc.Get_MAC_Address() &&
		Port == nc.Get_Port() &&
		VLAN == nc.Get_VLAN() &&
		Use_TUN_TAP_Script == nc.Get_Use_TUN_TAP_Script() &&
		TUN_TAP_Script == nc.Get_TUN_TAP_Script() &&
		Interface_Name == nc.Get_Interface_Name() &&
		File_Descriptor == nc.Get_File_Descriptor() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Net_Card::operator!=( const VM_Net_Card &nc ) const
{
	return ! ( operator==(nc) );
}

QString VM_Net_Card::Generate_MAC() const
{
	QSettings settings;
	QString mode = settings.value( "MAC_Generation_Mode", "" ).toString();
	
	QString nmac = QUuid::createUuid().toString(); // Random HEX for MAC
	
	if( mode == "Random" )
	{
		nmac = nmac.mid( 25, 10 );
		nmac.prepend( "00" ); // Two Zero First, For Valid MAC
	}
	else if( mode == "QEMU_Segment" )
	{
		nmac = nmac.mid( 25, 6 );
		nmac.prepend( "525400" ); // QEMU Segment
	}
	else // Default. Valid Card MAC
	{
		nmac = nmac.mid( 25, 6 );
		
		// MAC Data Base
		QStringList novell_mac;
		novell_mac << "00001B"
				   << "0000D8";
		
		QStringList sun_mac;
		sun_mac << "00015D"
				<< "0003BA"
				<< "000782"
				<< "00104F"
				<< "0010E0";
		
		QStringList amd_mac;
		amd_mac << "00001A"
				<< "000C87";
		
		QStringList smc_mac;
		smc_mac << "0004E2"
				<< "000BC5"
				<< "0013F7"
				<< "00222D"
				<< "0023C6";
		
		QStringList motorola_mac;
		motorola_mac << "00080E"
					 << "000A28"
					 << "000B06"
					 << "000CE5"
					 << "000E5C";
		
		QStringList intel_mac;
		intel_mac << "001111"
				  << "0012F0"
				  << "0013CE"
				  << "0013E8"
				  << "001676";
		
		QString model = Get_Card_Model();
		
		if( model == "ne2k_pci" || model == "ne2k_isa" )
		{
			int rm = Get_Random( 0,1 );
			if( rm >= 0 && rm < novell_mac.count() ) nmac.prepend( novell_mac[rm] );
		}
		else if( model == "i82551" || model == "i82557b" || model == "i82559er" || model == "e1000" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 && rm < intel_mac.count() ) nmac.prepend( intel_mac[rm] );
		}
		else if( model == "pcnet" )
		{
			int rm = Get_Random( 0,1 );
			if( rm >= 0 ) nmac.prepend( amd_mac[rm] );
		}
		else if( model == "rtl8139" )
		{
			nmac.prepend( "00E04C" );
		}
		else if( model == "smc91c111" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( smc_mac[rm] );
		}
		else if( model == "mcf_fec" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( motorola_mac[rm] );
		}
		else if( model == "lance" )
		{
			int rm = Get_Random( 0,4 );
			if( rm >= 0 ) nmac.prepend( sun_mac[rm] );
		}
		else if( model == "fseth" )
		{
			// FIXME qemu-system-cris
			nmac = QUuid::createUuid().toString(); // Random HEX for MAC
			nmac = nmac.mid( 25, 10 );
			nmac.prepend( "00" ); // Two Zero First, For Valid MAC
		}
		else if( model == "xilinx-ethlite" )
		{
			// FIXME qemu-system-microblaze
			nmac = QUuid::createUuid().toString(); // Random HEX for MAC
			nmac = nmac.mid( 25, 10 );
			nmac.prepend( "00" ); // Two Zero First, For Valid MAC
		}
		else if( model == "" ) // Default 00 + Random
		{
			nmac = QUuid::createUuid().toString(); // Random HEX for MAC
			nmac = nmac.mid( 25, 10 );
			nmac.prepend( "00" ); // Two Zero First, For Valid MAC
		}
		else
		{
			AQWarning( "QString VM_Net_Card::Generate_MAC() const",
					   "Net Card Model Invalid!" );
		}
		
		// MAC Found?
		if( nmac.count() != 12 )
		{
			AQError( "QString VM_Net_Card::Generate_MAC() const",
					 "Cannot Get MAC for This Card!" );
			
			nmac = QUuid::createUuid().toString(); // Random HEX for MAC
			nmac = nmac.mid( 25, 6 );
			nmac.prepend( "525400" ); // QEMU Segment
		}
	}
	
	// Adding separators :
	for( int nx = 2; nx < nmac.count(); nx+=3 )
	{
		nmac = nmac.insert( nx, ":" );
	}
	
	nmac = nmac.toLower();
	return nmac;
}

const QString &VM_Net_Card::Get_Card_Model() const
{
	return Card_Model;
}

void VM_Net_Card::Set_Card_Model( const QString &m )
{
	Card_Model = m;
}

VM::Network_Mode VM_Net_Card::Get_Net_Mode() const
{
	return Net_Mode;
}

void VM_Net_Card::Set_Net_Mode( VM::Network_Mode mode )
{
	Net_Mode = mode;
}

const QString &VM_Net_Card::Get_Hostname() const
{
	return Hostname;
}

void VM_Net_Card::Set_Hostname( const QString &hn )
{
	Hostname = hn;
}

const QString &VM_Net_Card::Get_IP_Address() const
{
	return IP;
}

void VM_Net_Card::Set_IP_Address( const QString &ia )
{
	IP = ia;
}

const QString &VM_Net_Card::Get_MAC_Address() const
{
	return MAC;
}

void VM_Net_Card::Set_MAC_Address( const QString &ma )
{
	MAC = ma;
}

int VM_Net_Card::Get_Port() const
{
	return Port;
}

void VM_Net_Card::Set_Port( int p )
{
	Port = p;
}

int VM_Net_Card::Get_VLAN() const
{
	return VLAN;
}

void VM_Net_Card::Set_VLAN( int vl )
{
	VLAN = vl;
}

bool VM_Net_Card::Get_Use_TUN_TAP_Script() const
{
	return Use_TUN_TAP_Script;
}

void VM_Net_Card::Set_Use_TUN_TAP_Script( bool use )
{
	Use_TUN_TAP_Script = use;
}

const QString &VM_Net_Card::Get_TUN_TAP_Script() const
{
	return TUN_TAP_Script;
}

void VM_Net_Card::Set_TUN_TAP_Script( const QString &s )
{
	TUN_TAP_Script = s;
}

const QString &VM_Net_Card::Get_Interface_Name() const
{
	return Interface_Name;
}

void VM_Net_Card::Set_Interface_Name( const QString &n )
{
	Interface_Name = n;
}

int VM_Net_Card::Get_File_Descriptor() const
{
	return File_Descriptor;
}

void VM_Net_Card::Set_File_Descriptor( int f )
{
	File_Descriptor = f;
}

//===========================================================================

// VM_Redirections Class ----------------------------------------------------

VM_Redirection::VM_Redirection()
{
	Protocol = "NOT";
	Host_Port = 1;
	Guest_IP = "0.0.0.0";
	Guest_Port = 1;
}

VM_Redirection::VM_Redirection( const VM_Redirection &r )
{
	Protocol = r.Get_Protocol();
	Host_Port = r.Get_Host_Port();
	Guest_IP = r.Get_Guest_IP();
	Guest_Port = r.Get_Guest_Port();
}

bool VM_Redirection::operator==( const VM_Redirection &r ) const
{
	if( Protocol == r.Get_Protocol() &&
		Host_Port == r.Get_Host_Port() &&
		Guest_IP == r.Get_Guest_IP() &&
		Guest_Port == r.Get_Guest_Port() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Redirection::operator!=( const VM_Redirection &r ) const
{
	return ! ( operator==(r) );
}

const QString &VM_Redirection::Get_Protocol() const
{
	return Protocol;
}

void VM_Redirection::Set_Protocol( const QString &p )
{
	Protocol = p;
}

int VM_Redirection::Get_Host_Port() const
{
	return Host_Port;
}

void VM_Redirection::Set_Host_Port( int hp )
{
	Host_Port = hp;
}

const QString &VM_Redirection::Get_Guest_IP() const
{
	return Guest_IP;
}

void VM_Redirection::Set_Guest_IP( const QString &ip )
{
	Guest_IP = ip;
}

int VM_Redirection::Get_Guest_Port() const
{
	return Guest_Port;
}

void VM_Redirection::Set_Guest_Port( int p )
{
	Guest_Port = p;
}

//===========================================================================

// VM_Port ------------------------------------------------------------------

VM_Port::VM_Port()
{
	Port_Redir = VM::PR_Default;
	Param_Line = "";
}

VM_Port::VM_Port( const VM_Port &p )
{
	Port_Redir = p.Get_Port_Redirection();
	Param_Line = p.Get_Parametrs_Line();
}

bool VM_Port::operator==( const VM_Port &p ) const
{
	if( Port_Redir == p.Get_Port_Redirection() &&
		Param_Line == p.Get_Parametrs_Line() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Port::operator!=( const VM_Port &p ) const
{
	return ! ( operator==(p) );
}

VM::Port_Redirection VM_Port::Get_Port_Redirection() const
{
	return Port_Redir;
}

void VM_Port::Set_Port_Redirection( VM::Port_Redirection pr )
{
	Port_Redir = pr;
}

const QString &VM_Port::Get_Parametrs_Line() const
{
	return Param_Line;
}

void VM_Port::Set_Parametrs_Line( const QString &pl )
{
	Param_Line = pl;
}

//===========================================================================

// VM_USB Class -------------------------------------------------------------

VM_USB::VM_USB()
{
	Use_Host_Device = true;
	Manufacturer_Name = "";
	Product_Name = "";
	Vendor_ID = "";
	Product_ID = "";
	Bus = "";
	Addr = "";
	DevPath = "";
	Speed = "0";
	Serial_Number = "";
	USB_Keyboard = USB_Tablet = USB_WacomTablet = USB_Braille = false;
	USB_Mouse = true;
}

VM_USB::VM_USB( const VM_USB &u )
{
	Use_Host_Device = u.Get_Use_Host_Device();
	Manufacturer_Name = u.Get_Manufacturer_Name();
	Product_Name = u.Get_Product_Name();
	Vendor_ID = u.Get_Vendor_ID();
	Product_ID = u.Get_Product_ID();
	Bus = u.Get_Bus();
	Addr = u.Get_Addr();
	DevPath = u.Get_DevPath();
	Serial_Number = u.Get_Serial_Number();
	Speed = u.Get_Speed();
	u.Get_USB_QEMU_Devices( USB_Keyboard, USB_Mouse, USB_Tablet, USB_WacomTablet, USB_Braille );
}

bool VM_USB::operator==( const VM_USB &u ) const
{
	bool k, m, t, wt, b;
	k = m = t = wt = b = false;
	
	u.Get_USB_QEMU_Devices( k, m, t, wt, b );
	
	if( Use_Host_Device == u.Get_Use_Host_Device() &&
		Vendor_ID == u.Get_Vendor_ID() &&
		Product_ID == u.Get_Product_ID() &&
		Serial_Number == u.Get_Serial_Number() &&
		USB_Keyboard == k &&
		USB_Mouse == m &&
		USB_Tablet == t &&
		USB_WacomTablet == wt &&
		USB_Braille == b )
	{
		return true;
	}
	else return false;
}

bool VM_USB::operator!=( const VM_USB &u ) const
{
	return ! ( operator==(u) );
}

bool VM_USB::Get_Use_Host_Device() const
{
	return Use_Host_Device;
}

void VM_USB::Set_Use_Host_Device( bool use )
{
	Use_Host_Device = use;
}

const QString &VM_USB::Get_Manufacturer_Name() const
{
	return Manufacturer_Name;
}

void VM_USB::Set_Manufacturer_Name( const QString &name )
{
	Manufacturer_Name = name;
}

const QString &VM_USB::Get_Product_Name() const
{
	return Product_Name;
}

void VM_USB::Set_Product_Name( const QString &name )
{
	Product_Name = name;
}

const QString &VM_USB::Get_Vendor_ID() const
{
	return Vendor_ID;
}

void VM_USB::Set_Vendor_ID( const QString &dev )
{
	Vendor_ID = dev;
}

const QString &VM_USB::Get_Product_ID() const
{
	return Product_ID;
}

void VM_USB::Set_Product_ID( const QString &dev )
{
	Product_ID = dev;
}

QString VM_USB::Get_ID_Line() const
{
	return Vendor_ID + ":" + Product_ID;
}

void VM_USB::Set_ID_Line( const QString &line )
{
	QRegExp id_line = QRegExp( "([\\dabcdef]{4})[:]{1}([\\dabcdef]{4})" );
	
	if( ! id_line.exactMatch(line) )
	{
		AQError( "void VM_USB::Set_ID_Line( const QString &line )",
				 "Cannot Match QRegExp!" );
		return;
	}
	
	QStringList id_str = id_line.capturedTexts();
	
	Vendor_ID = id_str[1];
	Product_ID = id_str[2];
}

QString VM_USB::Get_Bus() const
{
	return Bus;
}

void VM_USB::Set_Bus( const QString &bus )
{
	Bus = bus;
}

QString VM_USB::Get_Addr() const
{
	return Addr;
}

void VM_USB::Set_Addr( const QString &addr )
{
	Addr = addr;
}

QString VM_USB::Get_DevPath() const
{
	return DevPath;
}

void VM_USB::Set_DevPath( const QString &devPath )
{
	DevPath = devPath;
}

QString VM_USB::Get_Serial_Number() const
{
	return Serial_Number;
}

void VM_USB::Set_Serial_Number( const QString &serial )
{
	Serial_Number = serial;
}

QString VM_USB::Get_Speed() const
{
	return Speed;
}

void VM_USB::Set_Speed( const QString &speed )
{
	Speed = speed;
}

void VM_USB::Get_USB_QEMU_Devices( bool &keyboard, bool &mouse, bool &tablet, bool &wacomTablet, bool &braille ) const
{
	keyboard = USB_Keyboard;
	mouse = USB_Mouse;
	tablet = USB_Tablet;
	wacomTablet = USB_WacomTablet;
	braille = USB_Braille;
}

void VM_USB::Set_USB_QEMU_Devices( bool keyboard, bool mouse, bool tablet, bool wacomTablet, bool braille )
{
	USB_Keyboard = keyboard;
	USB_Mouse = mouse;
	USB_Tablet = tablet;
	USB_WacomTablet = wacomTablet;
	USB_Braille = braille;
}

//===========================================================================

// VM_Init_Graphic_Mode -----------------------------------------------------

VM_Init_Graphic_Mode::VM_Init_Graphic_Mode()
{
	Enabled = false;
	Width = 800;
	Height = 600;
	Depth = 16;
}

VM_Init_Graphic_Mode::VM_Init_Graphic_Mode( const VM_Init_Graphic_Mode &mode )
{
	Enabled = mode.Get_Enabled();
	Width = mode.Get_Width();
	Height = mode.Get_Height();
	Depth = mode.Get_Depth();
}

bool VM_Init_Graphic_Mode::operator==( const VM_Init_Graphic_Mode &mode ) const
{
	if( Enabled == mode.Get_Enabled() &&
		Width == mode.Get_Width() &&
		Height == mode.Get_Height() &&
		Depth == mode.Get_Depth() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_Init_Graphic_Mode::operator!=( const VM_Init_Graphic_Mode &mode ) const
{
	return ! ( operator==(mode) );
}

bool VM_Init_Graphic_Mode::Get_Enabled() const
{
	return Enabled;
}

void VM_Init_Graphic_Mode::Set_Enabled( bool use )
{
	Enabled = use;
}

int VM_Init_Graphic_Mode::Get_Width() const
{
	return Width;
}

void VM_Init_Graphic_Mode::Set_Width( int w )
{
	Width = w;
}

int VM_Init_Graphic_Mode::Get_Height() const
{
	return Height;
}

void VM_Init_Graphic_Mode::Set_Height( int h )
{
	Height = h;
}

int VM_Init_Graphic_Mode::Get_Depth() const
{
	return Depth;
}

void VM_Init_Graphic_Mode::Set_Depth( int d )
{
	Depth = d;
}

//===========================================================================

// VM_SPICE -----------------------------------------------------------------

VM_SPICE::VM_SPICE()
{
	Enable_SPICE = false;
	GXL_Devices_Count = 1;
	RAM_Size = 64;
	
	Port = 5930;
	_Use_SPort = false;
	SPort = 1;
	_Use_Hostname = false;
	Hostname = "";
	
	_Use_Image_Compression = false;
	Image_Compression = VM::SPICE_IC_Type_on;
	_Use_Video_Stream_Compression = true;
	_Use_Renderer = false;
	Renderer_List << VM::SPICE_Renderer_cairo
				  << VM::SPICE_Renderer_oglpbuf
				  << VM::SPICE_Renderer_oglpixmap;
	_Use_Playback_Compression = true;
	
	_Use_Password = false;
	Password = "";
}

VM_SPICE::VM_SPICE( const VM_SPICE &vm_spice )
{
	Enable_SPICE = vm_spice.Use_SPICE();
	GXL_Devices_Count = vm_spice.Get_GXL_Devices_Count();
	RAM_Size = vm_spice.Get_RAM_Size();
	Port = vm_spice.Get_Port();
	_Use_SPort = vm_spice.Use_SPort();
	SPort = vm_spice.Get_SPort();
	_Use_Hostname = vm_spice.Use_Hostname();
	Hostname = vm_spice.Get_Hostname();
	_Use_Image_Compression = vm_spice.Use_Image_Compression();
	Image_Compression = vm_spice.Get_Image_Compression();
	_Use_Video_Stream_Compression = vm_spice.Use_Video_Stream_Compression();
	_Use_Renderer = vm_spice.Use_Renderer();
	Renderer_List = vm_spice.Get_Renderer_List();
	_Use_Playback_Compression = vm_spice.Use_Playback_Compression();
	_Use_Password = vm_spice.Use_Password();
	Password = vm_spice.Get_Password();
}

bool VM_SPICE::operator==( const VM_SPICE &vm_spice ) const
{
	if( Enable_SPICE == vm_spice.Use_SPICE() &&
		GXL_Devices_Count == vm_spice.Get_GXL_Devices_Count() &&
		RAM_Size == vm_spice.Get_RAM_Size() &&
		Port == vm_spice.Get_Port() &&
		_Use_SPort == vm_spice.Use_SPort() &&
		SPort == vm_spice.Get_SPort() &&
		_Use_Hostname == vm_spice.Use_Hostname() &&
		Hostname == vm_spice.Get_Hostname() &&
		_Use_Image_Compression == vm_spice.Use_Image_Compression() &&
		Image_Compression == vm_spice.Get_Image_Compression() &&
		_Use_Video_Stream_Compression == vm_spice.Use_Video_Stream_Compression() &&
		_Use_Renderer == vm_spice.Use_Renderer() &&
		Renderer_List == vm_spice.Get_Renderer_List() &&
		_Use_Playback_Compression == vm_spice.Use_Playback_Compression() &&
		_Use_Password == vm_spice.Use_Password() &&
		Password == vm_spice.Get_Password() )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool VM_SPICE::Use_SPICE() const
{
	return Enable_SPICE;
}

void VM_SPICE::Use_SPICE( bool use )
{
	Enable_SPICE = use;
}

bool VM_SPICE::operator!=( const VM_SPICE &vm_spice ) const
{
	return ! ( operator==(vm_spice) );
}

unsigned int VM_SPICE::Get_GXL_Devices_Count() const
{
	return GXL_Devices_Count;
}

void VM_SPICE::Set_GXL_Devices_Count( unsigned int count )
{
	GXL_Devices_Count = count;
}

int VM_SPICE::Get_RAM_Size() const
{
	return RAM_Size;
}

void VM_SPICE::Set_RAM_Size( int size )
{
	RAM_Size = size;
}

unsigned int VM_SPICE::Get_Port() const
{
	return Port;
}

void VM_SPICE::Set_Port( unsigned int port )
{
	Port = port;
}

bool VM_SPICE::Use_SPort() const
{
	return _Use_SPort;
}

void VM_SPICE::Use_SPort( bool use )
{
	_Use_SPort = use;
}

unsigned int VM_SPICE::Get_SPort() const
{
	return SPort;
}

void VM_SPICE::Set_SPort( unsigned int sport )
{
	SPort = sport;
}

bool VM_SPICE::Use_Hostname() const
{
	return _Use_Hostname;
}

void VM_SPICE::Use_Hostname( bool use )
{
	_Use_Hostname = use;
}

QString VM_SPICE::Get_Hostname() const
{
	return Hostname;
}

void VM_SPICE::Set_Hostname( QString hostname )
{
	Hostname = hostname;
}

bool VM_SPICE::Use_Image_Compression() const
{
	return _Use_Image_Compression;
}

void VM_SPICE::Use_Image_Compression( bool use )
{
	_Use_Image_Compression = use;
}

VM::SPICE_Image_Compression_Type VM_SPICE::Get_Image_Compression() const
{
	return Image_Compression;
}

void VM_SPICE::Set_Image_Compression( VM::SPICE_Image_Compression_Type type )
{
	Image_Compression = type;
}

bool VM_SPICE::Use_Video_Stream_Compression() const
{
	return _Use_Video_Stream_Compression;
}

void VM_SPICE::Use_Video_Stream_Compression( bool use )
{
	_Use_Video_Stream_Compression = use;
}

bool VM_SPICE::Use_Renderer() const
{
	return _Use_Renderer;
}

void VM_SPICE::Use_Renderer( bool use )
{
	_Use_Renderer = use;
}

const QList<VM::SPICE_Renderer> &VM_SPICE::Get_Renderer_List() const
{
	return Renderer_List;
}

void VM_SPICE::Set_Renderer_List( const QList<VM::SPICE_Renderer> &list )
{
	Renderer_List = list;
}

bool VM_SPICE::Use_Playback_Compression() const
{
	return _Use_Playback_Compression;
}

void VM_SPICE::Use_Playback_Compression( bool use )
{
	_Use_Playback_Compression = use;
}

bool VM_SPICE::Use_Password() const
{
	return _Use_Password;
}

void VM_SPICE::Use_Password( bool use )
{
	_Use_Password = use;
}

QString VM_SPICE::Get_Password() const
{
	return Password;
}

void VM_SPICE::Set_Password( QString password )
{
	Password = password;
}

//===========================================================================
