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
//adapted code from http://stackoverflow.com/a/29160735 by AGo

#ifndef IP_EDIT_H
#define IP_EDIT_H

#include <QLineEdit>
#include <QFrame>

class CustomLineEdit : public QLineEdit
{
    Q_OBJECT

    public:
        explicit CustomLineEdit(const QString & contents = "", QWidget *parent = 0);
        virtual ~CustomLineEdit() {}

    signals:
        void jumpForward();
        void jumpBackward();

    public slots:
        void jumpIn();

    protected:
        virtual void focusInEvent(QFocusEvent *event);
        virtual void keyPressEvent(QKeyEvent * event);
        virtual void mouseReleaseEvent(QMouseEvent *event);

    private:
        bool selectOnMouseRelease;
};

class IP_Edit : public QFrame
{
    Q_OBJECT

    public:
        explicit IP_Edit(QWidget *parent = 0);
        ~IP_Edit();
        void setText(const QString&);
        QString text();

    signals:
        void textChanged(const QString &);

    private:
        CustomLineEdit* p1;
        CustomLineEdit* p2;
        CustomLineEdit* p3;
        CustomLineEdit* p4;
};

#endif

