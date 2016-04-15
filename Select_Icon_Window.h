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

#ifndef SELECT_ICON_WINDOW_H
#define SELECT_ICON_WINDOW_H

#include <QSettings>

#include "ui_Select_Icon_Window.h"

class Select_Icon_Window: public QDialog
{
	Q_OBJECT

public:
	Select_Icon_Window( QWidget *parent = 0 );
	void Set_Previous_Icon_Path( const QString& path );
	QString Get_New_Icon_Path() const;

private slots:
	void on_Button_OK_clicked();
	void on_Button_Browse_clicked();	
	void on_All_Icons_List_itemDoubleClicked( QListWidgetItem *item );

private:
	bool GB_Locked;
	QSettings Settings;
	QString New_Icon_Path;
	Ui::Select_Icon_Window ui;
};

#endif
