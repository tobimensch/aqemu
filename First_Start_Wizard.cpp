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

#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QTranslator>
#include <QFileDialog>
#include <QProgressDialog>

#include "First_Start_Wizard.h"
#include "Utils.h"
#include "System_Info.h"
#include "Settings_Window.h"
#include "Advanced_Settings_Window.h"
#include "Edit_Emulator_Version_Window.h"
#include "Emulator_Options_Window.h"

First_Start_Wizard::First_Start_Wizard( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	
	Emulators_Find_Done = false;
	Next_Move = true;
	
	retranslateUi();
	Load_Settings();
}

bool First_Start_Wizard::Find_Emulators()
{
	on_Button_Find_Emulators_clicked();
	//Update_Emulators_List(); // FIXME
	return Save_Settings();
}

void First_Start_Wizard::on_Button_Cancel_clicked()
{
	if( QMessageBox::information(this, tr("Warning!"), tr("Are you sure? You can configure these options later in Settings windows."),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
	{
		reject();
	}
}

void First_Start_Wizard::on_Button_Back_clicked()
{
	Next_Move = false;

	if( ui.All_Pages->currentWidget() == ui.Find_Emulators_Page ||
		ui.All_Pages->currentWidget() == ui.Add_Emulator_Page )
		ui.All_Pages->setCurrentWidget( ui.General_Settings_Page );
	else
		ui.All_Pages->setCurrentIndex( ui.All_Pages->currentIndex() -1 );

	ui.Button_Next->setEnabled( true );
}

void First_Start_Wizard::on_Button_Next_clicked()
{
	if( ui.All_Pages->currentWidget() == ui.Welcome_Page )
	{
		retranslateUi();
	}
	else if( ui.All_Pages->currentWidget() == ui.General_Settings_Page )
	{
		#ifdef Q_OS_WIN32
		ui.All_Pages->setCurrentWidget( ui.Add_Emulator_Page );
		#else
		ui.All_Pages->setCurrentWidget( ui.Find_Emulators_Page );
		#endif

		return;
	}
	else if( ui.All_Pages->currentIndex() == ui.All_Pages->count()-1 )
	{
		if( Save_Settings() )
		{
			Settings_Window *sw = new Settings_Window();
			sw->on_Button_OK_clicked();
			delete sw;
			
			Advanced_Settings_Window *asw = new Advanced_Settings_Window();
			asw->on_Button_OK_clicked();
			delete asw;
			
			accept();
		}
	}
	
	Next_Move = true;
	ui.All_Pages->setCurrentIndex( ui.All_Pages->currentIndex() +1 );
	
	if( ui.All_Pages->currentWidget() == ui.Find_Emulators_Page )
	{
		if( ! Emulators_Find_Done ) ui.Button_Next->setEnabled( false );
	}
}

void First_Start_Wizard::on_Edit_VM_Dir_textChanged()
{
	ui.Button_Next->setEnabled( ! ui.Edit_VM_Dir->text().isEmpty() );
}

void First_Start_Wizard::on_TB_Browse_VM_Dir_clicked()
{
	QString folder = QFileDialog::getExistingDirectory( this, tr("Set Folder for you VM's"),
														Settings.value("VM_Directory", "~").toString() );
	
	if( ! folder.isEmpty() )
	{
		if( ! (folder.endsWith("/") || folder.endsWith("\\")) )
			folder += "/";
		
		ui.Edit_VM_Dir->setText( QDir::toNativeSeparators(folder) );
	}
}

void First_Start_Wizard::on_Button_Find_Emulators_clicked()
{
	Emulators_Find_Done = true;
	ui.Button_Next->setEnabled( true );
	
	// Clear old emulators list and remove emulators files
	ui.Edit_Enulators_List->clear();
	
	// Remove old files
	Remove_All_Emulators_Files();
	
	// Find emualtors files
	// Get environment values
	QStringList sys_env = QProcess::systemEnvironment();
	if( sys_env.count() <= 0 )
	{
		AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
				 "Cannot Get System Environment Information!" );
		return;
	}
	else
	{
		// Find PATH
		QStringList paths;
		for( int ix = 0; ix < sys_env.count(); ix++ )
		{
			if( sys_env[ix].startsWith("PATH=") )
			{
				QString tmp = sys_env[ ix ].remove( "PATH=" );
				paths = tmp.split( ":", QString::SkipEmptyParts );
				break;
			}
		}
		
		// Delete /usr/bin/X11/ from PATH's list
		for( int ix = 0; ix < paths.count(); ix++ )
		{
			if( paths[ix].contains("/usr/bin/X11") ) paths.removeAt( ix );
		}
		
		// Remove duplicates
		paths.removeDuplicates();
		
		// Add / to line end
		for( int ix = 0; ix < paths.count(); ++ix )
		{
			paths[ ix ] = QDir::toNativeSeparators( (paths[ix].endsWith("/") || paths[ix].endsWith("\\"))
													? paths[ix]
													: paths[ix] + "/" );
		}
		
		if( paths.count() <= 0 )
		{
			AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
					 "Cannot Find PATH in System Environment Information!" );
			return;
		}
		else
		{
			// Find QEMU-IMG
			bool qemuIMG_Finded = false;
			
			for( int ix = 0; ix < paths.count(); ++ix )
			{
				if( QFile::exists(paths[ix] + "qemu-img") )
				{
					Settings.setValue( "QEMU-IMG_Path", paths[ix] + "qemu-img" );
					qemuIMG_Finded = true;
					break;
				}
				else if( QFile::exists(paths[ix] + "kvm-img") )
				{
					Settings.setValue( "QEMU-IMG_Path", paths[ix] + "kvm-img" );
					qemuIMG_Finded = true;
					break;
				}
				else if( QFile::exists(paths[ix] + "qemu-img-kvm") )
				{
					Settings.setValue( "QEMU-IMG_Path", paths[ix] + "qemu-img-kvm" );
					qemuIMG_Finded = true;
					break;
				}
				else if( QFile::exists(paths[ix] + "qemu-kvm-img") )
				{
					Settings.setValue( "QEMU-IMG_Path", paths[ix] + "qemu-kvm-img" );
					qemuIMG_Finded = true;
					break;
				}
			}
			
			if( qemuIMG_Finded )
				AQDebug( "void First_Start_Wizard::on_Button_Find_Emulators_clicked()",
						 "qemu-img find on: " + Settings.value("QEMU-IMG_Path","").toString() );
			else
				AQError( "void First_Start_Wizard::on_Button_Find_Emulators_clicked()",
						 "Cannot find qemu-img!" );
			
			// Find QEMU
			QList<Emulator> qemuEmulatorsList;
			
			for( int qx = 0; qx < paths.count(); ++qx )
			{
				QMap<QString, QString> qemu_list = System_Info::Find_QEMU_Binary_Files( paths[qx] );
				
				// Found emulators files in this dir?				
				bool qemuBinFilesFound = false;
				for( QMap<QString, QString>::const_iterator it = qemu_list.constBegin(); it != qemu_list.constEnd(); ++it )
				{
					if( ! it.value().isEmpty() )
					{
						qemuBinFilesFound = true;
						break;
					}
				}
				
				if( ! qemuBinFilesFound )
				{
					AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "In " + paths[qx] + " QEMU Not Found" );
					continue;
				}
				
				// Bin files found. Work...
				AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
						 "QEMU Found. Path: " + paths[qx] );
				
				// Check Version
				VM::Emulator_Version qemu_version = VM::Obsolete;
				
				QMap<QString, QString>::const_iterator iter = qemu_list.constBegin();
				while( iter != qemu_list.constEnd() )
				{
					if( QFile::exists(iter.value()) )
						qemu_version = System_Info::Get_Emulator_Version( iter.value() );
					
					if( qemu_version != VM::Obsolete ) break;
					
					iter++;
				}
				
				if( qemu_version == VM::Obsolete )
				{
					AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "Cannot Get QEMU Version! Using Default: 0.10.0" );
					
					qemu_version = VM::QEMU_0_10;
				}
				
				// Get emulator info
				int allEmulBinCount = qemu_list.count();
				QProgressDialog progressWin( tr("Search..."), tr("Cancel"), 0, allEmulBinCount, this );
				progressWin.setWindowModality( Qt::WindowModal );
				
				QMap<QString, Available_Devices> devList;
				
				iter = qemu_list.constBegin();
				for( int emulBinIndex = 0; iter != qemu_list.constEnd(); ++emulBinIndex )
				{
					progressWin.setValue( emulBinIndex );
					
					if( ! iter.value().isEmpty() )
					{
						bool ok = false;
						Available_Devices tmpDev = System_Info::Get_Emulator_Info( iter.value(), &ok, qemu_version, iter.key() );
						
						if( ok )
							devList[ iter.key() ] = tmpDev;
						else
							AQGraphic_Warning( "void First_Start_Wizard::on_Button_Find_clicked()", tr("Error!"),
												tr("Cannot get emulator info! For file: %1").arg(iter.value()) );
					}
					++iter;
					
					if( progressWin.wasCanceled() ) break;
				}
				
				progressWin.setValue( allEmulBinCount );
				
				// Create new emulator
				Emulator emul;
				
				// Emulator name
				QString emulName = Emulator_Version_To_String( qemu_version );
				int emulDublicateNameCount = 1;
				for( int ix = 0; ix < qemuEmulatorsList.count(); ++ix )
				{
					if( emulName == qemuEmulatorsList[ix].Get_Name() )
					{
						++emulDublicateNameCount;
						emulName = QString("%1 #%2").arg( Emulator_Version_To_String(qemu_version) )
													.arg( emulDublicateNameCount );
						ix = 0;
					}
				}
				emul.Set_Name( emulName );
				
				emul.Set_Type( VM::QEMU );
				emul.Set_Version( qemu_version );
				emul.Set_Path( paths[qx] );
				emul.Set_Devices( devList );
				emul.Set_Binary_Files( qemu_list );
				emul.Set_Check_Version( false );
				emul.Set_Check_Available_Options( false );
				emul.Set_Force_Version( false );
				
				qemuEmulatorsList << emul;
				
				// Add Text
				ui.Edit_Enulators_List->appendPlainText( tr("QEMU Found in \"%1\", version: %2").
														 arg(paths[qx]).
														 arg(Emulator_Version_To_String(qemu_version)) );
			}
			
			// Find KVM
			QList<Emulator> kvmEmulatorsList;
			
			for( int kx = 0; kx < paths.count(); ++kx )
			{
				QMap<QString, QString> kvm_list = System_Info::Find_KVM_Binary_Files( paths[kx] );
				
				// Found emulators files in this dir?				
				bool kvmBinFilesFound = false;
				for( QMap<QString, QString>::const_iterator it = kvm_list.constBegin(); it != kvm_list.constEnd(); ++it )
				{
					if( ! it.value().isEmpty() )
					{
						kvmBinFilesFound = true;
						break;
					}
				}
				
				if( ! kvmBinFilesFound )
				{
					AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "In " + paths[kx] + " KVM Not Found" );
					continue;
				}
				
				// Bin files found. Work...
				AQDebug( "void First_Start_Wizard::on_Button_Find_clicked()",
						 "KVM Found. Path: " + paths[kx] );
				
				// Check Version
				VM::Emulator_Version kvm_version = VM::Obsolete;
				
				QMap<QString, QString>::const_iterator iter = kvm_list.constBegin();
				while( iter != kvm_list.constEnd() )
				{
					if( QFile::exists(iter.value()) )
						kvm_version = System_Info::Get_Emulator_Version( iter.value() );
					
					if( kvm_version != VM::Obsolete ) break;
					
					++iter;
				}
				
				if( kvm_version == VM::Obsolete )
				{
					AQError( "void First_Start_Wizard::on_Button_Find_clicked()",
							 "Cannot Get KVM Version! Using Default: 8X" );
					
					kvm_version = VM::KVM_8X;
				}
				
				// Get emulator info
				QMap<QString, Available_Devices> devList;
				
				iter = kvm_list.constBegin();
				while( iter != kvm_list.constEnd() )
				{
					if( ! iter.value().isEmpty() )
					{
						bool ok = false;
						Available_Devices tmpDev = System_Info::Get_Emulator_Info( iter.value(), &ok, kvm_version, iter.key() );
						
						if( ok )
							devList[ iter.key() ] = tmpDev;
						else
							AQGraphic_Warning( tr("Error!"), tr("Cannot get emulator info! For file: %1").arg(iter.value()) );
					}
					
					++iter;
				}
				
				// Create new emulator
				Emulator emul;
				
				// Emulator name
				QString emulName = Emulator_Version_To_String( kvm_version );
				int emulDublicateNameCount = 1;
				for( int ix = 0; ix < qemuEmulatorsList.count(); ++ix )
				{
					if( emulName == qemuEmulatorsList[ix].Get_Name() )
					{
						++emulDublicateNameCount;
						emulName = QString("%1 #%2").arg( Emulator_Version_To_String(kvm_version) )
													.arg( emulDublicateNameCount );
						ix = 0;
					}
				}
				emul.Set_Name( emulName );
				
				emul.Set_Type( VM::KVM );
				emul.Set_Version( kvm_version );
				emul.Set_Path( paths[kx] );
				emul.Set_Devices( devList );
				emul.Set_Binary_Files( kvm_list );
				emul.Set_Check_Version( false );
				emul.Set_Check_Available_Options( false );
				emul.Set_Force_Version( false );
				
				kvmEmulatorsList << emul;
				
				// Add Text
				ui.Edit_Enulators_List->appendPlainText( tr("KVM Found in \"%1\", version: %2").
														 arg(paths[kx]).
														 arg(Emulator_Version_To_String(kvm_version)) );
			}
			
			// Set default emulators
			if( qemuEmulatorsList.count() > 0 || kvmEmulatorsList.count() > 0 )
			{
				// Enable Edit Emulator Version Manualy Button
				ui.Button_Edit->setEnabled( true );
				
				// Find and set QEMU default emulator
				if( qemuEmulatorsList.count() > 1 )
				{
					VM::Emulator_Version maxVer = VM::Obsolete;
					int maxVerIndex = 0;
					for( int ix = 0; ix < qemuEmulatorsList.count(); ++ix )
					{
						if( qemuEmulatorsList[ix].Get_Version() > maxVer )
							maxVerIndex = ix;
					}
					
					qemuEmulatorsList[ maxVerIndex ].Set_Default( true );
					
					// Save emulators
					for( int ix = 0; ix < qemuEmulatorsList.count(); ++ix )
						qemuEmulatorsList[ ix ].Save();
				}
				else if( qemuEmulatorsList.count() > 0 )
				{
					qemuEmulatorsList[ 0 ].Set_Default( true );
					qemuEmulatorsList[ 0 ].Save();
				}
				
				// Find and set KVM default emulator
				if( kvmEmulatorsList.count() > 1 )
				{
					VM::Emulator_Version maxVer = VM::Obsolete;
					int maxVerIndex = 0;
					for( int ix = 0; ix < kvmEmulatorsList.count(); ++ix )
					{
						if( kvmEmulatorsList[ix].Get_Version() > maxVer )
							maxVerIndex = ix;
					}
					
					kvmEmulatorsList[ maxVerIndex ].Set_Default( true );
					
					// Save emulators
					for( int ix = 0; ix < kvmEmulatorsList.count(); ++ix )
						kvmEmulatorsList[ ix ].Save();
				}
				else if( kvmEmulatorsList.count() > 0 )
				{
					kvmEmulatorsList[ 0 ].Set_Default( true );
					kvmEmulatorsList[ 0 ].Save();
				}
			}
		}
	}
}

void First_Start_Wizard::on_Button_Skip_Find_clicked()
{
	ui.Button_Next->setEnabled( true );
}

void First_Start_Wizard::on_Button_Edit_clicked()
{
	Edit_Emulator_Version_Window *edit_win = new Edit_Emulator_Version_Window();
	
	if( edit_win->exec() == QDialog::Accepted )
	{
		// Update Emulators Info Text
		ui.Edit_Enulators_List->setPlainText( tr("Emulator Version Modified By User") );
	}
	
	delete edit_win;
}

#ifdef Q_OS_WIN32
void First_Start_Wizard::on_TB_Add_Emulator_Browse_clicked()
{
	QString emulatorDirPath = QFileDialog::getExistingDirectory( this, tr("Select QEMU emulator directory"),
																 Get_Last_Dir_Path(ui.Edit_Add_Emulator_Path->text()) );

	if( ! emulatorDirPath.isEmpty() )
		ui.Edit_Add_Emulator_Path->setText( QDir::toNativeSeparators(emulatorDirPath) );
}

void First_Start_Wizard::on_Button_Add_Emulator_Find_clicked()
{

}

void First_Start_Wizard::on_Button_Add_Emulator_Manual_Mode_clicked()
{
	Emulator_Options_Window *emulatorOptionsWin = new Emulator_Options_Window( this );

	Emul.Set_Name( ui.CB_Add_Emulator_Version->currentText() );
	Emul.Set_Path( ui.Edit_Add_Emulator_Path->text() );
	Emul.Set_Version( String_To_Emulator_Version(ui.CB_Add_Emulator_Version->currentText()) );
	Emul.Set_Force_Version( true );
	emulatorOptionsWin->Set_Emulator( Emul );

	if( emulatorOptionsWin->exec() == QDialog::Accepted )
	{
		Emul = emulatorOptionsWin->Get_Emulator();

		ui.Edit_Add_Emulator_Path->setText( Emul.Get_Path() );
		ui.CB_Add_Emulator_Version->setEditText( Emulator_Version_To_String(Emul.Get_Version()) );
	}

	delete emulatorOptionsWin;
}
#endif

void First_Start_Wizard::on_All_Pages_currentChanged( int index )
{
	// Back, Next Buttons State
	if( index == 0 )
		ui.Button_Back->setEnabled( false );
	else if( index == ui.All_Pages->count()-1 )
		ui.Button_Next->setText( tr("&Finish") );
	else
	{
		ui.Button_Back->setEnabled( true );
		ui.Button_Next->setText( tr("&Next") );
	}
	
	// Set Header Text
	ui.Label_Caption->setText( Header_Captions[ui.All_Pages->currentIndex()] );
	
	// Pages Actions
	if( ui.All_Pages->currentWidget() == ui.General_Settings_Page && Next_Move )
		retranslateUi();
}

void First_Start_Wizard::Load_Settings()
{
	// Find All Language Files (*.qm)
	QDir data_dir( Settings.value("AQEMU_Data_Folder", "/usr/share/aqemu/").toString() );
	QFileInfoList lang_files = data_dir.entryInfoList( QStringList("*.qm"), QDir::Files, QDir::Name );
	
	if( lang_files.count() > 0 )
	{
		// Add Languages to List
		for( int dd = 0; dd < lang_files.count(); ++dd )
		{
			ui.CB_Language->addItem( lang_files[dd].completeBaseName() );
			
			if( lang_files[dd].completeBaseName() == Settings.value( "Language", "en" ).toString() )
				ui.CB_Language->setCurrentIndex( dd + 1 ); // First Item 'English'
		}
	}
	
	// Virtual Machines Folder
	#ifdef Q_OS_WIN32
	ui.Edit_Add_Emulator_Path->setText( QDir::toNativeSeparators(QDir::currentPath() + "/QEMU/") );
	ui.Edit_VM_Dir->setText( QDir::toNativeSeparators(Settings.value("VM_Directory", QDir::homePath() + "/AQEMU_VM/").toString()) );
	#else
	ui.Edit_VM_Dir->setText( QDir::toNativeSeparators(Settings.value("VM_Directory", QDir::homePath() + "/.aqemu/").toString()) );
	#endif

	// Use Device Manager
	ui.CH_Device_Manager->setChecked( Settings.value("Use_Device_Manager", "no").toString() == "yes" );
	
	// Use VNC Embedded Display
	#ifdef VNC_DISPLAY
	ui.Label_Embedded_VNC->setEnabled( true );
	ui.CH_Embedded_VNC->setEnabled( true );
	ui.CH_Embedded_VNC->setChecked( Settings.value("Use_VNC_Display", "no").toString() == "yes" );
	#endif
}

bool First_Start_Wizard::Save_Settings()
{
	// Virtual Machines Folder
	QDir dir;
	
	ui.Edit_VM_Dir->setText( QDir::toNativeSeparators(ui.Edit_VM_Dir->text()) );

	if( ! (ui.Edit_VM_Dir->text().endsWith("/") || ui.Edit_VM_Dir->text().endsWith("\\")) )
		ui.Edit_VM_Dir->setText( ui.Edit_VM_Dir->text() + QDir::toNativeSeparators("/") );
	
	if( dir.exists(ui.Edit_VM_Dir->text()) )
	{
		if( ! dir.exists(ui.Edit_VM_Dir->text() + QDir::toNativeSeparators("/os_templates/")) )
			dir.mkdir( ui.Edit_VM_Dir->text() + QDir::toNativeSeparators("/os_templates/") );
	}
	else if( ! (dir.mkdir(ui.Edit_VM_Dir->text()) && dir.mkdir(ui.Edit_VM_Dir->text() + QDir::toNativeSeparators("/os_templates/"))) )
	{
		AQGraphic_Warning( tr("Error!"), tr("Cannot Create New Folder!") );
		return false;
	}
	
	Settings.setValue( "VM_Directory", ui.Edit_VM_Dir->text() );
	
	// Use Device Manager
	if( ui.CH_Device_Manager->isChecked() )
		Settings.setValue( "Use_Device_Manager", "yes" );
	else
		Settings.setValue( "Use_Device_Manager", "no" );
	
	// Use VNC Embedded Display
	#ifdef VNC_DISPLAY
	if( ui.CH_Embedded_VNC->isChecked() )
		Settings.setValue( "Use_VNC_Display", "yes" );
	else
		Settings.setValue( "Use_VNC_Display", "no" );
	#endif
	
	// Interface Language
	if( ui.CB_Language->currentIndex() == 0 )
		Settings.setValue( "Language", "en" );
	else
		Settings.setValue( "Language", ui.CB_Language->itemText(ui.CB_Language->currentIndex()) );
	
	// Off First Start
	Settings.setValue( "First_Start", "no" );
	
	#ifdef Q_OS_WIN32
	// Check emulator
	if( Emul.Get_Name().isEmpty() )
	{
		Emul.Set_Name( ui.CB_Add_Emulator_Version->currentText() );
		Emul.Set_Path( ui.Edit_Add_Emulator_Path->text() );
		Emul.Set_Version( String_To_Emulator_Version(ui.CB_Add_Emulator_Version->currentText()) );
		Emul.Set_Force_Version( true );
		
		bool foundEmulBinary = false;
		bool searchAgain = true;
		
		while( true )
		{
			QList<QString> emulBinPaths = Emul.Get_Binary_Files().values();
			for( int ix = 0; ix < emulBinPaths.count(); ++ix )
			{
				if( QFile::exists(emulBinPaths[ix]) )
				{
					foundEmulBinary = true;
					break;
				}
			}
			
			if( ! foundEmulBinary )
			{
				if( ! searchAgain ) break;
				
				searchAgain = false;
				Emul.Set_Binary_Files( System_Info::Find_QEMU_Binary_Files(ui.Edit_Add_Emulator_Path->text()) );
			}
			else break;
		}
		
		if( ! foundEmulBinary )
		{
			AQGraphic_Warning( tr("Error"), tr("Emulator binary files not exists! Please click Back and set it manual.") );
			return false;
		}
		
		// qemu-img
		if( ! QFile::exists(Settings.value("QEMU-IMG_Path", "").toString()) )
		{
			QString qemuImgPath = (ui.Edit_Add_Emulator_Path->text().endsWith("/") || ui.Edit_Add_Emulator_Path->text().endsWith("\\"))
								  ? ui.Edit_Add_Emulator_Path->text() + "qemu-img.exe"
								  : ui.Edit_Add_Emulator_Path->text() + QDir::toNativeSeparators("/qemu-img.exe");
			
			if( QFile::exists(qemuImgPath) )
				Settings.setValue( "QEMU-IMG_Path", QDir::toNativeSeparators(qemuImgPath) );
			else
				AQGraphic_Warning( tr("Warning!"),
								   tr("Cannot find qemu-img.exe! You most set it path in: File->Advanced Settings->Advanced->qemu-img path") );
		}
	}
	
	Remove_All_Emulators_Files();
	Emul.Save();
	#endif
	
	return true;
}

void First_Start_Wizard::retranslateUi()
{
	// Load Language
	QTranslator appTranslator;
	
	if( ui.CB_Language->currentIndex() != 0 )
	{
		if( appTranslator.load(Settings.value("AQEMU_Data_Folder", "").toString() +
							   ui.CB_Language->itemText(ui.CB_Language->currentIndex()) + ".qm") )
		{
			QCoreApplication::installTranslator( &appTranslator );
			
			AQDebug( "void First_Start_Wizard::retranslateUi()",
					 "Translation File\"" + Settings.value("AQEMU_Data_Folder", "").toString() +
					 ui.CB_Language->itemText(ui.CB_Language->currentIndex()) + ".qm" + "\" Loaded" );
		}
		else AQError( "void First_Start_Wizard::retranslateUi()", "Cannot Load Translations!" );
	}
	
	Header_Captions.clear();
	Header_Captions << tr( "Welcome" );
	Header_Captions << tr( "General Settings" );
	Header_Captions << tr( "Find Emulators" );
	Header_Captions << tr( "Setup Emulator" );
	Header_Captions << tr( "Finish" );
	
	ui.Label_Caption->setText( Header_Captions[0] );
	
	ui.Button_Back->setText( tr("&Back") );
	ui.Button_Next->setText( tr("&Next") );
	ui.Button_Cancel->setText( tr("&Cancel") );
	
	ui.Label_Welcome_Text->setText( tr("Welcome to AQEMU settings wizard!\nThis wizard will help you to choose options AQEMU needed to work correctly.\nPush the \"Next\" button to go to next page or \"Back\" button to go back to previous page.") );
	ui.Label_Select_Language->setText( tr("Here you can choose interface language") );
	ui.Label_VM_Dir->setText( tr("Please select folder for AQEMU virtual machines") );
	ui.Label_Device_Manager->setText( tr("If the \"Device manager\" mode is active, storage devices (Floppy, CD/DVD, HDD) will be displayed in virtual machine as icons, like in file manager window.") );
	ui.CH_Device_Manager->setText( tr("Use Device manager") );
	ui.Label_Embedded_VNC->setText( tr("New experimental work mode is available in AQEMU. In this mode QEMU/KVM display will be showed in AQEMU main window. Note that this mode is still unstable so here may be some problems. If you want to enable this feature you must mark the \"Include Emdedded VNC Display in Main Window\" flag.") );
	ui.CH_Embedded_VNC->setText( tr("Include Emdedded VNC Display in Main Window") );
	ui.Label_Find_Emulators->setText( tr("For correct working AQEMU must know your emulators locations and versions. For searching push the \"Search\" button. If automatic search can't find out your emulators, you should configure AQEMU yourself later. You can do it in \"File->Advanced Settings\" menu.") );
	ui.Button_Find_Emulators->setText( tr("&Search") );
	ui.Button_Skip_Find->setText( tr("S&kip Search") );
	ui.Button_Edit->setText( tr("Set &Versions Manualy") );
	ui.Label_Add_Emulator_Help->setText( "Help text (not writed...)" );
	ui.Label_Add_Emulator_Path->setText( "Path to emulator directory" );
	ui.Label_Add_Emulator_Version->setText( "Emulator version" );
	ui.Button_Add_Emulator_Find->setText( "Start &searching" );
	ui.Button_Add_Emulator_Manual_Mode->setText( "Show all settings..." );
	ui.Label_Finish_Text->setText( tr("Congratulations! The Wizard has got all necessary settings for configuring AQEMU. Push \"Finish\" button to save your settings.") );
}
