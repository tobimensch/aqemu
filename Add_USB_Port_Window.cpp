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

#include <QHeaderView>
#include "Add_USB_Port_Window.h"
#include "System_Info.h"

Add_USB_Port_Window::Add_USB_Port_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	QHeaderView *hv = new QHeaderView( Qt::Vertical, ui.Table_Host_USB );
	hv->setResizeMode( QHeaderView::Fixed );
	ui.Table_Host_USB->setVerticalHeader( hv );
	
	/*hv = new QHeaderView( Qt::Horizontal, ui.Table_Host_USB );
	hv->setResizeMode( QHeaderView::Interactive );
	ui.Table_Host_USB->setHorizontalHeader( hv );*/
	
	on_Button_Update_Host_USB_clicked();
}

VM_USB Add_USB_Port_Window::Get_Port() const
{
	if( ui.RB_QEMU_USB->isChecked() ) Current_Item.Set_Use_Host_Device( false );
	else Current_Item.Set_Use_Host_Device( true );
	
	bool usb_k, usb_m, usb_t, usb_wt, usb_b;
	usb_k = usb_m = usb_t = usb_wt = usb_b = false;
	
	if( ui.RB_QEMU_Keyboard->isChecked() ) usb_k = true;
	else if( ui.RB_QEMU_Mouse->isChecked() ) usb_m = true;
	else if( ui.RB_QEMU_Tablet->isChecked() ) usb_t = true;
	else if( ui.RB_QEMU_WacomTablet->isChecked() ) usb_wt = true;
	else if( ui.RB_QEMU_Braille->isChecked() ) usb_b = true;
	
	Current_Item.Set_USB_QEMU_Devices( usb_k, usb_m, usb_t, usb_wt, usb_b );
	
	return Current_Item;
}

void Add_USB_Port_Window::Set_Port( const VM_USB &port )
{
	Current_Item = port;
	
	if( port.Get_Use_Host_Device() == false ) on_RB_QEMU_USB_clicked( true );
	else on_RB_Host_USB_clicked( true );
	
	bool usb_k, usb_m, usb_t, usb_wt, usb_b;
	usb_k = usb_m = usb_t = usb_wt = usb_b = false;
	port.Get_USB_QEMU_Devices( usb_k, usb_m, usb_t, usb_wt, usb_b );
	
	if( usb_k ) ui.RB_QEMU_Keyboard->setChecked( true );
	else if( usb_m ) ui.RB_QEMU_Mouse->setChecked( true );
	else if( usb_t ) ui.RB_QEMU_Tablet->setChecked( true );
	else if( usb_wt ) ui.RB_QEMU_WacomTablet->setChecked( true );
	else if( usb_b ) ui.RB_QEMU_Braille->setChecked( true );
	else
	{
		AQError( "void Add_USB_Port_Window::Set_Port( const VM_USB &port )",
				 "No QEMU USB Device!" );
	}
	
	ui.S_Manufacturer->setText( tr("Manufacturer: ") + port.Get_Manufacturer_Name() );
	ui.S_Product->setText( tr("Product: ") + port.Get_Product_Name() );
	ui.S_Speed->setText( tr("Speed: ") + port.Get_Speed() );
	ui.S_BusAddr->setText( tr("bus.addr: ") + port.Get_BusAddr() );
	ui.S_Vendor_ID->setText( tr("Vendor ID: ") + port.Get_Vendor_ID() );
	ui.S_Product_ID->setText( tr("Product ID: ") + port.Get_Product_ID() );
}

void Add_USB_Port_Window::on_Button_Update_Host_USB_clicked()
{
	// Update USB Info
	System_Info::Update_Host_USB();
	
	// Update User Interface
	ui.Table_Host_USB->clearContents();
	for( int rx = 0; rx < ui.Table_Host_USB->rowCount(); ) ui.Table_Host_USB->removeRow( rx );
	
	if( ui.RB_Show_All->isChecked() )
	{
		USB_Host_List = System_Info::Get_All_Host_USB();
	}
	else
	{
		USB_Host_List = System_Info::Get_All_Host_USB();
		QList<VM_USB> used_USB_List = System_Info::Get_Used_USB_List();
		
		for( int ix = 0; ix < used_USB_List.count(); ix++ )
		{
			USB_Host_List.removeOne( used_USB_List[ix] );
		}
	}
	
	if( USB_Host_List.count() < 1 )
	{
		AQWarning( "void Add_USB_Port_Window::on_Button_Update_Host_USB_clicked()",
				   "No Devices Found!" );
		return;
	}
	
	// Add new items
	for( int ux = 0; ux < USB_Host_List.count(); ++ux )
	{
		ui.Table_Host_USB->insertRow( ui.Table_Host_USB->rowCount() );
		QTableWidgetItem *newItem;
		
		// Product
		if( USB_Host_List[ux].Get_Product_Name().isEmpty() )
			newItem = new QTableWidgetItem( USB_Host_List[ux].Get_Product_ID() );
		else
			newItem = new QTableWidgetItem( USB_Host_List[ux].Get_Product_Name() );
		
		ui.Table_Host_USB->setItem( ui.Table_Host_USB->rowCount()-1, 1, newItem );
		
		// busAddr
		newItem = new QTableWidgetItem( USB_Host_List[ux].Get_BusAddr() );
		ui.Table_Host_USB->setItem( ui.Table_Host_USB->rowCount()-1, 2, newItem );
		
		// Manufacturer
		if( USB_Host_List[ux].Get_Manufacturer_Name().isEmpty() )
			newItem = new QTableWidgetItem( USB_Host_List[ux].Get_Vendor_ID() );
		else
			newItem = new QTableWidgetItem( USB_Host_List[ux].Get_Manufacturer_Name() );
		
		ui.Table_Host_USB->setItem( ui.Table_Host_USB->rowCount()-1, 0, newItem );
	}
	
	ui.Table_Host_USB->resizeColumnsToContents();
}

void Add_USB_Port_Window::on_RB_QEMU_USB_clicked( bool checked )
{
	ui.Widget_QEMU_Devices->setEnabled( true );
	ui.Widget_Host_Devices->setEnabled( false );
	
	ui.RB_QEMU_USB->setCheckable( true );
	
	ui.RB_QEMU_USB->setChecked( true );
	ui.RB_Host_USB->setChecked( false );
	
	ui.RB_Host_USB->setCheckable( false );
}

void Add_USB_Port_Window::on_RB_Host_USB_clicked( bool checked )
{
	ui.Widget_QEMU_Devices->setEnabled( false );
	ui.Widget_Host_Devices->setEnabled( true );
	
	ui.RB_Host_USB->setCheckable( true );
	
	ui.RB_Host_USB->setChecked( true );
	ui.RB_QEMU_USB->setChecked( false );
	
	ui.RB_QEMU_USB->setCheckable( false );
}

void Add_USB_Port_Window::on_Table_Host_USB_currentItemChanged( QTableWidgetItem *current, QTableWidgetItem *previous )
{
	if( ui.Table_Host_USB->currentRow() >= 0 &&
		ui.Table_Host_USB->currentRow() < USB_Host_List.count() )
	{
		Current_Item = USB_Host_List[ ui.Table_Host_USB->currentRow() ];
		
		ui.S_Manufacturer->setText( tr("Manufacturer: ") + Current_Item.Get_Manufacturer_Name() );
		ui.S_Product->setText( tr("Product: ") + Current_Item.Get_Product_Name() );
		ui.S_Speed->setText( tr("Speed: ") + Current_Item.Get_Speed() );
		ui.S_BusAddr->setText( tr("Bus.Address: ") + Current_Item.Get_BusAddr() );
		ui.S_Vendor_ID->setText( tr("Vendor ID: ") + Current_Item.Get_Vendor_ID() );
		ui.S_Product_ID->setText( tr("Product ID: ") + Current_Item.Get_Product_ID() );
	}
}

void Add_USB_Port_Window::on_RB_Show_All_toggled( bool on )
{
	on_Button_Update_Host_USB_clicked();
}
