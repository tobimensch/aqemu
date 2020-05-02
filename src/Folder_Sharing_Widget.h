/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
** Copyright (C) 2016 Tobias Gläßer
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

#ifndef FOLDER_SHARING_WIDGET_H
#define FOLDER_SHARING_WIDGET_H

#include "VM.h"
//#include "Properties_Window.h"
#include "ui_Folder_Sharing_Widget.h"

class Device_Manager_Widget;

class Folder_Sharing_Widget: public QWidget
{
	Q_OBJECT
	
	public:
		explicit Folder_Sharing_Widget( QWidget *parent = 0 );
		~Folder_Sharing_Widget();
		void Set_VM( const Virtual_Machine &vm );
		
		void Set_Enabled( bool on );
		
		QList<VM_Shared_Folder> Shared_Folders;

        void syncLayout(Device_Manager_Widget*);
		
	public slots:
		void Update_Enabled_Actions();
		void Update_List_Mode();
		
	signals:
		void Folder_Changed();
	
	private slots:
		// For Context Menu
		void on_Folders_List_customContextMenuRequested( const QPoint &pos );
		void on_Folders_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
		void on_Folders_List_itemDoubleClicked( QListWidgetItem *item );

		void on_actionAdd_Samba_Folder_triggered();
		void on_actionAdd_Folder_triggered();
		void on_actionProperties_triggered();
		void on_actionRemove_triggered();
		void on_actionIcon_Mode_triggered();
		void on_actionList_Mode_triggered();
		
		void Update_Icons();
		
	private:
		bool Enabled;
		
		//Properties_Window *pw;
		Ui::Folder_Sharing_Widget ui;
		QMenu *Context_Menu;
};

#endif
