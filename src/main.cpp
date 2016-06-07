/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include <QApplication>
#include <QResource>
#include <QMessageBox>
#include <QTranslator>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QtDBus>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/types.h>
#endif
#include <iostream>

#include "Utils.h"
#include "Main_Window.h"
#include "First_Start_Wizard.h"

#include "docopt/docopt.h"

#include "Run_Guard.h"

#include "main.h"
#include "Service.h"

const char USAGE[] =
R"(Usage: aqemu [options]
       aqemu start     ( <AQEMU_FILE> | <VM_Name> )
       aqemu stop      ( <AQEMU_FILE> | <VM_Name> )
       aqemu shutdown  ( <AQEMU_FILE> | <VM_Name> )
       aqemu pause     ( <AQEMU_FILE> | <VM_Name> )
       aqemu reset     ( <AQEMU_FILE> | <VM_Name> )
       aqemu save      ( <AQEMU_FILE> | <VM_Name> ) [ <Tag> ]
       aqemu load      ( <AQEMU_FILE> | <VM_Name> ) [ <Tag> ]
       aqemu monitor   ( <AQEMU_FILE> | <VM_Name> )
       aqemu error     ( <AQEMU_FILE> | <VM_Name> )
       aqemu control   ( <AQEMU_FILE> | <VM_Name> )
       aqemu status    ( <AQEMU_FILE> | <VM_Name> )

Frontend for qemu.

With no extra arguments given the main graphical user interface
of aqemu gets started.

Use the command arguments (start, ... , control) to control VMs.
All the commands except for start and load presume that the given
VM is already running.

For all commands you can give either the path to the aqemu file
or the name of the VM to specify on which VM to operate.

  Options:
      -h --help          Show this screen.
      --version          Show version.

  Qt Options:
      --style THEME      Set theme/style."

  Examples:
    aqemu start "Obscure OS""
    aqemu stop "$HOME/.aqemu/Obscure OS.aqemu"
)";

AQEMU_Main::AQEMU_Main()
{
    settings = nullptr;
    application = nullptr;
    window = nullptr;
}

AQEMU_Main::~AQEMU_Main()
{
    delete settings;
    delete window;
    delete application;
}

int AQEMU_Main::main(int argc, char *argv[])
{
    QString version = QString("aqemu ") + CURRENT_AQEMU_VERSION;
    std::map<std::string, docopt::value> args
        = docopt::docopt(USAGE,
                         { argv + 1, argv + argc },
                         true,               // show help if requested
                          qPrintable( version ) );  // version string

    for(auto const& arg : args) {
        std::cout << arg.first <<  arg.second << std::endl;
    }

    // Create QApplication
    application = new QApplication( argc, argv );

    QString AQEMU_FILE;
    if ( args.at("<AQEMU_FILE>").isString() )
        AQEMU_FILE = QString::fromStdString(args.at("<AQEMU_FILE>").asString());

    AQEMU_Service& service = AQEMU_Service::get();
    service.setMain(this);

    if ( args.at("start").asBool() )
        service.call("start",AQEMU_FILE,false);
    else if ( args.at("stop").asBool() )
        service.call("stop",AQEMU_FILE,false);
    else if ( args.at("shutdown").asBool() )
        service.call("shutdown",AQEMU_FILE,false);
    else if ( args.at("reset").asBool() )
        service.call("reset",AQEMU_FILE,false);
    else if ( args.at("pause").asBool() )
        service.call("pause",AQEMU_FILE,false);
    else if ( args.at("save").asBool() )
        service.call("save",AQEMU_FILE,false);
    else if ( args.at("error").asBool() )
        service.call("error",AQEMU_FILE,false);
    else if ( args.at("control").asBool() )
        service.call("control",AQEMU_FILE,false);
    else if ( args.at("status").asBool() )
        service.call("status",AQEMU_FILE,false);

    if ( service.wasCalled() )
    {
        if ( service.isExternal() ) // called the already running service
        {
            return 0;
        }
        else if ( service.successfulInit() ) // became the service
        {
            if ( service.machineCount() < 1 )
                return 0;
            else
            {
                application->setQuitOnLastWindowClosed(false);
                return application->exec();
            }
        }
        else //something went wrong while initializing service
        {
            return 1;
        }
    }

    service.setMainWindow( true );

    int ret = main_window();

    // the main window is closed at this point
    if ( service.isActive() ) //but vms might still be running
    {
        delete window;
        window = nullptr;
        service.setMainWindow( false );
        application->setQuitOnLastWindowClosed(false);
        return application->exec();
    }
    return ret;
}

int AQEMU_Main::load_settings()
{
    init_qsettings();

    log_settings();

    Set_Show_Error_Window( true );

    // Init emulators settings "data base"
    System_Info::Update_VM_Computers_List();

    int ret = root_warning();
    if ( ret != 0 )
        return ret;

    upgrade_settings();

    ret = find_data_folders();
    if ( ret != 0 )
        return ret;

    register_qresource();

    first_start_wizard();

    load_language();

    vm_dir_exists_or_create();

    // Check QEMU and KVM Versions
    Update_Emulators_List(); // FIXME

    return 0;
}

int AQEMU_Main::main_window()
{
    Run_Guard guard( "Gmp[0Ab5598" ); //unique random key
    if ( !guard.tryToRun() )
    {
        std::cout << qPrintable(QObject::tr("AQEMU is already running. Only one main window can be used at one time.")) << std::endl;
        return 0;
    }

    int ret = load_settings();
    if ( ret != 0 )
        return ret;

    // Show main window
    window = new Main_Window;
    window->show();

    application->setWindowIcon(QIcon(":/aqemu.png"));

    return application->exec();
}

void AQEMU_Main::load_language()
{
    // Load Language
    QTranslator appTranslator;

    if( settings->value("Language", "").toString() != "en" )
    {
        appTranslator.load( settings->value("AQEMU_Data_Folder", "").toString() + settings->value("Language", "").toString() + ".qm",
                            application->applicationDirPath() );

        application->installTranslator( &appTranslator );
    }
}

void AQEMU_Main::first_start_wizard()
{
    // Is this the first start of AQEMU on this computer?
    if( settings->value("First_Start", "yes").toString() == "yes" )
    {
        First_Start_Wizard first_start_win( NULL );

        if( first_start_win.exec() == QDialog::Accepted ) AQDebug( "int main( int argc, char *argv[] )", "First Start Wizard Complete" );
        else AQWarning( "int main( int argc, char *argv[] )", "First Start Wizard Canceled!" );
    }
}

void AQEMU_Main::register_qresource()
{
    // Load Images
    /*QString iconsThemeFile = "";

    iconsThemeFile = QDir::toNativeSeparators( settings->value("AQEMU_Data_Folder", "").toString() + "/icons.rcc" );

    if( ! QResource::registerResource(iconsThemeFile) )
    {
        AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
                         QObject::tr("Cannot Load AQEMU Icon Theme!\nFile \"%1\" Not Found!").arg(iconsThemeFile), false );
    }*/
}

void AQEMU_Main::init_qsettings()
{
    // Set QSettings Data
    QCoreApplication::setOrganizationName( "aqemu" );
    QCoreApplication::setApplicationName( "AQEMU" );
    #ifdef Q_OS_WIN32
    QSettings::setDefaultFormat( QSettings::IniFormat );
    #endif
    settings = new QSettings();
}

void AQEMU_Main::upgrade_settings()
{
    // This is an Upgrade of AQEMU? Find Previous Config...
    //FIXME if( QFile::exists(QDir::homePath() + "/.config/aqemu/AQEMU.conf") )
    if( QFile::exists(settings->fileName()) )
    {
        QString conf_ver = settings->value( "AQEMU_Config_Version", CURRENT_AQEMU_VERSION ).toString();

        if( conf_ver == "0.5" )
        {
            AQDebug( "int main( int argc, char *argv[] )",
                     "AQEMU Config Version: 0.5\nRun Firt Start Wizard" );

            settings->setValue( "First_Start", "yes" );
            settings->setValue( "AQEMU_Config_Version", CURRENT_AQEMU_VERSION );
        }
        else if( conf_ver == "0.7.2" || conf_ver == "0.7.3" || conf_ver == "0.8" )
        {
            AQDebug( "int main( int argc, char *argv[] )",
                     "AQEMU Config Version: 0.7.X\nStart Emulators Search..." );

            QMessageBox::information( NULL, QObject::tr("AQEMU emulators search"),
                                      QObject::tr("AQEMU will search for emulators after uptating. Please wait."),
                                      QMessageBox::Ok );

            First_Start_Wizard *first_start_win = new First_Start_Wizard( NULL );

            if( first_start_win->Find_Emulators() )
                AQDebug( "int main( int argc, char *argv[] )",
                         "Find Emulators and Save Settings Complete" );
            else
                AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
                                 QObject::tr("Cannot Find any Emulators installed in your OS! You should choose them In Advanced Settings!"), false );

            delete first_start_win;

            settings->setValue( "AQEMU_Config_Version", CURRENT_AQEMU_VERSION );
        }
        else if( conf_ver == CURRENT_AQEMU_VERSION )
        {
            AQDebug( "int main( int argc, char *argv[] )",
                     QString("AQEMU Config Version: %1").arg(CURRENT_AQEMU_VERSION) );
        }
        else
        {
            // Remove Old Config!
            //FIXME if( QFile::copy(QDir::homePath() + "/.config/aqemu/AQEMU.conf",
            //	QDir::homePath() + "/.config/aqemu/AQEMU.conf.bak") )
            if( QFile::copy(settings->fileName(), settings->fileName() + ".bak") )
            {
                AQWarning( "int main( int argc, char *argv[] )",
                           "AQEMU Configuration File No Version 0.5. File Saved: AQEMU.conf.bak" );

                settings->clear();
                settings->sync();

                settings->setValue( "AQEMU_Config_Version", CURRENT_AQEMU_VERSION );
            }
            else AQError( "int main( int argc, char *argv[] )", "Cannot Save Old Version AQEMU Configuration File!" );
        }
    }
    else
    {
        // Config File Not Found. This is the First Install
        settings->setValue( "AQEMU_Config_Version", CURRENT_AQEMU_VERSION );
    }
}

int AQEMU_Main::find_data_folders()
{
    // Find Data Folder
    if( settings->value("AQEMU_Data_Folder", "").toString().isEmpty() )
    {
        #ifdef Q_OS_WIN32
        if( QDir(QDir::currentPath() + "\\os_icons").exists() &&
            QDir(QDir::currentPath() + "\\os_templates").exists() )
        {
            settings->setValue( "AQEMU_Data_Folder", QDir::toNativeSeparators(QDir::currentPath()) );
            AQDebug( "int main( int argc, char *argv[] )", "Use Data Folder: " + QDir::currentPath() );
        }
        else
        {
            AQGraphic_Error( "int main( int argc, char *argv[] )", QObject::tr("Error!"),
                             QObject::tr("Cannot Find AQEMU Data!"), false );
        }
        #else
        QStringList dataDirs;
        dataDirs << "/usr/share/aqemu/"
                 << "/usr/share/apps/aqemu/"
                 << "/usr/local/share/aqemu/";

        // Find data dir
        for( int dx = 0; dx < dataDirs.count(); ++dx )
        {
            QDir dataDir( dataDirs[dx] );

            if( dataDir.exists("./os_icons") &&
                dataDir.exists("./os_templates") )
            {
                settings->setValue( "AQEMU_Data_Folder", dataDirs[dx] );
                break;
            }
        }

        // Found?
        if( settings->value("AQEMU_Data_Folder", "").toString().isEmpty() )
        {
            QMessageBox::information( NULL, QObject::tr("Error!"),
                                      QObject::tr("Cannot Locate AQEMU Data Folder!\n"
                                                  "You Should Select This Folder in Next Window!"),
                                      QMessageBox::Ok );

            QString aqemuDataDir = QFileDialog::getExistingDirectory( NULL, QObject::tr("Please Select AQEMU Data Folder:"),
                                                                      "/", QFileDialog::ShowDirsOnly );

            if( aqemuDataDir.isEmpty() )
            {
                QMessageBox::critical( NULL, QObject::tr("Error!"),
                                       QObject::tr("AQEMU won't Work If Data Folder isn't Selected!") );
                return -1;
            }
            else
            {
                if( ! aqemuDataDir.endsWith("/") ) aqemuDataDir += "/";
                settings->setValue( "AQEMU_Data_Folder", aqemuDataDir );
            }
        }
        #endif
    }

    return 0;
}

void AQEMU_Main::log_settings()
{
    // Use Log?
    if( settings->value( "Log/Save_In_File", "yes" ).toString() == "yes" )
    {
        settings->setValue( "Log/Save_In_File", "yes" );
        AQUse_Log( true );

        if( settings->value("Log/Log_Path", "").toString().isEmpty() )
        {
            QFileInfo logFileDir( settings->fileName() );
            QString logDirPath = logFileDir.absolutePath();

            // Dir for log file exists?
            if( ! QFile::exists(logDirPath) )
            {
                QDir dir;
                if( ! dir.mkpath(logDirPath) )
                    AQGraphic_Warning( QObject::tr("Error"),
                                       QObject::tr("Cannot create directory for log file! Path: %1").arg(logDirPath) );
            }

            settings->setValue( "Log/Log_Path", QDir::toNativeSeparators(logDirPath + "/aqemu.log") );
        }
        else
        {
            // Log Size
            if( QFile::exists(settings->value("Log/Log_Path", "").toString()) )
            {
                QFileInfo log_info( settings->value("Log/Log_Path", "").toString() );

                // Log > 1MB
                if( log_info.size() > (1 * 1024 * 1024) )
                {
                    // FIXME Delete Half Log Size
                    QFile::remove( settings->value("Log/Log_Path", "").toString() );
                }
            }
        }
    }
    else AQUse_Log( false );

    // Log File Name
    AQLog_Path( settings->value("Log/Log_Path", "").toString() );

    // Log Filter
    AQUse_Debug_Output( settings->value("Log/Print_In_STDOUT", "yes").toString() == "yes",
                        settings->value("Log/Save_Debug", "no").toString() == "yes",
                        settings->value("Log/Save_Warning", "yes").toString() == "yes",
                        settings->value("Log/Save_Error", "yes").toString() == "yes" );
}

int AQEMU_Main::root_warning()
{
    // Check For First Start in root Mode
    #ifdef Q_OS_LINUX
    if( settings->value("First_Start", "yes").toString() == "yes" )
    {
        uid_t user_uid = getuid();

        if( user_uid == 0 )
        {
            int ret = QMessageBox::question( NULL, QObject::tr("Warning!"),
                                             QObject::tr("This is the first start of AQEMU with root privileges.\n"
                                                         "On some Linux distributions you may run into problems.\n"
                                                         "Close AQEMU?"),
                                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

            if( ret == QMessageBox::Yes ) return -1;
        }
    }
    #endif
    return 0;
}

void AQEMU_Main::vm_dir_exists_or_create()
{
    // VM Directory Exists?
    QDir vm_dir;
    if( ! vm_dir.exists(settings->value("VM_Directory", "").toString()) )
    {
        int ret = QMessageBox::question( NULL, QObject::tr("Warning!"),
                                         QObject::tr("AQEMU VM Folder doesn't Exists! Create It?"),
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

        if( ret == QMessageBox::Yes )
        {
            #ifdef Q_OS_WIN32
            vm_dir.mkpath( QDir::toNativeSeparators(QDir::homePath() + "/AQEMU_VM/") );
            #else
            vm_dir.mkpath( QDir::homePath() + "/.aqemu" );
            #endif
        }
    }
}

int main( int argc, char *argv[] )
{
    return AQEMU_Main().main(argc,argv);
}
