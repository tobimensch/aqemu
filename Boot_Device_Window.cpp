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

#include <QCheckBox>
#include "Boot_Device_Window.h"
#include "Utils.h"

Boot_Device_Window::Boot_Device_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

QList<VM::Boot_Order> Boot_Device_Window::data() const
{
	QList<VM::Boot_Order> list;
	
	for( int ix = 0; ix < ui.Boot_Devices_List->topLevelItemCount(); ix++ )
	{
		QTreeWidgetItem *item = ui.Boot_Devices_List->topLevelItem( ix );
		
		if( item != NULL )
		{
			VM::Boot_Order tmpDev;
			
			if( item->checkState(0) == Qt::Checked ) tmpDev.Enabled = true;
			else tmpDev.Enabled = false;
			
			QString typeStr = item->data( 0, Qt::UserRole ).toString();
			
			if( typeStr == "FDA" ) tmpDev.Type = VM::Boot_From_FDA;
			else if( typeStr == "FDB" ) tmpDev.Type = VM::Boot_From_FDB;
			else if( typeStr == "CDROM" ) tmpDev.Type = VM::Boot_From_CDROM;
			else if( typeStr == "HDD" ) tmpDev.Type = VM::Boot_From_HDD;
			else if( typeStr == "Network1" ) tmpDev.Type = VM::Boot_From_Network1;
			else if( typeStr == "Network2" ) tmpDev.Type = VM::Boot_From_Network2;
			else if( typeStr == "Network3" ) tmpDev.Type = VM::Boot_From_Network3;
			else if( typeStr == "Network4" ) tmpDev.Type = VM::Boot_From_Network4;
			else
			{
				AQError( "QList<VM::Boot_Order> Boot_Device_Window::data() const",
						 "Incorrent boot device type \"" + typeStr + "\"!" );
			}
			
			list << tmpDev;
		}
	}
	
	return list;
}

void Boot_Device_Window::setData( const QList<VM::Boot_Order> &list )
{
	ui.Boot_Devices_List->clear();
	ui.Boot_Devices_List->resizeColumnToContents( 0 );
	
	for( int ix = 0; ix < list.count(); ix++ )
	{
		QTreeWidgetItem *item = new QTreeWidgetItem( ui.Boot_Devices_List );
		
		if( list[ix].Enabled ) item->setCheckState( 0, Qt::Checked );
		else item->setCheckState( 0, Qt::Unchecked );
		
		switch( list[ix].Type )
		{
			case VM::Boot_From_FDA:
				item->setText( 0, tr("Floppy A") );
				item->setData( 0, Qt::UserRole, "FDA" );
				break;
				
			case VM::Boot_From_FDB:
				item->setText( 0, tr("Floppy B") );
				item->setData( 0, Qt::UserRole, "FDB" );
				break;
				
			case VM::Boot_From_CDROM:
				item->setText( 0, tr("CD-ROM") );
				item->setData( 0, Qt::UserRole, "CDROM" );
				break;
				
			case VM::Boot_From_HDD:
				item->setText( 0, tr("Hard Drive") );
				item->setData( 0, Qt::UserRole, "HDD" );
				break;
				
			case VM::Boot_From_Network1:
				item->setText( 0, tr("Network 1") );
				item->setData( 0, Qt::UserRole, "Network1" );
				break;
				
			case VM::Boot_From_Network2:
				item->setText( 0, tr("Network 2") );
				item->setData( 0, Qt::UserRole, "Network2" );
				break;
				
			case VM::Boot_From_Network3:
				item->setText( 0, tr("Network 3") );
				item->setData( 0, Qt::UserRole, "Network3" );
				break;
				
			case VM::Boot_From_Network4:
				item->setText( 0, tr("Network 4") );
				item->setData( 0, Qt::UserRole, "Network4" );
				break;
				
			default:
				AQWarning( "void Boot_Device_Window::setData( QList<VM::Boot_Order> &list )",
						   "Incorrect boot device type!" );
				break;
		}
	}
}

bool Boot_Device_Window::useBootMenu() const
{
	return ui.CH_Boot_Menu->isChecked();
}

void Boot_Device_Window::setUseBootMenu( bool use )
{
	ui.CH_Boot_Menu->setChecked( use );
}

void Boot_Device_Window::on_TB_Up_clicked()
{
	int index = Get_Current_Index();
	if( index < 1 ||
		index > ui.Boot_Devices_List->topLevelItemCount() ) return;
	
	QTreeWidgetItem *item = ui.Boot_Devices_List->takeTopLevelItem( index );
	ui.Boot_Devices_List->insertTopLevelItem( index - 1, item );
	
	ui.Boot_Devices_List->setCurrentItem( item );
}

void Boot_Device_Window::on_TB_Down_clicked()
{
	int index = Get_Current_Index();
	if( index < 0 ||
		index > ui.Boot_Devices_List->topLevelItemCount()-2 ) return;
	
	QTreeWidgetItem *item = ui.Boot_Devices_List->takeTopLevelItem( index );
	ui.Boot_Devices_List->insertTopLevelItem( index + 1, item );
	
	ui.Boot_Devices_List->setCurrentItem( item );
}

int Boot_Device_Window::Get_Current_Index()
{
	QTreeWidgetItem *currentItem = ui.Boot_Devices_List->currentItem();
	if( ! currentItem ) return -1;
	
	for( int ix = 0; ix < ui.Boot_Devices_List->topLevelItemCount(); ++ix )
	{
		if( ui.Boot_Devices_List->topLevelItem(ix) == currentItem ) return ix;
	}
	
	return -1; // not finded
}
