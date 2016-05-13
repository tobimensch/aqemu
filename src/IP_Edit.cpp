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

#include <QVBoxLayout>
#include <QKeyEvent>
#include <QIntValidator>
#include <QApplication>
#include <QFontMetrics>
#include <QLabel>
#include <QSpacerItem>

#include "IP_Edit.h"

CustomLineEdit::CustomLineEdit(const QString & contents, QWidget *parent) :
    QLineEdit(contents, parent), selectOnMouseRelease(false)
{
    QIntValidator *valid = new QIntValidator(0, 255, this);
    setValidator(valid);
}

void CustomLineEdit::jumpIn()
{
    setFocus();

    selectOnMouseRelease = false;
    selectAll();
}

void CustomLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    selectOnMouseRelease = true;
}

void CustomLineEdit::keyPressEvent(QKeyEvent * event)
{
    int key = event->key();
    int cursorPos = cursorPosition();

    // Jump forward by Space
    if (key == Qt::Key_Space)
    {
        emit jumpForward();
        event->accept();
        return;
    }

    // Jump Backward only from 0 cursor position
    if (cursorPos == 0)
    {
        if ((key == Qt::Key_Left) || (key == Qt::Key_Backspace))
        {
            emit jumpBackward();
            event->accept();
            return;
        }
    }

    // Jump forward from last postion by right arrow
    if (cursorPos == text().count())
    {
        if (key == Qt::Key_Right) {
            emit jumpForward();
            event->accept();
            return;
        }
    }

    // After key is placed cursor has new position
    QLineEdit::keyPressEvent(event);
    int freshCurPos = cursorPosition();

    if ((freshCurPos == 3) && (key != Qt::Key_Right))
        emit jumpForward();
}

void CustomLineEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if(!selectOnMouseRelease)
        return;

    selectOnMouseRelease = false;
    selectAll();

    QLineEdit::mouseReleaseEvent(event);
}

void makeCommonStyle(QWidget* line)
{
    line->setContentsMargins(0, 0, 0, 0);

    auto lineedit = dynamic_cast<QLineEdit*>(line);
    if (lineedit)
        lineedit->setAlignment(Qt::AlignCenter);
    //line->setStyleSheet("QLineEdit { border: 0px none; }");
    //line->setFrame(false);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

QWidget* makeIpSplitter()
{
    QLabel *splitter = new QLabel(".");
    makeCommonStyle(splitter);

    splitter->setAlignment(Qt::AlignCenter);
    splitter->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    splitter->setMaximumWidth(10);
    //splitter->setReadOnly(true);
    splitter->setFocusPolicy(Qt::NoFocus);
    return splitter;
}

IP_Edit::IP_Edit(QWidget *parent) :
    QFrame(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setStyleSheet("QFrame { border: 0px; }");

    QList<CustomLineEdit*> lines;
    QList<CustomLineEdit*>::iterator linesIterator;

    p1 = new CustomLineEdit;
    p2 = new CustomLineEdit;
    p3 = new CustomLineEdit;
    p4 = new CustomLineEdit;

    QFontMetrics metrics(QApplication::font());

    int width = metrics.width("888") + 24;

    p1->setMaximumWidth(width);
    p2->setMaximumWidth(width);
    p3->setMaximumWidth(width);
    p4->setMaximumWidth(width);
    
    lines.append(p1);
    lines.append(p2);
    lines.append(p3);
    lines.append(p4);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    setLayout(mainLayout);

    for (linesIterator = lines.begin(); linesIterator != lines.end(); ++linesIterator) {
        makeCommonStyle(*linesIterator);
        mainLayout->addWidget(*linesIterator);

        if (*linesIterator != lines.last()) {
            connect(*linesIterator, &CustomLineEdit::jumpForward,
                    *(linesIterator+1), &CustomLineEdit::jumpIn);
            mainLayout->addWidget(makeIpSplitter());
        }
        if (*linesIterator != lines.first()) {
            connect(*linesIterator, &CustomLineEdit::jumpBackward,
                    *(linesIterator-1), &CustomLineEdit::jumpIn);
        }
    }
    mainLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed));

    connect(p1,SIGNAL(textChanged(const QString &)),this,SIGNAL(textChanged(const QString &)));
    connect(p2,SIGNAL(textChanged(const QString &)),this,SIGNAL(textChanged(const QString &)));
    connect(p3,SIGNAL(textChanged(const QString &)),this,SIGNAL(textChanged(const QString &)));
    connect(p4,SIGNAL(textChanged(const QString &)),this,SIGNAL(textChanged(const QString &)));
}

IP_Edit::~IP_Edit()
{
    delete p1;
    delete p2;
    delete p3;
    delete p4;
}

void IP_Edit::setText(const QString& s)
{
    auto list = s.split(".");
    if ( list.count() > 3 )
    {
        p1->setText(list[0]);
        p2->setText(list[1]);
        p3->setText(list[2]);
        p4->setText(list[3]);
    }
}


QString IP_Edit::text()
{
    return p1->text()+"."+p2->text()+"."+p3->text()+"."+p4->text();
}

