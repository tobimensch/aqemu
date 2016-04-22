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

#ifndef ADD_PORT_WINDOW_H
#define ADD_PORT_WINDOW_H

#include "ui_Add_Port_Window.h"
#include "VM_Devices.h"

class Add_Port_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Add_Port_Window( QWidget *parent = 0 );
		
		VM_Port Get_Port() const;
		void Set_Port( const VM_Port &port );
		
	private slots:
		void Connect_Slots();
		void Show_Arg_Help();
		void on_TB_Browse_clicked();
		
	private:
		Ui::Add_Port_Window ui;
};

#endif
