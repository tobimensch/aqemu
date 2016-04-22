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

#ifndef SNAPSHOT_WINDOW_H
#define SNAPSHOT_WINDOW_H

#include <QSettings>
#include "VM.h"
#include "ui_Snapshots_Window.h"

class Snapshots_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Snapshots_Window( QWidget *parent = 0 );
		void Set_VM( Virtual_Machine *vm );
		
	private slots:
		void on_Snapshots_Tree_itemSelectionChanged();
		
		void on_Button_Create_clicked();
		void on_Button_Delete_clicked();
		void on_Button_Start_clicked();
		void on_Button_Properties_clicked();
		void on_Button_Update_clicked();
		
	private:
		bool Update_Info();
		
		Ui::Snapshots_Window ui;
		Virtual_Machine *Current_VM;
		QSettings Settings;
};

#endif
