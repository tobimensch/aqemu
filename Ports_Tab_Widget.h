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

#ifndef PORTS_TAB_WINGET_H
#define PORTS_TAB_WINGET_H

#include "VM_Devices.h"
#include "ui_Ports_Tab_Widget.h"

class Ports_Tab_Widget: public QWidget
{
	Q_OBJECT
	
	public:
		Ports_Tab_Widget( QWidget *parent = 0 );
		
		QList<VM_Port> Get_Serial_Ports() const;
		void Set_Serial_Ports( const QList<VM_Port> &list );
		
		QList<VM_Port> Get_Parallel_Ports() const;
		void Set_Parallel_Ports( const QList<VM_Port> &list );
		
		QList<VM_USB> Get_USB_Ports() const;
		void Set_USB_Ports( const QList<VM_USB> &list );
		
		void Clear_Old_Ports();
		
	private:
		QString Get_Port_Info( const VM_Port &port );
		QString Get_USB_Port_Info( const VM_USB &port );
		
	private slots:
		void on_TB_Add_Serial_Port_clicked();
		void on_TB_Add_Parallel_Port_clicked();
		void on_TB_Add_USB_Port_clicked();
		void on_TB_Edit_Port_clicked();
		void on_TB_Delete_Port_clicked();
		
		void on_Ports_Table_itemDoubleClicked( QTableWidgetItem *item );
		
	signals:
		void Settings_Changet();
		
	private:
		Ui::Ports_Tab_Widget ui;
		
		int Index;
		
		QMap<int, VM_Port> Serial_Ports;
		QMap<int, VM_Port> Parallel_Ports;
		QMap<int, VM_USB> USB_Ports;
};

#endif
