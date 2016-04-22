/****************************************************************************
**
** Copyright (C) 2009-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include <QFile>
#include <QSettings>

#include "Utils.h"
#include "HDD_Image_Info.h"

HDD_Image_Info::HDD_Image_Info( QObject *parent )
		: QObject( parent )
{
	QEMU_IMG_Proc = new QProcess( this );
}

VM::Disk_Info HDD_Image_Info::Get_Disk_Info() const
{
	return Info;
}

void HDD_Image_Info::Update_Disk_Info( const QString &path )
{
	Info.Image_File_Name = path;
	
	if( Info.Image_File_Name.isEmpty() )
	{
		Clear_Info();
		return;
	}
	
	if( QFile::exists(Info.Image_File_Name) == false )
	{
		AQWarning( "void QEMU_IMG_Thread::run()",
				   "Image \"" + Info.Image_File_Name + "\" not exists!" );
		Clear_Info();
		return;
	}
	else
	{
		QStringList args;
		args << "info" << Info.Image_File_Name;
		
		QEMU_IMG_Proc = new QProcess( this );
		QSettings settings;
		QEMU_IMG_Proc->start( settings.value("QEMU-IMG_Path", "qemu-img").toString(), args );
		
		connect( QEMU_IMG_Proc, SIGNAL(finished(int, QProcess::ExitStatus)),
				 this, SLOT(Parse_Info(int, QProcess::ExitStatus)), Qt::DirectConnection );
		
		connect( QEMU_IMG_Proc, SIGNAL(error(QProcess::ProcessError)),
				 this, SLOT(Clear_Info()), Qt::DirectConnection );
	}
}

void HDD_Image_Info::Clear_Info()
{
	AQDebug( "void HDD_Image_Info::Clear_Info()",
			 "HDD Info Not Read!" );
	
	VM_HDD tmp_hdd;
	Info.Disk_Format = "";
	Info.Virtual_Size = tmp_hdd.String_to_Device_Size( "0 G" );
	Info.Disk_Size = tmp_hdd.String_to_Device_Size( "0 G" );
	Info.Cluster_Size = 0;
	
	emit Completed( false );
}

void HDD_Image_Info::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )
{
	QByteArray info_str_ba = QEMU_IMG_Proc->readAll();
	QString info_str = QString( info_str_ba ); // Create QString
	if( info_str.isEmpty() )
	{
		AQDebug( "void HDD_Image_Info::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )",
				 "Data is empty." );
		return;
	}
	
	QRegExp RegInfo = QRegExp( ".*image:[\\s]+([^\n\r]+).*file format:[\\s]+([\\w\\d]+).*virtual size:[\\s]+([\\d]+[.]*[\\d]*[KMG]+).*disk size:[\\s]+([\\d]+[.]*[\\d]*[KMG]+).*cluster_size:[\\s]+([\\d]+).*" );
	
	bool cluster = true;
	
	if( ! RegInfo.exactMatch(info_str) )
	{
		AQWarning( "void QEMU_IMG_Thread::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )",
				   "QRegExp With Cluster Size Not Matched!" );
		
		RegInfo = QRegExp( ".*image:[\\s]+(.+).+file format:[\\s]+([\\w\\d]+).*virtual size:[\\s]+([\\d]+[.]*[\\d]*[KMG]+).*disk size:[\\s]+([\\d]+[.]*[\\d]*[KMG]+).*" );
		
		if( ! RegInfo.exactMatch(info_str) )
		{
			AQError( "void QEMU_IMG_Thread::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )",
					 "QRegExp Without Cluster Size Not Matched! Image: " + Info.Image_File_Name + "\nData: " + info_str );
			Clear_Info();
			return;
		}
		else
		{
			cluster = false;
		}
	}
	
	QStringList info_lines = RegInfo.capturedTexts();
	
	if( cluster && info_lines.count() != 6 )
	{
		AQError( "void QEMU_IMG_Thread::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )",
				 "info_str.count() != 6" );
		Clear_Info();
		return;
	}
	else if( ! cluster && info_lines.count() != 5 )
	{
		AQError( "void QEMU_IMG_Thread::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )",
				 "info_str.count() != 5" );
		Clear_Info();
		return;
	}
	
	if( info_lines[1] != Info.Image_File_Name )
	{
		AQWarning( "void QEMU_IMG_Thread::Parse_Info( int exitCode, QProcess::ExitStatus exitStatus )",
				   QString("info_lines[1] != Image_File_Name\nDetails:\n[[%1]]\n[[%2]]").arg(info_lines[1]).arg(Info.Image_File_Name) );
	}
	
	Info.Disk_Format = info_lines[ 2 ];
	VM_HDD tmp_hdd;
	Info.Virtual_Size = tmp_hdd.String_to_Device_Size( info_lines[3] );
	Info.Disk_Size = tmp_hdd.String_to_Device_Size( info_lines[4] );
	if( cluster ) Info.Cluster_Size = info_lines[ 5 ].toInt();
	else Info.Cluster_Size = 0;
	
	emit Completed( true );
}
