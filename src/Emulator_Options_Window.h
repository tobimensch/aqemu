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

#ifndef EMULATOR_OPTIONS_WINDOW_H
#define EMULATOR_OPTIONS_WINDOW_H

#include <QSettings>
#include "VM_Devices.h"
#include "ui_Emulator_Options_Window.h"

class Emulator_Options_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Emulator_Options_Window( QWidget *parent = 0 );
		
		Emulator Get_Emulator() const;
		void Set_Emulator( const Emulator &emul );
		
		void Set_All_Emulators_Names( const QStringList &allNames );
		
	private slots:
		void on_Button_OK_clicked();
		void on_Edit_Name_textChanged();
		void on_Edit_Path_to_Dir_textChanged();
		void on_Button_Find_clicked();
		void on_TB_Browse_clicked();
		void on_Table_Systems_itemDoubleClicked( QTableWidgetItem *item );
		void on_RB_QEMU_toggled( bool checked );
		void on_RB_KVM_toggled( bool checked );
		
		bool Name_Valid( const QString &name );
		void Update_Emulator();
		
	private:
		Ui::Emulator_Options_Window ui;
		
		QStringList All_Emulators_Names;
		QSettings Settings;
		Emulator Current_Emulator;
		bool Update_Info; // Update emulator information before exit
};

#endif
