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

#ifndef SETTINGS_WINDOW_H
#define SETTINGS_WINDOW_H

#include <QSettings>
#include <QList>

#include "ui_Settings_Window.h"

class Settings_Window: public QDialog
{
	Q_OBJECT
	
	public:
		Settings_Window( QWidget *parent = 0 );
		~Settings_Window();
		
	public slots:
		void on_Button_OK_clicked();
		
	private slots:
		void on_Button_Create_Template_from_VM_clicked();
		void on_TB_VM_Folder_clicked();
		void CB_Language_currentIndexChanged( int index );
		void CB_Icons_Theme_currentIndexChanged( int index );
		void VNC_Warning( bool state );
		void Load_Templates();
		
	private:
		QSettings *Settings;
		Ui::Settings_Window ui;
};

#endif
