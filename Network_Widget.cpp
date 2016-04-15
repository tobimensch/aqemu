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

#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include "Utils.h"
#include "Network_Widget.h"

Network_Widget::Network_Widget( QWidget *parent ) : QWidget( parent )
{
	ui.setupUi( this );
	
	on_CB_Network_Type_currentIndexChanged( 0 );
	
	Connect_Slots();
}

bool Network_Widget::Get_Network_Cards( QList<VM_Net_Card_Nativ> &cards )
{
	if( ui.Items_List->count() < 1 )
	{
		AQDebug( "bool Network_Widget::Get_Network_Card( QList<VM_Net_Card_Nativ> &cards ) const",
				 "No Items" );
		cards.clear();
		return true;
	}
	
	if( ! Net_Card_is_Valid() ) return false;
	
	if( ui.Items_List->currentRow() >= 0 && Network_Cards.count() > 0 )
	{
		Network_Cards[ ui.Items_List->currentRow() ] = Get_Net_Card_From_Ui();
	}
	
	cards = Network_Cards;
	
	return true;
}

void Network_Widget::Set_Network_Cards( const QList<VM_Net_Card_Nativ> &cards )
{
	ui.Items_List->clear();
	Network_Cards = cards;
	
	if( Network_Cards.count() > 0 )
	{
		nic = user = chanel = tap = socket = multi = vde = dump = 0;
		
		// Max net card count == 8
		for( int nx = 0; nx < Network_Cards.count() && nx < 8; ++nx )
		{
			QString item_name = "";
			
			if( Network_Cards[nx].Get_Name().isEmpty() == false &&
				Network_Cards[nx].Use_Name() == true )
			{
				item_name = Network_Cards[nx].Get_Name();
			}
			
			switch( Network_Cards[nx].Get_Network_Type() )
			{
				case VM::Net_Mode_Nativ_NIC:
					new QListWidgetItem( "NIC " + ( item_name.isEmpty() ?
													QString::number(++nic) :
													"(" + Network_Cards[nx].Get_Name() + ")" ),
										 ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_User:
					new QListWidgetItem( "User " + ( item_name.isEmpty() ?
													 QString::number(++user) :
													 "(" + Network_Cards[nx].Get_Name() + ")" ),
										 ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_Chanel:
					new QListWidgetItem( "Channel " + QString::number(++chanel), ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_TAP:
					new QListWidgetItem( "TAP " + ( item_name.isEmpty() ?
													QString::number(++tap) :
													"(" + Network_Cards[nx].Get_Name() + ")" ),
										 ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_Socket:
					new QListWidgetItem( "Socket " + ( item_name.isEmpty() ?
													   QString::number(++socket) :
													   "(" + Network_Cards[nx].Get_Name() + ")" ),
										 ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_MulticastSocket:
					new QListWidgetItem( "Multicast Socket " + ( item_name.isEmpty() ?
																 QString::number(++multi) :
																 "(" + Network_Cards[nx].Get_Name() + ")" ),
										 ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_VDE:
					new QListWidgetItem( "VDE " + ( item_name.isEmpty() ?
													QString::number(++vde) :
													"(" + Network_Cards[nx].Get_Name() + ")" ),
										 ui.Items_List );
					break;
					
				case VM::Net_Mode_Nativ_Dump:
					new QListWidgetItem( "Dump " + QString::number(++dump), ui.Items_List );
					break;
					
				default:
					new QListWidgetItem( tr("Unknown Type Item"), ui.Items_List );
					break;
			}
		}
		
		ui.Items_List->setCurrentRow( 0 );
		
		// Max net card count == 8
		if( Network_Cards.count() < 8 ) Enable_Buttons( true, true );
		else Enable_Buttons( false, true );
	}
	else
	{
		Enable_Buttons( true, false );
	}
}

void Network_Widget::Set_Network_Card_Models( const QList<Device_Map> &models )
{
	if( ui.CB_model->count() > 0 ) ui.CB_model->clear();
	if( Card_Models_QEMU_Name.count() > 0 ) Card_Models_QEMU_Name.clear();
	
	for( int ix = 0; ix < models.count(); ix++ )
	{
		ui.CB_model->addItem( models[ix].Caption );
		Card_Models_QEMU_Name << models[ix].QEMU_Name;
	}
}

void Network_Widget::Set_Devices( const Available_Devices &devices )
{
	// Set network cards models
	if( ui.CB_model->count() > 0 ) ui.CB_model->clear();
	if( Card_Models_QEMU_Name.count() > 0 ) Card_Models_QEMU_Name.clear();
	
	for( int ix = 0; ix < devices.Network_Card_List.count(); ix++ )
	{
		ui.CB_model->addItem( devices.Network_Card_List[ix].Caption );
		Card_Models_QEMU_Name << devices.Network_Card_List[ix].QEMU_Name;
	}
	
	// Set types
	ui.CB_Network_Type->clear();
	ui.CB_Network_Type->addItem( "nic" );
	ui.CB_Network_Type->addItem( "user" );
	ui.CB_Network_Type->addItem( "channel" );
	ui.CB_Network_Type->addItem( "tap" );
	ui.CB_Network_Type->addItem( "socket" );
	ui.CB_Network_Type->addItem( "multicast socket" );
	
	if( devices.PSO_Net_type_vde ) ui.CB_Network_Type->addItem( "vde" );
	if( devices.PSO_Net_type_dump ) ui.CB_Network_Type->addItem( "dump" );
	
	// Set PSO
	PSO_Net_name = devices.PSO_Net_name;
	PSO_Net_addr = devices.PSO_Net_addr;
	PSO_Net_vectors = devices.PSO_Net_vectors;
	
	PSO_Net_net = devices.PSO_Net_net;
	PSO_Net_host = devices.PSO_Net_host;
	PSO_Net_restrict = devices.PSO_Net_restrict;
	PSO_Net_dhcpstart = devices.PSO_Net_dhcpstart;
	PSO_Net_dns = devices.PSO_Net_dns;
	PSO_Net_tftp = devices.PSO_Net_tftp;
	PSO_Net_bootfile = devices.PSO_Net_bootfile;
	PSO_Net_smb = devices.PSO_Net_smb;
	PSO_Net_hostfwd = devices.PSO_Net_hostfwd;
	PSO_Net_guestfwd = devices.PSO_Net_guestfwd;
	
	PSO_Net_ifname = devices.PSO_Net_ifname;
	PSO_Net_script = devices.PSO_Net_script;
	PSO_Net_downscript = devices.PSO_Net_downscript;
	PSO_Net_sndbuf = devices.PSO_Net_sndbuf;
	PSO_Net_vnet_hdr = devices.PSO_Net_vnet_hdr;
	PSO_Net_vhost = devices.PSO_Net_vhost;
	PSO_Net_vhostfd = devices.PSO_Net_vhostfd;
	
	PSO_Net_listen = devices.PSO_Net_listen;
	PSO_Net_connect = devices.PSO_Net_connect;
	
	PSO_Net_mcast = devices.PSO_Net_mcast;
	
	PSO_Net_sock = devices.PSO_Net_sock;
	PSO_Net_port = devices.PSO_Net_port;
	PSO_Net_group = devices.PSO_Net_group;
	PSO_Net_mode = devices.PSO_Net_mode;
	
	PSO_Net_file = devices.PSO_Net_file;
	PSO_Net_len = devices.PSO_Net_len;
}

void Network_Widget::Set_Enabled( bool on )
{
	ui.Label_Items_List->setEnabled( on );
	ui.Button_Add->setEnabled( on );
	ui.Button_Delete->setEnabled( on );
	ui.General_Widget->setEnabled( on );
}

void Network_Widget::Connect_Slots()
{
	connect( ui.CB_Network_Type, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CB_model, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_macaddr, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_macaddr, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_vlan, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_vlan, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_name, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_name, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_hostname, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_hostname, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_port_dev, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_fd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_fd, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_ifname, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_ifname, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_script, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_script, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_downscript, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_downscript, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_listen, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_listen, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_connect, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_connect, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_mcast, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_mcast, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_sock, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_sock, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_port, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_port, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_group, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_group, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_mode, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_mode, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_file, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_file, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_len, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CB_len, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_addr, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_addr, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_vectors, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_vectors, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_net, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_net, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_host, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_host, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_restrict, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CB_restrict, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_dhcpstart, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_dhcpstart, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_dns, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_dns, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_tftp, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_tftp, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_bootfile, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_bootfile, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_smb, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_smb, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_smbserver, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_smbserver, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_sndbuf, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_sndbuf, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_vnet_hdr, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CB_vnet_hdr, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_vhost, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CB_vhost, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_vhostfd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_vhostfd, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_hostfwd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_hostfwd, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_guestfwd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_guestfwd, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	// Update Name
	connect( ui.CH_name, SIGNAL(clicked()),
			 this, SLOT(Update_Card_Item()) );
	
	connect( ui.Edit_name, SIGNAL(textChanged(const QString &)),
			 this, SLOT(Update_Card_Item()) );
	
	connect( ui.CB_Network_Type, SIGNAL(currentIndexChanged(int)),
			 this, SLOT(Update_Card_Item()) );
}

void Network_Widget::Disconnect_Slots()
{
	disconnect( ui.CB_Network_Type, SIGNAL(currentIndexChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CB_model, SIGNAL(currentIndexChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_macaddr, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_macaddr, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_vlan, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_vlan, SIGNAL(valueChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_name, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_name, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_hostname, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_hostname, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_port_dev, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_fd, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_fd, SIGNAL(valueChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_ifname, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_ifname, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_script, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_script, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_downscript, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_downscript, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_listen, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_listen, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_connect, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_connect, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_mcast, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_mcast, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_sock, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_sock, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_port, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_port, SIGNAL(valueChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_group, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_group, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_mode, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_mode, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_file, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_file, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_len, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CB_len, SIGNAL(currentIndexChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_addr, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_addr, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_vectors, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_vectors, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_net, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_net, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_host, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_host, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_restrict, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CB_restrict, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_dhcpstart, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_dhcpstart, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_dns, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_dns, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_tftp, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_tftp, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_bootfile, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_bootfile, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_smb, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_smb, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_smbserver, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_smbserver, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_sndbuf, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_sndbuf, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_vnet_hdr, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CB_vnet_hdr, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_vhost, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CB_vhost, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_vhostfd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_vhostfd, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_hostfwd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_hostfwd, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_guestfwd, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_guestfwd, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	// Update Name
	disconnect( ui.CH_name, SIGNAL(clicked()),
				this, SLOT(Update_Card_Item()) );
	
	disconnect( ui.Edit_name, SIGNAL(textChanged(const QString &)),
				this, SLOT(Update_Card_Item()) );
	
	disconnect( ui.CB_Network_Type, SIGNAL(currentIndexChanged(int)),
				this, SLOT(Update_Card_Item()) );
}

void Network_Widget::on_Button_Add_clicked()
{
	if( ui.Items_List->count() >= 8 )
		QMessageBox::information( this, tr("Warning!"), tr("Maximum Network Cards Count is 8.") );
	else
	{
		VM_Net_Card_Nativ tmp_card;
		Network_Cards.append( tmp_card );
		
		new QListWidgetItem( tr("New Item"), ui.Items_List );
		ui.Items_List->setCurrentRow( ui.Items_List->count()-1 );
		
		if( ui.Items_List->count() >= 8 ) Enable_Buttons( false, true );
		else Enable_Buttons( true, true );
		
		Update_Card_Item();
		
		emit Changet();
	}
}

void Network_Widget::on_Button_Delete_clicked()
{
	if( Network_Cards.count() < 1 || ui.Items_List->count() < 1 ) return;
	
	Network_Cards.removeAt( ui.Items_List->currentRow() );
	ui.Items_List->takeItem( ui.Items_List->currentRow() );
	
	if( Network_Cards.count() <= 0 ) Enable_Buttons( true, false );
	else if( Network_Cards.count() < 8 ) Enable_Buttons( true, true );
	
	emit Changet();
}

void Network_Widget::on_Items_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous )
{
	if( Network_Cards.count() < 1 ) return;
	
	int cur = ui.Items_List->row( current );
	int prev = ui.Items_List->row( previous );
	
	if( cur  < 0 || cur  >= Network_Cards.count() || prev >= Network_Cards.count() )
	{
		if( Network_Cards.count() <= 0 ) Enable_Buttons( true, false );
		return;
	}
	else if( prev < 0 )
	{
		// This first change item. No previous.
		// Show Current Item
		Disconnect_Slots();
		Set_Net_Card_To_Ui( Network_Cards[0] );
		Connect_Slots();
		
		return;
	}
	
	// Check Previous Item
	if( ! Net_Card_is_Valid() ) return;
	else Network_Cards[ prev ] = Get_Net_Card_From_Ui();
	
	// Show Current Item
	Disconnect_Slots();
	Set_Net_Card_To_Ui( Network_Cards[cur] );
	Connect_Slots();
}

void Network_Widget::on_TB_Help_clicked()
{
	// -net nic[,vlan=n][,macaddr=addr][,model=type][,name=name]
	if( ui.CB_Network_Type->currentText() == "nic" )
		QMessageBox::information( this, tr("nic"), tr("-net nic[,vlan=n][,macaddr=addr][,model=type][,name=name] \nCreate a new Network Interface Card and connect it to VLAN n (n = 0 is the default). The NIC is an ne2k_pci by default on the PC target. Optionally, the MAC address can be changed to addr and a name can be assigned for use in monitor commands. If no \'-net\' option is specified, a single NIC is created. Qemu can emulate several different models of network card. Valid values for type are i82551, i82557b, i82559er, ne2k_pci, ne2k_isa, pcnet, rtl8139, e1000, smc91c111, lance and mcf_fec. Not all devices are supported on all targets. Use -net nic,model=? for a list of available devices for your target.") );
	
	// -net user[,vlan=n][,hostname=name][,name=name]
	else if( ui.CB_Network_Type->currentText() == "user" )
		QMessageBox::information( this, tr("user"), tr("-net user[,vlan=n][,hostname=name][,name=name] \nUse the user mode network stack which requires no administrator privilege to run. \'hostname=name\' can be used to specify the client hostname reported by the builtin DHCP server.") );
	
	// -net channel,port:dev
	else if( ui.CB_Network_Type->currentText() == "channel" )
		QMessageBox::information( this, tr("channel"), tr("-net channel,port:dev \nForward \'user\' TCP connection to port port to character device dev") );
	
	// -net tap[,vlan=n][,name=name][,fd=h][,ifname=name][,script=file][,downscript=dfile]
	else if( ui.CB_Network_Type->currentText() == "tap" )
		QMessageBox::information( this, tr("tap"), tr("-net tap[,vlan=n][,name=name][,fd=h][,ifname=name][,script=file][,downscript=dfile] \nConnect the host TAP network interface name to VLAN n, use the network script file to configure it and the network script dfile to deconfigure it. If name is not provided, the OS automatically provides one. \'fd\'=h can be used to specify the handle of an already opened host TAP interface. The default network configure script is \'/etc/qemu-ifup\' and the default network deconfigure script is \'/etc/qemu-ifdown\'. Use \'script=no\' or \'downscript=no\' to disable script execution.") );
	
	// -net socket[,vlan=n][,name=name][,fd=h][,listen=[host]:port][,connect=host:port]
	else if( ui.CB_Network_Type->currentText() == "socket" )
		QMessageBox::information( this, tr("socket"), tr("-net socket[,vlan=n][,name=name][,fd=h][,listen=[host]:port][,connect=host:port] \nConnect the VLAN n to a remote VLAN in another QEMU virtual machine using a TCP socket connection. If \'listen\' is specified, QEMU waits for incoming connections on port (host is optional). \'connect\' is used to connect to another QEMU instance using the \'listen\' option. \'fd\'=h specifies an already opened TCP socket.") );
	
	// -net socket[,vlan=n][,name=name][,fd=h][,mcast=maddr:port]
	else if( ui.CB_Network_Type->currentText() == "multicast socket" )
		QMessageBox::information( this, tr("socket"), tr("-net socket[,vlan=n][,name=name][,fd=h][,mcast=maddr:port] \nCreate a VLAN n shared with another QEMU virtual machines using a UDP multicast socket, effectively making a bus for every QEMU with same multicast address maddr and port. \nNOTES: \n1. Several QEMU can be running on different hosts and share same bus (assuming correct multicast setup for these hosts). \n2. mcast support is compatible with User Mode Linux (argument \'ethN=mcast\'), see http://user-mode-linux.sf.net. \n3. Use \'fd=h\' to specify an already opened UDP multicast socket.") );
	
	// -net vde[,vlan=n][,name=name][,sock=socketpath][,port=n][,group=groupname][,mode=octalmode]
	else if( ui.CB_Network_Type->currentText() == "vde" )
		QMessageBox::information( this, tr("vde"), tr("-net vde[,vlan=n][,name=name][,sock=socketpath][,port=n][,group=groupname][,mode=octalmode] \nConnect VLAN n to PORT n of a vde switch running on host and listening for incoming connections on socketpath. Use GROUP groupname and MODE octalmode to change default ownership and permissions for communication port. This option is available only if QEMU has been compiled with vde support enabled.") );
	
	// -net dump[,vlan=n][,file=file][,len=len]
	else if( ui.CB_Network_Type->currentText() == "dump" )
		QMessageBox::information( this, tr("dump"), tr("-net dump[,vlan=n][,file=f][,len=n] \ndump traffic on vlan \'n\' to file \'f\' (max n bytes per packet)") );
	
	else
	{
		AQError( "void Network_Widget::on_TB_Help_clicked()",
				 "Invalid connection type! Data: " + ui.CB_Network_Type->currentText() );
	}
}

void Network_Widget::on_TB_Browse_script_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select script"),
													 Get_Last_Dir_Path(ui.Edit_script->text()),
													 tr("All Files (*);;Scripts (*.sh)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_script->setText( QDir::toNativeSeparators(fileName) );
}

void Network_Widget::on_TB_Browse_downscript_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select script"),
													 Get_Last_Dir_Path(ui.Edit_downscript->text()),
													 tr("All Files (*);;Scripts (*.sh)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_downscript->setText( QDir::toNativeSeparators(fileName) );
}

void Network_Widget::on_CB_Network_Type_currentIndexChanged( int index )
{
	// Hide All
	ui.CH_vlan->setVisible( false );
	ui.SB_vlan->setVisible( false );
	
	ui.CH_macaddr->setVisible( false );
	ui.Edit_macaddr->setVisible( false );
	ui.TB_Generate_New_MAC->setVisible( false );
	
	ui.Label_model->setVisible( false );
	ui.CB_model->setVisible( false );
	
	ui.CH_name->setVisible( false );
	ui.Edit_name->setVisible( false );
	
	ui.CH_hostname->setVisible( false );
	ui.Edit_hostname->setVisible( false );
	
	ui.Label_port_dev->setVisible( false );
	ui.Edit_port_dev->setVisible( false );
	
	ui.CH_fd->setVisible( false );
	ui.SB_fd->setVisible( false );
	
	ui.CH_ifname->setVisible( false );
	ui.Edit_ifname->setVisible( false );
	
	ui.CH_script->setVisible( false );
	ui.Edit_script->setVisible( false );
	ui.TB_Browse_script->setVisible( false );
	
	ui.CH_downscript->setVisible( false );
	ui.Edit_downscript->setVisible( false );
	ui.TB_Browse_downscript->setVisible( false );
	
	ui.CH_listen->setVisible( false );
	ui.Edit_listen->setVisible( false );
	
	ui.CH_connect->setVisible( false );
	ui.Edit_connect->setVisible( false );
	
	ui.CH_mcast->setVisible( false );
	ui.Edit_mcast->setVisible( false );
	
	ui.CH_sock->setVisible( false );
	ui.Edit_sock->setVisible( false );
	
	ui.CH_port->setVisible( false );
	ui.SB_port->setVisible( false );
	
	ui.CH_group->setVisible( false );
	ui.Edit_group->setVisible( false );
	
	ui.CH_mode->setVisible( false );
	ui.Edit_mode->setVisible( false );
	
	ui.CH_file->setVisible( false );
	ui.Edit_file->setVisible( false );
	
	ui.CH_len->setVisible( false );
	ui.CB_len->setVisible( false );
	
	ui.CH_addr->setVisible( false );
	ui.Edit_addr->setVisible( false );
	
	ui.CH_vectors->setVisible( false );
	ui.SB_vectors->setVisible( false );
	
	ui.CH_net->setVisible( false );
	ui.Edit_net->setVisible( false );
	
	ui.CH_host->setVisible( false );
	ui.Edit_host->setVisible( false );
	
	ui.CH_restrict->setVisible( false );
	ui.CB_restrict->setVisible( false );
	
	ui.CH_dhcpstart->setVisible( false );
	ui.Edit_dhcpstart->setVisible( false );
	
	ui.CH_dns->setVisible( false );
	ui.Edit_dns->setVisible( false );
	
	ui.CH_tftp->setVisible( false );
	ui.Edit_tftp->setVisible( false );
	
	ui.CH_bootfile->setVisible( false );
	ui.Edit_bootfile->setVisible( false );
	ui.TB_Browse_bootfile->setVisible( false );
	
	ui.CH_smb->setVisible( false );
	ui.Edit_smb->setVisible( false );
	ui.TB_Browse_smb->setVisible( false );
	ui.CH_smbserver->setVisible( false );
	ui.Edit_smbserver->setVisible( false );
	
	ui.CH_hostfwd->setVisible( false );
	ui.Edit_hostfwd->setVisible( false );
	
	ui.CH_guestfwd->setVisible( false );
	ui.Edit_guestfwd->setVisible( false );
	
	ui.CH_sndbuf->setVisible( false );
	ui.SB_sndbuf->setVisible( false );
	
	ui.CH_vnet_hdr->setVisible( false );
	ui.CB_vnet_hdr->setVisible( false );
	
	ui.CH_vhost->setVisible( false );
	ui.CB_vhost->setVisible( false );
	
	ui.CH_vhostfd->setVisible( false );
	ui.SB_vhostfd->setVisible( false );
	
	// -net nic[,vlan=n][,macaddr=mac][,model=type][,name=str][,addr=str][,vectors=v]
	if( ui.CB_Network_Type->currentText() == "nic" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_macaddr->setVisible( true );
		ui.Edit_macaddr->setVisible( true );
		ui.TB_Generate_New_MAC->setVisible( true );
		
		ui.Label_model->setVisible( true );
		ui.CB_model->setVisible( true );
		
		ui.CH_name->setVisible( true );
		ui.Edit_name->setVisible( true );
		
		ui.CH_addr->setVisible( true );
		ui.Edit_addr->setVisible( true );
		
		ui.CH_vectors->setVisible( true );
		ui.SB_vectors->setVisible( true );
	}
	// -net user[,vlan=n][,name=str][,net=addr[/mask]][,host=addr][,restrict=y|n]
	//			[,hostname=host][,dhcpstart=addr][,dns=addr][,tftp=dir][,bootfile=f]
	//			[,hostfwd=rule][,guestfwd=rule][,smb=dir[,smbserver=addr]]
	else if( ui.CB_Network_Type->currentText() == "user" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_hostname->setVisible( true );
		ui.Edit_hostname->setVisible( true );
		
		ui.CH_name->setVisible( true );
		ui.Edit_name->setVisible( true );
		
		ui.CH_net->setVisible( true );
		ui.Edit_net->setVisible( true );
		
		ui.CH_host->setVisible( true );
		ui.Edit_host->setVisible( true );
		
		ui.CH_restrict->setVisible( true );
		ui.CB_restrict->setVisible( true );
		
		ui.CH_dhcpstart->setVisible( true );
		ui.Edit_dhcpstart->setVisible( true );
		
		ui.CH_dns->setVisible( true );
		ui.Edit_dns->setVisible( true );
		
		ui.CH_tftp->setVisible( true );
		ui.Edit_tftp->setVisible( true );
		
		ui.CH_bootfile->setVisible( true );
		ui.Edit_bootfile->setVisible( true );
		ui.TB_Browse_bootfile->setVisible( true );
		
		ui.CH_hostfwd->setVisible( true );
		ui.Edit_hostfwd->setVisible( true );
		
		ui.CH_guestfwd->setVisible( true );
		ui.Edit_guestfwd->setVisible( true );
		
		ui.CH_smb->setVisible( true );
		ui.Edit_smb->setVisible( true );
		ui.TB_Browse_smb->setVisible( true );
		
		ui.CH_smbserver->setVisible( true );
		ui.Edit_smbserver->setVisible( true );
	}
	// -net channel,port:dev
	else if( ui.CB_Network_Type->currentText() == "channel" )
	{
		ui.Label_port_dev->setVisible( true );
		ui.Edit_port_dev->setVisible( true );
	}
	// -net tap[,vlan=n][,name=str][,fd=h][,ifname=name][,script=file][,downscript=dfile]
	//		   [,sndbuf=nbytes][,vnet_hdr=on|off][,vhost=on|off][,vhostfd=h]
	else if( ui.CB_Network_Type->currentText() == "tap" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_name->setVisible( true );
		ui.Edit_name->setVisible( true );
		
		ui.CH_fd->setVisible( true );
		ui.SB_fd->setVisible( true );
		
		ui.CH_ifname->setVisible( true );
		ui.Edit_ifname->setVisible( true );
		
		ui.CH_script->setVisible( true );
		ui.Edit_script->setVisible( true );
		ui.TB_Browse_script->setVisible( true );
		
		ui.CH_downscript->setVisible( true );
		ui.Edit_downscript->setVisible( true );
		ui.TB_Browse_downscript->setVisible( true );
		
		ui.CH_sndbuf->setVisible( true );
		ui.SB_sndbuf->setVisible( true );
		
		ui.CH_vnet_hdr->setVisible( true );
		ui.CB_vnet_hdr->setVisible( true );
		
		ui.CH_vhost->setVisible( true );
		ui.CB_vhost->setVisible( true );
		
		ui.CH_vhostfd->setVisible( true );
		ui.SB_vhostfd->setVisible( true );
	}
	// -net socket[,vlan=n][,name=str][,fd=h][,listen=[host]:port][,connect=host:port]
	else if( ui.CB_Network_Type->currentText() == "socket" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_name->setVisible( true );
		ui.Edit_name->setVisible( true );
		
		ui.CH_fd->setVisible( true );
		ui.SB_fd->setVisible( true );
		
		ui.CH_listen->setVisible( true );
		ui.Edit_listen->setVisible( true );
		
		ui.CH_connect->setVisible( true );
		ui.Edit_connect->setVisible( true );
	}
	// -net socket[,vlan=n][,name=str][,fd=h][,mcast=maddr:port]
	else if( ui.CB_Network_Type->currentText() == "multicast socket" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_name->setVisible( true );
		ui.Edit_name->setVisible( true );
		
		ui.CH_fd->setVisible( true );
		ui.SB_fd->setVisible( true );
		
		ui.CH_mcast->setVisible( true );
		ui.Edit_mcast->setVisible( true );
	}
	// -net vde[,vlan=n][,name=str][,sock=socketpath][,port=n][,group=groupname][,mode=octalmode]
	else if( ui.CB_Network_Type->currentText() == "vde" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_name->setVisible( true );
		ui.Edit_name->setVisible( true );
		
		ui.CH_sock->setVisible( true );
		ui.Edit_sock->setVisible( true );
		
		ui.CH_port->setVisible( true );
		ui.SB_port->setVisible( true );
		
		ui.CH_group->setVisible( true );
		ui.Edit_group->setVisible( true );
		
		ui.CH_mode->setVisible( true );
		ui.Edit_mode->setVisible( true );
	}
	// -net dump[,vlan=n][,file=file][,len=len]
	else if( ui.CB_Network_Type->currentText() == "dump" )
	{
		ui.CH_vlan->setVisible( true );
		ui.SB_vlan->setVisible( true );
		
		ui.CH_file->setVisible( true );
		ui.Edit_file->setVisible( true );
		
		ui.CH_len->setVisible( true );
		ui.CB_len->setVisible( true );
	}
	else
	{
		if( ui.CB_Network_Type->count() > 0 )
			AQError( "void Network_Widget::on_CB_Network_Type_currentIndexChanged( int index )",
					 "Invalid connection type! Data: " + ui.CB_Network_Type->currentText() );
	}
	
	// Set PSO
	if( ! PSO_Net_name )
	{
		ui.CH_name->setVisible( false );
		ui.Edit_name->setVisible( false );
	}
	
	if( ! PSO_Net_addr )
	{
		ui.CH_addr->setVisible( false );
		ui.Edit_addr->setVisible( false );
	}
	
	if( ! PSO_Net_vectors )
	{
		ui.CH_vectors->setVisible( false );
		ui.SB_vectors->setVisible( false );
	}
	
	if( ! PSO_Net_net )
	{
		ui.CH_net->setVisible( false );
		ui.Edit_net->setVisible( false );
	}
	
	if( ! PSO_Net_host )
	{
		ui.CH_host->setVisible( false );
		ui.Edit_host->setVisible( false );
	}
	
	if( ! PSO_Net_restrict )
	{
		ui.CH_restrict->setVisible( false );
		ui.CB_restrict->setVisible( false );
	}
	
	if( ! PSO_Net_dhcpstart )
	{
		ui.CH_dhcpstart->setVisible( false );
		ui.Edit_dhcpstart->setVisible( false );
	}
	
	if( ! PSO_Net_dns )
	{
		ui.CH_dns->setVisible( false );
		ui.Edit_dns->setVisible( false );
	}
	
	if( ! PSO_Net_tftp )
	{
		ui.CH_tftp->setVisible( false );
		ui.Edit_tftp->setVisible( false );
	}
	
	if( ! PSO_Net_bootfile )
	{
		ui.CH_bootfile->setVisible( false );
		ui.Edit_bootfile->setVisible( false );
		ui.TB_Browse_bootfile->setVisible( false );
	}
	
	if( ! PSO_Net_hostfwd )
	{
		ui.CH_hostfwd->setVisible( false );
		ui.Edit_hostfwd->setVisible( false );
	}
	
	if( ! PSO_Net_guestfwd )
	{
		ui.CH_guestfwd->setVisible( false );
		ui.Edit_guestfwd->setVisible( false );
	}
	
	if( ! PSO_Net_smb )
	{
		ui.CH_smb->setVisible( false );
		ui.Edit_smb->setVisible( false );
		ui.TB_Browse_smb->setVisible( false );
		ui.CH_smbserver->setVisible( false );
		ui.Edit_smbserver->setVisible( false );
	}
	
	if( ! PSO_Net_ifname )
	{
		ui.CH_ifname->setVisible( false );
		ui.Edit_ifname->setVisible( false );
	}
	
	if( ! PSO_Net_script )
	{
		ui.CH_script->setVisible( false );
		ui.Edit_script->setVisible( false );
		ui.TB_Browse_script->setVisible( false );
	}
	
	if( ! PSO_Net_downscript )
	{
		ui.CH_downscript->setVisible( false );
		ui.Edit_downscript->setVisible( false );
		ui.TB_Browse_downscript->setVisible( false );
	}
	
	if( ! PSO_Net_sndbuf )
	{
		ui.CH_sndbuf->setVisible( false );
		ui.SB_sndbuf->setVisible( false );
	}
	
	if( ! PSO_Net_vnet_hdr )
	{
		ui.CH_vnet_hdr->setVisible( false );
		ui.CB_vnet_hdr->setVisible( false );
	}
	
	if( ! PSO_Net_vhost )
	{
		ui.CH_vhost->setVisible( false );
		ui.CB_vhost->setVisible( false );
	}
	
	if( ! PSO_Net_vhostfd )
	{
		ui.CH_vhostfd->setVisible( false );
		ui.SB_vhostfd->setVisible( false );
	}
	
	if( ! PSO_Net_listen )
	{
		ui.CH_listen->setVisible( false );
		ui.Edit_listen->setVisible( false );
	}
	
	if( ! PSO_Net_connect )
	{
		ui.CH_connect->setVisible( false );
		ui.Edit_connect->setVisible( false );
	}
	
	if( ! PSO_Net_mcast )
	{
		ui.CH_mcast->setVisible( false );
		ui.Edit_mcast->setVisible( false );
	}
	
	if( ! PSO_Net_sock )
	{
		ui.CH_sock->setVisible( false );
		ui.Edit_sock->setVisible( false );
	}
	
	if( ! PSO_Net_port )
	{
		ui.CH_port->setVisible( false );
		ui.SB_port->setVisible( false );
	}
	
	if( ! PSO_Net_group )
	{
		ui.CH_group->setVisible( false );
		ui.Edit_group->setVisible( false );
	}
	
	if( ! PSO_Net_mode )
	{
		ui.CH_mode->setVisible( false );
		ui.Edit_mode->setVisible( false );
	}
	
	if( ! PSO_Net_file )
	{
		ui.CH_file->setVisible( false );
		ui.Edit_file->setVisible( false );
	}
	
	if( ! PSO_Net_len )
	{
		ui.CH_len->setVisible( false );
		ui.CB_len->setVisible( false );
	}
	
	
	/* FIXME
	if( ! PSO_Net_hostfwd )
	if( ! PSO_Net_guestfwd )
	
	ui.CH_vlan->setVisible( false );
	ui.SB_vlan->setVisible( false );
	
	ui.CH_macaddr->setVisible( false );
	ui.Edit_macaddr->setVisible( false );
	ui.TB_Generate_New_MAC->setVisible( false );
	
	ui.Label_model->setVisible( false );
	ui.CB_model->setVisible( false );
	
	ui.CH_hostname->setVisible( false );
	ui.Edit_hostname->setVisible( false );
	
	ui.Label_port_dev->setVisible( false );
	ui.Edit_port_dev->setVisible( false );
	
	ui.CH_fd->setVisible( false );
	ui.SB_fd->setVisible( false );*/
}

void Network_Widget::on_TB_Generate_New_MAC_clicked()
{
	if( ! ui.Items_List->currentItem() ) return;
	
	ui.Edit_macaddr->setText( Network_Cards[ ui.Items_List->currentRow() ].Generate_MAC() );
}

void Network_Widget::on_TB_Browse_bootfile_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Boot file"),
													 Get_Last_Dir_Path(ui.Edit_bootfile->text()),
													 tr("All Files (*)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_bootfile->setText( QDir::toNativeSeparators(fileName) );
}

void Network_Widget::on_TB_Browse_smb_clicked()
{
	QString dirName = QFileDialog::getExistingDirectory( this, tr("SMB directory"),
														 Get_Last_Dir_Path(ui.Edit_bootfile->text()) );
	
	if( ! dirName.isEmpty() )
		ui.Edit_smb->setText( QDir::toNativeSeparators(dirName) );
}

void Network_Widget::on_CH_smb_toggled( bool checked )
{
	ui.CH_smbserver->setEnabled( checked );
	
	if( ui.CH_smbserver->isEnabled() && ui.CH_smbserver->isChecked() )
		ui.Edit_smbserver->setEnabled( true );
	else
		ui.Edit_smbserver->setEnabled( false );
}

VM_Net_Card_Nativ Network_Widget::Get_Net_Card_From_Ui() const
{
	VM_Net_Card_Nativ card;
	
	switch( ui.CB_Network_Type->currentIndex() )
	{
		case 0:
			card.Set_Network_Type( VM::Net_Mode_Nativ_NIC );
			break;
			
		case 1:
			card.Set_Network_Type( VM::Net_Mode_Nativ_User );
			break;
			
		case 2:
			card.Set_Network_Type( VM::Net_Mode_Nativ_Chanel );
			break;
			
		case 3:
			card.Set_Network_Type( VM::Net_Mode_Nativ_TAP );
			break;
			
		case 4:
			card.Set_Network_Type( VM::Net_Mode_Nativ_Socket );
			break;
			
		case 5:
			card.Set_Network_Type( VM::Net_Mode_Nativ_MulticastSocket );
			break;
			
		case 6:
			card.Set_Network_Type( VM::Net_Mode_Nativ_VDE );
			break;
			
		case 7:
			card.Set_Network_Type( VM::Net_Mode_Nativ_Dump );
			break;
			
		default:
			AQError( "VM_Net_Card_Nativ Network_Widget::Get_Net_Card_From_Ui() const",
					 "Cannot Read Network Type! Use Default: User Mode" );
			card.Set_Network_Type( VM::Net_Mode_Nativ_User );
			break;
	}
	
	// Model
	card.Set_Card_Model( Card_Models_QEMU_Name[ui.CB_model->currentIndex()] );
	
	// MAC
	card.Use_MAC_Address( ui.CH_macaddr->isChecked() );
	card.Set_MAC_Address( ui.Edit_macaddr->text() );
	
	// VLAN
	card.Use_VLAN( ui.CH_vlan->isChecked() );
	card.Set_VLAN( ui.SB_vlan->value() );
	
	// name
	card.Use_Name( ui.CH_name->isChecked() );
	card.Set_Name( ui.Edit_name->text() );
	
	// Hostname
	card.Use_Hostname( ui.CH_hostname->isChecked() );
	card.Set_Hostname( ui.Edit_hostname->text() );
	
	// port:dev
	card.Set_PortDev( ui.Edit_port_dev->text() );
	
	// FD
	card.Use_File_Descriptor( ui.CH_fd->isChecked() );
	card.Set_File_Descriptor( ui.SB_fd->value() );
	
	// ifname
	card.Use_Interface_Name( ui.CH_ifname->isChecked() );
	card.Set_Interface_Name( ui.Edit_ifname->text() );
	
	// script
	card.Use_TUN_TAP_Script( ui.CH_script->isChecked() );
	card.Set_TUN_TAP_Script( ui.Edit_script->text() );
	
	// down script
	card.Use_TUN_TAP_Down_Script( ui.CH_downscript->isChecked() );
	card.Set_TUN_TAP_Down_Script( ui.Edit_downscript->text() );
	
	// listen
	card.Use_Listen( ui.CH_listen->isChecked() );
	card.Set_Listen( ui. Edit_listen->text() );
	
	// connect
	card.Use_Connect( ui.CH_connect->isChecked() );
	card.Set_Connect( ui.Edit_connect->text() );
	
	// mcast
	card.Use_MCast( ui.CH_mcast->isChecked() );
	card.Set_MCast( ui.Edit_mcast->text() );
	
	// sock
	card.Use_Sock( ui.CH_sock->isChecked() );
	card.Set_Sock( ui.Edit_sock->text() );
	
	// port
	card.Use_Port( ui.CH_port->isChecked() );
	card.Set_Port( ui.SB_port->value() );
	
	// group
	card.Use_Group( ui.CH_group->isChecked() );
	card.Set_Group( ui.Edit_group->text() );
	
	// mode
	card.Use_Mode( ui.CH_mode->isChecked() );
	card.Set_Mode( ui.Edit_mode->text() );
	
	// file
	card.Use_File( ui.CH_file->isChecked() );
	card.Set_File( ui.Edit_file->text() );
	
	// len
	card.Use_Len( ui.CH_len->isChecked() );
	
	switch( ui.CB_len->currentIndex() )
	{
		case 0:
			card.Set_Len( 64 );
			break;
			
		case 1:
			card.Set_Len( 128 );
			break;
			
		case 2:
			card.Set_Len( 512 );
			break;
			
		case 3:
			card.Set_Len( 1024 );
			break;
			
		case 4:
			card.Set_Len( 5120 );
			break;
			
		case 5:
			card.Set_Len( 10240 );
			break;
			
		default:
			card.Set_Len( 64 );
			break;
	}
	
	// Addr
	card.Use_Addr( ui.CH_addr->isChecked() );
	card.Set_Addr( ui.Edit_addr->text() );
	
	// Vectors
	card.Use_Vectors( ui.CH_vectors->isChecked() );
	card.Set_Vectors( ui.SB_vectors->value() );
	
	// Net
	card.Use_Net( ui.CH_net->isChecked() );
	card.Set_Net( ui.Edit_net->text() );
	
	// Host
	card.Use_Host( ui.CH_host->isChecked() );
	card.Set_Host( ui.Edit_host->text() );
	
	// Restrict
	card.Use_Restrict( ui.CH_restrict->isChecked() );
	card.Set_Restrict( ui.CB_restrict->currentIndex() == 0 );
	
	// DHCPstart
	card.Use_DHCPstart( ui.CH_dhcpstart->isChecked() );
	card.Set_DHCPstart( ui.Edit_dhcpstart->text() );
	
	// DNS
	card.Use_DNS( ui.CH_dns->isChecked() );
	card.Set_DNS( ui.Edit_dns->text() );
	
	// Tftp
	card.Use_Tftp( ui.CH_tftp->isChecked() );
	card.Set_Tftp( ui.Edit_tftp->text() );
	
	// Bootfile
	card.Use_Bootfile( ui.CH_bootfile->isChecked() );
	card.Set_Bootfile( ui.Edit_bootfile->text() );
	
	// HostFwd
	card.Use_HostFwd( ui.CH_hostfwd->isChecked() );
	card.Set_HostFwd( ui.Edit_hostfwd->text() );
	
	// GuestFwd
	card.Use_GuestFwd( ui.CH_guestfwd->isChecked() );
	card.Set_GuestFwd( ui.Edit_guestfwd->text() );
	
	// SMB
	card.Use_SMB( ui.CH_smb->isChecked() );
	card.Set_SMB( ui.Edit_smb->text() );
	
	// SMBserver
	card.Use_SMBserver( ui.CH_smbserver->isChecked() );
	card.Set_SMBserver( ui.Edit_smbserver->text() );
	
	// Sndbuf
	card.Use_Sndbuf( ui.CH_sndbuf->isChecked() );
	card.Set_Sndbuf( ui.SB_sndbuf->value() );
	
	// VNet_hdr
	card.Use_VNet_hdr( ui.CH_vnet_hdr->isChecked() );
	card.Set_VNet_hdr( ui.CB_vnet_hdr->currentIndex() == 0 );
	
	// VHost
	card.Use_VHost( ui.CH_vhost->isChecked() );
	card.Set_VHost( ui.CB_vhost->currentIndex() == 0 );
	
	// VHostFd
	card.Use_VHostFd( ui.CH_vhostfd->isChecked() );
	card.Set_VHostFd( ui.SB_vhostfd->value() );
	
	return card;
}

void Network_Widget::Set_Net_Card_To_Ui( const VM_Net_Card_Nativ &card )
{
	switch( card.Get_Network_Type() )
	{
		case VM::Net_Mode_Nativ_NIC:
			ui.CB_Network_Type->setCurrentIndex( 0 );
			break;
			
		case VM::Net_Mode_Nativ_User:
			ui.CB_Network_Type->setCurrentIndex( 1 );
			break;
			
		case VM::Net_Mode_Nativ_Chanel:
			ui.CB_Network_Type->setCurrentIndex( 2 );
			break;
			
		case VM::Net_Mode_Nativ_TAP:
			ui.CB_Network_Type->setCurrentIndex( 3 );
			break;
			
		case VM::Net_Mode_Nativ_Socket:
			ui.CB_Network_Type->setCurrentIndex( 4 );
			break;
			
		case VM::Net_Mode_Nativ_MulticastSocket:
			ui.CB_Network_Type->setCurrentIndex( 5 );
			break;
			
		case VM::Net_Mode_Nativ_VDE:
			ui.CB_Network_Type->setCurrentIndex( 6 );
			break;
			
		case VM::Net_Mode_Nativ_Dump:
			ui.CB_Network_Type->setCurrentIndex( 7 );
			break;
			
		default:
			AQError( "void Network_Widget::Set_Net_Card_To_Ui( const VM_Net_Card_Nativ &card )",
					 "Cannot Read Network Type! Use Default: User Mode" );
			ui.CB_Network_Type->setCurrentIndex( 1 );
			break;
	}
	
	// Model
	if( ui.CB_model->count() < 1 )
	{
		AQError( "void Network_Widget::Set_Net_Card_To_Ui( const VM_Net_Card_Nativ &card )",
				 "Cannot Read Network Model!" );
	}
	else
	{
		ui.CB_model->setCurrentIndex( 0 );
		
		for( int ix = 0; ix < Card_Models_QEMU_Name.count(); ix++ )
		{
			if( card.Get_Card_Model() == Card_Models_QEMU_Name[ix] )
			{
				ui.CB_model->setCurrentIndex( ix );
			}
		}
	}
	
	// MAC
	ui.CH_macaddr->setChecked( card.Use_MAC_Address() );
	ui.Edit_macaddr->setText( card.Get_MAC_Address() );
	
	// VLAN
	ui.CH_vlan->setChecked( card.Use_VLAN() );
	ui.SB_vlan->setValue( card.Get_VLAN() );
	
	// name
	ui.CH_name->setChecked( card.Use_Name() );
	ui.Edit_name->setText( card.Get_Name() );
	
	// Hostname
	ui.CH_hostname->setChecked( card.Use_Hostname() );
	ui.Edit_hostname->setText( card.Get_Hostname() );
	
	// port:dev
	ui.Edit_port_dev->setText( card.Get_PortDev() );
	
	// FD
	ui.CH_fd->setChecked( card.Use_File_Descriptor() );
	ui.SB_fd->setValue( card.Get_File_Descriptor() );
	
	// ifname
	ui.CH_ifname->setChecked( card.Use_Interface_Name() );
	ui.Edit_ifname->setText( card.Get_Interface_Name() );
	
	// script
	ui.CH_script->setChecked( card.Use_TUN_TAP_Script() );
	ui.Edit_script->setText( card.Get_TUN_TAP_Script() );
	
	// down script
	ui.CH_downscript->setChecked( card.Use_TUN_TAP_Down_Script() );
	ui.Edit_downscript->setText( card.Get_TUN_TAP_Down_Script() );
	
	// listen
	ui.CH_listen->setChecked( card.Use_Listen() );
	ui.Edit_listen->setText( card.Get_Listen() );
	
	// connect
	ui.CH_connect->setChecked( card.Use_Connect() );
	ui.Edit_connect->setText( card.Get_Connect() );
	
	// mcast
	ui.CH_mcast->setChecked( card.Use_MCast() );
	ui.Edit_mcast->setText( card.Get_MCast() );
	
	// sock
	ui.CH_sock->setChecked( card.Use_Sock() );
	ui.Edit_sock->setText( card.Get_Sock() );
	
	// port
	ui.CH_port->setChecked( card.Use_Port() );
	ui.SB_port->setValue( card.Get_Port() );
	
	// group
	ui.CH_group->setChecked( card.Use_Group() );
	ui.Edit_group->setText( card.Get_Group() );
	
	// mode
	ui.CH_mode->setChecked( card.Use_Mode() );
	ui.Edit_mode->setText( card.Get_Mode() );
	
	// file
	ui.CH_file->setChecked( card.Use_File() );
	ui.Edit_file->setText( card.Get_File() );
	
	// len
	ui.CH_len->setChecked( card.Use_Len() );
	
	switch( card.Get_Len() )
	{
		case 64:
			ui.CB_len->setCurrentIndex( 0 );
			break;
			
		case 128:
			ui.CB_len->setCurrentIndex( 1 );
			break;
			
		case 512:
			ui.CB_len->setCurrentIndex( 2 );
			break;
			
		case 1024:
			ui.CB_len->setCurrentIndex( 3 );
			break;
			
		case 5120:
			ui.CB_len->setCurrentIndex( 4 );
			break;
			
		case 10240:
			ui.CB_len->setCurrentIndex( 5 );
			break;
			
		default:
			ui.CB_len->setCurrentIndex( 0 );
			break;
	}
	
	// Addr
	ui.CH_addr->setChecked( card.Use_Addr() );
	ui.Edit_addr->setText( card.Get_Addr() );
	
	// Vectors
	ui.CH_vectors->setChecked( card.Use_Vectors() );
	ui.SB_vectors->setValue( card.Get_Vectors() );
	
	// Net
	ui.CH_net->setChecked( card.Use_Net() );
	ui.Edit_net->setText( card.Get_Net() );
	
	// Host
	ui.CH_host->setChecked( card.Use_Host() );
	ui.Edit_host->setText( card.Get_Host() );
	
	// Restrict
	ui.CH_restrict->setChecked( card.Use_Restrict() );
	ui.CB_restrict->setCurrentIndex( card.Get_Restrict() ? 0 : 1 );
	
	// DHCPstart
	ui.CH_dhcpstart->setChecked( card.Use_DHCPstart() );
	ui.Edit_dhcpstart->setText( card.Get_DHCPstart() );
	
	// DNS
	ui.CH_dns->setChecked( card.Use_DNS() );
	ui.Edit_dns->setText( card.Get_DNS() );
	
	// Tftp
	ui.CH_tftp->setChecked( card.Use_Tftp() );
	ui.Edit_tftp->setText( card.Get_Tftp() );
	
	// Bootfile
	ui.CH_bootfile->setChecked( card.Use_Bootfile() );
	ui.Edit_bootfile->setText( card.Get_Bootfile() );
	
	// HostFwd
	ui.CH_hostfwd->setChecked( card.Use_HostFwd() );
	ui.Edit_hostfwd->setText( card.Get_HostFwd() );
	
	// GuestFwd
	ui.CH_guestfwd->setChecked( card.Use_GuestFwd() );
	ui.Edit_guestfwd->setText( card.Get_GuestFwd() );
	
	// SMB
	ui.CH_smb->setChecked( card.Use_SMB() );
	ui.Edit_smb->setText( card.Get_SMB() );
	
	// SMBserver
	ui.CH_smbserver->setChecked( card.Use_SMBserver() );
	ui.Edit_smbserver->setText( card.Get_SMBserver() );
	
	on_CH_smb_toggled( card.Use_SMB() );
	
	// Sndbuf
	ui.CH_sndbuf->setChecked( card.Use_Sndbuf() );
	ui.SB_sndbuf->setValue( card.Get_Sndbuf() );
	
	// VNet_hdr
	ui.CH_vnet_hdr->setChecked( card.Use_VNet_hdr() );
	ui.CB_vnet_hdr->setCurrentIndex( card.Get_VNet_hdr() ? 0 : 1 );
	
	// VHost
	ui.CH_vhost->setChecked( card.Use_VHost() );
	ui.CB_vhost->setCurrentIndex( card.Get_VHost() ? 0 : 1 );
	
	// VHostFd
	ui.CH_vhostfd->setChecked( card.Use_VHostFd() );
	ui.SB_vhostfd->setValue( card.Get_VHostFd() );
}

bool Network_Widget::Net_Card_is_Valid()
{
	bool u_macaddr, u_name, u_hostname, u_port_dev, u_ifname, u_script, u_downscript,
 		 u_listen, u_connect, u_mcast, u_sock, u_group, u_mode, u_file, u_len;
	
	u_macaddr = u_name = u_hostname = u_port_dev = u_ifname = u_script = u_downscript =
	u_listen = u_connect = u_mcast = u_sock = u_group = u_mode = u_file = u_len = false;
	
	switch( ui.CB_Network_Type->currentIndex() )
	{
		// -net nic[,vlan=n][,macaddr=addr][,model=type][,name=name]
		case 0:
			u_macaddr = u_name = true;
			break;
		
		// -net user[,vlan=n][,hostname=name][,name=name]
		case 1:
			u_hostname = u_name = true;
			break;
		
		// -net channel,port:dev
		case 2:
			u_port_dev = true;
			break;
		
		// -net tap[,vlan=n][,name=name][,fd=h][,ifname=name][,script=file][,downscript=dfile]
		case 3:
			u_name = u_ifname = u_script = u_downscript = true;
			break;
		
		// -net socket[,vlan=n][,name=name][,fd=h][,listen=[host]:port][,connect=host:port]
		case 4:
			u_name = u_listen = u_connect = true;
			break;
		
		// -net socket[,vlan=n][,name=name][,fd=h][,mcast=maddr:port]
		case 5:
			u_name = u_mcast = true;
			break;
		
		// -net vde[,vlan=n][,name=name][,sock=socketpath][,port=n][,group=groupname][,mode=octalmode]
		case 6:
			u_name = u_sock = u_group = u_mode = true;
			break;
		
		// -net dump[,vlan=n][,file=file][,len=len]
		case 7:
			u_file = u_len = true;
			break;
		
		default:
			break;
	}
	
	if( u_macaddr && ui.CH_macaddr->isChecked() )
	{
		QRegExp rx = QRegExp( "^[\\da-fA-F]{2}(\\:[\\da-fA-F]{2}){5}$" );
		
		if( ! rx.exactMatch(ui.Edit_macaddr->text()) )
		{
			AQGraphic_Warning( tr("Error!"), tr("Incorrect MAC address!") );
			return false;
		}
	}
	
	if( u_name && ui.CH_name->isChecked() )
	{
		QRegExp rx = QRegExp( "^[\\da-zA-Z_\\-]+$" );
		
		if( ! rx.exactMatch(ui.Edit_name->text()) )
		{
			AQGraphic_Warning( tr("Error!"), tr("Invalid name! Valid characters are:: a-z A-Z 0-9 _ -") );
			return false;
		}
	}
	
	if( u_hostname && ui.CH_hostname->isChecked() )
	{
		QRegExp rx = QRegExp( "^[\\da-zA-Z_\\-]+$" );
		
		if( ! rx.exactMatch(ui.Edit_hostname->text()) )
		{
			AQGraphic_Warning( tr("Error!"), tr("Invalid Hostname! Valid characters are:: a-z A-Z 0-9 _ -") );
			return false;
		}
	}
	
	// FIXME
	if( u_port_dev )
	{
	}
	
	if( u_ifname && ui.CH_ifname->isChecked() )
	{
	}
	
	if( u_script && ui.CH_script->isChecked() )
	{
		if( ! QFile::exists(ui.Edit_script->text()) )
		{
			if( No_File_Found("script", ui.Edit_script->text()) )
			{
				ui.CH_script->setChecked( false );
			}
			else return false;
		}
	}
	
	if( u_downscript && ui.CH_downscript->isChecked() )
	{
		if( ! QFile::exists(ui.Edit_downscript->text()) )
		{
			if( No_File_Found("downscript", ui.Edit_downscript->text()) )
			{
				ui.CH_downscript->setChecked( false );
			}
			else return false;
		}
	}
	
	if( u_listen && ui.CH_listen->isChecked() )
	{
	}
	
	if( u_connect && ui.CH_connect->isChecked() )
	{
	}
	
	if( u_mcast && ui.CH_mcast->isChecked() )
	{
	}
	
	if( u_sock && ui.CH_sock->isChecked() )
	{
	}
	
	if( u_group && ui.CH_group->isChecked() )
	{
	}
	
	if( u_mode && ui.CH_mode->isChecked() )
	{
	}
	
	if( u_file && ui.CH_file->isChecked() )
	{
	}
	
	return true;
}

bool Network_Widget::No_File_Found( const QString &name, const QString &path )
{
	int retVal = QMessageBox::critical( this, tr("Error!"),
										tr("%1 File \"%2\" doesn't Exist! Continue Without this File?").arg(name).arg(path),
										QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
	
	if( retVal == QMessageBox::No ) return false;
	else return true;
}

void Network_Widget::Update_Card_Item()
{
	if( ! ui.Items_List->currentItem() ) return;
	
	QString item_name = "";
	
	if( ui.Edit_name->text().isEmpty() == false &&
		ui.CH_name->isChecked() == true )
	{
		item_name = ui.Edit_name->text();
	}
	
	QString n = "(" + ui.Edit_name->text() + ")";
	
	switch( ui.CB_Network_Type->currentIndex() )
	{
		case 0:
			ui.Items_List->currentItem()->setText( "NIC " + (item_name.isEmpty() ?
												   Get_Items_Count(VM::Net_Mode_Nativ_NIC) : n) );
			break;
			
		case 1:
			ui.Items_List->currentItem()->setText( "User " + (item_name.isEmpty() ?
												   Get_Items_Count(VM::Net_Mode_Nativ_User) : n) );
			break;
			
		case 2:
			ui.Items_List->currentItem()->setText( "Channel " + Get_Items_Count(VM::Net_Mode_Nativ_Chanel) );
			break;
			
		case 3:
			ui.Items_List->currentItem()->setText( "TAP " + (item_name.isEmpty() ?
												   Get_Items_Count(VM::Net_Mode_Nativ_TAP) : n) );
			break;
			
		case 4:
			ui.Items_List->currentItem()->setText( "Socket " + (item_name.isEmpty() ?
												   Get_Items_Count(VM::Net_Mode_Nativ_Socket) : n) );
			break;
			
		case 5:
			ui.Items_List->currentItem()->setText( "Multicast Socket " + (item_name.isEmpty() ?
												   Get_Items_Count(VM::Net_Mode_Nativ_MulticastSocket) : n) );
			break;
			
		case 6:
			ui.Items_List->currentItem()->setText( "VDE " + (item_name.isEmpty() ?
												   Get_Items_Count(VM::Net_Mode_Nativ_VDE) : n) );
			break;
			
		case 7:
			ui.Items_List->currentItem()->setText( "Dump " + Get_Items_Count(VM::Net_Mode_Nativ_Dump) );
			break;
			
		default:
			ui.Items_List->currentItem()->setText( tr("Unknown Type Item") );
			break;
	}
}

QString Network_Widget::Get_Items_Count( VM::Network_Mode_Nativ type )
{
	int count = 0;
	
	for( int ix = 0; ui.Items_List->currentRow() >= ix; ix++ )
	{
		if( Network_Cards[ix].Get_Network_Type() == type ) count++;
	}
	
	return QString::number( count ? count : 1 );
}

void Network_Widget::Enable_Buttons( bool add, bool del )
{
	ui.Button_Add->setEnabled( add );
	
	ui.Button_Delete->setEnabled( del );
	ui.General_Widget->setEnabled( del );
	
	if( ! del ) // Clear GUI
	{
		VM_Net_Card_Nativ tmp_card;
		Set_Net_Card_To_Ui( tmp_card );
	}
}
