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

#ifndef HDD_IMAGE_INFO_H
#define HDD_IMAGE_INFO_H

#include <QProcess>
#include "VM_Devices.h"

class HDD_Image_Info : public QObject
{
	Q_OBJECT
	
	public:
		HDD_Image_Info( QObject *parent = 0 );
		VM::Disk_Info Get_Disk_Info() const;
		
	public slots:
		void Update_Disk_Info( const QString &path );
		
	private slots:
		void Parse_Info( int exitCode, QProcess::ExitStatus exitStatus );
		void Clear_Info();
		
	signals:
		void Completed( bool ok );
		
	private:
		VM::Disk_Info Info;
		QProcess* QEMU_IMG_Proc;
};

#endif
