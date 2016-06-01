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

#ifndef AQEMU_MAIN_H
#define AQEMU_MAIN_H

#include <QApplication>
#include <QSettings>
#include <QObject>
#include <QList>

class Virtual_Machine;
class Run_Guard;
class Main_Window;
class AQEMU_Service;

class AQEMU_Main : public QObject
{
    friend AQEMU_Service;

    Q_OBJECT

    public:
        AQEMU_Main();
        ~AQEMU_Main();
        int main(int argc, char *argv[]);


    private:
        int main_window();

        int load_settings();
        void load_language();
        void first_start_wizard();
        void register_qresource();
        void init_qsettings();
        void upgrade_settings();
        int find_data_folders();
        void log_settings();
        int root_warning();
        void vm_dir_exists_or_create();

        QSettings* settings;
        QApplication* application;
        Main_Window* window;
};

#endif
