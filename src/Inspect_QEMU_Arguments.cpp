/****************************************************************************
**
** Copyirght (C) 2016 Tobias Gläßer
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

#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QTimer>

#include "Main_Window.h"
#include "Inspect_QEMU_Arguments.h"
#include "Add_New_Device_Window.h"
#include "Utils.h"
#include "Create_HDD_Image_Window.h"
#include "System_Info.h"
#include <iostream>

Inspect_QEMU_Arguments::Inspect_QEMU_Arguments( Virtual_Machine* vm, QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );

	QString line = "";
	
    vm->Get_QEMU_Args();
}

Inspect_QEMU_Arguments::~Inspect_QEMU_Arguments()
{
}

