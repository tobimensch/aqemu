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

#ifndef FIRST_START_WIZARD_H
#define FIRST_START_WIZARD_H

#include <QSettings>
#include "VM_Devices.h"
#include "ui_First_Start_Wizard.h"

class First_Start_Wizard: public QDialog
{
	Q_OBJECT
	
	public:
		First_Start_Wizard( QWidget *parent = 0 );
		bool Find_Emulators();
	
	private slots:
		void on_Button_Cancel_clicked();
		void on_Button_Back_clicked();
		void on_Button_Next_clicked();
		void on_Edit_VM_Dir_textChanged();
		void on_TB_Browse_VM_Dir_clicked();
		
		void on_Button_Find_Emulators_clicked();
		void on_Button_Skip_Find_clicked();
		void on_Button_Edit_clicked();
		
		#ifdef Q_OS_WIN32
		void on_TB_Add_Emulator_Browse_clicked();
		void on_Button_Add_Emulator_Find_clicked();
		void on_Button_Add_Emulator_Manual_Mode_clicked();
		#endif
		
		void on_All_Pages_currentChanged( int index );
		
		void Load_Settings();
		bool Save_Settings();
		void retranslateUi();
	
	private:
		Ui::First_Start_Wizard ui;
		bool Next_Move;
		bool Emulators_Find_Done;
		QStringList Header_Captions;
		QSettings Settings;
		Emulator Emul;
};

#endif
