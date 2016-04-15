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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDateTime>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>
#include <QRegExp>
#include <QProcess>
#include <QStringList>
#include <QTextStream>

#ifdef Q_OS_WIN32
#include <iostream>
#include <windows.h>
HANDLE Console_HANDLE = GetStdHandle( STD_OUTPUT_HANDLE );
#else
#include <QDebug>
#endif

#include "Utils.h"
#include "System_Info.h"

static uint Messages_Index = 0;

static bool Save_Messages_To_Log = true;
static QString Log_Path = "";

static bool Use_Stdout;
static bool Stdout_Debug;
static bool Stdout_Warning;
static bool Stdout_Error;

static QStringList Recent_CD_Images;
static QStringList Recent_FDD_Images;

void AQDebug( const QString &sender, const QString &mes )
{
	if( Use_Stdout && Stdout_Debug )
	{
	#ifdef Q_OS_WIN32
		SetConsoleTextAttribute( Console_HANDLE, 10 );
		std::cout << QString( "\nAQEMU Debug [%1] >>>\nSender: %2\nMessage: %3" )
							  .arg(Messages_Index).arg(sender).arg(mes).toStdString();
	#else
		qDebug( qPrintable(QString(
			"\n\33[32mAQEMU Debug\33[0m [%1] >>>\n\33[32mSender:\33[0m %2\n\33[32mMessage:\33[0m %3")
			.arg(Messages_Index).arg(sender).arg(mes)) );
	#endif
	}
	
	if( Save_Messages_To_Log && Stdout_Debug )
		AQSave_To_Log( "Debug", sender, mes );
	
	Messages_Index++;
}

void AQWarning( const QString &sender, const QString &mes )
{
	if( Use_Stdout && Stdout_Warning )
	{
	#ifdef Q_OS_WIN32
		SetConsoleTextAttribute( Console_HANDLE, 14 );
		std::cout << QString( "\nAQEMU Warning [%1] >>>\nSender: %2\nMessage: %3" )
							  .arg(Messages_Index).arg(sender).arg(mes).toStdString();
	#else
		qDebug( qPrintable(QString(
			"\n\33[34mAQEMU Warning\33[0m [%1] >>>\n\33[34mSender:\33[0m %2\n\33[34mMessage:\33[0m %3")
			.arg(Messages_Index).arg(sender).arg(mes)) );
	#endif
	}
	
	if( Save_Messages_To_Log && Stdout_Warning )
		AQSave_To_Log( "Warning", sender, mes );
	
	Messages_Index++;
}

void AQError( const QString &sender, const QString &mes )
{
	if( Use_Stdout && Stdout_Error )
	{
	#ifdef Q_OS_WIN32
		SetConsoleTextAttribute( Console_HANDLE, 12 );
		std::cout << QString( "\nAQEMU Error [%1] >>>\nSender: %2\nMessage: %3" )
							  .arg(Messages_Index).arg(sender).arg(mes).toStdString();
	#else
		qDebug( qPrintable(QString(
			"\n\33[31mAQEMU Error\33[0m [%1] >>>\n\33[31mSender:\33[0m %2\n\33[31mMessage:\33[0m %3")
			.arg(Messages_Index).arg(sender).arg(mes)) );
	#endif
	}
	
	if( Save_Messages_To_Log && Stdout_Error )
		AQSave_To_Log( "Error", sender, mes );
	
	Messages_Index++;
}

void AQGraphic_Warning( const QString &caption, const QString &mes )
{
	QMessageBox::warning( NULL, caption, mes, QMessageBox::Ok );
}

void AQGraphic_Warning( const QString &sender, const QString &caption, const QString &mes, bool fatal )
{
	if( fatal )
	{
		QMessageBox::warning( NULL, caption,
					QString("Sender: %1\nMessage: %2\n").arg(sender).arg(mes) +
					QObject::tr("This Fatal Error. Recomend Close AQEMU."),
					QMessageBox::Ok );
	}
	else
	{
		QMessageBox::warning( NULL, caption,
					QString("Sender: %1\nMessage: %2").arg(sender).arg(mes),
					QMessageBox::Ok );
	}
	
	if( Save_Messages_To_Log )
		AQSave_To_Log( "Warning", sender, mes );
}

void AQGraphic_Error( const QString &sender, const QString &caption, const QString &mes, bool fatal )
{
	if( fatal )
	{
		QMessageBox::critical( NULL, caption,
					QString("Sender: %1\nMessage: %2\n").arg(sender).arg(mes) +
					QObject::tr("Fatal error. It's recommended to close AQEMU"),
					QMessageBox::Ok );
	}
	else
	{
		QMessageBox::critical( NULL, caption,
					QString("Sender: %1\nMessage: %2").arg(sender).arg(mes),
					QMessageBox::Ok );
	}
	
	if( Save_Messages_To_Log )
		AQSave_To_Log( "Error", sender, mes );
}

void AQUse_Log( bool use )
{
	Save_Messages_To_Log = use;
}

void AQUse_Debug_Output( bool use, bool d, bool w, bool e )
{
	Use_Stdout = use;
	Stdout_Debug = d;
	Stdout_Warning = w;
	Stdout_Error = e;
}

void AQLog_Path( const QString& path )
{
	Log_Path = path;
}

void AQSave_To_Log( const QString &mes_type, const QString &sender, const QString &mes )
{
	if( Log_Path.isEmpty() ) return;

	QFile log_file( Log_Path );
	
	if( ! log_file.open(QIODevice::Append | QIODevice::Text) )
	{
		AQUse_Log( false ); // off loging
		AQError( "void AQSave_To_Log( const QString& mes_type, const QString& sender, const QString& mes )",
				 "Cannot Open Log file to Write! Log Path: \"" + Log_Path + "\"" );
	}
	else
	{
		QTextStream out( &log_file );
		out << "Type: " << mes_type << " Num: " << Messages_Index << "\nDate: "
			<< QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss zzz")
			<< "\nSender: " << sender << "\nMessage: " << mes << "\n\n";
	}
}

bool Create_New_HDD_Image( bool encrypted, const QString &base_image,
						   const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )
{
	// Create command line
	QStringList args;
	args << "create";
	
	if( encrypted )
		args << "-e";
	
	if( ! base_image.isEmpty() )
		args << "-b" << base_image;
	
	args << "-f" << format;
	
	args << file_name;
	 
	switch( size.Suffix )
	{
		case VM::Size_Suf_Mb: // MB
			args << QString::number( (long)(size.Size * 1024) );
			break;
			
		case VM::Size_Suf_Gb: // GB
			args << QString::number( (long)(size.Size * 1024) ) + "M";
			break;
			
		default: // KG
			args << QString::number( (long)size.Size );
			break;
	}
	
	// Start qemu-img process
	QProcess qemu_img;
	QSettings settings;
	qemu_img.start( settings.value("QEMU-IMG_Path", "qemu-img").toString(), args );
	
	if( ! qemu_img.waitForStarted(2000) )
	{
		AQGraphic_Error( "bool Create_New_HDD_Image( bool encrypted, const QString &base_image,"
						 "const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )",
						 QObject::tr("Error!"), QObject::tr("Cannot Start qemu-img! Image isn't Created!") );
		return false;
	}
	
	if( ! qemu_img.waitForFinished(10000) )
	{
		AQGraphic_Error( "bool Create_New_HDD_Image( bool encrypted, const QString &base_image,"
						 "const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )",
						 QObject::tr("Error!"), QObject::tr("qemu-img Cannot Finish! Image isn't Created!") );
		return false;
	}
	else
	{
		QByteArray err = qemu_img.readAllStandardError();
		QByteArray out = qemu_img.readAllStandardOutput();
		
		if( err.count() > 0 )
		{
			AQGraphic_Error( "bool Create_New_HDD_Image( bool encrypted, const QString &base_image,"
							 "const QString &file_name, const QString &format, VM::Device_Size size, bool verbose )",
							 QObject::tr("Error!"), QObject::tr("Cannot Create Image!\nInformation: ") + err );
		}
		
		QRegExp rx( "Format*ing*fmt*size*" );
		rx.setPatternSyntax( QRegExp::Wildcard );
		
		if( verbose )
		{
			if( rx.exactMatch( out ) )
			{
				QMessageBox::information( NULL, QObject::tr("Complete!"),
						QObject::tr("QEMU-IMG is Creates HDD Image.\nAdditional Information:\n") + out );
			}
			else
			{
				QMessageBox::information( NULL, QObject::tr("Warning!"),
						 QObject::tr("QEMU-IMG is Returned non Standard Message!.\nAdditional Information:\n") + out );
			}
		}
		
		return true;
	}
}

bool Create_New_HDD_Image( const QString &file_name, VM::Device_Size size )
{
	QSettings settings;
	QString format = settings.value( "Default_HDD_Image_Format", "qcow2" ).toString();
	
	return Create_New_HDD_Image( false, "", file_name, format, size, false );
}

bool Format_HDD_Image( const QString &file_name, VM::Disk_Info info )
{
	if( file_name.isEmpty() )
	{
		AQError( "bool Format_HDD_Image( const QString &file_name )",
				 "File Name is Empty!" );
		
		return false;
	}
	
	VM_HDD tmp_hd = VM_HDD( true, file_name );
	tmp_hd.Set_Disk_Info( info );
	QString hd_format = tmp_hd.Get_Image_Format();
	
	if( hd_format.isEmpty() )
	{
		AQError( "bool Format_HDD_Image( const QString &file_name )",
				 "Format is Empty!" );
		
		return false;
	}
	
	return Create_New_HDD_Image( false, "", file_name, hd_format, tmp_hd.Get_Virtual_Size(), false );
}

QList<QString> Get_Templates_List()
{
	QList<QString> all_templates;
	QSettings settings;
	
	// VM Templates
	QDir sys_templates_dir( QDir::toNativeSeparators(settings.value("AQEMU_Data_Folder", "").toString() + "/os_templates/") );
	
	QFileInfoList sys_templates_list = sys_templates_dir.entryInfoList(
			QStringList("*.aqvmt"), QDir::Files, QDir::Unsorted );
	
	QDir user_templates_dir( QDir::toNativeSeparators(settings.value("VM_Directory", "").toString() + "/os_templates/") );
	
	QFileInfoList user_templates_list = user_templates_dir.entryInfoList(
			QStringList("*.aqvmt"), QDir::Files, QDir::Unsorted );
	
	for( int tx = 0; tx < sys_templates_list.count(); ++tx )
	{
		for( int ux = 0; ux < user_templates_list.count(); ++ux )
		{
			if( sys_templates_list[tx].completeBaseName() ==
						 user_templates_list[ux].completeBaseName() )
			{
				sys_templates_list.takeAt( tx ); // delete system template
				tx -= 1;
				ux = user_templates_list.count();
			}
		}
	}
	
	// OK. In Template Lists Only Unique Values
	for( int ix = 0; ix < sys_templates_list.count(); ++ix )
		all_templates.append( sys_templates_list[ix].absoluteFilePath() );
	
	for( int ix = 0; ix < user_templates_list.count(); ++ix )
		all_templates.append( user_templates_list[ix].absoluteFilePath() );
	
	// sort
	qSort( all_templates );
	
	return all_templates;
}


QString Get_FS_Compatible_VM_Name( const QString &name )
{
	//QRegExp vm_name_val = QRegExp( "[^a-zA-Z0-9_]" ); // old style
	QRegExp vm_name_val = QRegExp( "[^\\w\\.]" );
	
	QString tmp = name;
	tmp = tmp.replace( vm_name_val, "_" );
	
	return tmp.replace( "__", "_" );
}

QString Get_Complete_VM_File_Path( const QString &vm_name )
{
	//QRegExp vm_name_val = QRegExp( "[^a-zA-Z0-9_]" ); // old style
	QRegExp vm_name_val = QRegExp( "[^\\w]" );
	
	QString tmp = vm_name;
	tmp = tmp.replace( vm_name_val, "_" );
	
	QString new_name = tmp.replace( "__", "_" );
	QString tmp_str = new_name;
	
	QSettings settings;
	
	QString ret_str = settings.value("VM_Directory", "").toString() + tmp_str;
	
	if( ! ret_str.endsWith(".aqemu") )
		ret_str += ".aqemu";
	
	for( int ix = 0; QFile::exists(ret_str); ++ix )
		tmp_str = new_name + QString::number( ix );
	
	return ret_str;
}

QString Get_TR_Size_Suffix( VM::Device_Size suf )
{
	switch( suf.Suffix )
	{
		case VM::Size_Suf_Kb:
			return QObject::tr("Kb");
				
		case VM::Size_Suf_Mb:
			return QObject::tr("Mb");
				
		case VM::Size_Suf_Gb:
			return QObject::tr("Gb");
				
		default:
			AQError( "QString Get_TR_Size_Suffix( VM::Device_Size suf )",
					 "Virtual Size Suffix Default Section!" );
			return "";
	}
}

QString Get_Last_Dir_Path( const QString &path )
{
	QFileInfo info( path );
	QString dir = info.path();
	
	if( dir.isEmpty() ) return "/";
	else return dir;
}

bool It_Host_Device( const QString &path )
{
	#ifdef Q_OS_WIN32
	// FIXME
	return false;
	#else
	if( path.startsWith("/dev/") ) return true;
	else return false;
	#endif
}

void Check_AQEMU_Permissions()
{
	QSettings settings;
	QFileInfo test_perm;
	
	#ifndef Q_OS_WIN32
	// This Section For Unix Like OS.
	test_perm = QFileInfo( settings.fileName() );
	
	if( test_perm.exists() )
	{
		if( ! test_perm.isWritable() )
		{
			AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
							 QObject::tr("AQEMU Config File is Read Only!\nCheck Permissions For File: ") +
							 settings.fileName(), true );
		}
	}
	#endif
	
	// Check VM Dir Permissions
	test_perm = QFileInfo( settings.value("VM_Directory", "~").toString() );
	
	if( test_perm.exists() )
	{
		if( ! test_perm.isWritable() )
		{
			AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
							 QObject::tr("AQEMU VM Directory is Read Only!\nCheck Permissions For: ") +
							 settings.value("VM_Directory", "~").toString(), true );
		}
	}
	
	// Check VM Templates Dir Permissions
	test_perm = QFileInfo( settings.value("VM_Directory", "~").toString() + "os_templates" );
	
	if( test_perm.exists() )
	{
		if( ! test_perm.isWritable() )
		{
			AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
							 QObject::tr("AQEMU VM Template Directory is Read Only!\nCheck Permissions For: ") +
							 settings.value("VM_Directory", "~").toString() + "os_templates", true );
		}
	}
	
	// Check AQEMU Log File Permissions
	if( ! settings.value("Log/Log_Path", "").toString().isEmpty() )
	{
		test_perm = QFileInfo( settings.value("Log/Log_Path", "").toString() );
		
		if( test_perm.exists() )
		{
			if( ! test_perm.isWritable() )
			{
				AQGraphic_Error( "void Check_AQEMU_Permissions()", QObject::tr("Error!"),
								 QObject::tr("AQEMU Log File is Read Only!\nCheck Permissions For File: ") +
								 settings.value("Log/Log_Path", "").toString(), false );
			}
		}
	}
}

VM::Emulator_Version String_To_Emulator_Version( const QString &str )
{
	if( str == "QEMU 0.9.0" ) return VM::QEMU_0_9_0;
	else if( str == "QEMU 0.9.1" ) return VM::QEMU_0_9_1;
	else if( str == "QEMU 0.10.X" ) return VM::QEMU_0_10;
	else if( str == "QEMU 0.11.X" ) return VM::QEMU_0_11;
	else if( str == "QEMU 0.12.X" ) return VM::QEMU_0_12;
	else if( str == "QEMU 0.13.X" ) return VM::QEMU_0_13;
	else if( str == "QEMU 0.14.X" ) return VM::QEMU_0_14;
	else if( str == "KVM 7X" ) return VM::KVM_7X;
	else if( str == "KVM 8X" ) return VM::KVM_8X;
	else if( str == "KVM 0.11.X" ) return VM::KVM_0_11;
	else if( str == "KVM 0.12.X" ) return VM::KVM_0_12;
	else if( str == "KVM 0.13.X" ) return VM::KVM_0_13;
	else if( str == "KVM 0.14.X" ) return VM::KVM_0_14;
	else if( str == "Obsolete" ) return VM::Obsolete;
	else
	{
		AQError( "VM::Emulator_Version String_To_Emulator_Version( const QString &str )",
				 QString("Emulator version \"%1\" not valid!").arg(str) );
		return VM::Obsolete;
	}
}

QString Emulator_Version_To_String( VM::Emulator_Version ver )
{
	switch( ver )
	{
		case VM::QEMU_0_9_0:
			return "QEMU 0.9.0";
			
		case VM::QEMU_0_9_1:
			return "QEMU 0.9.1";
			
		case VM::QEMU_0_10:
			return "QEMU 0.10.X";
			
		case VM::QEMU_0_11:
			return "QEMU 0.11.X";
			
		case VM::QEMU_0_12:
			return "QEMU 0.12.X";
			
		case VM::QEMU_0_13:
			return "QEMU 0.13.X";
			
		case VM::QEMU_0_14:
			return "QEMU 0.14.X";
			
		case VM::KVM_7X:
			return "KVM 7X";
			
		case VM::KVM_8X:
			return "KVM 8X";
			
		case VM::KVM_0_11:
			return "KVM 0.11.X";
			
		case VM::KVM_0_12:
			return "KVM 0.12.X";
			
		case VM::KVM_0_13:
			return "KVM 0.13.X";
			
		case VM::KVM_0_14:
			return "KVM 0.14.X";
			
		case VM::Obsolete:
			return "Obsolete";
			
		default:
			AQError( "QString Emulator_Version_To_String( VM::Emulator_Version ver )",
					 QString("Emulator version \"%1\" not valid!").arg((int)ver) );
			return "";
	}
}

static QList<Emulator> Emulators_List;
static QList<Emulator> Empty_Emul_List;
static Emulator Empty_Emul;

bool Update_Emulators_List()
{
	// Clear old emulator list
	Emulators_List.clear();
	
	// Get dir path
	QSettings settings;
	QFileInfo info( settings.fileName() );
	QString aqemuSettingsFolder = info.absoluteDir().absolutePath();
	if( ! (aqemuSettingsFolder.endsWith("/") || aqemuSettingsFolder.endsWith("\\")) )
		aqemuSettingsFolder = QDir::toNativeSeparators( aqemuSettingsFolder + "/" );
	
	if( ! QFile::exists(aqemuSettingsFolder) )
	{
		AQError( "bool Update_Emulators_List()",
				 QString("Cannot get path for save emulator! Folder \"%1\" not exists!").arg(aqemuSettingsFolder) );
		return false;
	}
	
	// Get all *.emulators files
	QDir emulDir( aqemuSettingsFolder );
	QStringList emulFiles = emulDir.entryList( QStringList("*.emulator"), QDir::Files, QDir::Name );
	
	if( emulFiles.isEmpty() )
	{
		AQWarning( "bool Update_Emulators_List()",
				   QString("No emulators found in \"%1\"").arg(aqemuSettingsFolder) );
		return false;
	}
	
	// Check default emulators
	bool qDef = false, kDef = false;
	bool qInst = false, kInst = false; // Installed?
	
	// Load emulators
	for( int ex = 0; ex < emulFiles.count(); ++ex )
	{
		Emulator tmp;
		
		if( ! tmp.Load(aqemuSettingsFolder + emulFiles[ex]) )
		{
			AQError( "bool Update_Emulators_List()",
					 QString("Cannot load emulator from file: \"%1\"").arg(emulFiles[ex]) );
			continue;
		}
		
		// Installed?
		if( tmp.Get_Type() == VM::QEMU ) qInst = true;
		else if( tmp.Get_Type() == VM::KVM ) kInst = true;
		
		// Default?
		if( tmp.Get_Default() )
		{
			if( tmp.Get_Type() == VM::QEMU )
			{
				if( qDef )
				{
					AQWarning( "bool Update_Emulators_List()",
							   "Default QEMU emulator already loaded." );
					tmp.Set_Default( false );
				}
				else qDef = true;
			}
			else if( tmp.Get_Type() == VM::KVM )
			{
				if( kDef )
				{
					AQWarning( "bool Update_Emulators_List()",
							   "Default KVM emulator already loaded." );
					tmp.Set_Default( false );
				}
				else kDef = true;
			}
		}
		
		// Update available options?
		if( tmp.Get_Check_Available_Options() )
		{
			QMap<QString, QString> tmpBinFiles = tmp.Get_Binary_Files();
			QMap<QString, Available_Devices> tmpDevMap; // result
			
			for( QMap<QString, QString>::const_iterator it = tmpBinFiles.constBegin(); it != tmpBinFiles.constEnd(); ++it )
			{
				if( ! it.value().isEmpty() )
				{
					bool ok = false;
					Available_Devices tmpDev = System_Info::Get_Emulator_Info( it.value(), &ok, tmp.Get_Version(), it.key() );
					
					if( ! ok )
					{
						AQError( "bool Update_Emulators_List()",
								 "Cannot set new emulator available options!" );
						continue;
					}
					
					// Adding device
					tmpDevMap[ it.key() ] = tmpDev;
				}
			}
			
			// Set all devices
			tmp.Set_Devices( tmpDevMap );
		}
		
		// Check version?
		if( tmp.Get_Check_Version() )
		{
			// Get bin file path
			QString binFilePath = "";
			QMap<QString, QString> tmpBinFiles = tmp.Get_Binary_Files();
			for( QMap<QString, QString>::const_iterator it = tmpBinFiles.constBegin(); it != tmpBinFiles.constEnd(); ++it )
			{
				if( QFile::exists(it.value()) )
				{
					binFilePath = it.value();
					break;
				}
			}
			
			if( binFilePath.isEmpty() )
			{
				AQError( "bool Update_Emulators_List()",
						 QString("Cannot find exists emulator binary file for emulator \"%1\"!").arg(tmp.Get_Name()) );
			}
			
			// All OK, check version
			tmp.Set_Version( System_Info::Get_Emulator_Version(binFilePath) );
		}
		
		// Adding emulator
		Emulators_List << tmp;
	}
	
	// Emulator loaded?
	if( Emulators_List.isEmpty() )
	{
		AQWarning( "bool Update_Emulators_List()",
				   "No emulators loaded!" );
		return false;
	}
	
	// Check defaults
	if( qInst == true && qDef == false )
	{
		for( int ex = 0; ex < Emulators_List.count(); ++ex )
		{
			if( Emulators_List[ex].Get_Type() == VM::QEMU )
			{
				Emulators_List[ex].Set_Default( true );
				break;
			}
		}
	}
	
	if( kInst == true && kDef == false )
	{
		for( int ex = 0; ex < Emulators_List.count(); ++ex )
		{
			if( Emulators_List[ex].Get_Type() == VM::KVM )
			{
				Emulators_List[ex].Set_Default( true );
				break;
			}
		}
	}
	
	// All OK
	return true;
}

const QList<Emulator> &Get_Emulators_List()
{
	if( Update_Emulators_List() ) return Emulators_List; // FIXME Update
	else
	{
		AQError( "QList<Emulator> Get_Emulators_List()",
				 "Cannot Update Emulators List" );
		return Empty_Emul_List;
	}
}

bool Remove_All_Emulators_Files()
{
	// Get emulators dir path
	QSettings settings;
	QFileInfo info( settings.fileName() );
	QString aqemuSettingsFolder = info.absoluteDir().absolutePath();
	aqemuSettingsFolder += aqemuSettingsFolder.endsWith( QDir::toNativeSeparators("/") )
						   ? ""
						   : QDir::toNativeSeparators( "/" );
	
	if( ! QFile::exists(aqemuSettingsFolder) )
	{
		AQError( "bool Remove_All_Emulators_Files()",
				 QString("Cannot get path for save emulator! Folder \"%1\" not exists!").arg(aqemuSettingsFolder) );
		return false;
	}
	else
	{
		// Get all *.emulators files
		QDir emulDir( aqemuSettingsFolder );
		QStringList emulFiles = emulDir.entryList( QStringList("*.emulator"), QDir::Files, QDir::Name );
		
		bool allFilesRemoved = true;
		for( int dx = 0; dx < emulFiles.count(); ++dx )
		{
			if( ! QFile::remove(aqemuSettingsFolder + emulFiles[dx]) )
			{
				AQError( "bool Remove_All_Emulators_Files()",
						 QString("Cannot delete file \"%1\"!").arg(emulFiles[dx]) );
				allFilesRemoved = false;
			}
		}
		
		return allFilesRemoved;
	}
}

const Emulator &Get_Default_Emulator( VM::Emulator_Type type )
{
	if( Emulators_List.count() <= 0 )
		AQError( "const Emulator &Get_Default_Emulator( VM::Emulator_Type type )",
				 "Emulator Count == 0" );
	else
	{
		for( int ix = 0; ix < Emulators_List.count(); ix++ )
		{
			if( Emulators_List[ix].Get_Type() == type && Emulators_List[ix].Get_Default() )
				return Emulators_List[ ix ];
		}
	}
	
	AQWarning( "const Emulator &Get_Default_Emulator( VM::Emulator_Type type )",
			   "Cannot Find!" );
	return Empty_Emul;
}

const Emulator &Get_Emulator_By_Name( const QString &name )
{
	if( Emulators_List.count() <= 0 )
		AQError( "const Emulator Get_Emulator_By_Name( const QString &name )",
				 "Emulator Count == 0" );
	else
	{
		for( int ix = 0; ix < Emulators_List.count(); ix++ )
		{
			if( Emulators_List[ix].Get_Name() == name )
				return Emulators_List[ ix ];
		}
	}
	
	AQWarning( "const Emulator Get_Emulator_By_Name( const QString &name )",
			   "Cannot Find!" );
	return Empty_Emul;
}

int Get_Random( int min, int max )
{
	if( min < 0 || max > RAND_MAX || min > max )
	{
		return -1;
	}
	
	qsrand( QTime::currentTime().msec() );
	
	return int( qrand() / (RAND_MAX + 1.0) * (max + 1 - min) + min );
}

void Load_Recent_Images_List()
{
	QSettings settings;
	
	// CD
	int max_cd = settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	Recent_CD_Images.clear();
	
	for( int ix = 0; ix < max_cd; ++ix )
	{
		QString tmp = settings.value( "CD_ROM_Exits_Images/item" + QString::number(ix), "" ).toString();
		
		if( ! tmp.isEmpty() ) Recent_CD_Images << tmp;
	}
	
	// FDD
	int max_fdd = settings.value( "Floppy_Exits_Images/Max", "5" ).toString().toInt();
	
	Recent_FDD_Images.clear();
	
	for( int ix = 0; ix < max_fdd; ++ix )
	{
		QString tmp = settings.value( "Floppy_Exits_Images/item" + QString::number(ix), "" ).toString();
		
		if( ! tmp.isEmpty() ) Recent_FDD_Images << tmp;
	}
}

const QStringList &Get_CD_Recent_Images_List()
{
	return Recent_CD_Images;
}

void Add_To_Recent_CD_Files( const QString &path )
{
	QSettings settings;
	int max = settings.value( "CD_ROM_Exits_Images/Max", "5" ).toString().toInt();
	
	// This Unique Path?
	for( int fx = 0; fx < Recent_CD_Images.count() && fx < max; ++fx )
	{
		if( Recent_CD_Images[fx] == path )
		{
			AQDebug( "void Add_To_Recent_CD_Files( const QString &path )",
					 "CD-ROM Path Not Unique." );
			
			// Up it path in list
			if( fx < Recent_CD_Images.count()-1 )
			{
				// swap items
				QString tmp = Recent_CD_Images[ fx+1 ];
				Recent_CD_Images[ fx+1 ] = Recent_CD_Images[ fx ];
				Recent_CD_Images[ fx ] = tmp;
			}
			else return;
		}
	}
	
	// Add to List
	if( Recent_CD_Images.count() < max )
	{
		Recent_CD_Images << path;
		settings.setValue( "CD_ROM_Exits_Images/item" + QString::number(Recent_CD_Images.count()-1), path );
	}
	else
	{
		// Delete first element and add new to end
		for( int ix = 0; ix < Recent_CD_Images.count() -1 && ix < max -1; ++ix )
		{
			Recent_CD_Images[ ix ] = Recent_CD_Images[ ix +1 ];
		}
		
		Recent_CD_Images[ max -1 ] = path;
		
		// Save Items
		for( int ix = 0; ix < Recent_CD_Images.count(); ix++ )
		{
			settings.setValue( "CD_ROM_Exits_Images/item" + QString::number(ix), Recent_CD_Images[ix] );
		}
	}
}

const QStringList &Get_FDD_Recent_Images_List()
{
	return Recent_FDD_Images;
}

void Add_To_Recent_FDD_Files( const QString &path )
{
	QSettings settings;
	int max = settings.value( "Floppy_Exits_Images/Max", "5" ).toString().toInt();
	
	// This Unique Path?
	for( int fx = 0; fx < Recent_FDD_Images.count() && fx < max; ++fx )
	{
		if( Recent_FDD_Images[fx] == path )
		{
			AQDebug( "void Add_To_Recent_FDD_Files( const QString &path )",
					 "Floppy Path Not Unique." );
			
			// Up it path in list
			if( fx < Recent_FDD_Images.count()-1 )
			{
				// swap items
				QString tmp = Recent_FDD_Images[ fx+1 ];
				Recent_FDD_Images[ fx+1 ] = Recent_FDD_Images[ fx ];
				Recent_FDD_Images[ fx ] = tmp;
			}
			else return;
		}
	}
	
	// Add to List
	if( Recent_FDD_Images.count() < max )
	{
		Recent_FDD_Images << path;
		settings.setValue( "Floppy_Exits_Images/item" + QString::number(Recent_FDD_Images.count()-1), path );
	}
	else
	{
		// Delete first element and add new to end
		for( int ix = 0; ix < Recent_FDD_Images.count() -1 && ix < max -1; ++ix )
		{
			Recent_FDD_Images[ ix ] = Recent_FDD_Images[ ix +1 ];
		}
		
		Recent_FDD_Images[ max -1 ] = path;
		
		// Save Items
		for( int ix = 0; ix < Recent_FDD_Images.count(); ix++ )
		{
			settings.setValue( "Floppy_Exits_Images/item" + QString::number(ix), Recent_FDD_Images[ix] );
		}
	}
}

static bool Show_Error_Window;

bool Get_Show_Error_Window()
{
	return Show_Error_Window;
}

void Set_Show_Error_Window( bool show )
{
	Show_Error_Window = show;
}
