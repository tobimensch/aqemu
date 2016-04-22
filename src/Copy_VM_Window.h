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

#ifndef COPY_VM_WINDOW_H
#define COPY_VM_WINDOW_H

#include "ui_Copy_VM_Window.h"

class Copy_VM_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Copy_VM_Window( QWidget *parent = 0 );
		
		QString Get_New_VM_Name() const;
		bool Get_Copy_Disk_Images() const;
		bool Get_Copy_Floppy() const;
		bool Get_Copy_Hard_Drive() const;
		
		void Add_VM_Machine_Name( const QString &name );
	
	private slots:
		void on_Button_OK_clicked();
	
	private:
		Ui::Copy_VM_Window ui;
		QStringList All_Machine_Names;
};

#endif
