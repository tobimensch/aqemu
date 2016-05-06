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

#ifndef DIALOG_BUTTON_BOX_H
#define DIALOG_BUTTON_BOX_H

#include <QDialogButtonBox>
#include <QListWidget>
#include <QWheelEvent>
#include <QList>
#include <QMap>

class QTabWidget;
class QStackedWidget;
class QBoxLayout;
class QSize;
class QSplitter;
class QPaintEvent;

class Dialog_Button_Box : public QDialogButtonBox
{
    Q_OBJECT

    public:
        Dialog_Button_Box(QWidget*);
        Dialog_Button_Box();
        ~Dialog_Button_Box();

    private:
        void init();
        void showEvent(QShowEvent*);
};

#endif


