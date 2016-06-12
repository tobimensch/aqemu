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

#ifndef NO_BOOT_DEVICE_H
#define NO_BOOT_DEVICE_H

#include "VM.h"
#include "ui_No_Boot_Device.h"

class No_Boot_Device: public QDialog
{

	Q_OBJECT
	
	public:
		No_Boot_Device( QWidget *parent = 0 );
		~No_Boot_Device();
		void Set_VM( const Virtual_Machine &vm );

    private slots:
        void Change_Boot_Order();
        void Device_Manager();
        void Special_Image();
        void Kernel_Boot();
        void Computer_Port();

    private:
		Ui::No_Boot_Device ui;
};

#endif

