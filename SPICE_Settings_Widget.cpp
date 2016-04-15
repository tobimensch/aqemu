/****************************************************************************
**
** Copyright (C) 2010 Andrey Rijov <ANDron142@yandex.ru>
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

#include "SPICE_Settings_Widget.h"
#include "QListWidgetItem"
#include "Utils.h"

SPICE_Settings_Widget::SPICE_Settings_Widget( QWidget *parent )
	: QWidget( parent )
{
	ui.setupUi( this );
	
	// Connect slots
	connect( ui.CH_Use_SPICE, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	
	connect( ui.SB_QXL_Num, SIGNAL(valueChanged(int)), this, SIGNAL(State_Changet()) );
	connect( ui.CB_RAM_Size, SIGNAL(currentIndexChanged(int)), this, SIGNAL(State_Changet()) );
	
	connect( ui.Edit_SPICE_Port, SIGNAL(textChanged(const QString &)), this, SIGNAL(State_Changet()) );
	connect( ui.CH_SPICE_SPort, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	connect( ui.Edit_SPICE_SPort, SIGNAL(textChanged(const QString &)), this, SIGNAL(State_Changet()) );
	connect( ui.CH_SPICE_Host, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	connect( ui.Edit_SPICE_Host, SIGNAL(textChanged(const QString &)), this, SIGNAL(State_Changet()) );
	
	connect( ui.CH_Use_Image_Compression, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	connect( ui.CB_Image_Compression, SIGNAL(currentIndexChanged(int)), this, SIGNAL(State_Changet()) );
	
	connect( ui.CH_Set_Renderer, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	connect( ui.TB_Up, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	connect( ui.TB_Down, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	
	connect( ui.CH_Use_Video_Compression, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	connect( ui.CH_Use_Playback_Compression, SIGNAL(clicked()), this, SIGNAL(State_Changet()) );
	
	connect( ui.RB_No_Password, SIGNAL(toggled(bool)), this, SIGNAL(State_Changet()) );
	connect( ui.Edit_Password, SIGNAL(textChanged(const QString &)), this, SIGNAL(State_Changet()) );
}

const VM_SPICE &SPICE_Settings_Widget::Get_Settings( bool &settingsValidated ) const
{
	static VM_SPICE spiceSettings;
	
	spiceSettings.Use_SPICE( ui.CH_Use_SPICE->isChecked() );
	
	spiceSettings.Set_GXL_Devices_Count( ui.SB_QXL_Num->value() );
	
	bool ok = false;
	unsigned int val = ui.CB_RAM_Size->currentText().toUInt( &ok );
	if( ok ) spiceSettings.Set_RAM_Size( val );
	else
	{
		AQGraphic_Warning( tr("Error"),
						   tr("SPICE RAM size incorrect!") );
		settingsValidated = false;
		return spiceSettings;
	}
	
	val = ui.Edit_SPICE_Port->text().toUInt( &ok );
	if( ok ) spiceSettings.Set_Port( val );
	else
	{
		AQGraphic_Warning( tr("Error"),
						   tr("SPICE port number incorrect!") );
		settingsValidated = false;
		return spiceSettings;
	}
	
	spiceSettings.Use_SPort( ui.CH_SPICE_SPort->isChecked() );
	
	val = ui.Edit_SPICE_SPort->text().toUInt( &ok );
	if( ok ) spiceSettings.Set_SPort( val );
	else
	{
		AQGraphic_Warning( tr("Error"),
						   tr("SPICE SPort number incorrect!") );
		settingsValidated = false;
		return spiceSettings;
	}
	
	spiceSettings.Use_Hostname( ui.CH_SPICE_Host->isChecked() );
	
	spiceSettings.Set_Hostname( ui.Edit_SPICE_Host->text() );
	
	spiceSettings.Use_Image_Compression( ui.CH_Use_Image_Compression->isChecked() );
	
	if( ui.CB_Image_Compression->currentIndex() == 0 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_on );
	else if( ui.CB_Image_Compression->currentIndex() == 1 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_auto_glz );
	else if( ui.CB_Image_Compression->currentIndex() == 2 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_auto_lz );
	else if( ui.CB_Image_Compression->currentIndex() == 3 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_quic );
	else if( ui.CB_Image_Compression->currentIndex() == 4 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_glz );
	else if( ui.CB_Image_Compression->currentIndex() == 5 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_lz );
	else if( ui.CB_Image_Compression->currentIndex() == 6 )
		spiceSettings.Set_Image_Compression( VM::SPICE_IC_Type_off );
	else
	{
		AQGraphic_Warning( tr("Error"),
						   tr("SPICE image compression type invalid!") );
		settingsValidated = false;
		return spiceSettings;
	}
	
	spiceSettings.Use_Renderer( ui.CH_Set_Renderer->isChecked() );
	
	QList<VM::SPICE_Renderer> tmpRendersList;
	for( int ix = 0; ix < ui.Renderer_Order_List->count(); ++ix )
	{
		tmpRendersList << (VM::SPICE_Renderer)ui.Renderer_Order_List->item( ix )->data( Qt::UserRole ).toInt();
	}
	spiceSettings.Set_Renderer_List( tmpRendersList );
	
	spiceSettings.Use_Video_Stream_Compression( ui.CH_Use_Video_Compression->isChecked() );
	spiceSettings.Use_Playback_Compression( ui.CH_Use_Playback_Compression->isChecked() );
	
	spiceSettings.Use_Password( ! ui.RB_No_Password->isChecked() );
	spiceSettings.Set_Password( ui.Edit_Password->text() );
	
	settingsValidated = true;
	return spiceSettings;
}

void SPICE_Settings_Widget::Set_Settings( const VM_SPICE &settings )
{
	ui.CH_Use_SPICE->setChecked( settings.Use_SPICE() );
	
	ui.SB_QXL_Num->setValue( settings.Get_GXL_Devices_Count() );
	ui.CB_RAM_Size->setEditText( QString::number(settings.Get_RAM_Size()) );
	
	ui.Edit_SPICE_Port->setText( QString::number(settings.Get_Port()) );
	ui.CH_SPICE_SPort->setChecked( settings.Use_SPort() );
	ui.Edit_SPICE_SPort->setText( QString::number(settings.Get_SPort()) );
	ui.CH_SPICE_Host->setChecked( settings.Use_Hostname() );
	ui.Edit_SPICE_Host->setText( settings.Get_Hostname() );
	
	ui.CH_Use_Image_Compression->setChecked( settings.Use_Image_Compression() );
	switch( settings.Get_Image_Compression() )
	{
		case VM::SPICE_IC_Type_on:
			ui.CB_Image_Compression->setCurrentIndex( 0 );
			break;
			
		case VM::SPICE_IC_Type_auto_glz:
			ui.CB_Image_Compression->setCurrentIndex( 1 );
			break;
			
		case VM::SPICE_IC_Type_auto_lz:
			ui.CB_Image_Compression->setCurrentIndex( 2 );
			break;
			
		case VM::SPICE_IC_Type_quic:
			ui.CB_Image_Compression->setCurrentIndex( 3 );
			break;
			
		case VM::SPICE_IC_Type_glz:
			ui.CB_Image_Compression->setCurrentIndex( 4 );
			break;
			
		case VM::SPICE_IC_Type_lz:
			ui.CB_Image_Compression->setCurrentIndex( 5 );
			break;
			
		case VM::SPICE_IC_Type_off:
			ui.CB_Image_Compression->setCurrentIndex( 6 );
			break;
			
		default:
			ui.CB_Image_Compression->setCurrentIndex( 0 );
			break;
	}
	
	ui.CH_Set_Renderer->setChecked( settings.Use_Renderer() );
	Show_Renderer_List( settings.Get_Renderer_List() );
	
	ui.CH_Use_Video_Compression->setChecked( settings.Use_Video_Stream_Compression() );
	ui.CH_Use_Playback_Compression->setChecked( settings.Use_Playback_Compression() );
	
	ui.RB_No_Password->setChecked( ! settings.Use_Password() );
	ui.Edit_Password->setText( settings.Get_Password() );
}

void SPICE_Settings_Widget::Set_PSO_GXL( bool use )
{
	ui.Label_QXL->setEnabled( use );
	ui.Label_Number_of_QXL_Devices->setEnabled( use );
	ui.SB_QXL_Num->setEnabled( use );
	ui.Label_RAM_Size->setEnabled( use );
	ui.CB_RAM_Size->setEnabled( use );
}

void SPICE_Settings_Widget::on_TB_Up_clicked()
{
	if( ! ui.Renderer_Order_List->currentItem() ) return;
	if( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) <= 0 ) return;
	
	// Swap
	QString tmpStr = ui.Renderer_Order_List->item(
			ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->text();
	VM::SPICE_Renderer tmpRender = (VM::SPICE_Renderer)ui.Renderer_Order_List->item(
			ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->data( Qt::UserRole ).toInt();
	
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->setText(
			ui.Renderer_Order_List->item(ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())-1)->text() );
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->setData(
			Qt::UserRole, ui.Renderer_Order_List->item(ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())-1)->data(Qt::UserRole) );
	
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())-1 )->setText( tmpStr );
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())-1 )->setData( Qt::UserRole, tmpRender );
	
	// Select item
	ui.Renderer_Order_List->setCurrentRow( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())-1 );
}

void SPICE_Settings_Widget::on_TB_Down_clicked()
{
	if( ! ui.Renderer_Order_List->currentItem() ) return;
	if( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) >= ui.Renderer_Order_List->count()-1 ) return;
	
	// Swap
	QString tmpStr = ui.Renderer_Order_List->item(
			ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->text();
	VM::SPICE_Renderer tmpRender = (VM::SPICE_Renderer)ui.Renderer_Order_List->item(
			ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->data( Qt::UserRole ).toInt();
	
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->setText(
			ui.Renderer_Order_List->item(ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())+1)->text() );
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem()) )->setData(
			Qt::UserRole, ui.Renderer_Order_List->item(ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())+1)->data(Qt::UserRole) );
	
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())+1 )->setText( tmpStr );
	ui.Renderer_Order_List->item( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())+1 )->setData( Qt::UserRole, tmpRender );
	
	// Select item
	ui.Renderer_Order_List->setCurrentRow( ui.Renderer_Order_List->row(ui.Renderer_Order_List->currentItem())+1 );
}
	
void SPICE_Settings_Widget::Show_Renderer_List( const QList<VM::SPICE_Renderer> &list )
{
	ui.Renderer_Order_List->clear();
	
	for( int ix = 0; ix < list.count(); ++ix )
	{
		QString renderStr;
		switch( list[ix] )
		{
			case VM::SPICE_Renderer_cairo:
				renderStr = tr( "cairo" );
				break;
				
			case VM::SPICE_Renderer_oglpbuf:
				renderStr = tr( "oglpbuf" );
				break;
				
			case VM::SPICE_Renderer_oglpixmap:
				renderStr = tr( "oglpixmap" );
				break;
				
			default:
				renderStr = tr( "Error!" );
				AQError( "void SPICE_Settings_Widget::Show_Renderer_List( const QList<VM::SPICE_Renderer> &list )",
						 "Invalid SPICE render type!" );
				break;
		}
		
		QListWidgetItem *tmpItem = new QListWidgetItem( renderStr, ui.Renderer_Order_List, QListWidgetItem::Type );
		tmpItem->setData( Qt::UserRole, list[ix] );
		ui.Renderer_Order_List->addItem( tmpItem );
	}
}
