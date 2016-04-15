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

#include <QFileDialog>
#include <QMessageBox>
#include "Old_Network_Widget.h"
#include "Utils.h"

Old_Network_Widget::Old_Network_Widget( QWidget *parent ) : QWidget( parent )
{
	ui.setupUi( this );
	
	// Set Network Card Validators
	Check_Network_Card = true;
	
	// IP Address
	QRegExp rx( "^([1-9]|[1-9][\\d]|[1][\\d][\\d]|2[0-4][\\d]|25[0-5])(\\.([0-9]|[1-9][\\d]|[1][\\d][\\d]|2[0-4][\\d]|25[0-5])){3}$" );
	QValidator *validator = new QRegExpValidator( rx, this );
	ui.Edit_IP_Address->setValidator( validator );
	
	// MAC Address
	rx = QRegExp( "^[\\da-fA-F]{2}(\\:[\\da-fA-F]{2}){5}$" );
	validator = new QRegExpValidator( rx, this );
	ui.Edit_MAC_Address->setValidator( validator );
	
	// Connecting Slots
	Connect_Slots();
}

bool Old_Network_Widget::Get_Network_Cards( QList<VM_Net_Card> &cards )
{
	if( Network_Cards.count() < 1 )
	{
		AQDebug( "bool Old_Network_Widget::Get_Network_Cards( QList<VM_Net_Card> &cards )",
				 "No Items" );
		cards.clear();
		return true;
	}
	
	for( int nx = 0; nx < Network_Cards.count(); nx++ )
	{
		if( ! Net_Card_is_Valid(nx) )
		{
			AQGraphic_Warning( tr("Error"), tr("Network Card #% Not Valid!").arg(nx) );
			return false;
		}
	}
	
	cards = Network_Cards;
	
	return true;
}

void Old_Network_Widget::Set_Network_Cards( const QList<VM_Net_Card> &cards )
{
	Check_Network_Card = false;
	
	ui.Network_Cards_List->clear();
	Network_Cards = cards;
	
	if( Network_Cards.count() > 0 )
	{
		for( int nx = 0; nx < Network_Cards.count(); ++nx )
		{
			new QListWidgetItem( tr("Card ") + QString::number(nx+1), ui.Network_Cards_List );
		}
		
		Check_Network_Card = true;
		ui.Net_Card_Options->setEnabled( true );
		ui.Network_Cards_List->setCurrentRow( 0 );
	}
	else
	{
		ui.Net_Card_Options->setEnabled( false );
	}
}

void Old_Network_Widget::Set_Network_Card_Models( const QList<Device_Map> &models )
{
	if( ui.CB_Network_Card_Model->count() > 0 ) ui.CB_Network_Card_Model->clear();
	if( Card_Models_QEMU_Name.count() > 0 ) Card_Models_QEMU_Name.clear();
	
	for( int ix = 0; ix < models.count(); ix++ )
	{
		ui.CB_Network_Card_Model->addItem( models[ix].Caption );
		Card_Models_QEMU_Name.append( models[ix].QEMU_Name );
	}
}

void Old_Network_Widget::Set_Enabled( bool on )
{
	ui.Label_Cards_List->setEnabled( on );
	ui.Button_Add_Net_Card->setEnabled( on );
	ui.Button_Delete_Net_Card->setEnabled( on );
	ui.Net_Card_Options->setEnabled( on );
}

void Old_Network_Widget::Connect_Slots()
{
	connect( ui.CB_Connection_Mode, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CB_Network_Card_Model, SIGNAL(currentIndexChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_Hostname, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_IP_Address, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_MAC_Address, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_File_Descriptor, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_Port, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.SB_VLAN, SIGNAL(valueChanged(int)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.CH_TUN_TAP_Script, SIGNAL(clicked()),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_TUN_TAP_Script, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
	
	connect( ui.Edit_Interface_Name, SIGNAL(textChanged(const QString &)),
			 this, SIGNAL(Changet()) );
}

void Old_Network_Widget::Disconnect_Slots()
{
	disconnect( ui.CB_Connection_Mode, SIGNAL(currentIndexChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CB_Network_Card_Model, SIGNAL(currentIndexChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_Hostname, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_IP_Address, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_MAC_Address, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_File_Descriptor, SIGNAL(valueChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_Port, SIGNAL(valueChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.SB_VLAN, SIGNAL(valueChanged(int)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.CH_TUN_TAP_Script, SIGNAL(clicked()),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_TUN_TAP_Script, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
	
	disconnect( ui.Edit_Interface_Name, SIGNAL(textChanged(const QString &)),
				this, SIGNAL(Changet()) );
}

void Old_Network_Widget::on_Network_Cards_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous )
{
	if( Network_Cards.count() < 1 || Check_Network_Card == false ) return;
	
	int bx = ui.Network_Cards_List->row( current );
	if( bx < 0 )
	{
		AQError( "void Old_Network_Widget::on_Network_Cards_List_currentItemChanged( QListWidgetItem *current, QListWidgetItem *previous )",
				 "Current Netcard Index Out of Range!" );
		return;
	}
	else
	{
		Disconnect_Slots();
		Set_Net_Card_To_Ui( Network_Cards[bx] );
		Connect_Slots();
	}
}

void Old_Network_Widget::on_Button_Add_Net_Card_clicked()
{
	if( ui.Network_Cards_List->count() >= 8 )
		QMessageBox::information( this, tr("Warning!"), tr("Maximum Network Cards Count is 8.") );
	else
	{
		VM_Net_Card tmp_card;
		
		Network_Cards.append( tmp_card );
		
		new QListWidgetItem( tr("Card ") + QString::number(ui.Network_Cards_List->count()+1),
							 ui.Network_Cards_List );
		
		ui.Network_Cards_List->setCurrentRow( ui.Network_Cards_List->count()-1 );
		
		ui.Net_Card_Options->setEnabled( true );
		ui.Button_Delete_Net_Card->setEnabled( true );
		
		if( ui.Network_Cards_List->count() == 8 )
			ui.Button_Add_Net_Card->setEnabled( false );
		
		emit Changet();
	}
}

void Old_Network_Widget::on_Button_Delete_Net_Card_clicked()
{
	if( ui.Network_Cards_List->count() == 0 )
	{
		QMessageBox::information( this, tr("Warning!"),  tr("Network Cards List is Empty!") );
	}
	else if( ui.Network_Cards_List->currentRow() < ui.Network_Cards_List->count() &&
			 ui.Network_Cards_List->currentRow() >= 0 )
	{
		Network_Cards.removeAt( ui.Network_Cards_List->currentRow() );
		
		Check_Network_Card = false;
		ui.Network_Cards_List->clear();
		
		for( int nx = 0; nx < Network_Cards.count(); ++nx )
		{
			new QListWidgetItem( tr("Card ") + QString::number(nx+1), ui.Network_Cards_List );
		}
		
		ui.Button_Add_Net_Card->setEnabled( true );
		
		if( ui.Network_Cards_List->count() == 0 )
		{
			ui.Button_Delete_Net_Card->setEnabled( false );
			ui.Net_Card_Options->setEnabled( false );
		}
		else
		{
			ui.Network_Cards_List->setCurrentRow( ui.Network_Cards_List->count()-1 );
			Set_Net_Card_To_Ui( Network_Cards[Network_Cards.count()-1] );
		}
		
		Check_Network_Card = true;
		
		emit Changet();
	}
	else QMessageBox::information( this, tr("Warning!"), tr("Cannot Delete Network Card!") );
}

void Old_Network_Widget::on_CB_Network_Card_Model_currentIndexChanged( int index )
{
	if( index < 0 ||
		Network_Cards.count() <= 0 ||
		Card_Models_QEMU_Name.count() <= 0 ||
		ui.Network_Cards_List->currentRow() < 0 ||
		ui.CB_Network_Card_Model->count() <= 0 )
	{
		return;
	}
	
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Card_Model( Card_Models_QEMU_Name[index] );
}

void Old_Network_Widget::on_CB_Connection_Mode_currentIndexChanged( int index )
{
	if( index < 0 ||
		Network_Cards.count() <= 0 ||
		Card_Models_QEMU_Name.count() <= 0 ||
		ui.Network_Cards_List->currentRow() < 0 ||
		ui.CB_Network_Card_Model->count() <= 0 )
	{
		return;
	}
	
	switch( index )
	{
		case 0: // user mode
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Usermode );
			
			ui.Label_IP->setEnabled( false );
			ui.Edit_IP_Address->setEnabled( false );
			ui.Label_Host_Name->setEnabled( true );
			ui.Edit_Hostname->setEnabled( true );
			ui.Label_Port->setEnabled( false );
			ui.SB_Port->setEnabled( false );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( false );
			ui.SB_File_Descriptor->setEnabled( false );
			break;
			
		case 1: // open TUN/TAP
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Tuntap );
			
			ui.Label_IP->setEnabled( false );
			ui.Edit_IP_Address->setEnabled( false );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( false );
			ui.SB_Port->setEnabled( false );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( true );
			ui.Edit_TUN_TAP_Script->setEnabled( true );
			ui.TB_Browse_TUN_Script->setEnabled( true );
			ui.Label_If_Name->setEnabled( true );
			ui.Edit_Interface_Name->setEnabled( true );
			ui.Label_File_Descriptor->setEnabled( false );
			ui.SB_File_Descriptor->setEnabled( false );
			break;
			
		case 2: // already open TUN/TAP
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Tuntapfd );
			
			ui.Label_IP->setEnabled( false );
			ui.Edit_IP_Address->setEnabled( false );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( false );
			ui.SB_Port->setEnabled( false );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( true );
			ui.Edit_Interface_Name->setEnabled( true );
			ui.Label_File_Descriptor->setEnabled( true );
			ui.SB_File_Descriptor->setEnabled( true );
			break;
			
		case 3: // open listening TCP socket
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Tcplisten );
			
			ui.Label_IP->setEnabled( true );
			ui.Edit_IP_Address->setEnabled( true );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( true );
			ui.SB_Port->setEnabled( true );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( false );
			ui.SB_File_Descriptor->setEnabled( false );
			break;
			
		case 4: // already open TCP socket
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Tcpfd );
			
			ui.Label_IP->setEnabled( false );
			ui.Edit_IP_Address->setEnabled( false );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( false );
			ui.SB_Port->setEnabled( false );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( true );
			ui.SB_File_Descriptor->setEnabled( true );
			break;
			
		case 5: // connect to listening TCP socket
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Tcpconnect );
			
			ui.Label_IP->setEnabled( true );
			ui.Edit_IP_Address->setEnabled( true );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( true );
			ui.SB_Port->setEnabled( true );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( false );
			ui.SB_File_Descriptor->setEnabled( false );
			break;
			
		case 6: // create shared VLAN via UDP multicast socket
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Multicast );
			
			ui.Label_IP->setEnabled( true );
			ui.Edit_IP_Address->setEnabled( true );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( true );
			ui.SB_Port->setEnabled( true );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( false );
			ui.SB_File_Descriptor->setEnabled( false );
			break;
			
		case 7: // use already open UDP multicast socket
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_Multicastfd );
			
			ui.Label_IP->setEnabled( false );
			ui.Edit_IP_Address->setEnabled( false );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( false );
			ui.SB_Port->setEnabled( false );
			ui.Label_MAC->setEnabled( true );
			ui.Edit_MAC_Address->setEnabled( true );
			ui.TB_Generate_New_MAC->setEnabled( true );
			ui.Label_VLAN->setEnabled( true );
			ui.SB_VLAN->setEnabled( true );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( true );
			ui.SB_File_Descriptor->setEnabled( true );
			break;
			
		default: // no connection
			Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Net_Mode( VM::Net_Mode_None );
			
			ui.Label_IP->setEnabled( false );
			ui.Edit_IP_Address->setEnabled( false );
			ui.Label_Host_Name->setEnabled( false );
			ui.Edit_Hostname->setEnabled( false );
			ui.Label_Port->setEnabled( false );
			ui.SB_Port->setEnabled( false );
			ui.Label_MAC->setEnabled( false );
			ui.Edit_MAC_Address->setEnabled( false );
			ui.TB_Generate_New_MAC->setEnabled( false );
			ui.Label_VLAN->setEnabled( false );
			ui.SB_VLAN->setEnabled( false );
			ui.CH_TUN_TAP_Script->setEnabled( false );
			ui.Edit_TUN_TAP_Script->setEnabled( false );
			ui.TB_Browse_TUN_Script->setEnabled( false );
			ui.Label_If_Name->setEnabled( false );
			ui.Edit_Interface_Name->setEnabled( false );
			ui.Label_File_Descriptor->setEnabled( false );
			ui.SB_File_Descriptor->setEnabled( false );
			break;
	}
}

void Old_Network_Widget::on_Edit_Hostname_textChanged()
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Hostname( ui.Edit_Hostname->text() );
}

void Old_Network_Widget::on_Edit_IP_Address_textChanged()
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_IP_Address( ui.Edit_IP_Address->text() );
}

void Old_Network_Widget::on_Edit_MAC_Address_textChanged()
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_MAC_Address( ui.Edit_MAC_Address->text() );
}

void Old_Network_Widget::on_SB_Port_valueChanged( int i )
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Port( i );
}

void Old_Network_Widget::on_SB_VLAN_valueChanged( int i )
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_VLAN( i );
}

void Old_Network_Widget::on_CH_TUN_TAP_Script_stateChanged( int state )
{
	if( state == Qt::Checked ) Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Use_TUN_TAP_Script( true );
	else if( state == Qt::Unchecked ) Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Use_TUN_TAP_Script( false );
}

void Old_Network_Widget::on_Edit_TUN_TAP_Script_textChanged()
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_TUN_TAP_Script( ui.Edit_TUN_TAP_Script->text() );
}

void Old_Network_Widget::on_Edit_Interface_Name_textChanged()
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_Interface_Name( ui.Edit_Interface_Name->text() );
}

void Old_Network_Widget::on_SB_File_Descriptor_valueChanged( int i )
{
	Network_Cards[ ui.Network_Cards_List->currentRow() ].Set_File_Descriptor( i );
}

void Old_Network_Widget::on_TB_Generate_New_MAC_clicked()
{
	ui.Edit_MAC_Address->setText( Network_Cards[ ui.Network_Cards_List->currentRow() ].Generate_MAC() );
}

void Old_Network_Widget::on_TB_Browse_TUN_Script_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select script"),
													 Get_Last_Dir_Path(ui.Edit_TUN_TAP_Script->text()),
													 tr("All Files (*);;Scripts (*.sh)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_TUN_TAP_Script->setText( QDir::toNativeSeparators(fileName) );
}

void Old_Network_Widget::Set_Net_Card_To_Ui( const VM_Net_Card &card )
{
	// Card Model
	for( int ix = 0; ix < Card_Models_QEMU_Name.count(); ix++ )
	{
		if( card.Get_Card_Model() == Card_Models_QEMU_Name[ix] )
		{
			ui.CB_Network_Card_Model->setCurrentIndex( ix );
		}
	}
	
	// Connection Type
	switch( card.Get_Net_Mode() )
	{
		case VM::Net_Mode_Usermode:
			ui.CB_Connection_Mode->setCurrentIndex( 0 );
			break;
		
		case VM::Net_Mode_Tuntap:
			ui.CB_Connection_Mode->setCurrentIndex( 1 );
			break;
		
		case VM::Net_Mode_Tuntapfd:
			ui.CB_Connection_Mode->setCurrentIndex( 2 );
			break;
		
		case VM::Net_Mode_Tcplisten:
			ui.CB_Connection_Mode->setCurrentIndex( 3 );
			break;
		
		case VM::Net_Mode_Tcpfd:
			ui.CB_Connection_Mode->setCurrentIndex( 4 );
			break;
		
		case VM::Net_Mode_Tcpconnect:
			ui.CB_Connection_Mode->setCurrentIndex( 5 );
			break;
		
		case VM::Net_Mode_Multicast:
			ui.CB_Connection_Mode->setCurrentIndex( 6 );
			break;
		
		case VM::Net_Mode_Multicastfd:
			ui.CB_Connection_Mode->setCurrentIndex( 7 );
			break;
		
		case VM::Net_Mode_None:
			ui.CB_Connection_Mode->setCurrentIndex( 8 );
			break;
		
		default:
			AQError( "void Main_Window::Set_Net_Card_To_Ui( const VM_Net_Card &card )",
					 "Default Section!" );
			ui.CB_Connection_Mode->setCurrentIndex( 8 );
			break;
	}
	
	ui.Edit_Hostname->setText( card.Get_Hostname() );
	ui.Edit_IP_Address->setText( card.Get_IP_Address() );
	ui.Edit_MAC_Address->setText( card.Get_MAC_Address() );
	ui.SB_Port->setValue( card.Get_Port() );
	ui.SB_VLAN->setValue( card.Get_VLAN() );
	ui.CH_TUN_TAP_Script->setChecked( card.Get_Use_TUN_TAP_Script() );
	ui.Edit_TUN_TAP_Script->setText( card.Get_TUN_TAP_Script() );
	ui.Edit_Interface_Name->setText( card.Get_Interface_Name() );
	ui.SB_File_Descriptor->setValue( card.Get_File_Descriptor() );
}

bool Old_Network_Widget::Net_Card_is_Valid( int index )
{
	if( index < 0 || index >= Network_Cards.count() )
	{
		AQError( "bool Main_Window::Net_Card_is_Valid( int index )", "Index Out Of Range!" );
		return false;
	}
	
	// IP Address
	if( Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tcplisten ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tcpconnect ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Multicast )
	{
		QRegExp rx_ip( "^([1-9]|[1-9][\\d]|[1][\\d][\\d]|2[0-4][\\d]|25[0-5])(\\.([0-9]|[1-9][\\d]|[1][\\d][\\d]|2[0-4][\\d]|25[0-5])){3}$" );
		
		if( ! rx_ip.exactMatch(Network_Cards[index].Get_IP_Address()) )
		{
			if( ! Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tcplisten &&
				Network_Cards[index].Get_IP_Address().isEmpty() )
			{
				AQGraphic_Warning( tr("Error!"),
									tr("IP Address in Card ") + QString::number(index+1) + tr(" Invalid!") );
				return false;
			}
		}
	}
	
	// MAC Address
	if( Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Usermode ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tuntap ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tuntapfd ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tcplisten ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tcpfd ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tcpconnect ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Multicast ||
		Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Multicastfd )
	{
		QRegExp rx_mac = QRegExp( "^[\\da-fA-F]{2}(\\:[\\da-fA-F]{2}){5}$" );
		
		if( ! Network_Cards[index].Get_MAC_Address().isEmpty() &&
			! rx_mac.exactMatch(Network_Cards[index].Get_MAC_Address()) )
		{
			AQGraphic_Warning( tr("Error!"),
							   tr("MAC Address in Card ") + QString::number(index+1) + tr(" Invalid!") );
			return false;
		}
	}
	
	// TUN/TAP Script
	if( Network_Cards[index].Get_Net_Mode() == VM::Net_Mode_Tuntap )
	{
		if( Network_Cards[index].Get_Use_TUN_TAP_Script() == true &&
		    Network_Cards[index].Get_TUN_TAP_Script().isEmpty() == false &&
			Network_Cards[index].Get_TUN_TAP_Script() != "no" &&
			QFile::exists(Network_Cards[index].Get_TUN_TAP_Script()) == false )
		{
			int retVal = QMessageBox::critical( this, tr("Error!"), tr("In Card ") + QString::number(index+1) +
												tr(" TUN/TAP Script File \"%1\" Not Exists!\nContinue Without It File?").arg(Network_Cards[index].Get_TUN_TAP_Script()),
												QMessageBox::Yes | QMessageBox::No, QMessageBox::No );
			
			if( retVal == QMessageBox::No ) return false;
			else
			{
				Network_Cards[index].Set_Use_TUN_TAP_Script( false );
				
				if( index == ui.Network_Cards_List->currentRow() )
					ui.CH_TUN_TAP_Script->setChecked( false );
			}
		}
	}
	
	return true;
}
