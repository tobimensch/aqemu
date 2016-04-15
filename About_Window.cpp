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

#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>

#include "Utils.h"
#include "About_Window.h"

About_Window::About_Window( QWidget *parent ): QDialog( parent )
{
	ui.setupUi( this );
	
	// Minimum Size
	resize( width(), minimumSizeHint().height() );
	
	HTTP = new QHttp( this );
	
	connect( HTTP, SIGNAL(requestFinished(int, bool)),
			 this, SLOT(HTTP_Request_Finished(int, bool)) );
	
	connect( HTTP, SIGNAL(responseHeaderReceived(const QHttpResponseHeader&)),
			 this, SLOT(Read_Response_Header(const QHttpResponseHeader&)) );
	
	// Thanks HTML Text
	ui.Edit_Thanks_To_Text->setHtml( tr(
	"<b>Developers:</b>\n"
	"<br>Andrey Rijov (aka RDron) - Author\n"
	
	"<br><br><b>Contributors:</b>\n"
	"<br>Max Brazhnikov - FreeBSD Port\n"
	"<br>Boris Savelev - ALTLinux Package\n"
	"<br>Denis Smirnov (aka reMiND) - Arch Linux Package and Other Help\n"
	"<br>Ignace Mouzannar - Debian Package\n"
	"<br>Michael Brandstetter - Improvement Scripts\n"
	"<br>Robert Warnke - German AQEMU Documentation Translation\n"
	"<br>Michael Schmöller (aka schmoemi) - German Translation and Correcting English translation\n"
	"<br>Guillem Rieu - Patch for Network Redirections\n"
	"<br>Karol Krenski - IP Address RegExp Fix\n"
	"<br>Alexander Romanov (aka romale) - Testing, New Ideas and Other Help\n"
	"<br>Timothy Redaelli - Patch for CMake\n"
	"<br>Pavel Serebryakov (aka Kronoph) - Correcting English translation\n"
	
	"<br><br><b>Special thanks:</b>\n"
	"<br>Sergey Sinitsa (aka sin)\n"
	"<br>Grigory Pulyaev (aka Rodegast)\n"
	"<br>Alexander Saifulin (aka Ne01eX)\n"
	"<br>Mihail Parshin (aka Skeeper)\n"
	"<br>Garret Acott (aka gacott)\n"
	"<br>Damir Vafin (aka Denver-22)\n"
	
	"<br><br><b>Thanks all www.nclug.ru group for testing and suggestions for improvement.</b>\n"
	
	"<br><br><b>AQEMU used code from QtEMU and Krdc thanks all developers:</b>\n"
	"<br>Tim Jansen (tim@tjansen.de)\n"
	"<br>Urs Wolfer (uwolfer@kde.org)\n"
	"<br>Ben Klopfenstein (benklop@gmail.com)\n"
	
	"<br><br><b>Icons:</b>\n"
	"<br>Crystal SVG - Icon Theme From Everaldo.com Design Studio\n"
	"<br>Oxygen - Icon Theme From Oxygen Team\n") );
	
	// Load Links
	QSettings settings;
	QFileInfo logFileDir( settings.fileName() );
	linksFilePath = QDir::toNativeSeparators( logFileDir.absolutePath() + "/aqemu_links.html" );
	
	Show_Links_File();
}

void About_Window::on_Button_Update_Links_clicked()
{
	QUrl url( "http://aqemu.sourceforge.net/aqemu_links.html" );
	
	//if( QFile::exists(linksFilePath) ) QFile::remove( linksFilePath );
	
	File = new QFile( linksFilePath );
	if( ! File->open(QIODevice::WriteOnly) )
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("Unable to save the file %1: %2.").arg(linksFilePath).arg(File->errorString()) );
		
		delete File;
		File = 0;
		return;
	}
	
	QHttp::ConnectionMode mode = QHttp::ConnectionModeHttp;
	HTTP->setHost( url.host(), mode, url.port() == -1 ? 0 : url.port() );
	
	HTTP_Request_Aborted = false;
	QByteArray path = QUrl::toPercentEncoding( url.path(), "!$&'()*+,;=:@/" );
	if( path.isEmpty() ) path = "/";
	HTTP_Get_Id = HTTP->get( path, File );
	
	ui.Button_Update_Links->setEnabled( false );
}

void About_Window::Show_Links_File()
{
	QSettings settings;
	QString show_url;
	
	if( QFile::exists(linksFilePath) )
	{
		show_url = linksFilePath;
	}
	else if( QFile::exists(QDir::toNativeSeparators(settings.value("AQEMU_Data_Folder", "").toString() + "/aqemu_links.html")) )
	{
		show_url = QDir::toNativeSeparators( settings.value("AQEMU_Data_Folder", "").toString() + "/aqemu_links.html" );
	}
	else
	{
		AQGraphic_Warning( tr("Error!"),
						   tr("Cannot Find AQEMU Links File!") );
		return;
	}
	
	QFile links_file( show_url );
	
	if( ! links_file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		AQError( "void About_Window::Show_Links_File()",
				 "Cannot Open \"aqemu_links.html\" File!" );
		return;
	}
	else
	{
		QTextStream out( &links_file );
		QString all = out.readAll();
		ui.Links_View->setHtml( all );
	}
}

void About_Window::Cancel_Download()
{
	HTTP_Request_Aborted = true;
	HTTP->abort();
	ui.Button_Update_Links->setEnabled( true );
}

void About_Window::HTTP_Request_Finished( int requestId, bool error )
{
	if( requestId != HTTP_Get_Id ) return;
	
	if( HTTP_Request_Aborted )
	{
		if( File )
		{
			File->close();
			File->remove();
			delete File;
			File = 0;
		}
		return;
	}
	
	File->close();
	
	if( error )
	{
		File->remove();
		AQGraphic_Warning( tr("Error!"),
						   tr("Download failed: %1.").arg(HTTP->errorString()) );
	}
	
	ui.Button_Update_Links->setEnabled( true );
	delete File;
	File = 0;
	
	if( ! error ) Show_Links_File();
}

void About_Window::Read_Response_Header( const QHttpResponseHeader &responseHeader )
{
	switch( responseHeader.statusCode() )
	{
		case 200: // OK
		case 301: // Moved Permanently
		case 302: // Found
		case 303: // See Other
		case 307: // Temporary Redirect
		break;
		
		default:
			AQGraphic_Warning( tr("Error!"),
							   tr("Download failed: %1.").arg(responseHeader.reasonPhrase()) );
			HTTP_Request_Aborted = true;
			HTTP->abort();
	}
}
