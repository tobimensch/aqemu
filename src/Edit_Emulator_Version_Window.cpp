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

#include <QSettings>
#include "Edit_Emulator_Version_Window.h"
#include "Utils.h"

Edit_Emulator_Version_Window::Edit_Emulator_Version_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	Emulators = Get_Emulators_List();
	Load_Emulators();
}

void Edit_Emulator_Version_Window::Load_Emulators()
{
	ui.List_Emulators->clear();
	
	for( int ix = 0; ix < Emulators.count(); ++ix )
	{
		if( Emulators[ix].Get_Type() == VM::QEMU )
		{
			new QListWidgetItem( tr("%1 (Located in %2)").arg(Emulator_Version_To_String(Emulators[ix].Get_Version())).arg(Emulators[ix].Get_Path()),
								 ui.List_Emulators );
		}
		else if( Emulators[ix].Get_Type() == VM::KVM )
		{
			new QListWidgetItem( tr("%1 (Located in %2)").arg(Emulator_Version_To_String(Emulators[ix].Get_Version())).arg(Emulators[ix].Get_Path()),
								 ui.List_Emulators );
		}
	}
}

void Edit_Emulator_Version_Window::on_Button_OK_clicked()
{
	// FIXME Optimize save only changet
	
	// Delete old emulators
	Remove_All_Emulators_Files();
	
	// Save New Emulators
	for( int ix = 0; ix < Emulators.count(); ++ix )
		Emulators[ ix ].Save();
	
	accept();
}

void Edit_Emulator_Version_Window::on_List_Emulators_currentRowChanged( int currentRow )
{
	disconnect( ui.CB_Versions, SIGNAL(currentIndexChanged(const QString &)),
				this, SLOT(CB_Versions_currentIndexChanged(const QString &)) );
	
	if( currentRow >= 0 &&
		currentRow < ui.List_Emulators->count() )
	{
		int item_index = -1;
		
		// Add Items
		if( Emulators[currentRow].Get_Type() == VM::QEMU )
		{
			ui.CB_Versions->clear();
			
			ui.CB_Versions->addItem( tr("QEMU 0.9.0") );
			ui.CB_Versions->addItem( tr("QEMU 0.9.1") );
			ui.CB_Versions->addItem( tr("QEMU 0.10.X") );
			ui.CB_Versions->addItem( tr("QEMU 0.11.X") );
			ui.CB_Versions->addItem( tr("QEMU 0.12.X") );
			ui.CB_Versions->addItem( tr("QEMU 0.13.X") );
		}
		else if( Emulators[currentRow].Get_Type() == VM::KVM )
		{
			ui.CB_Versions->clear();
			
			ui.CB_Versions->addItem( tr("KVM 7X") );
			ui.CB_Versions->addItem( tr("KVM 8X") );
			ui.CB_Versions->addItem( tr("KVM 0.11.X") );
			ui.CB_Versions->addItem( tr("KVM 0.12.X") );
			ui.CB_Versions->addItem( tr("KVM 0.13.X") );
		}
		
		item_index = ui.CB_Versions->findText( Emulator_Version_To_String(Emulators[currentRow].Get_Version()) );

		// Select Version
		if( item_index < 0 )
			AQError( "void Edit_Emulator_Version_Window::on_List_Emulators_currentRowChanged( int currentRow )",
					 "Cannot Find Version: " + Emulator_Version_To_String(Emulators[currentRow].Get_Version()) );
		else
			ui.CB_Versions->setCurrentIndex( item_index );
	}
	
	connect( ui.CB_Versions, SIGNAL(currentIndexChanged(const QString &)),
			 this, SLOT(CB_Versions_currentIndexChanged(const QString &)) );
}

void Edit_Emulator_Version_Window::CB_Versions_currentIndexChanged( const QString &text )
{
	if( ui.List_Emulators->currentRow() < 0 ) return;
	
	Emulators[ ui.List_Emulators->currentRow() ].Set_Name( text );
	Emulators[ ui.List_Emulators->currentRow() ].Set_Version( String_To_Emulator_Version(text) );
	
	Load_Emulators();
}
