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

#include <QPushButton>

#include "Dialog_Button_Box.h"

Dialog_Button_Box::Dialog_Button_Box(QWidget* parent) : QDialogButtonBox(parent)
{
}

Dialog_Button_Box::Dialog_Button_Box() : QDialogButtonBox()
{
}

void Dialog_Button_Box::init()
{
    if ( button(QDialogButtonBox::Ok) )
        button(QDialogButtonBox::Ok)->setIcon(QIcon(":/ok.png"));
    if ( button(QDialogButtonBox::Cancel) )
        button(QDialogButtonBox::Cancel)->setIcon(QIcon(":/cancel.png"));
    if ( button(QDialogButtonBox::Save) )
        button(QDialogButtonBox::Save)->setIcon(QIcon(":/save.png"));
}

Dialog_Button_Box::~Dialog_Button_Box()
{
}

void Dialog_Button_Box::showEvent(QShowEvent* e)
{
    init();

    e->accept();
}

