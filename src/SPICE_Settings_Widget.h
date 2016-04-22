/****************************************************************************
**
** Copyright (C) 2010 Andrey Rijov <ANDron142@yandex.ru>
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

#ifndef SPICE_SETTINGS_WIDGET_H
#define SPICE_SETTINGS_WIDGET_H

#include "ui_SPICE_Settings_Widget.h"
#include "VM_Devices.h"

class SPICE_Settings_Widget: public QWidget
{
	Q_OBJECT
	
	public:
		SPICE_Settings_Widget( QWidget *parent = 0 );
		
		const VM_SPICE &Get_Settings( bool &settingsValidated ) const;
		void Set_Settings( const VM_SPICE &settings );
		
		void Set_PSO_GXL( bool use );
		
	private slots:
		void on_TB_Up_clicked();
		void on_TB_Down_clicked();
		
	signals:
		void State_Changet();
	
	private:
		void Show_Renderer_List( const QList<VM::SPICE_Renderer> &list );
		
		Ui::SPICE_Settings_Widget ui;
};

#endif
