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

#ifndef EDIT_EMULATOR_VERSION_WINDOW_H
#define EDIT_EMULATOR_VERSION_WINDOW_H

#include "ui_Edit_Emulator_Version_Window.h"
#include "VM_Devices.h"

class Edit_Emulator_Version_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Edit_Emulator_Version_Window( QWidget *parent = 0 );
		
		void Load_Emulators();
	
	private slots:
		void on_Button_OK_clicked();
		void on_List_Emulators_currentRowChanged( int currentRow );
		void CB_Versions_currentIndexChanged( const QString &text );
		
	private:
		Ui::Edit_Emulator_Version_Window ui;
		QList<Emulator> Emulators;
};

#endif
