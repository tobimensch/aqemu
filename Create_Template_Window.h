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

#ifndef CREATE_TEMPLATE_WINDOW_H
#define CREATE_TEMPLATE_WINDOW_H

#include <QFlags>
#include <QSettings>
#include <QList>

#include "ui_Create_Template_Window.h"

class Create_Template_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Create_Template_Window( QWidget *parent = 0 );
		void Set_VM_Path( const QString &path );
		
		// Options For Create Templates
		enum Template_Option { Template_Save_FDD_CD=0x2,
							   Template_Save_HDD=0x40,
							   Template_Save_Network=0x800,
							   Template_Save_Default=0x16000,
							   Template_Save_Ports=0x320000 };
		
		Q_DECLARE_FLAGS( Template_Options, Template_Option )
	
	private slots:
		void on_TB_VM_File_Browse_clicked();
		void on_TB_Template_Folder_Browse_clicked();
		void on_Button_Create_clicked();
	
	private:
		bool Name_is_Unique();
		QSettings Settings;
		QList<QString> List_Templates;
		Ui::Create_Template_Window ui;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( Create_Template_Window::Template_Options )

#endif
