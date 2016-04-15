/****************************************************************************
**
** Copyright (C) 2008 Ben Klopfenstein <benklop@gmail.com>
** Copyright (C) 2009-2010 Andrey Rijov <ANDron142@yandex.ru>
**
** This file is part of QtEMU, AQEMU.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU Library General Public License
** along with this library; see the file COPYING.LIB.  If not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
** Boston, MA 02110-1301, USA.
**
****************************************************************************/

#ifndef MACHINE_VIEW_H
#define MACHINE_VIEW_H

#include "vncview.h"

#include <QScrollArea>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDynamicPropertyChangeEvent>
#include <QShortcut>
#include <QPalette>

/**
	@author Ben Klopfenstein <benklop@gmail.com>
	@author Andrey Rijov <ANDron142@yandex.ru>
*/
class MachineView : public QScrollArea
{
	Q_OBJECT
	
	public:
		MachineView( QWidget *parent = 0 );
		
		void Set_VNC_URL( const QString &host, int port );
		void Set_Scaling( bool s );
		void Set_Fullscreen( bool on );
		
		void showSplash( bool show );
		void captureAllKeys( bool enabled );
		void sendKey( QKeyEvent *event );
		
	protected:
		bool event( QEvent *event );
		
	public slots:
		void newViewSize( int w, int h );
		void fullscreen( bool enable );
		void initView();
		void Check_Connection();
		void VNC_Connected_OK();
		void reinitVNC();
		void disconnectVNC();
		
	signals:
		void fullscreenToggled( bool enabled );
		void Full_Size( int width, int height );
		void Connected();
		
	private:
		void resizeEvent( QResizeEvent *event );
		void resizeView( int widgetWidth, int widgetHeight );
		
		bool VNC_Connected;
		
		VncView *View;
		int VNC_Port;
		QString VNC_Host;
		bool Scaling;
		
		bool splashShown;
		bool fullscreenEnabled;
		int Port;
		QTimer *Reinit_Timer;
		bool stop_reinit;
		
		QAction *scaleAction;
		
		int Init_Count;
		int VNC_Width;
		int VNC_Height;
};

#endif
