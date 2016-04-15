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

#include "SMP_Settings_Window.h"

SMP_Settings_Window::SMP_Settings_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
}

VM::SMP_Options SMP_Settings_Window::Get_Values() const
{
	VM::SMP_Options smp;
	
	smp.SMP_Count = ui.SB_SMP->value();
	smp.SMP_Cores = ui.SB_Cores->value();
	smp.SMP_Threads = ui.SB_Threads->value();
	smp.SMP_Sockets = ui.SB_Sockets->value();
	smp.SMP_MaxCPUs = ui.SB_MaxCPUs->value();
	
	return smp;
}

void SMP_Settings_Window::Set_Values( const VM::SMP_Options &smp, unsigned short PSO_SMP_Count, bool PSO_SMP_Cores,
									  bool PSO_SMP_Threads, bool PSO_SMP_Sockets, bool PSO_SMP_MaxCPUs )
{
	Backup_SMP = smp;
	
	ui.SB_SMP->setValue( smp.SMP_Count );
	ui.SB_Cores->setValue( smp.SMP_Cores );
	ui.SB_Threads->setValue( smp.SMP_Threads );
	ui.SB_Sockets->setValue( smp.SMP_Sockets );
	ui.SB_MaxCPUs->setValue( smp.SMP_MaxCPUs );
	
	ui.SB_SMP->setMaximum( PSO_SMP_Count );
	
	ui.SB_Cores->setEnabled( PSO_SMP_Cores );
	ui.Label_Cores->setEnabled( PSO_SMP_Cores );
	ui.SB_Threads->setEnabled( PSO_SMP_Threads );
	ui.Lebel_Threads->setEnabled( PSO_SMP_Threads );
	ui.SB_Sockets->setEnabled( PSO_SMP_Sockets );
	ui.Label_Sockets->setEnabled( PSO_SMP_Sockets );
	ui.SB_MaxCPUs->setEnabled( PSO_SMP_MaxCPUs );
	ui.Label_MaxCPUs->setEnabled( PSO_SMP_MaxCPUs );
}

void SMP_Settings_Window::Set_SMP_Count( int count )
{
	Backup_SMP.SMP_Count = count;
	ui.SB_SMP->setValue( count );
	ui.SB_Cores->setValue( 0 );
	ui.SB_Threads->setValue( 0 );
	ui.SB_Sockets->setValue( 0 );
	ui.SB_MaxCPUs->setValue( 0 );
}

void SMP_Settings_Window::on_Button_OK_clicked()
{
	// FIXME Calculate Max CPU Count
	accept();
}

void SMP_Settings_Window::on_Button_Cancel_clicked()
{
	ui.SB_SMP->setValue( Backup_SMP.SMP_Count );
	ui.SB_Cores->setValue( Backup_SMP.SMP_Cores );
	ui.SB_Threads->setValue( Backup_SMP.SMP_Threads );
	ui.SB_Sockets->setValue( Backup_SMP.SMP_Sockets );
	ui.SB_MaxCPUs->setValue( Backup_SMP.SMP_MaxCPUs );
	
	reject();
}
