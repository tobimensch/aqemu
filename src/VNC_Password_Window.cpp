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

#include "Utils.h"
#include "VNC_Password_Window.h"

VNC_Password_Window::VNC_Password_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

QString VNC_Password_Window::Get_Password() const
{
	return ui.Edit_Password->text();
}

void VNC_Password_Window::on_Button_OK_clicked()
{
	if( ui.Edit_Password->text().isEmpty() )
	{
		AQGraphic_Warning( tr("Warning"), tr("Password is Empty!") );
		return;
	}
	
	if( ui.Edit_Password->text() != ui.Edit_Confirm_Password->text() )
	{
		AQGraphic_Warning( tr("Warning"), tr("Password Not Confirmed!") );
		return;
	}
	
	accept();
}
