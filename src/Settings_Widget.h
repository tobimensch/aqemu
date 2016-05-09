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

#ifndef SETTINGS_WIDGET_H
#define SETTINGS_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QWheelEvent>
#include <QList>
#include <QMap>

class QTabWidget;
class QStackedWidget;
class QBoxLayout;
class QSize;
class QSplitter;

class My_List_Widget : public QListWidget
{
    Q_OBJECT
    
    public:
        My_List_Widget(QWidget* parent);

    protected:
        virtual void wheelEvent(QWheelEvent* e);
};

class Settings_Widget : public QWidget
{
    Q_OBJECT

    public:
        Settings_Widget(QTabWidget*, QBoxLayout::Direction dir, bool erase_margins = false, bool erase_parent_margins = true);
        ~Settings_Widget();
        void setIconSize(QSize);
        void addToGroup(QString);
        static void syncGroupIconSizes(QString);

    private:
        My_List_Widget* list;
        QStackedWidget* stack;
        QSplitter* splitter;

        QString my_Group;
        static QMap<QString,QList<Settings_Widget*>> groups;
};

#endif


