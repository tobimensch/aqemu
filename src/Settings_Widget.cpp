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
#include "Utils.h"

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
                
Settings_Widget::Settings_Widget(QTabWidget* tab_widget, QBoxLayout::Direction dir, bool erase_margins, bool erase_parent_margins)
{
    QBoxLayout *l = nullptr;

    splitter = nullptr;
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
        list->setWrapping( false );

        stack->setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Expanding );
    }

    splitter = new QSplitter(this);

    if ( dir != QBoxLayout::TopToBottom )
        splitter->setOrientation(Qt::Vertical);
    else
        splitter->setOrientation(Qt::Horizontal);

    splitter->addWidget(list);
    splitter->addWidget(stack);

    l->addWidget(splitter);

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

    if ( erase_margins && erase_parent_margins )
        tab_widget->parentWidget()->layout()->setContentsMargins(0,0,0,0);
    

    if ( dir != QBoxLayout::TopToBottom )
    {
        list->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
        list->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
        list->setSpacing(5);
        list->setMinimumHeight(list->sizeHintForRow(0)+15);
        list->setMaximumHeight(list->sizeHintForRow(0)+15);
    }
    else
    {
        if ( erase_margins )
            stack->setContentsMargins(0,0,0,0);
    }
}

void Settings_Widget::setCurrentIndex(int i)
{
    list->setCurrentRow(i);
}

QMap<QString,QList<Settings_Widget*>> Settings_Widget::groups = QMap<QString,QList<Settings_Widget*>>();

void Settings_Widget::addToGroup(QString g)
{
    if(g.isEmpty())
        return;

    my_Group = g;

    groups[g].append(this);
}

void Settings_Widget::syncGroupIconSizes(QString g)
{
    QList<Settings_Widget*> list = groups[g];

    QList<int> max_width;

    int max_list_height = 0;

    //first find max_width for each index
    for( int i = 0; i < list.count(); i++ )
    {
        auto sw = list.at(i);

        for ( int j = 0; j < sw->list->count(); j++ )
        {
            int w = sw->list->item(j)->sizeHint().width();

            int list_h = sw->list->minimumSizeHint().height();

            if ( list_h > 0 )
            {
                if ( max_list_height == 0 )
                    max_list_height = list_h;
                else if ( max_list_height > list_h )
                    max_list_height = list_h;
            }

            if ( w == -1 ) // which will always be the case :-(
            {
                QString t = sw->list->item(j)->text();
                QLabel l(t);

                w = l.sizeHint().width() + 20;
            }

            
            if ( max_width.count() < j + 1 )
            {
                //index number not in list
                max_width.append( w);
            }
            else
            {
                //index number is in list
                if ( w > max_width.at(j) )
                    max_width.replace(j, w);
            }
            
        }
    }

    //minimum total list width
    int min_total_list_width = 15;
    for ( int j = 0; j < max_width.count(); j++ )
    {
        //int h = sw->list->item(j)->sizeHint().height();
        min_total_list_width += max_width.at(j) +5;
    }

    if ( max_list_height > 0  && max_list_height <= 58 )
    {
        max_list_height = 68;
    }

    //then apply max_width to all
    for( int i = 0; i < list.count(); i++ )
    {
        auto sw = list.at(i);

        for ( int j = 0; j < sw->list->count(); j++ )
        {
            //int h = sw->list->item(j)->sizeHint().height();
            sw->list->item(j)->setSizeHint(QSize(max_width.at(j),58));
        }

        if ( max_list_height > 0 )
        {
            sw->list->setMinimumHeight(max_list_height);
            sw->list->setMaximumHeight(max_list_height);
        }
        sw->list->setMinimumWidth(min_total_list_width);
        sw->stack->setMinimumWidth(min_total_list_width);
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

    delete splitter;
}

