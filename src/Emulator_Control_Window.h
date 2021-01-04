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

#ifndef EMULATOR_CONTROL_WINDOW_H
#define EMULATOR_CONTROL_WINDOW_H

#include <QSettings>
#include <QMap>

#include "VM.h"
#include "ui_Emulator_Control_Window.h"
#include "Monitor_Window.h"

#ifdef VNC_DISPLAY
#include "Embedded_Display/Machine_View.h"
#endif

class Virtual_Machine;

class Emulator_Control_Window: public QMainWindow
{
	Q_OBJECT
	
	public:
		Emulator_Control_Window( QWidget *parent = 0 );
		
		void Set_Current_VM( Virtual_Machine *vm );
		void Init();
		void Set_Show_Close_Warning( bool on );
		void Use_Minimal_Size( bool use );
        void Show_Monitor();
		
		Monitor_Window *Mon_Win;
		bool First_Start;
		
	signals:
		void Ready_Read_Command( QString com );
		
	private slots:
		void Apply_Full_Size( int w, int h );
		void QEMU_Quit();
		void VM_State_Changed( Virtual_Machine *vm, VM::VM_State state );
		void Get_Removable_Devices_List();
		void Create_Connect_Menu();
		void Create_Device_Menu();
		void Connect_Device();
		void Open_Device_File();
		void Eject_Device();
		
		//void on_actionUpdate_list_triggered(); //FIXME
		void on_actionSave_Screenshot_triggered();
		void on_actionSave_Screenshot_As_triggered();
		void on_actionSave_VM_triggered();
		void on_actionManage_Snapshots_triggered();
		void on_actionCommit_triggered();
		void on_actionPause_VM_triggered();
		void on_actionShutdown_triggered();
		void on_actionPower_Off_triggered();
		void on_actionReset_VM_triggered();
		void on_actionQEMU_Monitor_triggered();
		void on_actionQuit_triggered();
		
		void on_actionFD0_dev_fd0_triggered();
		void on_actionFD0_dev_null_triggered();
		void on_actionFD0_Other_triggered();
		void on_actionFD0_Eject_triggered();
		void Open_Recent_Floppy0_Image();
		
		void on_actionFD1_dev_fd0_triggered();
		void on_actionFD1_dev_null_triggered();
		void on_actionFD1_Other_triggered();
		void on_actionFD1_Eject_triggered();
		void Open_Recent_Floppy1_Image();
		
		void on_actionCDROM_dev_cdrom_triggered();
		void on_actionCDROM_dev_null_triggered();
		void on_actionCDROM_Other_triggered();
		void on_actionCDROM_Eject_triggered();
		void Open_Recent_CD_ROM_Image();
		
		void Delete_USB_From_VM();
		void Add_USB_To_VM();
		
		void on_actionUSB_Update_Device_List_triggered();
		void on_actionBy_Bus_Address_triggered();
		void on_actionUSB_Disconnect_All_Devices_triggered();
		
		void on_actionCtrl_Alt_Delete_triggered();
		void on_actionOther_Keys_triggered();
		void on_actionCtrl_Alt_Backspace_triggered();
		void on_actionCtrl_Alt_F1_triggered();
		void on_actionCtrl_Alt_F2_triggered();
		void on_actionCtrl_Alt_F3_triggered();
		void on_actionCtrl_Alt_F4_triggered();
		void on_actionCtrl_Alt_F5_triggered();
		void on_actionCtrl_Alt_F6_triggered();
		void on_actionCtrl_Alt_F7_triggered();
		void on_actionCtrl_Alt_F8_triggered();
		void on_actionCtrl_Alt_F9_triggered();
		void on_actionCtrl_Alt_F10_triggered();
		void on_actionCtrl_Alt_F11_triggered();
		void on_actionCtrl_Alt_F12_triggered();
		void on_actionClipboard_triggered();
		void on_actionGrab_Mouse_triggered();
		
		#ifdef VNC_DISPLAY
		void on_actionDisplay_Scaling_triggered();
		void on_actionFullscreen_Mode_triggered();
		void on_actionReinit_VNC_triggered();
		void Connect_VNC();
		bool Use_VNC();
		#endif
		
		void on_actionAbout_Emulator_Control_triggered();
		
	protected:
		void closeEvent( QCloseEvent *event );
		
	private:
		Ui::Emulator_Control_Window ui;
		bool Show_Close_Warning;
		bool Fullscreen_Menu_Added;
		Virtual_Machine *Cur_VM;
		QSettings Settings;
		QList<QAction*> Recent_Files_CD_Items;
		QList<QAction*> Recent_Files_FD0_Items;
		QList<QAction*> Recent_Files_FD1_Items;
		
		QMap<QString, QString> Removable_Devies_Map;
		
		#ifdef VNC_DISPLAY
		MachineView *Machine_View;
		#endif
		
		void Update_Recent_CD_ROM_Images_List();
		void Update_Recent_Floppy_Images_List();
		
		bool CD_ROM_Available();
		bool FD0_Available();
		bool FD1_Available();
		
		void Set_Device( const QString &dev_name, const QString &path );
};

#endif
