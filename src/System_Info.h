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

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include "Utils.h"
#include "VM_Devices.h"

class System_Info
{
	public:
		System_Info();
		
		static bool Update_VM_Computers_List();
		
		static VM::Emulator_Version Get_Emulator_Version( const QString &path = "" );
		
		static QMap<QString, QString> Find_QEMU_Binary_Files( const QString &path );
		static QMap<QString, QString> Find_KVM_Binary_Files( const QString &path );
		static QString Find_IMG( const QStringList &paths );
		
		static Available_Devices Get_Emulator_Info( const QString &path, bool *ok,
												   VM::Emulator_Version version, const QString &internalName );
		static QString Get_Emulator_Help_Output( const QString &path );
		static QString Get_Emulator_Output( const QString &path, const QStringList &args );
		
		static const QList<VM_USB> &Get_All_Host_USB();
		static const QList<VM_USB> &Get_Used_USB_List();
		static bool Add_To_Used_USB_List( const VM_USB &device );
		static bool Delete_From_Used_USB_List( const VM_USB &device );
		static bool Update_Host_USB();
		
		static void Get_Free_Memory_Size( int &allRAM, int &freeRAM );
		
		static QStringList Get_Host_FDD_List();
		static QStringList Get_Host_CDROM_List();
		
		static QMap<QString, Available_Devices> Emulator_QEMU_0_9_0;
		static QMap<QString, Available_Devices> Emulator_QEMU_0_9_1;
		static QMap<QString, Available_Devices> Emulator_QEMU_0_10;
		static QMap<QString, Available_Devices> Emulator_QEMU_0_11;
		static QMap<QString, Available_Devices> Emulator_QEMU_0_12;
		static QMap<QString, Available_Devices> Emulator_QEMU_0_13;
		static QMap<QString, Available_Devices> Emulator_QEMU_0_14;
		
		static QMap<QString, Available_Devices> Emulator_KVM_7X;
		static QMap<QString, Available_Devices> Emulator_KVM_8X;
		static QMap<QString, Available_Devices> Emulator_KVM_0_11;
		static QMap<QString, Available_Devices> Emulator_KVM_0_12;
		static QMap<QString, Available_Devices> Emulator_KVM_0_13;
		static QMap<QString, Available_Devices> Emulator_KVM_0_14;
		
	private:
		#ifdef Q_OS_LINUX
		static bool Scan_USB_Sys( QList<VM_USB> &list );
		static bool Scan_USB_Proc( QList<VM_USB> &list );
		static bool Read_SysFS_File( const QString &path, QString &data );
		#endif
		
		static QList<VM_USB> All_Host_USB;
		static QList<VM_USB> Used_Host_USB;
};

#endif
