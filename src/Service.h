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

#ifndef AQEMU_SERVICE_H
#define AQEMU_SERVICE_H

#include <QApplication>
#include <QSettings>
#include <QObject>
#include <QList>

#include "VM.h"

class Run_Guard;
class AQEMU_Main;

#define SERVICE_NAME "org.aqemu.service"

class AQEMU_Service : public QObject
{
    Q_OBJECT

    /* This is a comment directed to all singleton haters:
     * We need exactly 1 instance per application, not more,
     * not less. This makes the code easier, both to write
     * and to maintain. (Also easier to read IMO) .*/

    public:
        static AQEMU_Service& get()
        {
              static AQEMU_Service instance;
              return instance;
        }
        bool wasCalled()
        {
            return called_dbus;
        }
        bool successfulInit()
        {
            return successful_init;
        }
        bool isExternal()
        {
            return ( service == 0 );
        }
        bool isActive();
        bool call(const QString& command,const QList<QVariant>& params, bool noblock=true);
        bool call(const QString& command,const QString& vm, bool noblock=true);
        bool call(const QString& command,Virtual_Machine* vm, bool noblock=true);
        bool call(const QString &command, Virtual_Machine *vm, const QString &param2, bool noblock);
        void setMainWindow( bool );
        void setMain( AQEMU_Main* );
        int machineCount() const;

    private:
        AQEMU_Service();
        ~AQEMU_Service();

    public slots:
        QString start(const QString& vm);
        QString stop(const QString& vm);
        QString shutdown(const QString& vm);
        QString pause(const QString& vm);
        QString reset(const QString& vm);
        QString save(const QString& vm);
        QString error(const QString& vm);
        QString control(const QString& vm);
        QString status(const QString& vm);
        QString command(const QString& vm, const QString& command);

    private slots:
        void vm_state_changed(Virtual_Machine *vm, VM::VM_State s);

    private:
        Virtual_Machine* getMachine(const QString& vm);
        bool init_service();

        Run_Guard* service;
        AQEMU_Main* main;

        QList<Virtual_Machine*> machines;
        bool called_dbus;

        bool main_window;
        bool successful_init;
};

#endif
