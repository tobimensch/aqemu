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
#include "No_Boot_Device.h"
#include "Add_New_Device_Window.h"
#include "Utils.h"
#include "Create_HDD_Image_Window.h"
#include "System_Info.h"
#include <iostream>

No_Boot_Device::No_Boot_Device( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );

    connect(ui.boot_order_button,SIGNAL(clicked()),this,SLOT(Change_Boot_Order()));
    connect(ui.device_manager_button,SIGNAL(clicked()),this,SLOT(Device_Manager()));
    connect(ui.special_image_boot_button,SIGNAL(clicked()),this,SLOT(Special_Image()));
    connect(ui.computer_port_button,SIGNAL(clicked()),this,SLOT(Computer_Port()));
    connect(ui.kernel_boot_button,SIGNAL(clicked()),this,SLOT(Kernel_Boot()));

    auto mw = static_cast<Main_Window*>(parent);
    ui.boot_order_button->setDescription(tr("Current order:")+" "+mw->ui.CB_Boot_Priority->currentText());
}

No_Boot_Device::~No_Boot_Device()
{
}

void No_Boot_Device::Set_VM( const Virtual_Machine &vm )
{
}

void No_Boot_Device::Change_Boot_Order()
{
    auto mw = static_cast<Main_Window*>(parent());
    mw->ui.Tabs->setCurrentIndex(1);

    QTimer::singleShot(50, mw, SLOT(on_TB_Show_Boot_Settings_Window_clicked()));

    accept();
}

void No_Boot_Device::Device_Manager()
{
    auto mw = static_cast<Main_Window*>(parent());
    mw->ui.Tabs->setCurrentIndex(2);
    mw->Media_Settings_Widget->setCurrentIndex(0);
    accept();
}


void No_Boot_Device::Special_Image()
{
    auto mw = static_cast<Main_Window*>(parent());
    mw->ui.Tabs->setCurrentIndex(2);
    mw->Media_Settings_Widget->setCurrentIndex(4);
    accept();
}


void No_Boot_Device::Computer_Port()
{
    auto mw = static_cast<Main_Window*>(parent());
    mw->ui.Tabs->setCurrentIndex(2);
    mw->Media_Settings_Widget->setCurrentIndex(2);
    accept();
}


void No_Boot_Device::Kernel_Boot()
{
    auto mw = static_cast<Main_Window*>(parent());
    mw->ui.Tabs->setCurrentIndex(2);
    mw->Media_Settings_Widget->setCurrentIndex(3);
    accept();
}

