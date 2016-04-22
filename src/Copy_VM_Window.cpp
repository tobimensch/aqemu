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

#include "Utils.h"
#include "Copy_VM_Window.h"

Copy_VM_Window::Copy_VM_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	All_Machine_Names.clear();
}

QString Copy_VM_Window::Get_New_VM_Name() const
{
	return ui.Edit_New_VM_Name->text();
}

bool Copy_VM_Window::Get_Copy_Disk_Images() const
{
	return ui.CH_Copy_Disk_Images->isChecked();
}

bool Copy_VM_Window::Get_Copy_Floppy() const
{
	return ui.CH_Copy_Floppy->isChecked();
}

bool Copy_VM_Window::Get_Copy_Hard_Drive() const
{
	return ui.CH_Copy_Hard_Drive->isChecked();
}

void Copy_VM_Window::Add_VM_Machine_Name( const QString &name )
{
	All_Machine_Names << name;
}

void Copy_VM_Window::on_Button_OK_clicked()
{
	for( int ix = 0; ix < All_Machine_Names.count(); ++ix )
	{
		if( All_Machine_Names[ix] == ui.Edit_New_VM_Name->text() )
		{
			AQGraphic_Warning( tr("Error!"),
							   tr("This VM Name is Already Used!") );
			return;
		}
	}
	
	// OK, New Name Unuque
	accept();
}
