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

#ifndef OLD_NETWORK_WIDGET_H
#define OLD_NETWORK_WIDGET_H

#include "ui_Old_Network_Widget.h"
#include "VM_Devices.h"

class Old_Network_Widget: public QWidget
{
	Q_OBJECT
	
	public:
		Old_Network_Widget( QWidget *parent = 0 );
		
		bool Get_Network_Cards( QList<VM_Net_Card> &cards );
		void Set_Network_Cards( const QList<VM_Net_Card> &cards );
		
		void Set_Network_Card_Models( const QList<Device_Map> &models );
		
		void Set_Enabled( bool on );
		
	private slots:
		void Connect_Slots();
		void Disconnect_Slots();
		
		void on_Network_Cards_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous );
		void on_Button_Add_Net_Card_clicked();
		void on_Button_Delete_Net_Card_clicked();
		void on_CB_Network_Card_Model_currentIndexChanged( int index );
		void on_CB_Connection_Mode_currentIndexChanged( int index );
		void on_Edit_Hostname_textChanged();
		void on_Edit_IP_Address_textChanged();
		void on_Edit_MAC_Address_textChanged();
		void on_SB_Port_valueChanged( int i );
		void on_SB_VLAN_valueChanged( int i );
		void on_CH_TUN_TAP_Script_stateChanged( int state );
		void on_Edit_TUN_TAP_Script_textChanged();
		void on_Edit_Interface_Name_textChanged();
		void on_SB_File_Descriptor_valueChanged( int i );
		void on_TB_Generate_New_MAC_clicked();
		void on_TB_Browse_TUN_Script_clicked();
		
		void Set_Net_Card_To_Ui( const VM_Net_Card &card );
		bool Net_Card_is_Valid( int index );
	
	signals:
		void Changet();
		
	private:
		Ui::Old_Network_Widget ui;
		QList<VM_Net_Card> Network_Cards;
		bool Check_Network_Card;
		QStringList Card_Models_QEMU_Name;
};

#endif
