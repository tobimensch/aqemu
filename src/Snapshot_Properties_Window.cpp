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
#include "Snapshot_Properties_Window.h"

Snapshot_Properties_Window::Snapshot_Properties_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

QString Snapshot_Properties_Window::Get_Snapshot_Name() const
{
	return ui.Edit_Name->text();
}

void Snapshot_Properties_Window::Set_Snapshot_Name( const QString &name )
{
	ui.Edit_Name->setText( name );
}

QString Snapshot_Properties_Window::Get_Snapshot_Description() const
{
	return ui.Edit_Description->toPlainText();
}

void Snapshot_Properties_Window::Set_Snapshot_Description( const QString &desc )
{
	ui.Edit_Description->setPlainText( desc );
}

void Snapshot_Properties_Window::on_Button_OK_clicked()
{
	if( ui.Edit_Name->text().isEmpty() )
	{
		AQGraphic_Warning( tr("Warning"), tr("Snapshot name is empty!") );
		return;
	}
	else accept();
}
