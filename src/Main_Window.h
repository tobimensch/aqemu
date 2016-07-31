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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QList>
#include <QCloseEvent>

#include "VM.h"
#include "Utils.h"
#include "ui_Main_Window.h"
#include "ui_Advanced_Options.h"
#include "ui_KVM_Options.h"
#include "ui_Architecture_Options.h"
#include "System_Info.h"
#include "HDD_Image_Info.h"
#include "Network_Widget.h"
#include "Old_Network_Widget.h"
#include "SMP_Settings_Window.h"
#include "Add_New_Device_Window.h"
#include "SPICE_Settings_Widget.h"

class Ports_Tab_Widget;
class Device_Manager_Widget;
class Folder_Sharing_Widget;
class Network_Card_Widget;
class Block_VM_Changed_Signals;

class Main_Window: public QMainWindow
{
    friend class No_Boot_Device;
    friend class Block_VM_Changed_Signals;

    class Block_VM_Changed_Signals
    {
        public:
            Block_VM_Changed_Signals(Main_Window* _mw)
            {
                mw = _mw;
                mw->block_VM_changed_signals = true;
            }

            ~Block_VM_Changed_Signals()
            {
                mw->block_VM_changed_signals = false;
                mw->VM_Changed();
            }

        private:
            Main_Window* mw;
    };

	Q_OBJECT
	
	public:
		Main_Window( QWidget *parent = 0 );
        ~Main_Window();

    public slots:
        void VM_State_Changed(const QString& vm, int state);
		
	private slots:
		void on_Machines_List_currentItemChanged( QListWidgetItem *current,
							  QListWidgetItem *previous );
		void on_Machines_List_customContextMenuRequested( const QPoint &pos );
		void on_Machines_List_itemDoubleClicked( QListWidgetItem *item );
		void VM_State_Changed( Virtual_Machine *vm, VM::VM_State s );
		void Show_State_VM( Virtual_Machine *vm );
		void Show_State_Current( Virtual_Machine *vmvm );
		void Set_Widgets_State( bool enabled );
		void VM_Changed();
		void Update_Emulator_Control( Virtual_Machine *cur_vm );
		
        void SB_VNC_Display_changed(int);
        void SB_VNC_Display_Port_changed(int);

		// Actions
		void on_actionChange_Icon_triggered();
		void on_actionAbout_AQEMU_triggered();
		void on_actionAbout_Qt_triggered();
		void on_actionDelete_VM_triggered();
		void on_actionDelete_VM_And_Files_triggered();
		void on_actionExit_triggered();
		void on_actionShow_New_VM_Wizard_triggered();
		void on_actionAdd_New_VM_triggered();
		void on_actionCreate_HDD_Image_triggered();
		void on_actionConvert_HDD_Image_triggered();
		void on_actionShow_Advanced_Settings_Window_triggered();
		void on_actionShow_First_Run_Wizard_triggered();
		void on_actionPower_On_triggered();
		void on_actionSave_triggered();
        void on_actionShutdown_triggered();
        void on_actionPower_Off_triggered();
		void on_actionPause_triggered();
		void on_actionReset_triggered();
		void on_actionLoad_VM_From_File_triggered();
		void on_actionSave_As_Template_triggered();
		void on_actionCopy_triggered();
		void on_actionManage_Snapshots_triggered();
		void on_actionShow_Emulator_Control_triggered();
		void on_actionShow_QEMU_Arguments_triggered();
		void on_actionCreate_Shell_Script_triggered();
		void on_actionShow_QEMU_Error_Log_Window_triggered();
		
		void on_Tabs_currentChanged( int index );
		
		// Apply and Cancel Buttons
		void on_Button_Apply_clicked();
		void on_Button_Cancel_clicked();
		
		// General Tab
		void on_CB_Computer_Type_currentIndexChanged( int index );
		void on_CB_Machine_Accelerator_currentIndexChanged( int index );
		void CB_Boot_Priority_currentIndexChanged( int index );
		void on_TB_Show_Boot_Settings_Window_clicked();
		void Set_Boot_Order( const QList<VM::Boot_Order> &list );
		void on_TB_Show_Architecture_Options_Window_clicked();
		void on_TB_Show_Accelerator_Options_Window_clicked();
		void on_TB_Show_Advanced_Options_Window_clicked();
		void on_TB_Show_SMP_Settings_Window_clicked();
		bool Validate_CPU_Count( const QString &text );
		void Apply_Emulator( int mode );
		
		void on_CH_Local_Time_toggled( bool on );
		
		// Memory
		void on_Memory_Size_valueChanged( int value );
		void on_CB_RAM_Size_editTextChanged( const QString &text );
		void on_CH_Remove_RAM_Size_Limitation_stateChanged( int state );
		void on_TB_Update_Available_RAM_Size_clicked();
		void Update_RAM_Size_ComboBox( int freeRAM );
		
		QStringList Create_Info_HDD_String( const QString &disk_format,
						   const VM::Device_Size &virtual_size,
						   const VM::Device_Size &disk_size,
						    int cluster_size );

		// Network Tab
		void on_CH_Use_Network_toggled( bool on );
		void on_RB_Network_Mode_New_toggled( bool on );
		
		void on_Redirections_List_cellClicked( int row, int column );
		void on_Button_Add_Redirections_clicked();
		void on_Button_Delete_Redirections_clicked();
		void Update_Current_Redirection_Item();
		void on_Button_Clear_Redirections_clicked();
		
		void on_TB_Browse_SMB_clicked();
		void on_TB_Browse_TFTP_clicked();

		// Advanced
		void adv_on_CH_Start_Date_toggled( bool on );
		
		// Other Tab
		void on_TB_VNC_Unix_Socket_Browse_clicked();
		void on_TB_x509_Browse_clicked();
		void on_TB_x509verify_Browse_clicked();
		
		void on_TB_Linux_bzImage_SetPath_clicked();
		void on_TB_Linux_Initrd_SetPath_clicked();
		
		void on_TB_ROM_File_Browse_clicked();
		void on_TB_MTDBlock_File_Browse_clicked();
		void on_TB_SD_Image_File_Browse_clicked();
		void on_TB_PFlash_File_Browse_clicked();
		
	protected:
		void closeEvent( QCloseEvent *event );
		
	private:
		Virtual_Machine *Get_VM_By_UID( const QString &uid );
		Virtual_Machine *Get_Current_VM();
        void init_dbus();
		
		void Connect_Signals();
		
		const QMap<QString, Available_Devices> Get_Devices_Info( bool *ok ) const;
		Available_Devices Get_Current_Machine_Devices( bool *ok ) const;
		
		bool Create_VM_From_Ui( Virtual_Machine *tmp_vm, Virtual_Machine *old_vm, bool show_user_errors = true );
		
		bool Load_Settings();
		bool Save_Settings();
        bool Save_Or_Discard(bool forced = false);

        void Discard_Changes(QDialog*);

		void setStateActionsEnabled(bool enabled);
        void Change_The_Icon(Virtual_Machine*,QString);
		void Update_VM_Ui( bool update_info_tab = true );
		void Update_VM_Port_Number();
		void Update_Info_Text( int info_mode = 0 );
		void Update_Disabled_Controls();
		void Update_Recent_CD_ROM_Images_List();
		void Update_Recent_Floppy_Images_List();
        void Computer_Type_Changed();
        void Update_Machine_Accelerators();
        void Update_Accelerator_Options();
        void Update_Computer_Types();
		
		QString Get_Storage_Device_Info_String( const QString &path );
		
		bool Load_Virtual_Machines();
		bool Save_Virtual_Machines();
		
		QString Get_QEMU_Args();
		QString Get_Current_Binary_Name();
		bool Boot_Is_Correct( Virtual_Machine *tmp_vm );
		bool No_Device_Found( const QString &name, const QString &path, VM::Boot_Device type );
		
		QString Copy_VM_Hard_Drive( const QString &vm_name, const QString &hd_name, const VM_HDD &hd );
		QString Copy_VM_Floppy( const QString &vm_name, const QString &fd_name, const VM_Storage_Device &fd );
		
		Ui::Main_Window ui;
		Ui::Advanced_Options ui_ao;
		Ui::KVM_Options ui_kvm;
        Ui::Architecture_Options ui_arch;

        QDialog* Advanced_Options;
        QDialog* Accelerator_Options;
        QDialog* Architecture_Options;
        Settings_Widget* Display_Settings_Widget;
        Settings_Widget* Media_Settings_Widget;
        Settings_Widget* Network_Settings_Widget;

		QMenu *Icon_Menu; // Context menu for vm icons
		QMenu *VM_List_Menu; // Context menu for vm list
		QSettings Settings;
		
		bool GUI_User_Mode;
		QString VM_Folder;
		
		QList<Virtual_Machine*> VM_List;
		QList<Emulator> All_Emulators_List; // FIXME use call
		
        SMP_Settings_Window* SMP_Settings;
		
		QList<VM::Boot_Order> Boot_Order_List;
		bool Show_Boot_Menu;
		
		HDD_Image_Info* HDA_Info;
		HDD_Image_Info* HDB_Info;
		HDD_Image_Info* HDC_Info;
		HDD_Image_Info* HDD_Info;
		
		Add_New_Device_Window *Native_Device_Window;
		
		VM_Native_Storage_Device Native_FD0;
		VM_Native_Storage_Device Native_FD1;
		VM_Native_Storage_Device Native_CD_ROM;
		VM_Native_Storage_Device Native_HDA;
		VM_Native_Storage_Device Native_HDB;
		VM_Native_Storage_Device Native_HDC;
		VM_Native_Storage_Device Native_HDD;
		
		Ports_Tab_Widget *Ports_Tab;
		Device_Manager_Widget *Dev_Manager;
        Folder_Sharing_Widget* Folder_Sharing;
		
		Network_Widget *New_Network_Settings_Widget;
		Old_Network_Widget *Old_Network_Settings_Widget;
		
        SPICE_Settings_Widget* SPICE_Widget;

        bool block_VM_changed_signals;
};

#endif
