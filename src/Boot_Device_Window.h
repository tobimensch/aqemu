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

#ifndef BOOT_DEVICE_WINDOW_H
#define BOOT_DEVICE_WINDOW_H

#include "ui_Boot_Device_Window.h"
#include "VM_Devices.h"

class Boot_Device_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Boot_Device_Window( QWidget *parent = 0 );
		
		QList<VM::Boot_Order> data() const;
		void setData( const QList<VM::Boot_Order> &list );
		
		bool useBootMenu() const;
		void setUseBootMenu( bool use );
		
	private slots:
		void on_TB_Up_clicked();
		void on_TB_Down_clicked();
		int Get_Current_Index();
		
	private:
		Ui::Boot_Device_Window ui;
};

#endif
