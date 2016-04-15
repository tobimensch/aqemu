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

#ifndef DELETE_VM_FILES_WINDOW_H
#define DELETE_VM_FILES_WINDOW_H

#include "ui_Delete_VM_Files_Window.h"
#include "VM.h"

struct File_List_Item
{
	bool Hard_Drive;
	QString Name;
	QString Path;
};


class Delete_VM_Files_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Delete_VM_Files_Window( QWidget *parent = 0 );
		Delete_VM_Files_Window( Virtual_Machine *vm, QWidget *parent = 0 );
		void Set_VM( Virtual_Machine *vm );
	
	private slots:
		void on_Button_Delete_clicked();
		void on_RB_Show_HDD_toggled( bool checked );
		
		void Add_To_Files_List( const File_List_Item &item );
		bool Path_Valid( const QString &path );
		void Clear_List();
	
	private:
		Ui::Delete_VM_Files_Window ui;
		QList<File_List_Item> File_List_Items;
		QString VM_Name;
		QString VM_Path;
};

#endif
