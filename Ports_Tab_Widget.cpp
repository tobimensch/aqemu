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

#include <QTextStream>
#include <QSettings>
#include <QFile>
#include <QProcess>
#include <QHeaderView>

#include "Utils.h"
#include "System_Info.h"
#include "Ports_Tab_Widget.h"
#include "Add_Port_Window.h"
#include "Add_USB_Port_Window.h"

Ports_Tab_Widget::Ports_Tab_Widget( QWidget *parent )
	: QWidget( parent )
{
	ui.setupUi( this );
	
	Index = 0;
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Ports_Table );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Ports_Table->setVerticalHeader( hv );
	
	hv = new QHeaderView( Qt::Horizontal, ui.Ports_Table );
	hv->setStretchLastSection( true );
	hv->setResizeMode( QHeaderView::ResizeToContents );
	ui.Ports_Table->setHorizontalHeader( hv );
}

QList<VM_Port> Ports_Tab_Widget::Get_Serial_Ports() const
{
	QList<VM_Port> tmp_list;
	
	for( QMapIterator<int, VM_Port> iter(Serial_Ports); iter.hasNext(); )
	{
		iter.next();
		tmp_list << iter.value();
	}
	
	return tmp_list;
}

void Ports_Tab_Widget::Set_Serial_Ports( const QList<VM_Port> &list )
{
	for( int ix = 0; ix < list.count(); ix++ )
	{
		Index++;
		Serial_Ports[ Index ] = list[ ix ];
		
		ui.Ports_Table->insertRow( ui.Ports_Table->rowCount() );
		
		QTableWidgetItem *newItem = new QTableWidgetItem( Get_Port_Info(Serial_Ports[Index]) );
		newItem->setData( Qt::UserRole, Index );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 1, newItem );
		
		newItem = new QTableWidgetItem( tr("COM (Serial Port)") );
		newItem->setData( Qt::UserRole, "COM" );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 0, newItem );
	}
}

QList<VM_Port> Ports_Tab_Widget::Get_Parallel_Ports() const
{
	QList<VM_Port> tmp_list;
	
	for( QMapIterator<int, VM_Port> iter(Parallel_Ports); iter.hasNext(); )
	{
		iter.next();
		tmp_list << iter.value();
	}
	
	return tmp_list;
}

void Ports_Tab_Widget::Set_Parallel_Ports( const QList<VM_Port> &list )
{
	for( int ix = 0; ix < list.count(); ix++ )
	{
		Index++;
		Parallel_Ports[ Index ] = list[ ix ];
		
		ui.Ports_Table->insertRow( ui.Ports_Table->rowCount() );
		
		QTableWidgetItem *newItem = new QTableWidgetItem( Get_Port_Info(Parallel_Ports[Index]) );
		newItem->setData( Qt::UserRole, Index );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 1, newItem );
		
		newItem = new QTableWidgetItem( tr("LPT (Parallel Port)") );
		newItem->setData( Qt::UserRole, "LPT" );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 0, newItem );
	}
}

QList<VM_USB> Ports_Tab_Widget::Get_USB_Ports() const
{
	QList<VM_USB> tmp_list;
	
	for( QMapIterator<int, VM_USB> iter(USB_Ports); iter.hasNext(); )
	{
		iter.next();
		tmp_list << iter.value();
	}
	
	return tmp_list;
}

void Ports_Tab_Widget::Set_USB_Ports( const QList<VM_USB> &list )
{
	for( int ix = 0; ix < list.count(); ix++ )
	{
		Index++;
		USB_Ports[ Index ] = list[ ix ];
		
		ui.Ports_Table->insertRow( ui.Ports_Table->rowCount() );
		
		QTableWidgetItem *newItem = new QTableWidgetItem( Get_USB_Port_Info(USB_Ports[Index]) );
		newItem->setData( Qt::UserRole, Index );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 1, newItem );
		
		newItem = new QTableWidgetItem( tr("USB Port") );
		newItem->setData( Qt::UserRole, "USB" );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 0, newItem );
	}
}

void Ports_Tab_Widget::Clear_Old_Ports()
{
	ui.Ports_Table->clearContents();
	for( int rx = 0; rx < ui.Ports_Table->rowCount(); ) ui.Ports_Table->removeRow( rx );
	
	Index = 0;
	
	Serial_Ports.clear();
	Parallel_Ports.clear();
	USB_Ports.clear();
}

QString Ports_Tab_Widget::Get_Port_Info( const VM_Port &port )
{
	return "(This function not complete)"; // FIXME
}

QString Ports_Tab_Widget::Get_USB_Port_Info( const VM_USB &port )
{
	if( port.Get_Use_Host_Device() == false )
	{
		QString devName = "";
		
		bool usb_k, usb_m, usb_t, usb_wt, usb_b;
		usb_k = usb_m = usb_t = usb_wt = usb_b = false;
		port.Get_USB_QEMU_Devices( usb_k, usb_m, usb_t, usb_wt, usb_b );
		
		if( usb_k ) devName = tr("Keyboard");
		else if( usb_m ) devName = tr("Mouse");
		else if( usb_t ) devName = tr("Tablet");
		else if( usb_wt ) devName = tr("Wacom Tablet");
		else if( usb_b ) devName = tr("Braille");
		else
		{
			AQError( "QString Ports_Tab_Widget::Get_USB_Port_Info( const VM_USB &port )",
					 "No QEMU USB Device!" );
		}
		
		return tr( "Virtual QEMU/KVM Device: %1" ).arg(devName);
	}
	else
	{
		if( port.Get_BusAddr().isEmpty() )
		{
			return tr("%1 %2 (Vendor ID: %3 Product ID: %4)").arg( port.Get_Manufacturer_Name() )
									  .arg( port.Get_Product_Name() )
									  .arg( port.Get_Vendor_ID() )
									  .arg( port.Get_Product_ID() );
		}
		else
		{
			return tr("%1 %2 (BusAddr: %3)").arg( port.Get_Manufacturer_Name() )
											.arg( port.Get_Product_Name() )
											.arg( port.Get_BusAddr() );
		}
	}
}

void Ports_Tab_Widget::on_TB_Add_Serial_Port_clicked()
{
	if( Serial_Ports.count() >= 4 )
	{
		AQGraphic_Warning( tr("Warning"), tr("You can add no more than 4 COM ports") );
	}
	else
	{
		Add_Port_Window *add_port_win = new Add_Port_Window( this );
		add_port_win->Set_Port( VM_Port() );
		
		if( add_port_win->exec() == QDialog::Accepted )
		{
			Index++;
			Serial_Ports[ Index ] = add_port_win->Get_Port();
			
			ui.Ports_Table->insertRow( ui.Ports_Table->rowCount() );
			
			QTableWidgetItem *newItem = new QTableWidgetItem( Get_Port_Info(add_port_win->Get_Port()) );
			newItem->setData( Qt::UserRole, Index );
			ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 1, newItem );
			
			newItem = new QTableWidgetItem( tr("COM (Serial Port)") );
			newItem->setData( Qt::UserRole, "COM" );
			ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 0, newItem );
			
			emit Settings_Changet();
		}
		
		delete add_port_win;
	}
}

void Ports_Tab_Widget::on_TB_Add_Parallel_Port_clicked()
{
	if( Parallel_Ports.count() >= 3 )
	{
		AQGraphic_Warning( tr("Warning"), tr("You can add no more than 3 LPT ports") );
	}
	else
	{
		Add_Port_Window *add_port_win = new Add_Port_Window( this );
		add_port_win->Set_Port( VM_Port() );
		
		if( add_port_win->exec() == QDialog::Accepted )
		{
			Index++;
			Parallel_Ports[ Index ] = add_port_win->Get_Port();
			
			ui.Ports_Table->insertRow( ui.Ports_Table->rowCount() );
			
			QTableWidgetItem *newItem = new QTableWidgetItem( Get_Port_Info(add_port_win->Get_Port()) );
			newItem->setData( Qt::UserRole, Index );
			ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 1, newItem );
			
			newItem = new QTableWidgetItem( tr("LPT (Parallel Port)") );
			newItem->setData( Qt::UserRole, "LPT" );
			ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 0, newItem );
			
			emit Settings_Changet();
		}
		
		delete add_port_win;
	}
}

void Ports_Tab_Widget::on_TB_Add_USB_Port_clicked()
{
	Add_USB_Port_Window *add_usb_win = new Add_USB_Port_Window( this );
	add_usb_win->Set_Port( VM_USB() );
	
	if( add_usb_win->exec() == QDialog::Accepted )
	{
		Index++;
		USB_Ports[ Index ] = add_usb_win->Get_Port();
		
		ui.Ports_Table->insertRow( ui.Ports_Table->rowCount() );
		
		QTableWidgetItem *newItem = new QTableWidgetItem( Get_USB_Port_Info(add_usb_win->Get_Port()) );
		newItem->setData( Qt::UserRole, Index );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 1, newItem );
		
		newItem = new QTableWidgetItem( tr("USB Port") );
		newItem->setData( Qt::UserRole, "USB" );
		ui.Ports_Table->setItem( ui.Ports_Table->rowCount()-1, 0, newItem );
		
		emit Settings_Changet();
	}
	
	delete add_usb_win;
}

void Ports_Tab_Widget::on_TB_Edit_Port_clicked()
{
	if( ui.Ports_Table->currentRow() < 0 )
	{
		AQGraphic_Warning( tr("Warning"), tr("Device not select!") );
		return;
	}
	else
	{
		// Port Type
		QString type = ui.Ports_Table->item( ui.Ports_Table->currentRow(), 0 )->data( Qt::UserRole ).toString();
		int ix = ui.Ports_Table->item( ui.Ports_Table->currentRow(), 1 )->data( Qt::UserRole ).toInt();
		
		if( type == "COM" )
		{
			Add_Port_Window *add_port_win = new Add_Port_Window( this );
			add_port_win->Set_Port( Serial_Ports[ix] );
			
			if( add_port_win->exec() == QDialog::Accepted )
			{
				Serial_Ports[ ix ] = add_port_win->Get_Port();
				emit Settings_Changet();
			}
			
			delete add_port_win;
		}
		else if( type == "LPT" )
		{
			Add_Port_Window *add_port_win = new Add_Port_Window( this );
			add_port_win->Set_Port( Parallel_Ports[ix] );
			
			if( add_port_win->exec() == QDialog::Accepted )
			{
				Parallel_Ports[ ix ] = add_port_win->Get_Port();
				emit Settings_Changet();
			}
			
			delete add_port_win;
		}
		else if( type == "USB" )
		{
			Add_USB_Port_Window *add_usb_win = new Add_USB_Port_Window( this );
			add_usb_win->Set_Port( USB_Ports[ix] );
			
			if( add_usb_win->exec() == QDialog::Accepted )
			{
				USB_Ports[ ix ] = add_usb_win->Get_Port();
				emit Settings_Changet();
			}
			
			delete add_usb_win;
		}
		else
		{
			AQError( "void Ports_Tab_Widget::on_TB_Edit_Port_clicked()",
					 "Invalid port type!" );
			return;
		}
	}
}

void Ports_Tab_Widget::on_TB_Delete_Port_clicked()
{
	if( ui.Ports_Table->currentRow() < 0 )
	{
		AQGraphic_Warning( tr("Warning"), tr("Device doesn't selected!") );
		return;
	}
	else
	{
		QString type = ui.Ports_Table->item( ui.Ports_Table->currentRow(), 0 )->data( Qt::UserRole ).toString();
		int ix = ui.Ports_Table->item( ui.Ports_Table->currentRow(), 1 )->data( Qt::UserRole ).toInt();
		
		if( type == "COM" ) Serial_Ports.remove( ix );
		else if( type == "LPT" ) Parallel_Ports.remove( ix );
		else if( type == "USB" ) USB_Ports.remove( ix );
		else
		{
			AQError( "void Ports_Tab_Widget::on_TB_Delete_Port_clicked()",
					 "Invalid port type!" );
			return;
		}
		
		ui.Ports_Table->removeRow( ui.Ports_Table->currentRow() );
		
		emit Settings_Changet();
	}
}

void Ports_Tab_Widget::on_Ports_Table_itemDoubleClicked( QTableWidgetItem *item )
{
	on_TB_Edit_Port_clicked();
}
