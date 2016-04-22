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

#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QRegExp>
#include <QSettings>

#include "Utils.h"
#include "VM_Devices.h"
#include "Convert_HDD_Image_Window.h"

// Convert_HDD_Thread -------------------------------------------------------

Convert_HDD_Thread::Convert_HDD_Thread()
{
	Error_Message = "";
}

void Convert_HDD_Thread::Set_Args( const QStringList &args )
{
	Arguments = args;
}

const QString &Convert_HDD_Thread::Get_Error_Message() const
{
	return Error_Message;
}

void Convert_HDD_Thread::run()
{
	QProcess *proc = new QProcess();
	
	QSettings settings;
	proc->start( settings.value("QEMU-IMG_Path", "qemu-img").toString(), Arguments );
	
	if( ! proc->waitForStarted(2000) )
		AQError( "void Convert_HDD_Thread::run()", "Cannot Start qemu-img!" );
	
	if( ! proc->waitForFinished(-1) )
		AQError( "void Convert_HDD_Thread::run()", "Cannot Finish qemu-img!" );
	
	QString err_str = proc->readAllStandardError();
	delete proc;
	
	if( err_str.count() > 0 )
	{
		AQError( "void Convert_HDD_Thread::run()", "qemu-img Send Error String!\nDetalis: " + err_str );
		Error_Message = err_str;
		emit Conversion_Complete( false );
	}
	else
	{
		AQDebug( "void Convert_HDD_Thread::run()", "Conversion complete!" );
		emit Conversion_Complete( true );
	}
}

//===========================================================================

Convert_HDD_Image_Window::Convert_HDD_Image_Window( QWidget *parent )
	: QDialog( parent )
{
	ui.setupUi( this );
	resize( width(), minimumSizeHint().height() );
	
	HDD_Info = new HDD_Image_Info();
	connect( HDD_Info, SIGNAL(Completed(bool)),
			 this, SLOT(Update_Info(bool)) );
	Valid_Info = false;
	
	Possible_Encrypte = false;
	
	if( ! Get_QEMU_IMG_Info() )
	{
		AQWarning( "Convert_HDD_Image_Window::Convert_HDD_Image_Window( QWidget *parent )",
				   "Cannot get qemu-img info!" );
		
		QStringList formats;
		formats << "qcow2" << "qcow" << "vmdk" << "cow" << "raw" << "cloop";
		ui.CB_Output_Format->addItems( formats );
	}
}

void Convert_HDD_Image_Window::on_Button_Browse_Base_clicked()
{
	QString fileName = QFileDialog::getOpenFileName( this, tr("Select Base HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_Base_File_Name->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)") );
	
	if( fileName.isEmpty() ) return;

	fileName = QDir::toNativeSeparators( fileName );
	ui.Edit_Base_File_Name->setText( fileName );
	
	if( ! QFile::exists(fileName) )
	{
		AQGraphic_Warning( tr("Error!"), tr("Cannot Locate Input File!") );
		return;
	}
	
	// Get info about image
	HDD_Info->Update_Disk_Info( fileName );
}

void Convert_HDD_Image_Window::Update_Info( bool ok )
{
	Valid_Info = ok;
	
	if( ok )
	{
		ui.Label_HDD_Info_L->setText( tr("Image Format: %1\nAllocated Disk Space: %2 %3")
									  .arg(HDD_Info->Get_Disk_Info().Disk_Format)
									  .arg(HDD_Info->Get_Disk_Info().Disk_Size.Size)
									  .arg(Get_TR_Size_Suffix(HDD_Info->Get_Disk_Info().Disk_Size)) );
		
		ui.Label_HDD_Info_R->setText( tr("Virtual Size: %1 %2\nCluster Size: %3")
									  .arg(HDD_Info->Get_Disk_Info().Virtual_Size.Size)
									  .arg(Get_TR_Size_Suffix(HDD_Info->Get_Disk_Info().Virtual_Size))
									  .arg(HDD_Info->Get_Disk_Info().Cluster_Size) );
	}
	else
	{
		ui.Label_HDD_Info_L->setText( "Image Format: none\nAllocated Disk Space: 0" );
		ui.Label_HDD_Info_R->setText( "Virtual Size: 0\nCluster Size: 0" );
	}
}

void Convert_HDD_Image_Window::on_Button_Browse_Output_clicked()
{
	QString fileName = QFileDialog::getSaveFileName( this, tr("Save Out HDD Image File"),
													 Get_Last_Dir_Path(ui.Edit_Output_File_Name->text()),
													 tr("All Files (*);;Images Files (*.img *.qcow *.qcow2 *.wmdk)") );
	
	if( ! fileName.isEmpty() )
		ui.Edit_Output_File_Name->setText( QDir::toNativeSeparators(fileName) );
}

void Convert_HDD_Image_Window::on_CB_Output_Format_currentIndexChanged( const QString &text )
{
	if( text.contains("qcow", Qt::CaseInsensitive) )
	{
		ui.CH_Compressed->setEnabled( true );
		
		if( Possible_Encrypte )
			ui.CH_Encrypted->setEnabled( true );
	}
	else
	{
		ui.CH_Compressed->setEnabled( false );
		ui.CH_Encrypted->setEnabled( false );
	}
}

void Convert_HDD_Image_Window::on_Button_Convert_clicked()
{
	if( ui.Edit_Output_File_Name->text().isEmpty() )
	{
		AQGraphic_Warning( tr("File Name is Empty!"), tr("Please Enter Correct HDD Image filename!") );
		return;
	}
	
	if( ! Valid_Info )
	{
		AQGraphic_Warning( tr("Error!"), tr("Cannot get a valid format for current HDD image!") );
		return;
	}
	
	QStringList args;
	args << "convert";
	
	// compressed
	if( ui.CH_Compressed->isEnabled() && 
		ui.CH_Compressed->isChecked() ) args << "-c";
	
	// encrypted
	if( ui.CH_Encrypted->isEnabled() &&
		ui.CH_Encrypted->isChecked() ) args << "-e";
	
	args << "-f" << HDD_Info->Get_Disk_Info().Disk_Format; // Input format
	
	args << ui.Edit_Base_File_Name->text(); // In file name
	
	args << "-O" << ui.CB_Output_Format->currentText(); // Output format
	
	args << ui.Edit_Output_File_Name->text(); // Output File name
	
	connect( &Conv_Thread, SIGNAL(Conversion_Complete(bool)),
			 this, SLOT(Conversion_Done(bool)) );
	
	Conv_Thread.Set_Args( args );
	Conv_Thread.start();
	
	ProgDial = new QProgressDialog( tr("Please Wait. Converting HDD Image..."), tr("Cancel"), 0, 0, this );
	
	connect( ProgDial, SIGNAL(canceled()),
			 this, SLOT(Cancel_Convertion()) );
	
	ProgDial->setWindowModality( Qt::WindowModal );
	ProgDial->setValue( 0 );
	
	ProgDial->exec();
}

void Convert_HDD_Image_Window::Conversion_Done( bool ok )
{
	if( ok )
	{
		ProgDial->accept();
		Conv_Thread.terminate();
		QMessageBox::information( this, tr("Information:"), tr("Conversion completed sucessfuly!") );
		accept();
	}
	else
	{
		ProgDial->accept();
		Conv_Thread.terminate();
		AQGraphic_Warning( tr("Error!"),
						   tr("Cannot convert image!\nDetails:\n%1").arg(Conv_Thread.Get_Error_Message()) );
	}
}

void Convert_HDD_Image_Window::Cancel_Convertion()
{
	Conv_Thread.terminate();
}

bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()
{
	// Start process
	QSettings settings;
	QProcess *qemuImgProc = new QProcess( this );
	qemuImgProc->start( settings.value("QEMU-IMG_Path", "qemu-img").toString(), QStringList("-h") );
	
	if( ! qemuImgProc->waitForStarted(2000) )
		AQError( "bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()",
				 "Cannot start qemu-img!" );
	
	if( ! qemuImgProc->waitForFinished(3000) )
		AQError( "bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()",
				 "Cannot finish qemu-img!" );
	
	// Read all output text
	QString allText = qemuImgProc->readAllStandardError();
	allText += qemuImgProc->readAllStandardOutput();
	
	delete qemuImgProc;
	
	if( allText.isEmpty() )
	{
		AQError( "bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()",
				 "qemu-img not send text!" );
		return false;
	}
	else // Parse text
	{
		// Encryption
		QRegExp possibleEncrypte = QRegExp( "-e\\s+" );
		if( possibleEncrypte.exactMatch(allText) ) Possible_Encrypte = true;
		else Possible_Encrypte = false;
		
		// Formats
		QRegExp formats = QRegExp( ".*Supported formats:\\s+(.*)\n" );
		if( ! formats.exactMatch(allText) )
		{
			AQError( "bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()",
					 "Cannot match RegExp!" );
			return false;
		}
		
		QStringList tmpList = formats.capturedTexts();
		if( tmpList.count() < 2 )
		{
			AQError( "bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()",
					 "Captured text lines < 2" );
			return false;
		}
		
		QStringList formatsList = tmpList[ 1 ].split( ' ', QString::SkipEmptyParts );
		if( formatsList.isEmpty() )
		{
			AQError( "bool Convert_HDD_Image_Window::Get_QEMU_IMG_Info()",
					 "Formats list is empty!" );
			return false;
		}
		
		// Add formats
		formatsList.sort();
		ui.CB_Output_Format->addItems( formatsList );
		
		// qcow2 - is default format
		int qcow2 = ui.CB_Output_Format->findText( "qcow2" );
		if( qcow2 != -1 ) ui.CB_Output_Format->setCurrentIndex( qcow2 );
		else
		{
			// qcow2 not available use qcow
			int qcow = ui.CB_Output_Format->findText( "qcow" );
			if( qcow != -1 ) ui.CB_Output_Format->setCurrentIndex( qcow );
		}
		
		return true;
	}
}
