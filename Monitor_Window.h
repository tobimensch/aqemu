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

#ifndef MONITOR_WINDOW_H
#define MONITOR_WINDOW_H

#include "ui_Monitor_Window.h"

class Monitor_Window: public QDialog
{
	Q_OBJECT

	public:
		Monitor_Window( QWidget *parent = 0 );
		
	public slots:
		void Add_QEMU_Out( const QString &new_text );
		
	signals:
		void Command_Sended( const QString &com );
		void Closing_Win();
		
	private slots:
		void on_Button_Run_clicked();
		void on_Button_Font_clicked();
		QFont Get_Font();
		
	protected:
		void closeEvent( QCloseEvent *event );
		
	private:
		Ui::Monitor_Window ui;
};

#endif
