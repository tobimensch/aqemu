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

#ifndef ADD_USB_PORT_WINDOW_H
#define ADD_USB_PORT_WINDOW_H

#include "ui_Add_USB_Port_Window.h"
#include "VM_Devices.h"

class Add_USB_Port_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Add_USB_Port_Window( QWidget *parent = 0 );
		
		VM_USB Get_Port() const;
		void Set_Port( const VM_USB &port );
	
	private slots:
		void on_Button_Update_Host_USB_clicked();
		void on_RB_QEMU_USB_clicked( bool checked );
		void on_RB_Host_USB_clicked( bool checked );
		void on_Table_Host_USB_currentItemChanged( QTableWidgetItem *current, QTableWidgetItem *previous );
		void on_RB_Show_All_toggled( bool on );
		
	private:
		Ui::Add_USB_Port_Window ui;
		QList<VM_USB> USB_Host_List;
		mutable VM_USB Current_Item;
};

#endif
