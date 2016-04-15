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

#ifndef CREATE_HDD_IMAGE_WINDOW_H
#define CREATE_HDD_IMAGE_WINDOW_H

#include "ui_Create_HDD_Image_Window.h"
#include "VM_Devices.h"

class Create_HDD_Image_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Create_HDD_Image_Window( QWidget *parent = 0 );
		
		const QString &Get_Image_File_Name();
		void Set_Image_File_Name( const QString &path );
		void Set_Image_Info( VM::Disk_Info info );
		void Set_Image_Size( double gb );
		
	private slots:
		void on_Button_Browse_Base_Image_clicked();
		void on_Button_Browse_New_Image_clicked();
		void on_CB_Format_currentIndexChanged( const QString &text );
		void on_Button_Create_clicked();
		void on_Button_Format_Help_clicked();
		
	private:
		Ui::Create_HDD_Image_Window ui;
		QString Image_File_Name;
};

#endif
