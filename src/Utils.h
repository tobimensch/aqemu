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

#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QList>

#include "VM_Devices.h"

void AQDebug( const QString &sender, const QString &mes );
void AQWarning( const QString &sender, const QString &mes );
void AQError( const QString &sender, const QString &mes );

void AQGraphic_Warning( const QString &caption, const QString &mes );
void AQGraphic_Warning( const QString &sender, const QString &caption, const QString &mes, bool fatal = false );
void AQGraphic_Error( const QString &sender, const QString &caption, const QString &mes, bool fatal = false );

void AQUse_Log( bool use );
void AQUse_Debug_Output( bool use, bool d, bool w, bool e );
void AQLog_Path( const QString &path );
void AQSave_To_Log( const QString &mes_type, const QString &sender, const QString &mes );

bool Create_New_HDD_Image( bool encrypted, const QString &base_image,
						   const QString &file_name, const QString &format, VM::Device_Size size, bool verbose );
bool Create_New_HDD_Image( const QString &file_name, VM::Device_Size size );
bool Format_HDD_Image( const QString &file_name, VM::Disk_Info info );

QList<QString> Get_Templates_List();

QString Get_FS_Compatible_VM_Name( const QString &name );
QString Get_Complete_VM_File_Path( const QString &vm_name );

QString Get_TR_Size_Suffix( VM::Device_Size suf );

QString Get_Last_Dir_Path( const QString &path );

bool It_Host_Device( const QString &path );

void Check_AQEMU_Permissions();

VM::Emulator_Version String_To_Emulator_Version( const QString &str );
QString Emulator_Version_To_String( VM::Emulator_Version ver );

bool Update_Emulators_List();
const QList<Emulator> &Get_Emulators_List();
bool Remove_All_Emulators_Files();
const Emulator &Get_Default_Emulator( VM::Emulator_Type type );
const Emulator &Get_Emulator_By_Name( const QString &name );

int Get_Random( int min, int max );

void Load_Recent_Images_List();
const QStringList &Get_CD_Recent_Images_List();
void Add_To_Recent_CD_Files( const QString &path );
const QStringList &Get_FDD_Recent_Images_List();
void Add_To_Recent_FDD_Files( const QString &path );

bool Get_Show_Error_Window();
void Set_Show_Error_Window( bool show );

#endif
