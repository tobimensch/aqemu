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

#ifndef PROPERTIES_WINDOW_H
#define PROPERTIES_WINDOW_H

#include "VM_Devices.h"
#include "ui_Properties_Window.h"
#include "HDD_Image_Info.h"

class Properties_Window: public QDialog
{
	Q_OBJECT

	public:
		Properties_Window( QWidget *parent = 0 );
		
		const VM_Storage_Device &Get_Floppy();
		const VM_Storage_Device &Get_CD_ROM();
		const VM_HDD&Get_HDD();
		
		void Set_Floppy( const VM_Storage_Device &fd, const QString &name );
		void Set_CD_ROM( const VM_Storage_Device &cd, const QString &name );
		void Set_HDD( const VM_HDD &hd, const QString &name );
		
		void Set_Enabled( bool enabled );
		
		void Set_Current_Machine_Devices( const Available_Devices *dev );
		
	private slots:
		void on_Button_OK_clicked();
		
		void on_Button_Update_Info_clicked();
		
		void on_TB_FD_Image_Browse_clicked();
		void on_TB_CDROM_Image_Browse_clicked();
		void on_TB_HDD_Image_Browse_clicked();
		void on_Button_HDD_New_clicked();
		void on_Button_HDD_Format_clicked();
		
		void Update_HDD( bool ok );
		
		void on_TB_FD_Advanced_Settings_clicked();
		void on_CB_FD_Devices_editTextChanged( const QString &text );
		
		void on_TB_CDROM_Advanced_Settings_clicked();
		void on_CB_CDROM_Devices_editTextChanged( const QString &text );
		
		void on_TB_HDD_Advanced_Settings_clicked();
		void on_Edit_HDD_Image_Path_textChanged();
		
	private:
		Ui::Properties_Window ui;
		VM_Storage_Device PW_Storage;
		VM_HDD PW_HDD;
		HDD_Image_Info* HDD_Info;
		const Available_Devices *Current_Machine_Devices;
};

#endif
