/****************************************************************************
**
** Copyright (C) 2016 Tobias Gläßer
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

#ifndef INSPECT_QEMU_ARGUMENTS_H
#define INSPECT_QEMU_ARGUMENTS_H

#include "VM.h"
#include "ui_Inspect_QEMU_Arguments.h"

class Inspect_QEMU_Arguments: public QDialog
{

	Q_OBJECT
	
	public:
		Inspect_QEMU_Arguments( Virtual_Machine* vm, QWidget *parent = 0 );
		~Inspect_QEMU_Arguments();

    private slots:
    private:
		Ui::Inspect_QEMU_Arguments ui;
};

#endif

