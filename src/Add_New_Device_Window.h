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

#ifndef ADD_NEW_DEVICE_WINDOW_H
#define ADD_NEW_DEVICE_WINDOW_H

#include "VM.h"
#include "VM_Devices.h"

#include "ui_Add_New_Device_Window.h"

class Add_New_Device_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Add_New_Device_Window( QWidget *parent = 0 );
		
		VM_Nativ_Storage_Device Get_Device() const;
		void Set_Device( const VM_Nativ_Storage_Device &dev );
		
		void Set_Emulator_Devices( const Available_Devices &devices );
		
		void Set_Enabled( bool enabled );
	
	private slots:
		void on_CB_Interface_currentIndexChanged( const QString &text );
		void on_TB_File_Path_Browse_clicked();
		void on_Button_OK_clicked();
	
	private:
		VM_Nativ_Storage_Device Device;
		Ui::Add_New_Device_Window ui;
};

#endif
