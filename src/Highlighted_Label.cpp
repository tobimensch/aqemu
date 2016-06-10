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

#include <QPalette>
#include <Utils.h>

#include "Highlighted_Label.h"

Highlighted_Label::Highlighted_Label(QWidget* parent) : QLabel(parent)
{
    QPalette pal = palette();
    QColor background_color = pal.color(QPalette::Window);
    QColor link_color = pal.color(QPalette::Link);

    if ( calculateContrast(background_color,link_color) > 3.0 )
    {
        setStyleSheet(R"(
        Highlighted_Label
            {
                font-size: medium;
                font-weight: 600;
                color: palette(link);
            }
        )");
    }
    else
    {
        setStyleSheet(R"(
        Highlighted_Label
            {
                font-size: medium;
                font-weight: 600;
            }
        )");
    }
}

Highlighted_Label::Highlighted_Label()
{
}

