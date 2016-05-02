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

#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QBoxLayout>
#include <QApplication>
#include <QSplitter>

#include "Settings_Widget.h"

#include <iostream>

My_List_Widget::My_List_Widget(QWidget* parent) : QListWidget(parent)
{
}

void My_List_Widget::wheelEvent(QWheelEvent* e)
{
    if ( e->angleDelta().y() > 0 )
    {
        if ( currentRow() == 0 )
            setCurrentRow(count()-1);
        else
            setCurrentRow(currentRow()-1);
        e->accept();
    }
    else if ( e->angleDelta().y() < 0 )
    {
        if ( currentRow() == count() -1 )
            setCurrentRow(0);
        else
            setCurrentRow(currentRow()+1);
        e->accept();
    }
}
                
Settings_Widget::Settings_Widget(QTabWidget* tab_widget, QBoxLayout::Direction dir, bool erase_margins)
{
    QBoxLayout *l = nullptr;

    stack = new QStackedWidget(this);
    list = new My_List_Widget(this);

    if ( dir == QBoxLayout::TopToBottom )
    {
        l = new QBoxLayout(QBoxLayout::LeftToRight);

        list->setFlow( QListView::TopToBottom );
        list->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Preferred );
    }
    else
    {
        l = new QBoxLayout(QBoxLayout::TopToBottom);

        //list->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Maximum );
        //list->setMaximumHeight(60); //FIXME: dynamic would be preferred 

        /*auto app = dynamic_cast<QApplication*>(QApplication::instance());
        if ( app != nullptr )
        {
            QPalette pal = list->palette();
            pal.setColor(QPalette::Base, app->palette("QWidget").color(QPalette::Midlight));
            list->setPalette(pal);
        }*/

        //stack->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );


        list->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Preferred );

        list->setFlow( QListView::LeftToRight );
        list->setViewMode( QListView::IconMode );
        list->setMovement( QListView::Static );

        stack->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Expanding );
    }


    if ( dir != QBoxLayout::TopToBottom )
    {
        /*auto border = new QLabel(this);
        l->addWidget(border);
        border->setMinimumHeight(10);
        border->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        border->setText("test");

        auto app = dynamic_cast<QApplication*>(QApplication::instance());
        if ( app != nullptr )
        {
            QPalette pal = border->palette();
            pal.setColor(QPalette::Button, QColor(0,0,0) /*app->palette("QWidget").color(QPalette::Dark)* /);
            border->setPalette(pal);
        }*/
        auto splitter = new QSplitter(this);
        splitter->setOrientation(Qt::Vertical);
        splitter->addWidget(list);
        splitter->addWidget(stack);

        l->addWidget(splitter);
    }
    else
    {
        l->addWidget(list);
        l->addWidget(stack);
    }

    if ( erase_margins ) 
        l->setContentsMargins(0,0,0,0);

    setLayout(l);

    int i = 0;
    QWidget* widget = nullptr;

    while( ( widget = tab_widget->widget(i) ) != 0 )
    {
        list->addItem( new QListWidgetItem( tab_widget->tabIcon(i), tab_widget->tabText(i) ) );
        stack->addWidget(widget);
    }


    connect(list, SIGNAL(currentRowChanged(int)), stack, SLOT(setCurrentIndex(int)));

    tab_widget->hide();

    tab_widget->parentWidget()->layout()->replaceWidget( tab_widget, this );

    if ( erase_margins )
        tab_widget->parentWidget()->layout()->setContentsMargins(0,0,0,0);
    

    if ( dir != QBoxLayout::TopToBottom )
    {
        /*auto border = new QLabel(this);
        l->addWidget(border);
        border->setMinimumHeight(10);
        border->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        border->setText("test");

        auto app = dynamic_cast<QApplication*>(QApplication::instance());
        if ( app != nullptr )
        {
            QPalette pal = border->palette();
            pal.setColor(QPalette::Button, QColor(0,0,0) /*app->palette("QWidget").color(QPalette::Dark)* /);
            border->setPalette(pal);
        }*/
        list->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
        list->setSpacing(5);
        list->setMinimumHeight(list->sizeHintForRow(0)+15);
        list->setMaximumHeight(list->sizeHintForRow(0)+15);
    }
    else
    {
    }

}

void Settings_Widget::setIconSize(QSize s)
{
    list->setIconSize(s);
}

Settings_Widget::~Settings_Widget()
{
    delete list;
    delete stack;
}

