/****************************************************************************
**
** Copyright (C) 2008-2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include "Utils.h"
#include "Error_Log_Window.h"

Error_Log_Window::Error_Log_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );

    last_error_is_deprecated_option = false;
	
	Errors_Count = 0;
	
	if( Settings.value("No_Show_Error_Log_Forever", "no") == "yes" ) ui.RB_No_Show_Forever->setChecked( true );
    else if ( Settings.value("Error_Log_User_Mode","yes") == "yes" ) ui.RB_User_Normal_Mode->setChecked( true );
	else if( ! Get_Show_Error_Window() ) ui.RB_Show_AQEMU_Started->setChecked( true );

}

void Error_Log_Window::Add_to_Log( const QString& err_str )
{
	if( err_str.indexOf(QRegExp("\\w+"), 0) == -1 ) return;
	
	Errors_Count++;
	
	ui.Edit_Log->append( "<b> " + tr("Error #") + QString::number(Errors_Count) + "</b><br>" + err_str + "<br>" );

    last_error_is_deprecated_option = err_str.contains("option is deprecated");

    //deprecated options shouldn't block an actual error from being shown
    if ( err_str.contains("invalid") || err_str.contains("could not") )
    {
        last_error_is_deprecated_option = false;
    }
}

bool Error_Log_Window::No_Show_Before_AQEMU_Restart() const
{
	if( ui.RB_Show_AQEMU_Started->isChecked() ) Set_Show_Error_Window( false );
	else Set_Show_Error_Window( true );
	
	return ! Get_Show_Error_Window();
}

bool Error_Log_Window::No_Show_Before_VM_Restart() const
{
	if( ui.RB_Show_VM_Started->isChecked() ) return true;
	else return false;
}

bool Error_Log_Window::No_Show_Deprecated_Options_Error() const
{
	if( ui.RB_Normal_Mode->isChecked() )
    { 
        //the "Normal_Mode" which should really be called "Developer Mode"
        //is the only mode in which we want to see deprecated options errors
        return false;
	}
    return last_error_is_deprecated_option;
}

void Error_Log_Window::done(int r)
{
    if ( r == QDialog::Accepted )
    {
	    if( ui.RB_No_Show_Forever->isChecked() ) Settings.setValue( "No_Show_Error_Log_Forever", "yes" );
	    else Settings.setValue( "No_Show_Error_Log_Forever", "no" );

	    if( ui.RB_User_Normal_Mode->isChecked() ) Settings.setValue( "Error_Log_User_Mode", "yes" );
	    else Settings.setValue( "Error_Log_User_Mode", "no" );
	
	    No_Show_Before_AQEMU_Restart();
    }
    QDialog::done(r);
}
