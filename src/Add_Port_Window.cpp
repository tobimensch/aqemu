/****************************************************************************
**
** Copyright (C) 2009-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include <QFileDialog>
#include "Add_Port_Window.h"
#include "Utils.h"

Add_Port_Window::Add_Port_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	#ifdef Q_OS_UNIX
	ui.RB_stdio->setEnabled( true );
	#endif
	
	#ifdef Q_OS_LINUX
	ui.RB_pty->setEnabled( true );
	ui.RB_Dev->setEnabled( true );
	#endif
	
	#ifdef Q_OS_WIN32
	ui.RB_COM->setEnabled( true );
	#endif
	
	Connect_Slots();
}

VM_Port Add_Port_Window::Get_Port() const
{
	VM_Port port;
	
	if( ui.RB_Default->isChecked() ) port.Set_Port_Redirection( VM::PR_Default );
	else if( ui.RB_vc->isChecked() ) port.Set_Port_Redirection( VM::PR_vc );
	else if( ui.RB_pty->isChecked() ) port.Set_Port_Redirection( VM::PR_pty );
	else if( ui.RB_none->isChecked() ) port.Set_Port_Redirection( VM::PR_none );
	else if( ui.RB_null->isChecked() ) port.Set_Port_Redirection( VM::PR_null );
	else if( ui.RB_stdio->isChecked() ) port.Set_Port_Redirection( VM::PR_stdio );
	else if( ui.RB_Dev->isChecked() ) port.Set_Port_Redirection( VM::PR_dev );
	else if( ui.RB_File->isChecked() ) port.Set_Port_Redirection( VM::PR_file );
	else if( ui.RB_Pipe->isChecked() ) port.Set_Port_Redirection( VM::PR_pipe );
	else if( ui.RB_UDP->isChecked() ) port.Set_Port_Redirection( VM::PR_udp );
	else if( ui.RB_TCP->isChecked() ) port.Set_Port_Redirection( VM::PR_tcp );
	else if( ui.RB_Telnet->isChecked() ) port.Set_Port_Redirection( VM::PR_telnet );
	else if( ui.RB_Unix->isChecked() ) port.Set_Port_Redirection( VM::PR_unix );
	else if( ui.RB_COM->isChecked() ) port.Set_Port_Redirection( VM::PR_com );
	else if( ui.RB_msmouse->isChecked() ) port.Set_Port_Redirection( VM::PR_msmouse );
	else if( ui.RB_mon->isChecked() ) port.Set_Port_Redirection( VM::PR_mon );
	else if( ui.RB_braille->isChecked() ) port.Set_Port_Redirection( VM::PR_braille );
	else
	{
		AQError( "VM_Port Add_Port_Window::Get_Port() const",
				 "Invalid Port Redirection!" );
	}
	
	// Set Parametrs
	port.Set_Parametrs_Line( ui.Edit_Args->text() );
	
	return port;
}

void Add_Port_Window::Set_Port( const VM_Port &port )
{
	switch( port.Get_Port_Redirection() )
	{
		case VM::PR_vc:
			ui.RB_vc->setChecked( true );
			break;
			
		case VM::PR_pty:
			ui.RB_pty->setChecked( true );
			break;
			
		case VM::PR_none:
			ui.RB_none->setChecked( true );
			break;
			
		case VM::PR_null:
			ui.RB_null->setChecked( true );
			break;
			
		case VM::PR_dev:
			ui.RB_Dev->setChecked( true );
			break;
			
		case VM::PR_file:
			ui.RB_File->setChecked( true );
			break;
			
		case VM::PR_stdio:
			ui.RB_stdio->setChecked( true );
			break;
			
		case VM::PR_pipe:
			ui.RB_Pipe->setChecked( true );
			break;
			
		case VM::PR_udp:
			ui.RB_UDP->setChecked( true );
			break;
			
		case VM::PR_tcp:
			ui.RB_TCP->setChecked( true );
			break;
			
		case VM::PR_telnet:
			ui.RB_Telnet->setChecked( true );
			break;
			
		case VM::PR_unix:
			ui.RB_Unix->setChecked( true );
			break;
			
		case VM::PR_com:
			ui.RB_COM->setChecked( true );
			break;
			
		case VM::PR_msmouse:
			ui.RB_msmouse->setChecked( true );
			break;
			
		case VM::PR_mon:
			ui.RB_mon->setChecked( true );
			break;
			
		case VM::PR_braille:
			ui.RB_braille->setChecked( true );
			break;
			
		default:
			ui.RB_Default->setChecked( true );
			break;
	}
	
	ui.Edit_Args->setText( port.Get_Parametrs_Line() );
}

void Add_Port_Window::Connect_Slots()
{
	connect( ui.RB_Default, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_Dev, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_File, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_Unix, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_UDP, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_vc, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_null, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_pty, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_TCP, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_stdio, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_none, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_Telnet, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_Pipe, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_COM, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_msmouse, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_mon, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
	connect( ui.RB_braille, SIGNAL(toggled(bool)), this, SLOT(Show_Arg_Help()) );
}

void Add_Port_Window::on_TB_Browse_clicked()
{
	QString po = QFileDialog::getOpenFileName( this, tr("Open File"),
											   Get_Last_Dir_Path(ui.Edit_Args->text()),
											   tr("All Files (*)") );
	
	if( ! po.isEmpty() )
		ui.Edit_Args->setText( QDir::toNativeSeparators(po) );
}

void Add_Port_Window::Show_Arg_Help()
{
	// FIXME strings
	QStringList argsHelp;
	
	argsHelp << tr( "" );
	argsHelp << tr( "Optionally width and height can be given in pixel with\n800x600\nIt is also possible to specify width or height in characters:\n80Cx24C" );
	argsHelp << tr( "[Linux only] Pseudo TTY (a new PTY is automatically allocated)" );
	argsHelp << tr( "No device is allocated." );
	argsHelp << tr( "Void device" );
	argsHelp << tr( "[Unix only] standard input/output" );
	argsHelp << tr( "[Linux only] Use host tty, e.g. /dev/ttyS0.\nThe host serial port parameters are set according to the emulated ones." );
	argsHelp << tr( "COMn Use host serial port n" );
	argsHelp << tr( "filename - Write output to filename. No character can be read." );
	argsHelp << tr( "filename - name pipe filename" );
	argsHelp << tr( "[remote_host]:remote_port[@[src_ip]:src_port]\nThis implements UDP Net Console. When remote_host or src_ip are not specified they default to 0.0.0.0. When not using a specified src_port a random port is automatically chosen." );
	argsHelp << tr( "[host]:port[,server][,nowait][,nodelay]\nExample to send tcp console to 192.168.0.2 port 4444\n92.168.0.2:4444\nExample to listen and wait on port 4444 for connection\n:4444,server\nExample to not wait and listen on ip 192.168.0.100 port 4444\n192.168.0.100:4444,server,nowait" );
	argsHelp << tr( "host:port[,server][,nowait][,nodelay]\nThe telnet protocol is used instead of raw tcp sockets." );
	argsHelp << tr( "path[,server][,nowait]\nA unix domain socket is used instead of a tcp socket. The option works the same as if you had specified -serial tcp except the unix domain socket path is used for connections." );
	argsHelp << tr( "Three button serial mouse. Configure the guest to use Microsoft protocol." );
	argsHelp << tr( "mon:dev_string\nThis is a special option to allow the monitor to be multiplexed onto another serial port." );
	argsHelp << tr( "Braille device.\nThis will use BrlAPI to display the braille output on a real or fake device." );
	
	if( ui.RB_Default->isChecked() ) ui.Label_Help->setText( argsHelp[0] );
	else if( ui.RB_vc->isChecked() ) ui.Label_Help->setText( argsHelp[1] );
	else if( ui.RB_pty->isChecked() ) ui.Label_Help->setText( argsHelp[2] );
	else if( ui.RB_none->isChecked() ) ui.Label_Help->setText( argsHelp[3] );
	else if( ui.RB_null->isChecked() ) ui.Label_Help->setText( argsHelp[4] );
	else if( ui.RB_stdio->isChecked() ) ui.Label_Help->setText( argsHelp[5] );
	else if( ui.RB_Dev->isChecked() ) ui.Label_Help->setText( argsHelp[6] );
	else if( ui.RB_COM->isChecked() ) ui.Label_Help->setText( argsHelp[7] );
	else if( ui.RB_File->isChecked() ) ui.Label_Help->setText( argsHelp[8] );
	else if( ui.RB_Pipe->isChecked() ) ui.Label_Help->setText( argsHelp[9] );
	else if( ui.RB_UDP->isChecked() ) ui.Label_Help->setText( argsHelp[10] );
	else if( ui.RB_TCP->isChecked() ) ui.Label_Help->setText( argsHelp[11] );
	else if( ui.RB_Telnet->isChecked() ) ui.Label_Help->setText( argsHelp[12] );
	else if( ui.RB_Unix->isChecked() ) ui.Label_Help->setText( argsHelp[13] );
	else if( ui.RB_msmouse->isChecked() ) ui.Label_Help->setText( argsHelp[14] );
	else if( ui.RB_mon->isChecked() ) ui.Label_Help->setText( argsHelp[15] );
	else if( ui.RB_braille->isChecked() ) ui.Label_Help->setText( argsHelp[16] );
	else
	{
		ui.Label_Help->setText( "" );
		
		AQError( "void Add_Port_Window::Show_Arg_Help()",
				 "Invalid Port Redirection!" );
	}
}
