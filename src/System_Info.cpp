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

#include <QSettings>
#include <QRegExp>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include "Utils.h"
#include "System_Info.h"

System_Info::System_Info()
{
}

bool System_Info::Update_VM_Computers_List()
{
	Available_Devices tmp_sys;
	
	// Devices Lists
	QList<Device_Map> CPU_x86;
	QList<Device_Map> CPU_ARM;
	QList<Device_Map> CPU_MIPS;
	QList<Device_Map> CPU_PPC;
	QList<Device_Map> CPU_Sparc;
	
	QList<Device_Map> Machine_x86;
	QList<Device_Map> Machine_ARM;
	QList<Device_Map> Machine_M68K;
	QList<Device_Map> Machine_MIPS;
	QList<Device_Map> Machine_PPC;
	QList<Device_Map> Machine_SH4;
	QList<Device_Map> Machine_Sparc;
	
	QList<Device_Map> Network_Card_x86;
	QList<Device_Map> Network_Card_x86_v090;
	QList<Device_Map> Network_Card_ARM;
	QList<Device_Map> Network_Card_M68K;
	QList<Device_Map> Network_Card_MIPS;
	QList<Device_Map> Network_Card_PPC;
	QList<Device_Map> Network_Card_Sparc;
	
	QList<Device_Map> Video_Card_Standart;
	QList<Device_Map> Video_Card_x86;
	
	VM::Sound_Cards Audio_Card_x86;
	VM::Sound_Cards Audio_Card_PPC;
	
	CPU_x86 << Device_Map( "QEMU 32Bit", "qemu32" );
	CPU_x86 << Device_Map( "Intel 486", "486" );
	CPU_x86 << Device_Map( "Pentium", "pentium" );
	CPU_x86 << Device_Map( "Pentium 2", "pentium2" );
	CPU_x86 << Device_Map( "Pentium 3", "pentium3" );
	
	CPU_ARM << Device_Map( "ARM 926", "arm926" );
	CPU_ARM << Device_Map( "ARM 946", "arm946" );
	CPU_ARM << Device_Map( "ARM 1026", "arm1026" );
	CPU_ARM << Device_Map( "ARM 1136", "arm1136" );
	CPU_ARM << Device_Map( "ARM 11 MPCORE", "arm11mpcore" );
	CPU_ARM << Device_Map( "Cortex-M3", "cortex-m3" );
	CPU_ARM << Device_Map( "Cortex-A8", "cortex-a8" );
	CPU_ARM << Device_Map( "Ti925T", "ti925t" );
	CPU_ARM << Device_Map( "PXA 250", "pxa250" );
	CPU_ARM << Device_Map( "PXA 255", "pxa255" );
	CPU_ARM << Device_Map( "PXA 260", "pxa260" );
	CPU_ARM << Device_Map( "PXA 261", "pxa261" );
	CPU_ARM << Device_Map( "PXA 262", "pxa262" );
	CPU_ARM << Device_Map( "PXA 270", "pxa270" );
	CPU_ARM << Device_Map( "PXA 270-a0", "pxa270-a0" );
	CPU_ARM << Device_Map( "PXA 270-a1", "pxa270-a1" );
	CPU_ARM << Device_Map( "PXA 270-b0", "pxa270-b0" );
	CPU_ARM << Device_Map( "PXA 270-b1", "pxa270-b1" );
	CPU_ARM << Device_Map( "PXA 270-c0", "pxa270-c0" );
	CPU_ARM << Device_Map( "PXA 270-c5", "pxa270-c5" );
	CPU_ARM << Device_Map( "Any", "any" );
	
	CPU_MIPS << Device_Map( "4Kc", "4Kc" );
	CPU_MIPS << Device_Map( "4Km", "4Km" );
	CPU_MIPS << Device_Map( "4KEcR1", "4KEcR1" );
	CPU_MIPS << Device_Map( "4KEmR1", "4KEmR1" );
	CPU_MIPS << Device_Map( "4KEc", "4KEc" );
	CPU_MIPS << Device_Map( "4KEm", "4KEm" );
	CPU_MIPS << Device_Map( "24Kc", "24Kc" );
	CPU_MIPS << Device_Map( "24Kf", "24Kf" );
	CPU_MIPS << Device_Map( "34Kf", "34Kf" );
	
	CPU_PPC << Device_Map( "401", "401" );
	CPU_PPC << Device_Map( "401A1", "401A1" );
	CPU_PPC << Device_Map( "401B2", "401B2" );
	CPU_PPC << Device_Map( "401C2", "401C2" );
	CPU_PPC << Device_Map( "401D2", "401D2" );
	CPU_PPC << Device_Map( "401E2", "401E2" );
	CPU_PPC << Device_Map( "401F2", "401F2" );
	CPU_PPC << Device_Map( "401G2", "401G2" );
	CPU_PPC << Device_Map( "IOP480", "IOP480" );
	CPU_PPC << Device_Map( "Cobra", "Cobra" );
	CPU_PPC << Device_Map( "403", "403" );
	CPU_PPC << Device_Map( "403GA", "403GA" );
	CPU_PPC << Device_Map( "403GB", "403GB" );
	CPU_PPC << Device_Map( "403GC", "403GC" );
	CPU_PPC << Device_Map( "403GCX", "403GCX" );
	CPU_PPC << Device_Map( "405", "405" );
	CPU_PPC << Device_Map( "405D2", "405D2" );
	CPU_PPC << Device_Map( "405D4", "405D4" );
	CPU_PPC << Device_Map( "405CR", "405CR" );
	CPU_PPC << Device_Map( "405CRa", "405CRa" );
	CPU_PPC << Device_Map( "405CRb", "405CRb" );
	CPU_PPC << Device_Map( "405CRc", "405CRc" );
	CPU_PPC << Device_Map( "405EP", "405EP" );
	CPU_PPC << Device_Map( "405EZ", "405EZ" );
	CPU_PPC << Device_Map( "405GP", "405GP" );
	CPU_PPC << Device_Map( "405GPa", "405GPa" );
	CPU_PPC << Device_Map( "405GPb", "405GPb" );
	CPU_PPC << Device_Map( "405GPc", "405GPc" );
	CPU_PPC << Device_Map( "405GPd", "405GPd" );
	CPU_PPC << Device_Map( "405GPe", "405GPe" );
	CPU_PPC << Device_Map( "405GPR", "405GPR" );
	CPU_PPC << Device_Map( "405LP", "405LP" );
	CPU_PPC << Device_Map( "Npe405H", "Npe405H" );
	CPU_PPC << Device_Map( "Npe405H2", "Npe405H2" );
	CPU_PPC << Device_Map( "Npe405L", "Npe405L" );
	CPU_PPC << Device_Map( "Npe4GS3", "Npe4GS3" );
	CPU_PPC << Device_Map( "STB03", "STB03" );
	CPU_PPC << Device_Map( "STB04", "STB04" );
	CPU_PPC << Device_Map( "STB25", "STB25" );
	CPU_PPC << Device_Map( "x2vp4", "x2vp4" );
	CPU_PPC << Device_Map( "x2vp7", "x2vp7" );
	CPU_PPC << Device_Map( "x2vp20", "x2vp20" );
	CPU_PPC << Device_Map( "x2vp50", "x2vp50" );
	CPU_PPC << Device_Map( "MPC52xx", "MPC52xx" );
	CPU_PPC << Device_Map( "MPC82xx", "MPC82xx" );
	CPU_PPC << Device_Map( "PowerQUICC-II", "\"PowerQUICC-II\"" );
	CPU_PPC << Device_Map( "G2", "G2" );
	CPU_PPC << Device_Map( "G2H4", "G2H4" );
	CPU_PPC << Device_Map( "G2GP", "G2GP" );
	CPU_PPC << Device_Map( "G2LS", "G2LS" );
	CPU_PPC << Device_Map( "G2HiP3", "G2HiP3" );
	CPU_PPC << Device_Map( "G2HiP4", "G2HiP4" );
	CPU_PPC << Device_Map( "MPC603", "MPC603" );
	CPU_PPC << Device_Map( "G2le", "G2le" );
	CPU_PPC << Device_Map( "G2leGP", "G2leGP" );
	CPU_PPC << Device_Map( "G2leLS", "G2leLS" );
	CPU_PPC << Device_Map( "G2leGP1", "G2leGP1" );
	CPU_PPC << Device_Map( "G2leGP3", "G2leGP3" );
	CPU_PPC << Device_Map( "MPC8240", "MPC8240" );
	CPU_PPC << Device_Map( "MPC5200", "MPC5200" );
	CPU_PPC << Device_Map( "MPC5200_v10", "MPC5200_v10" );
	CPU_PPC << Device_Map( "MPC5200_v11", "MPC5200_v11" );
	CPU_PPC << Device_Map( "MPC5200_v12", "MPC5200_v12" );
	CPU_PPC << Device_Map( "MPC5200B", "MPC5200B" );
	CPU_PPC << Device_Map( "MPC5200B_v20", "MPC5200B_v20" );
	CPU_PPC << Device_Map( "MPC5200B_v21", "MPC5200B_v21" );
	CPU_PPC << Device_Map( "MPC8241", "MPC8241" );
	CPU_PPC << Device_Map( "MPC8245", "MPC8245" );
	CPU_PPC << Device_Map( "MPC8247", "MPC8247" );
	CPU_PPC << Device_Map( "MPC8248", "MPC8248" );
	CPU_PPC << Device_Map( "MPC8250", "MPC8250" );
	CPU_PPC << Device_Map( "MPC8250_HiP3", "MPC8250_HiP3" );
	CPU_PPC << Device_Map( "MPC8250_HiP4", "MPC8250_HiP4" );
	CPU_PPC << Device_Map( "MPC8255", "MPC8255" );
	CPU_PPC << Device_Map( "MPC8255_HiP3", "MPC8255_HiP3" );
	CPU_PPC << Device_Map( "MPC8255_HiP4", "MPC8255_HiP4" );
	CPU_PPC << Device_Map( "MPC8260", "MPC8260" );
	CPU_PPC << Device_Map( "MPC8260_HiP3", "MPC8260_HiP3" );
	CPU_PPC << Device_Map( "MPC8260_HiP4", "MPC8260_HiP4" );
	CPU_PPC << Device_Map( "MPC8264", "MPC8264" );
	CPU_PPC << Device_Map( "MPC8264_HiP3", "MPC8264_HiP3" );
	CPU_PPC << Device_Map( "MPC8264_HiP4", "MPC8264_HiP4" );
	CPU_PPC << Device_Map( "MPC8265", "MPC8265" );
	CPU_PPC << Device_Map( "MPC8265_HiP3", "MPC8265_HiP3" );
	CPU_PPC << Device_Map( "MPC8265_HiP4", "MPC8265_HiP4" );
	CPU_PPC << Device_Map( "MPC8266", "MPC8266" );
	CPU_PPC << Device_Map( "MPC8266_HiP3", "MPC8266_HiP3" );
	CPU_PPC << Device_Map( "MPC8266_HiP4", "MPC8266_HiP4" );
	CPU_PPC << Device_Map( "MPC8270", "MPC8270" );
	CPU_PPC << Device_Map( "MPC8271", "MPC8271" );
	CPU_PPC << Device_Map( "MPC8272", "MPC8272" );
	CPU_PPC << Device_Map( "MPC8275", "MPC8275" );
	CPU_PPC << Device_Map( "MPC8280", "MPC8280" );
	CPU_PPC << Device_Map( "e200", "e200" );
	CPU_PPC << Device_Map( "e200z5", "e200z5" );
	CPU_PPC << Device_Map( "e200z6", "e200z6" );
	CPU_PPC << Device_Map( "e300", "e300" );
	CPU_PPC << Device_Map( "e300c1", "e300c1" );
	CPU_PPC << Device_Map( "e300c2", "e300c2" );
	CPU_PPC << Device_Map( "e300c3", "e300c3" );
	CPU_PPC << Device_Map( "e300c4", "e300c4" );
	CPU_PPC << Device_Map( "MPC8343A", "MPC8343A" );
	CPU_PPC << Device_Map( "MPC8343EA", "MPC8343EA" );
	CPU_PPC << Device_Map( "MPC8347A", "MPC8347A" );
	CPU_PPC << Device_Map( "MPC8347AT", "MPC8347AT" );
	CPU_PPC << Device_Map( "MPC8347AP", "MPC8347AP" );
	CPU_PPC << Device_Map( "MPC8347EA", "MPC8347EA" );
	CPU_PPC << Device_Map( "MPC8347EAT", "MPC8347EAT" );
	CPU_PPC << Device_Map( "MPC8347EAP", "MPC8347EAP" );
	CPU_PPC << Device_Map( "MPC8349", "MPC8349" );
	CPU_PPC << Device_Map( "MPC8349A", "MPC8349A" );
	CPU_PPC << Device_Map( "MPC8349E", "MPC8349E" );
	CPU_PPC << Device_Map( "MPC8349EA", "MPC8349EA" );
	CPU_PPC << Device_Map( "MPC8377", "MPC8377" );
	CPU_PPC << Device_Map( "MPC8377E", "MPC8377E" );
	CPU_PPC << Device_Map( "MPC8378", "MPC8378" );
	CPU_PPC << Device_Map( "MPC8378E", "MPC8378E" );
	CPU_PPC << Device_Map( "MPC8379", "MPC8379" );
	CPU_PPC << Device_Map( "MPC8379E", "MPC8379E" );
	CPU_PPC << Device_Map( "e500", "e500" );
	CPU_PPC << Device_Map( "e500_v10", "e500_v10" );
	CPU_PPC << Device_Map( "e500_v20", "e500_v20" );
	CPU_PPC << Device_Map( "e500v2", "e500v2" );
	CPU_PPC << Device_Map( "e500v2_v10", "e500v2_v10" );
	CPU_PPC << Device_Map( "e500v2_v20", "e500v2_v20" );
	CPU_PPC << Device_Map( "e500v2_v21", "e500v2_v21" );
	CPU_PPC << Device_Map( "e500v2_v22", "e500v2_v22" );
	CPU_PPC << Device_Map( "e500v2_v30", "e500v2_v30" );
	CPU_PPC << Device_Map( "MPC8533", "MPC8533" );
	CPU_PPC << Device_Map( "MPC8533_v10", "MPC8533_v10" );
	CPU_PPC << Device_Map( "MPC8533_v11", "MPC8533_v11" );
	CPU_PPC << Device_Map( "MPC8533E", "MPC8533E" );
	CPU_PPC << Device_Map( "MPC8533E_v10", "MPC8533E_v10" );
	CPU_PPC << Device_Map( "MPC8533E_v11", "MPC8533E_v11" );
	CPU_PPC << Device_Map( "MPC8540", "MPC8540" );
	CPU_PPC << Device_Map( "MPC8540_v10", "MPC8540_v10" );
	CPU_PPC << Device_Map( "MPC8540_v20", "MPC8540_v20" );
	CPU_PPC << Device_Map( "MPC8540_v21", "MPC8540_v21" );
	CPU_PPC << Device_Map( "MPC8541", "MPC8541" );
	CPU_PPC << Device_Map( "MPC8541_v10", "MPC8541_v10" );
	CPU_PPC << Device_Map( "MPC8541_v11", "MPC8541_v11" );
	CPU_PPC << Device_Map( "MPC8541E", "MPC8541E" );
	CPU_PPC << Device_Map( "MPC8541E_v10", "MPC8541E_v10" );
	CPU_PPC << Device_Map( "MPC8541E_v11", "MPC8541E_v11" );
	CPU_PPC << Device_Map( "MPC8543", "MPC8543" );
	CPU_PPC << Device_Map( "MPC8543_v10", "MPC8543_v10" );
	CPU_PPC << Device_Map( "MPC8543_v11", "MPC8543_v11" );
	CPU_PPC << Device_Map( "MPC8543_v20", "MPC8543_v20" );
	CPU_PPC << Device_Map( "MPC8543_v21", "MPC8543_v21" );
	CPU_PPC << Device_Map( "MPC8543E", "MPC8543E" );
	CPU_PPC << Device_Map( "MPC8543E_v10", "MPC8543E_v10" );
	CPU_PPC << Device_Map( "MPC8543E_v11", "MPC8543E_v11" );
	CPU_PPC << Device_Map( "MPC8543E_v20", "MPC8543E_v20" );
	CPU_PPC << Device_Map( "MPC8543E_v21", "MPC8543E_v21" );
	CPU_PPC << Device_Map( "MPC8544", "MPC8544" );
	CPU_PPC << Device_Map( "MPC8544_v10", "MPC8544_v10" );
	CPU_PPC << Device_Map( "MPC8544_v11", "MPC8544_v11" );
	CPU_PPC << Device_Map( "MPC8544E", "MPC8544E" );
	CPU_PPC << Device_Map( "MPC8544E_v10", "MPC8544E_v10" );
	CPU_PPC << Device_Map( "MPC8544E_v11", "MPC8544E_v11" );
	CPU_PPC << Device_Map( "MPC8545", "MPC8545" );
	CPU_PPC << Device_Map( "MPC8545_v20", "MPC8545_v20" );
	CPU_PPC << Device_Map( "MPC8545_v21", "MPC8545_v21" );
	CPU_PPC << Device_Map( "MPC8545E", "MPC8545E" );
	CPU_PPC << Device_Map( "MPC8545E_v20", "MPC8545E_v20" );
	CPU_PPC << Device_Map( "MPC8545E_v21", "MPC8545E_v21" );
	CPU_PPC << Device_Map( "MPC8547E", "MPC8547E" );
	CPU_PPC << Device_Map( "MPC8547E_v20", "MPC8547E_v20" );
	CPU_PPC << Device_Map( "MPC8547E_v21", "MPC8547E_v21" );
	CPU_PPC << Device_Map( "MPC8548", "MPC8548" );
	CPU_PPC << Device_Map( "MPC8548_v10", "MPC8548_v10" );
	CPU_PPC << Device_Map( "MPC8548_v11", "MPC8548_v11" );
	CPU_PPC << Device_Map( "MPC8548_v20", "MPC8548_v20" );
	CPU_PPC << Device_Map( "MPC8548_v21", "MPC8548_v21" );
	CPU_PPC << Device_Map( "MPC8548E", "MPC8548E" );
	CPU_PPC << Device_Map( "MPC8548E_v10", "MPC8548E_v10" );
	CPU_PPC << Device_Map( "MPC8548E_v11", "MPC8548E_v11" );
	CPU_PPC << Device_Map( "MPC8548E_v20", "MPC8548E_v20" );
	CPU_PPC << Device_Map( "MPC8548E_v21", "MPC8548E_v21" );
	CPU_PPC << Device_Map( "MPC8555", "MPC8555" );
	CPU_PPC << Device_Map( "MPC8555_v10", "MPC8555_v10" );
	CPU_PPC << Device_Map( "MPC8555_v11", "MPC8555_v11" );
	CPU_PPC << Device_Map( "MPC8555E", "MPC8555E" );
	CPU_PPC << Device_Map( "MPC8555E_v10", "MPC8555E_v10" );
	CPU_PPC << Device_Map( "MPC8555E_v11", "MPC8555E_v11" );
	CPU_PPC << Device_Map( "MPC8560", "MPC8560" );
	CPU_PPC << Device_Map( "MPC8560_v10", "MPC8560_v10" );
	CPU_PPC << Device_Map( "MPC8560_v20", "MPC8560_v20" );
	CPU_PPC << Device_Map( "MPC8560_v21", "MPC8560_v21" );
	CPU_PPC << Device_Map( "MPC8567", "MPC8567" );
	CPU_PPC << Device_Map( "MPC8567E", "MPC8567E" );
	CPU_PPC << Device_Map( "MPC8568", "MPC8568" );
	CPU_PPC << Device_Map( "MPC8568E", "MPC8568E" );
	CPU_PPC << Device_Map( "MPC8572", "MPC8572" );
	CPU_PPC << Device_Map( "MPC8572E", "MPC8572E" );
	CPU_PPC << Device_Map( "e600", "e600" );
	CPU_PPC << Device_Map( "MPC8641", "MPC8641" );
	CPU_PPC << Device_Map( "MPC8641D", "MPC8641D" );
	CPU_PPC << Device_Map( "601", "601" );
	CPU_PPC << Device_Map( "601_v0", "601_v0" );
	CPU_PPC << Device_Map( "601_v1", "601_v1" );
	CPU_PPC << Device_Map( "601v", "601v" );
	CPU_PPC << Device_Map( "601_v2", "601_v2" );
	CPU_PPC << Device_Map( "602", "602" );
	CPU_PPC << Device_Map( "603", "603" );
	CPU_PPC << Device_Map( "Vanilla", "Vanilla" );
	CPU_PPC << Device_Map( "603e", "603e" );
	CPU_PPC << Device_Map( "Stretch", "Stretch" );
	CPU_PPC << Device_Map( "603e_v1.1", "\"603e_v1.1\"" );
	CPU_PPC << Device_Map( "603e_v1.2", "\"603e_v1.2\"" );
	CPU_PPC << Device_Map( "603e_v1.3", "\"603e_v1.3\"" );
	CPU_PPC << Device_Map( "603e_v1.4", "\"603e_v1.4\"" );
	CPU_PPC << Device_Map( "603e_v2.2", "\"603e_v2.2\"" );
	CPU_PPC << Device_Map( "603e_v3", "603e_v3" );
	CPU_PPC << Device_Map( "603e_v4", "603e_v4" );
	CPU_PPC << Device_Map( "603e_v4.1", "\"603e_v4.1\"" );
	CPU_PPC << Device_Map( "603e7", "603e7" );
	CPU_PPC << Device_Map( "603e7t", "603e7t" );
	CPU_PPC << Device_Map( "603e7v", "603e7v" );
	CPU_PPC << Device_Map( "Vaillant", "Vaillant" );
	CPU_PPC << Device_Map( "603e7v1", "603e7v1" );
	CPU_PPC << Device_Map( "603e7v2", "603e7v2" );
	CPU_PPC << Device_Map( "603p", "603p" );
	CPU_PPC << Device_Map( "603r", "603r" );
	CPU_PPC << Device_Map( "Goldeneye", "Goldeneye" );
	CPU_PPC << Device_Map( "604", "604" );
	CPU_PPC << Device_Map( "604e", "604e" );
	CPU_PPC << Device_Map( "Sirocco", "Sirocco" );
	CPU_PPC << Device_Map( "604e_v1.0", "\"604e_v1.0\"" );
	CPU_PPC << Device_Map( "604e_v2.2", "\"604e_v2.2\"" );
	CPU_PPC << Device_Map( "604e_v2.4", "\"604e_v2.4\"" );
	CPU_PPC << Device_Map( "604r", "604r" );
	CPU_PPC << Device_Map( "Mach5", "Mach5" );
	CPU_PPC << Device_Map( "740", "740" );
	CPU_PPC << Device_Map( "Arthur", "Arthur" );
	CPU_PPC << Device_Map( "750", "750" );
	CPU_PPC << Device_Map( "Typhoon", "Typhoon" );
	CPU_PPC << Device_Map( "G3", "G3" );
	CPU_PPC << Device_Map( "740_v1.0", "\"740_v1.0\"" );
	CPU_PPC << Device_Map( "750_v1.0", "\"750_v1.0\"" );
	CPU_PPC << Device_Map( "740_v2.0", "\"740_v2.0\"" );
	CPU_PPC << Device_Map( "750_v2.0", "\"750_v2.0\"" );
	CPU_PPC << Device_Map( "740_v2.1", "\"740_v2.1\"" );
	CPU_PPC << Device_Map( "750_v2.1", "\"750_v2.1\"" );
	CPU_PPC << Device_Map( "740_v2.2", "\"740_v2.2\"" );
	CPU_PPC << Device_Map( "750_v2.2", "\"750_v2.2\"" );
	CPU_PPC << Device_Map( "740_v3.0", "\"740_v3.0\"" );
	CPU_PPC << Device_Map( "750_v3.0", "\"750_v3.0\"" );
	CPU_PPC << Device_Map( "740_v3.1", "\"740_v3.1\"" );
	CPU_PPC << Device_Map( "750_v3.1", "\"750_v3.1\"" );
	CPU_PPC << Device_Map( "740e", "740e" );
	CPU_PPC << Device_Map( "750e", "750e" );
	CPU_PPC << Device_Map( "740p", "740p" );
	CPU_PPC << Device_Map( "750p", "750p" );
	CPU_PPC << Device_Map( "Conan/Doyle", "\"Conan/Doyle\"" );
	CPU_PPC << Device_Map( "750cl", "750cl" );
	CPU_PPC << Device_Map( "750cl_v1.0", "\"750cl_v1.0\"" );
	CPU_PPC << Device_Map( "750cl_v2.0", "\"750cl_v2.0\"" );
	CPU_PPC << Device_Map( "750cx", "750cx" );
	CPU_PPC << Device_Map( "750cx_v1.0", "\"750cx_v1.0\"" );
	CPU_PPC << Device_Map( "750cx_v2.0", "\"750cx_v2.0\"" );
	CPU_PPC << Device_Map( "750cx_v2.1", "\"750cx_v2.1\"" );
	CPU_PPC << Device_Map( "750cx_v2.2", "\"750cx_v2.2\"" );
	CPU_PPC << Device_Map( "750cxe", "750cxe" );
	CPU_PPC << Device_Map( "750cxe_v2.1", "\"750cxe_v2.1\"" );
	CPU_PPC << Device_Map( "750cxe_v2.2", "\"750cxe_v2.2\"" );
	CPU_PPC << Device_Map( "750cxe_v2.3", "\"750cxe_v2.3\"" );
	CPU_PPC << Device_Map( "750cxe_v2.4", "\"750cxe_v2.4\"" );
	CPU_PPC << Device_Map( "750cxe_v2.4b", "\"750cxe_v2.4b\"" );
	CPU_PPC << Device_Map( "750cxe_v3.0", "\"750cxe_v3.0\"" );
	CPU_PPC << Device_Map( "750cxe_v3.1", "\"750cxe_v3.1\"" );
	CPU_PPC << Device_Map( "750cxe_v3.1b", "\"750cxe_v3.1b\"" );
	CPU_PPC << Device_Map( "750cxr", "750cxr" );
	CPU_PPC << Device_Map( "750fl", "750fl" );
	CPU_PPC << Device_Map( "750fx", "750fx" );
	CPU_PPC << Device_Map( "750fx_v1.0", "\"750fx_v1.0\"" );
	CPU_PPC << Device_Map( "750fx_v2.0", "\"750fx_v2.0\"" );
	CPU_PPC << Device_Map( "750fx_v2.1", "\"750fx_v2.1\"" );
	CPU_PPC << Device_Map( "750fx_v2.2", "\"750fx_v2.2\"" );
	CPU_PPC << Device_Map( "750fx_v2.3", "\"750fx_v2.3\"" );
	CPU_PPC << Device_Map( "750gl", "750gl" );
	CPU_PPC << Device_Map( "750gx", "750gx" );
	CPU_PPC << Device_Map( "750gx_v1.0", "\"750gx_v1.0\"" );
	CPU_PPC << Device_Map( "750gx_v1.1", "\"750gx_v1.1\"" );
	CPU_PPC << Device_Map( "750gx_v1.2", "\"750gx_v1.2\"" );
	CPU_PPC << Device_Map( "750l", "750l" );
	CPU_PPC << Device_Map( "LoneStar", "LoneStar" );
	CPU_PPC << Device_Map( "750l_v2.0", "\"750l_v2.0\"" );
	CPU_PPC << Device_Map( "750l_v2.1", "\"750l_v2.1\"" );
	CPU_PPC << Device_Map( "750l_v2.2", "\"750l_v2.2\"" );
	CPU_PPC << Device_Map( "750l_v3.0", "\"750l_v3.0\"" );
	CPU_PPC << Device_Map( "750l_v3.2", "\"750l_v3.2\"" );
	CPU_PPC << Device_Map( "745", "745" );
	CPU_PPC << Device_Map( "755", "755" );
	CPU_PPC << Device_Map( "Goldfinger", "Goldfinger" );
	CPU_PPC << Device_Map( "745_v1.0", "\"745_v1.0\"" );
	CPU_PPC << Device_Map( "755_v1.0", "\"755_v1.0\"" );
	CPU_PPC << Device_Map( "745_v1.1", "\"745_v1.1\"" );
	CPU_PPC << Device_Map( "755_v1.1", "\"755_v1.1\"" );
	CPU_PPC << Device_Map( "745_v2.0", "\"745_v2.0\"" );
	CPU_PPC << Device_Map( "755_v2.0", "\"755_v2.0\"" );
	CPU_PPC << Device_Map( "745_v2.1", "\"745_v2.1\"" );
	CPU_PPC << Device_Map( "755_v2.1", "\"755_v2.1\"" );
	CPU_PPC << Device_Map( "745_v2.2", "\"745_v2.2\"" );
	CPU_PPC << Device_Map( "755_v2.2", "\"755_v2.2\"" );
	CPU_PPC << Device_Map( "745_v2.3", "\"745_v2.3\"" );
	CPU_PPC << Device_Map( "755_v2.3", "\"755_v2.3\"" );
	CPU_PPC << Device_Map( "745_v2.4", "\"745_v2.4\"" );
	CPU_PPC << Device_Map( "755_v2.4", "\"755_v2.4\"" );
	CPU_PPC << Device_Map( "745_v2.5", "\"745_v2.5\"" );
	CPU_PPC << Device_Map( "755_v2.5", "\"755_v2.5\"" );
	CPU_PPC << Device_Map( "745_v2.6", "\"745_v2.6\"" );
	CPU_PPC << Device_Map( "755_v2.6", "\"755_v2.6\"" );
	CPU_PPC << Device_Map( "745_v2.7", "\"745_v2.7\"" );
	CPU_PPC << Device_Map( "755_v2.7", "\"755_v2.7\"" );
	CPU_PPC << Device_Map( "745_v2.8", "\"745_v2.8\"" );
	CPU_PPC << Device_Map( "755_v2.8", "\"755_v2.8\"" );
	CPU_PPC << Device_Map( "7400", "7400" );
	CPU_PPC << Device_Map( "Max", "Max" );
	CPU_PPC << Device_Map( "G4", "G4" );
	CPU_PPC << Device_Map( "7400_v1.0", "\"7400_v1.0\"" );
	CPU_PPC << Device_Map( "7400_v1.1", "\"7400_v1.1\"" );
	CPU_PPC << Device_Map( "7400_v2.0", "\"7400_v2.0\"" );
	CPU_PPC << Device_Map( "7400_v2.1", "\"7400_v2.1\"" );
	CPU_PPC << Device_Map( "7400_v2.2", "\"7400_v2.2\"" );
	CPU_PPC << Device_Map( "7400_v2.6", "\"7400_v2.6\"" );
	CPU_PPC << Device_Map( "7400_v2.7", "\"7400_v2.7\"" );
	CPU_PPC << Device_Map( "7400_v2.8", "\"7400_v2.8\"" );
	CPU_PPC << Device_Map( "7400_v2.9", "\"7400_v2.9\"" );
	CPU_PPC << Device_Map( "7410", "7410" );
	CPU_PPC << Device_Map( "Nitro", "Nitro" );
	CPU_PPC << Device_Map( "7410_v1.0", "\"7410_v1.0\"" );
	CPU_PPC << Device_Map( "7410_v1.1", "\"7410_v1.1\"" );
	CPU_PPC << Device_Map( "7410_v1.2", "\"7410_v1.2\"" );
	CPU_PPC << Device_Map( "7410_v1.3", "\"7410_v1.3\"" );
	CPU_PPC << Device_Map( "7410_v1.4", "\"7410_v1.4\"" );
	CPU_PPC << Device_Map( "7448", "7448" );
	CPU_PPC << Device_Map( "7448_v1.0", "\"7448_v1.0\"" );
	CPU_PPC << Device_Map( "7448_v1.1", "\"7448_v1.1\"" );
	CPU_PPC << Device_Map( "7448_v2.0", "\"7448_v2.0\"" );
	CPU_PPC << Device_Map( "7448_v2.1", "\"7448_v2.1\"" );
	CPU_PPC << Device_Map( "7450", "7450" );
	CPU_PPC << Device_Map( "Vger", "Vger" );
	CPU_PPC << Device_Map( "7450_v1.0", "\"7450_v1.0\"" );
	CPU_PPC << Device_Map( "7450_v1.1", "\"7450_v1.1\"" );
	CPU_PPC << Device_Map( "7450_v1.2", "\"7450_v1.2\"" );
	CPU_PPC << Device_Map( "7450_v2.0", "\"7450_v2.0\"" );
	CPU_PPC << Device_Map( "7450_v2.1", "\"7450_v2.1\"" );
	CPU_PPC << Device_Map( "7441", "7441" );
	CPU_PPC << Device_Map( "7451", "7451" );
	CPU_PPC << Device_Map( "7441_v2.1", "\"7441_v2.1\"" );
	CPU_PPC << Device_Map( "7441_v2.3", "\"7441_v2.3\"" );
	CPU_PPC << Device_Map( "7451_v2.3", "\"7451_v2.3\"" );
	CPU_PPC << Device_Map( "7441_v2.10", "\"7441_v2.10\"" );
	CPU_PPC << Device_Map( "7451_v2.10", "\"7451_v2.10\"" );
	CPU_PPC << Device_Map( "7445", "7445" );
	CPU_PPC << Device_Map( "7455", "7455" );
	CPU_PPC << Device_Map( "Apollo6", "Apollo6" );
	CPU_PPC << Device_Map( "7445_v1.0", "\"7445_v1.0\"" );
	CPU_PPC << Device_Map( "7455_v1.0", "\"7455_v1.0\"" );
	CPU_PPC << Device_Map( "7445_v2.1", "\"7445_v2.1\"" );
	CPU_PPC << Device_Map( "7455_v2.1", "\"7455_v2.1\"" );
	CPU_PPC << Device_Map( "7445_v3.2", "\"7445_v3.2\"" );
	CPU_PPC << Device_Map( "7455_v3.2", "\"7455_v3.2\"" );
	CPU_PPC << Device_Map( "7445_v3.3", "\"7445_v3.3\"" );
	CPU_PPC << Device_Map( "7455_v3.3", "\"7455_v3.3\"" );
	CPU_PPC << Device_Map( "7445_v3.4", "\"7445_v3.4\"" );
	CPU_PPC << Device_Map( "7455_v3.4", "\"7455_v3.4\"" );
	CPU_PPC << Device_Map( "7447", "7447" );
	CPU_PPC << Device_Map( "7457", "7457" );
	CPU_PPC << Device_Map( "Apollo7", "Apollo7" );
	CPU_PPC << Device_Map( "7447_v1.0", "\"7447_v1.0\"" );
	CPU_PPC << Device_Map( "7457_v1.0", "\"7457_v1.0\"" );
	CPU_PPC << Device_Map( "7447_v1.1", "\"7447_v1.1\"" );
	CPU_PPC << Device_Map( "7457_v1.1", "\"7457_v1.1\"" );
	CPU_PPC << Device_Map( "7457_v1.2", "\"7457_v1.2\"" );
	CPU_PPC << Device_Map( "7447A", "7447A" );
	CPU_PPC << Device_Map( "7457A", "7457A" );
	CPU_PPC << Device_Map( "7447A_v1.0", "\"7447A_v1.0\"" );
	CPU_PPC << Device_Map( "7457A_v1.0", "\"7457A_v1.0\"" );
	CPU_PPC << Device_Map( "Apollo7PM", "Apollo7PM" );
	CPU_PPC << Device_Map( "7447A_v1.1", "\"7447A_v1.1\"" );
	CPU_PPC << Device_Map( "7457A_v1.1", "\"7457A_v1.1\"" );
	CPU_PPC << Device_Map( "7447A_v1.2", "\"7447A_v1.2\"" );
	CPU_PPC << Device_Map( "7457A_v1.2", "\"7457A_v1.2\"" );
	CPU_PPC << Device_Map( "ppc32", "ppc32" );
	CPU_PPC << Device_Map( "ppc", "ppc" );
	
	// Machines
	Machine_x86 << Device_Map( QObject::tr("Standard PC"), "pc" );
	Machine_x86 << Device_Map( QObject::tr("ISA-only PC"), "isapc" );
	
	Machine_ARM << Device_Map( QObject::tr("Integrator/CP (ARM926EJ-S)"), "integratorcp926" );
	Machine_ARM << Device_Map( QObject::tr("Integrator/CP (ARM1026EJ-S)"), "integratorcp1026" );
	Machine_ARM << Device_Map( QObject::tr("Versatile/PB (ARM926EJ-S)"), "versatilepb" );
	Machine_ARM << Device_Map( QObject::tr("Versatile/AB (ARM926EJ-S)"), "versatileab" );
	Machine_ARM << Device_Map( QObject::tr("RealView (ARM926EJ-S)"), "realview" );
	
	Machine_MIPS << Device_Map( QObject::tr("R4k platform"), "mips" );
	Machine_MIPS << Device_Map( QObject::tr("Malta Core LV"), "malta" );
	
	Machine_PPC << Device_Map( QObject::tr("Heathrow PowerMAC"), "g3bw" );
	Machine_PPC << Device_Map( QObject::tr("Mac99 PowerMAC"), "mac99" );
	Machine_PPC << Device_Map( QObject::tr("PowerPC PREP"), "prep" );
	
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 2 (Sun4c)"), "SS-2" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 5 (Sun4m)"), "SS-5" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 10 (Sun4m)"), "SS-10" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCstation 20 (Sun4m)"), "SS-20" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCserver 600MP (Sun4m)"), "SS-600MP" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCserver 1000 (Sun4d)"), "SS-1000" );
	Machine_Sparc << Device_Map( QObject::tr("SPARCcenter 2000 (Sun4d)"), "SS-2000" );
	
	// Net Cards
	Network_Card_x86 << Device_Map( QObject::tr("NE2000 PCI"), "ne2k_pci" );
	Network_Card_x86 << Device_Map( QObject::tr("NE2000 ISA"), "ne2k_isa" );
	Network_Card_x86 << Device_Map( QObject::tr("i82551"), "i82551" );
	Network_Card_x86 << Device_Map( QObject::tr("i82557B"), "i82557b" );
	Network_Card_x86 << Device_Map( QObject::tr("i82559ER"), "i82559er" );
	Network_Card_x86 << Device_Map( QObject::tr("PCnet32 PCI"), "pcnet" );
	Network_Card_x86 << Device_Map( QObject::tr("RTL8139"), "rtl8139" );
	
	Network_Card_x86_v090 << Device_Map( QObject::tr("NE2000 PCI"), "ne2k_pci" );
	Network_Card_x86_v090 << Device_Map( QObject::tr("NE2000 ISA"), "ne2k_isa" );
	Network_Card_x86_v090 << Device_Map( QObject::tr("RTL8139"), "rtl8139" );
	
	Network_Card_ARM << Device_Map( QObject::tr("SMC91C111"), "smc91c111" );
	
	Network_Card_M68K << Device_Map( QObject::tr("MCF FEC"), "mcf_fec" );
	
	Network_Card_MIPS << Device_Map( QObject::tr("NE2000 ISA"), "ne2k_isa" );
	
	Network_Card_PPC << Device_Map( QObject::tr("i82551"), "i82551" );
	Network_Card_PPC << Device_Map( QObject::tr("i82557B"), "i82557b" );
	Network_Card_PPC << Device_Map( QObject::tr("i82559ER"), "i82559er" );
	Network_Card_PPC << Device_Map( QObject::tr("NE2000 PCI"), "ne2k_pci" );
	Network_Card_PPC << Device_Map( QObject::tr("PCnet32 PCI"), "pcnet" );
	Network_Card_PPC << Device_Map( QObject::tr("RTL8139"), "rtl8139" );
	
	Machine_SH4 << Device_Map( QObject::tr("shix card"), "shix" );
	Machine_SH4 << Device_Map( QObject::tr("r2d-plus board"), "r2d" );
	
	Network_Card_Sparc << Device_Map( QObject::tr("Lance"), "lance" );
	
	// Video
	Video_Card_Standart << Device_Map( QObject::tr("No Video Card"), "-nographic" );
	
	Video_Card_x86 << Device_Map( QObject::tr("Cirrus CLGD 5446"), "" );
	Video_Card_x86 << Device_Map( QObject::tr("No Video Card"), "-nographic" );
	Video_Card_x86 << Device_Map( QObject::tr("Standard VGA"), "-std-vga" );
	
	// Sound Cards
	Audio_Card_x86.Audio_PC_Speaker = true;
	Audio_Card_x86.Audio_sb16 = true;
	Audio_Card_x86.Audio_Adlib = true;
	Audio_Card_x86.Audio_es1370 = true;
	
	Audio_Card_PPC.Audio_sb16 = true;
	Audio_Card_PPC.Audio_Adlib = true;
	Audio_Card_PPC.Audio_es1370 = true;
	
	// QEMU 0.10.0
	QList<Device_Map> QEMU_Video_Cards_v0_10_0;
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("StdVGA (VESA 2.0)"), "std" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("Cirrus CLGD 5446"), "cirrus" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("VMWare Video Card"), "vmware" );
	QEMU_Video_Cards_v0_10_0 << Device_Map( QObject::tr("None Video Card"), "none" );
	
	QList<Device_Map> CPU_x86_v0_10_0 = CPU_x86;
	CPU_x86_v0_10_0 << Device_Map( QObject::tr("Core Duo"), "coreduo" );
	CPU_x86_v0_10_0 << Device_Map( QObject::tr("Athlon"), "athlon" );
	CPU_x86_v0_10_0 << Device_Map( QObject::tr("Atom N270"), "n270" );
	
	QList<Device_Map> Network_Card_v0_10_0 = Network_Card_x86;
	Network_Card_v0_10_0 << Device_Map( QObject::tr("e1000"), "e1000" );
	Network_Card_v0_10_0 << Device_Map( QObject::tr("virtio"), "virtio" );
	
	QList<Device_Map> CPU_x86_64_v0_10_0 = CPU_x86_v0_10_0;
	CPU_x86_64_v0_10_0 << Device_Map( QObject::tr("Phenom"), "phenom" );
	CPU_x86_64_v0_10_0 << Device_Map( QObject::tr("Core 2 Duo"), "core2duo" );
	CPU_x86_64_v0_10_0 << Device_Map( QObject::tr("QEMU 64Bit"), "qemu64" );
	
	auto ad = Available_Devices();
	ad.System = Device_Map( "IBM PC 64Bit", "qemu-system-x86_64" );
	ad.CPU_List += CPU_x86_64_v0_10_0;
	ad.Machine_List += Machine_x86;
	ad.Network_Card_List += Network_Card_v0_10_0;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.Audio_Card_List.Audio_GUS = true;
	ad.Audio_Card_List.Audio_AC97 = true;
	ad.PSO_SMP_Count = 255;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	ad.PSO_RTC_TD_Hack = true;
	ad.PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-x86_64" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "ARM", "qemu-system-arm" );
	
	CPU_ARM.insert( 1, Device_Map(QObject::tr("ARM 1136 R2"), "arm1136-r2") );
	ad.CPU_List += CPU_ARM;
	
	QList<Device_Map> Machine_ARM_0_10_0;
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Integrator/CP (ARM926EJ-S)"), "integratorcp" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Versatile/PB (ARM926EJ-S)"), "versatilepb" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Versatile/AB (ARM926EJ-S)"), "versatileab" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("RealView (ARM926EJ-S)"), "realview" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Akita PDA (PXA270)"), "akita" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Spitz PDA (PXA270)"), "spitz" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Borzoi PDA (PXA270)"), "borzoi" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Terrier PDA (PXA270)"), "terrier" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Siemens SX1 (OMAP310) V1"), "sx1-v1" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Siemens SX1 (OMAP310) V2"), "sx1" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Palm Tungsten (OMAP310)"), "cheetah" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Nokia N800"), "n800" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Nokia N810"), "n810" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Stellaris LM3S811EVB"), "lm3s811evb" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Stellaris LM3S6965EVB"), "lm3s6965evb" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Gumstix Connex (PXA255)"), "connex" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Gumstix Verdex (PXA270)"), "verdex" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Mainstone II (PXA27x)"), "mainstone" );
	
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Marvell 88w8618 / MusicPal"), "musicpal" );
	Machine_ARM_0_10_0 << Device_Map( QObject::tr("Tosa PDA (PXA255)"), "tosa" );
	
	ad.Machine_List += Machine_ARM_0_10_0;
	
	ad.Network_Card_List += Network_Card_ARM;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-arm" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Cris", "qemu-system-cris" );
	ad.Machine_List << Device_Map( QObject::tr("Bare ETRAX FS board"), "bareetraxfs" );
	ad.Machine_List << Device_Map( QObject::tr("AXIS devboard 88"), "axis-dev88" );
	ad.Network_Card_List << Device_Map( QObject::tr("fseth"), "fseth" );
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-cris" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "ColdFire (M68K)", "qemu-system-m68k" );
	ad.Machine_List << Device_Map( QObject::tr("MCF5206EVB"), "mcf5208evb" );
	ad.Machine_List << Device_Map( QObject::tr("Arnewsh 5206"), "an5206 " );
	ad.Machine_List << Device_Map( QObject::tr("Dummy board"), "dummy" );
	ad.Network_Card_List += Network_Card_M68K;
	ad.Video_Card_List = QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-m68k" ] = ad;
	
	QList<Device_Map> Machine_MIPS_v0_10_0;
	Machine_MIPS << Device_Map( QObject::tr("MIPS Malta Core LV"), "malta" );
	Machine_MIPS << Device_Map( QObject::tr("MIPS Magnum"), "magnum" );
	Machine_MIPS << Device_Map( QObject::tr("Acer Pica 61"), "pica61" );
	Machine_MIPS << Device_Map( QObject::tr("MIPS MIPSsim platform"), "mipssim" );
	Machine_MIPS << Device_Map( QObject::tr("mips r4k platform"), "mips" );
	
	ad = Available_Devices();
	ad.System = Device_Map( "MIPS", "qemu-system-mips" );
	ad.CPU_List += CPU_MIPS;
	ad.Machine_List += Machine_MIPS_v0_10_0;
	ad.Network_Card_List += Network_Card_MIPS;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-mips" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "MIPS EL", "qemu-system-mipsel" );
	ad.CPU_List += CPU_MIPS;
	ad.Machine_List += Machine_MIPS_v0_10_0;
	ad.Network_Card_List += Network_Card_MIPS;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-mipsel" ] = ad;
	
	QList<Device_Map> MIPS_CPU_64Bit = CPU_MIPS;
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS R4000"), "R4000" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS VR5432"), "VR5432" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS 5Kc"), "5Kc" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS 5Kf"), "5Kf" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS 20Kc"), "20Kc" );
	MIPS_CPU_64Bit << Device_Map( QObject::tr("MIPS MIPS64R2-generic"), "MIPS64R2-generic" );
	
	ad = Available_Devices();
	ad.System = Device_Map( "MIPS64", "qemu-system-mips64" );
	ad.CPU_List += MIPS_CPU_64Bit;
	ad.Machine_List += Machine_MIPS_v0_10_0;
	ad.Network_Card_List += Network_Card_MIPS;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-mips64" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "MIPS64 EL", "qemu-system-mips64el" );
	ad.CPU_List += MIPS_CPU_64Bit;
	ad.Machine_List += Machine_MIPS_v0_10_0;
	ad.Network_Card_List += Network_Card_MIPS;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-mips64el" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Microblaze", "qemu-system-microblaze" );
	ad.Machine_List << Device_Map( QObject::tr("Spartan 3ADSP1800"), "petalogix-s3adsp1800" );
	ad.Network_Card_List << Device_Map( QObject::tr("xilinx-ethlite"), "xilinx-ethlite" );
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-microblaze" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Power PC 32Bit", "qemu-system-ppc" );
	
	CPU_PPC.insert( 121, Device_Map(QObject::tr("e500v1"), "e500v1") );
	ad.CPU_List += CPU_PPC;
	
	QList<Device_Map> Machine_PPC_0_10_0;
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("Heathrow PowerMAC"), "g3beige" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("Mac99 based PowerMAC"), "mac99" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("PowerPC PREP platform"), "prep" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("ref405ep"), "ref405ep" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("taihu"), "taihu" );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("bamboo "), "bamboo " );
	Machine_PPC_0_10_0 << Device_Map( QObject::tr("mpc8544ds"), "mpc8544ds" );
	ad.Machine_List += Machine_PPC_0_10_0;
	
	ad.Network_Card_List += Network_Card_v0_10_0;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_Initial_Graphic_Mode = true;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-ppc" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Power PC EMB 32Bit", "qemu-system-ppcemb" );
	ad.CPU_List += CPU_PPC;
	ad.Machine_List += Machine_PPC_0_10_0;
	ad.Network_Card_List += Network_Card_PPC;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_Initial_Graphic_Mode = true;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-ppcemb" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Power PC 64Bit", "qemu-system-ppc64" );
	
	QList<Device_Map> CPU_PPC_64 = CPU_PPC;
	CPU_PPC_64 << Device_Map( "620", "620" );
	CPU_PPC_64 << Device_Map( "Trident", "Trident" );
	CPU_PPC_64 << Device_Map( "970", "970" );
	CPU_PPC_64 << Device_Map( "970fx", "970fx" );
	CPU_PPC_64 << Device_Map( "970fx_v1.0", "\"970fx_v1.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v2.0", "\"970fx_v2.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v2.1", "\"970fx_v2.1\"" );
	CPU_PPC_64 << Device_Map( "970fx_v3.0", "\"970fx_v3.0\"" );
	CPU_PPC_64 << Device_Map( "970fx_v3.1", "\"970fx_v3.1\"" );
	CPU_PPC_64 << Device_Map( "970gx", "970gx" );
	CPU_PPC_64 << Device_Map( "970mp", "970mp" );
	CPU_PPC_64 << Device_Map( "970mp_v1.0", "\"970mp_v1.0\"" );
	CPU_PPC_64 << Device_Map( "970mp_v1.1", "\"970mp_v1.1\"" );
	CPU_PPC_64 << Device_Map( "PPC64", "ppc64" );
	ad.CPU_List += CPU_PPC_64;
	
	ad.Machine_List += Machine_PPC_0_10_0;
	ad.Network_Card_List += Network_Card_PPC;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_PPC;
	ad.PSO_Initial_Graphic_Mode = true;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-ppc64" ] = ad;
	
	QList<Device_Map> CPU_SH4;
	CPU_SH4 << Device_Map( QObject::tr("SH7750R"), "SH7750R" );
	CPU_SH4 << Device_Map( QObject::tr("SH7751R"), "SH7751R" );
	CPU_SH4 << Device_Map( QObject::tr("SH7785"), "SH7785" );
	
	ad = Available_Devices();
	ad.System = Device_Map( "SH4", "qemu-system-sh4" );
	ad.CPU_List += CPU_SH4;
	ad.Machine_List += Machine_SH4;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-sh4" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "SH4 EB", "qemu-system-sh4eb" );
	ad.CPU_List += CPU_SH4;
	ad.Machine_List += Machine_SH4;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	System_Info::Emulator_QEMU_2_0[ "qemu-system-sh4eb" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Sparc 32Bit", "qemu-system-sparc" );
	//ad.CPU_List = CPU_Sparc; // FIXME sparc CPU's...
	
	ad.Machine_List += Machine_Sparc;
	ad.Machine_List << Device_Map( QObject::tr("SPARCstation Voyager (Sun4m)"), "Voyager" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCstation LX (Sun4m)"), "LX" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCstation 4 (Sun4m)"), "SS-4" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCClassic (Sun4m)"), "SPARCClassic" );
	ad.Machine_List << Device_Map( QObject::tr("SPARCbook (Sun4m)"), "SPARCbook" );
	
	ad.Network_Card_List += Network_Card_Sparc;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_SMP_Count = 4;
	ad.PSO_Initial_Graphic_Mode = true;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-sparc" ] = ad;
	
	ad = Available_Devices();
	ad.System = Device_Map( "Sparc 64Bit", "qemu-system-sparc64" );
	//ad.CPU_List = CPU_Sparc; // FIXME sparc CPU's...
	ad.Machine_List << Device_Map( QObject::tr("Sun4u"), "sun4u" );
	ad.Machine_List << Device_Map( QObject::tr("Sun4v"), "sun4v" );
	ad.Machine_List << Device_Map( QObject::tr("Sun4v (Niagara)"), "Niagara" );
	
	ad.Network_Card_List += Network_Card_Sparc;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = VM::Sound_Cards();
	ad.PSO_Initial_Graphic_Mode = true;
	System_Info::Emulator_QEMU_2_0[ "qemu-system-sparc64" ] = ad;
	

	ad.PSO_SMP_Cores = true;
	ad.PSO_SMP_Threads = true;
	ad.PSO_SMP_Sockets = true;
	ad.PSO_SMP_MaxCPUs = true;
	
	ad.PSO_Drive = true;
	ad.PSO_Drive_File = true;
	ad.PSO_Drive_If = true;
	ad.PSO_Drive_Bus_Unit = true;
	ad.PSO_Drive_Index = true;
	ad.PSO_Drive_Media = true;
	ad.PSO_Drive_Cyls_Heads_Secs_Trans = true;
	ad.PSO_Drive_Snapshot = true;
	ad.PSO_Drive_Cache = true;
	ad.PSO_Drive_AIO = true;
	ad.PSO_Drive_Format = true;
	ad.PSO_Drive_Serial = true;
	ad.PSO_Drive_ADDR = true;
	
	ad.PSO_Boot_Order = true;
	//PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	//PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_KVM = false;
	ad.PSO_Enable_KVM = false;
	ad.PSO_RTC_TD_Hack = true;
	
	ad.PSO_MTDBlock = true;
	ad.PSO_SD = true;
	ad.PSO_PFlash = true;
	ad.PSO_Name = true;
	ad.PSO_Curses = true;
	ad.PSO_No_Frame = true;
	ad.PSO_Alt_Grab = true;
	ad.PSO_No_Quit = true;
	ad.PSO_SDL = true;
	ad.PSO_Portrait = true;
	ad.PSO_No_Shutdown = true;
	ad.PSO_Startdate = true;
	ad.PSO_Show_Cursor = true;
	ad.PSO_Bootp = true;
	
	ad.PSO_Net_type_vde = true;
	ad.PSO_Net_type_dump = true;
	
	ad.PSO_Net_name = true;
	ad.PSO_Net_addr = true;
	ad.PSO_Net_vectors = true;
	
	ad.PSO_Net_net = true;
	ad.PSO_Net_host = true;
	ad.PSO_Net_restrict = true;
	ad.PSO_Net_dhcpstart = true;
	ad.PSO_Net_dns = true;
	ad.PSO_Net_tftp = true;
	ad.PSO_Net_bootfile = true;
	ad.PSO_Net_smb = true;
	ad.PSO_Net_hostfwd = true;
	ad.PSO_Net_guestfwd = true;
	
	ad.PSO_Net_ifname = true;
	ad.PSO_Net_bridge = true;
	ad.PSO_Net_script = true;
	ad.PSO_Net_downscript = true;
	ad.PSO_Net_helper = true;
	
	ad.PSO_Net_listen = true;
	ad.PSO_Net_connect = true;
	
	ad.PSO_Net_mcast = true;
	
	ad.PSO_Net_sock = true;
	ad.PSO_Net_port = true;
	ad.PSO_Net_group = true;
	ad.PSO_Net_mode = true;
	
	ad.PSO_Net_file = true;
	ad.PSO_Net_len = true;
	
	ad.PSO_Enable_KVM = false;
	ad.PSO_No_KVM = false;
	ad.PSO_No_KVM_IRQChip = false;
	ad.PSO_No_KVM_Pit = false;
	ad.PSO_No_KVM_Pit_Reinjection = false;
	ad.PSO_Enable_Nesting = false;
	ad.PSO_KVM_Shadow_Memory = false;
	
	/*// KVM 0.12
	ad = Available_Devices();
	ad.System = Device_Map( "KVM (Intel VT/AMD SVM)", "qemu-kvm" );
	ad.CPU_List += CPU_x86_64_v0_10_0;
	ad.Machine_List += Machine_x86;
	ad.Network_Card_List += Network_Card_v0_10_0;
	ad.Video_Card_List += QEMU_Video_Cards_v0_10_0;
	ad.Audio_Card_List = Audio_Card_x86;
	ad.Audio_Card_List.Audio_GUS = true;
	ad.Audio_Card_List.Audio_AC97 = true;
	ad.PSO_SMP_Count = 255;
	
	ad.PSO_SMP_Cores = true;
	ad.PSO_SMP_Threads = true;
	ad.PSO_SMP_Sockets = true;
	ad.PSO_SMP_MaxCPUs = true;
	
	ad.PSO_Drive = true;
	ad.PSO_Drive_File = true;
	ad.PSO_Drive_If = true;
	ad.PSO_Drive_Bus_Unit = true;
	ad.PSO_Drive_Index = true;
	ad.PSO_Drive_Media = true;
	ad.PSO_Drive_Cyls_Heads_Secs_Trans = true;
	ad.PSO_Drive_Snapshot = true;
	ad.PSO_Drive_Cache = true;
	ad.PSO_Drive_AIO = true;
	ad.PSO_Drive_Format = true;
	ad.PSO_Drive_Serial = true;
	ad.PSO_Drive_ADDR = true;
	
	ad.PSO_Boot_Order = true;
	//PSO_Initial_Graphic_Mode = true;
	ad.PSO_No_FB_Boot_Check = true;
	ad.PSO_Win2K_Hack = true;
	//PSO_Kernel_KQEMU = true;
	ad.PSO_No_ACPI = true;
	ad.PSO_KVM = true;
	ad.PSO_Enable_KVM = true;
	ad.PSO_RTC_TD_Hack = true;
	
	ad.PSO_MTDBlock = true;
	ad.PSO_SD = true;
	ad.PSO_PFlash = true;
	ad.PSO_Name = true;
	ad.PSO_Curses = true;
	ad.PSO_No_Frame = true;
	ad.PSO_Alt_Grab = true;
	ad.PSO_No_Quit = true;
	ad.PSO_SDL = true;
	ad.PSO_Portrait = true;
	ad.PSO_No_Shutdown = true;
	ad.PSO_Startdate = true;
	ad.PSO_Show_Cursor = true;
	ad.PSO_Bootp = true;
	
	ad.PSO_Net_type_vde = true;
	ad.PSO_Net_type_dump = true;
	
	ad.PSO_Net_name = true;
	ad.PSO_Net_addr = true;
	ad.PSO_Net_vectors = true;
	
	ad.PSO_Net_net = true;
	ad.PSO_Net_host = true;
	ad.PSO_Net_restrict = true;
	ad.PSO_Net_dhcpstart = true;
	ad.PSO_Net_dns = true;
	ad.PSO_Net_tftp = true;
	ad.PSO_Net_bootfile = true;
	ad.PSO_Net_smb = true;
	ad.PSO_Net_hostfwd = true;
	ad.PSO_Net_guestfwd = true;
	
	ad.PSO_Net_ifname = true;
	ad.PSO_Net_bridge = true;
	ad.PSO_Net_script = true;
	ad.PSO_Net_downscript = true;
	ad.PSO_Net_helper = true;
	
	ad.PSO_Net_listen = true;
	ad.PSO_Net_connect = true;
	
	ad.PSO_Net_mcast = true;
	
	ad.PSO_Net_sock = true;
	ad.PSO_Net_port = true;
	ad.PSO_Net_group = true;
	ad.PSO_Net_mode = true;
	
	ad.PSO_Net_file = true;
	ad.PSO_Net_len = true;
	
	ad.PSO_Enable_KVM = true;
	ad.PSO_No_KVM = true;
	ad.PSO_No_KVM_IRQChip = true;
	ad.PSO_No_KVM_Pit = true;
	ad.PSO_No_KVM_Pit_Reinjection = true;
	ad.PSO_Enable_Nesting = true;
	ad.PSO_KVM_Shadow_Memory = true;
	
	//System_Info::Emulator_KVM_2_0[ "qemu-kvm" ] = ad;
    */ //tobgle //FIXME?
	
	return true;
}

VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )
{
	if( path.isEmpty() )
	{
		AQWarning( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
				   "path is Empty" );
		return VM::Obsolete;
	}
	
	// Get help text
	QString emulHelpText = Get_Emulator_Help_Output( path );
	QTextStream allHelpOutput( &emulHelpText, QIODevice::ReadOnly );
	
	QRegExp emulVerLineRegExp = QRegExp( ".*version.*" );
	QString line = "";
	
	for( int ix = 0; ix < 5; ++ix )
	{
		QString tmpLine = allHelpOutput.readLine();
		
		if( tmpLine.isEmpty() )
		{
			AQError( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
					 "Result String is Empty!" );
			break;
		}
		
		if( ! emulVerLineRegExp.exactMatch(tmpLine) ) continue;
		else
		{
			line = tmpLine;
			break;
		}
	}
	
	if( line.isEmpty() )
	{
		AQError( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
				 "Cannot get emulator version!" );
		return VM::Obsolete;
	}
	
	// This QEMU or KVM?
	bool emulatorKVM = false;
	if( path.contains("kvm", Qt::CaseInsensitive) ) emulatorKVM = true;
	//if( path.contains("qemu", Qt::CaseInsensitive) ) emulatorKVM = false;
	
    // QEMU
    //QRegExp emulVerRegExp = QRegExp( ".*version\\s+([\\d]+)[.]([\\d]+)[.]([\\d]+).*" );
    QRegExp emulVerRegExpNew = QRegExp( ".*version\\s+([\\d]+)[.]([\\d]+).*" );

    if( ! emulVerRegExpNew.exactMatch(line) )
    {
	    AQError( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
			     "Cannot match emulVerRegExp! Line: " + line );
	    return VM::Obsolete;
    }
    else // Version like: 1.0
    {
	    QStringList versionLines = emulVerRegExpNew.capturedTexts();
	    if( versionLines.count() < 3 )
	    {
		    AQError( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
				     "Cannot get major_ver, minor_ver vairables!" );
		    return VM::Obsolete;
	    }
	
	    int major_ver, minor_ver;
	    bool ok1, ok2;
	
	    major_ver = versionLines[1].toInt( &ok1, 10 );
	    minor_ver = versionLines[2].toInt( &ok2, 10 );
	
	    if( ! (ok1 && ok2) )
	    {
		    AQError( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
				     "Cannot Convert to Int! Line: " + versionLines[0] );
		    return VM::Obsolete;
	    }
	
	    if( major_ver > 1 || (major_ver == 1 && minor_ver > 0) )
        {
             return VM::QEMU_2_0;
        }
	    else
	    {
		    AQError( "VM::Emulator_Version System_Info::Get_Emulator_Version( const QString &path )",
				     QString("QEMU Version %1.%2 not defined!").arg(major_ver).arg(minor_ver) );
		    return VM::Obsolete;
	    }
    }
}

QMap<QString, QString> System_Info::Find_QEMU_Binary_Files( const QString &path )
{
	QMap<QString, QString> emulFiles;
	
	emulFiles[ "qemu-system-x86_64" ] = "";
	emulFiles[ "qemu-system-arm" ] = "";
	emulFiles[ "qemu-system-cris" ] = "";
	emulFiles[ "qemu-system-m68k" ] = "";
	emulFiles[ "qemu-system-microblaze" ] = "";
	emulFiles[ "qemu-system-mips" ] = "";
	emulFiles[ "qemu-system-mips64" ] = "";
	emulFiles[ "qemu-system-mips64el" ] = "";
	emulFiles[ "qemu-system-mipsel" ] = "";
	emulFiles[ "qemu-system-ppc" ] = "";
	emulFiles[ "qemu-system-ppc64" ] = "";
	emulFiles[ "qemu-system-ppcemb" ] = "";
	emulFiles[ "qemu-system-sh4" ] = "";
	emulFiles[ "qemu-system-sh4eb" ] = "";
	emulFiles[ "qemu-system-sparc" ] = "";
	emulFiles[ "qemu-system-sparc64" ] = "";
	
	// path empty - this not error. It return empty bin files list
	if( path.isEmpty() ) return emulFiles;
	
	// Path valid?
	if( ! QFile::exists(path) )
	{
		AQError( "QMap<QString, QString> System_Info::Find_QEMU_Binary_Files( const QString &path )",
				 QString("Path \"%1\" not exists!").arg(path) );
		return emulFiles;
	}
	
	// Find
	QString dirPath = QDir::toNativeSeparators( (path.endsWith("/") || path.endsWith("\\")) ? path : path + "/" );
	
	QMap<QString, QString>::iterator iter = emulFiles.begin();
	while( iter != emulFiles.end() )
	{
		#ifdef Q_OS_WIN32
		if( QFile::exists(dirPath + iter.key() + ".exe") ) iter.value() = dirPath + iter.key() + ".exe";
		#else
		if( QFile::exists(dirPath + iter.key()) ) iter.value() = dirPath + iter.key();
		#endif
		
		++iter;
	}
	
	/*// Next code for QEMU 1.0 in it version 'qemu' binary name changed to 'qemu-system-i386'
	if( emulFiles["qemu"].isEmpty() )
	{
		#ifdef Q_OS_WIN32
		if( QFile::exists(dirPath + "qemu-system-i386.exe") ) emulFiles[ "qemu" ] = dirPath + "qemu-system-i386.exe";
		#else
		if( QFile::exists(dirPath + "qemu-system-i386") ) emulFiles[ "qemu" ] = dirPath + "qemu-system-i386";
		#endif
	}*/
	
	return emulFiles;
}

/*
QMap<QString, QString> System_Info::Find_KVM_Binary_Files( const QString &path )
{
	QMap<QString, QString> emulFiles;
	emulFiles[ "qemu-kvm" ] = "";
	
	// path empty - this not error. It return empty bin files list
	if( path.isEmpty() ) return emulFiles;
	
	// Path valid?
	if( ! QFile::exists(path) )
	{
		AQError( "QMap<QString, QString> System_Info::Find_QEMU_Binary_Files( const QString &path )",
				 QString("Path \"%1\" not exists!").arg(path) );
		return emulFiles;
	}
	
	// Find
	QString dirPath = QDir::toNativeSeparators( (path.endsWith("/") || path.endsWith("\\")) ? path : path + "/" );
	
	if( QFile::exists(dirPath + "kvm") ) emulFiles[ "qemu-kvm" ] = dirPath + "kvm";
	else if( QFile::exists(dirPath + "qemu-kvm") ) emulFiles[ "qemu-kvm" ] = dirPath + "qemu-kvm";
	
	return emulFiles;
}*/

QString System_Info::Find_IMG( const QStringList &paths )
{
	if( paths.isEmpty() )
	{
		AQError( "static QString System_Info::Find_IMG( const QStringList &paths )",
				 "paths.isEmpty()" );
		return "";
	}
	
	// Find qemu-img or kvm-img
	for( int ix = 0; ix < paths.count(); ++ix )
	{
		QString dirPath = QDir::toNativeSeparators( (paths[ix].endsWith("/") || paths[ix].endsWith("\\"))
													? paths[ix]
													: paths[ix] + "/" );
		
		if( QFile::exists(dirPath + "qemu-img") ) return dirPath + "qemu-img";
		else if( QFile::exists(dirPath + "kvm-img") ) return dirPath + "kvm-img";
	}
	
	// No qemu-img or kvm-img in paths...
	AQWarning( "static QString System_Info::Find_IMG( const QStringList &paths )",
			   "Cannot find qemu-img or kvm-img!" );
	return "";
}

Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,
												 VM::Emulator_Version version, const QString &internalName )
{
	if( ! QFile::exists(path) )
	{
		AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				 "VM::Emulator_Version version, const QString &internalName )",
				 "Emulator binary \"" + path + "\" not exists!" );
		*ok = false;
		return Available_Devices();
	}
	
	// Get All Help
	QString all_help = Get_Emulator_Help_Output( path );
	
	if( all_help.isEmpty() )
	{
		AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				 "VM::Emulator_Version version, const QString &internalName )",
				 "Cannot read emulator help text" );
		*ok = false;
		return Available_Devices();
	}
	
	// Device for return
	Available_Devices tmp_dev;
	
	// Find Options
	
	// -M
	bool scan_Machines_List = false;
	QRegExp rx = QRegExp( ".*-M\\s+.*" );
	if( rx.exactMatch(all_help) ) scan_Machines_List = true;
	
	// -cpu
	bool scan_CPU_List = false;
	rx = QRegExp( ".*-cpu\\s.*" );
	if( rx.exactMatch(all_help) ) scan_CPU_List = true;
	
	// -smp FIXME PSO_SMP_Count use base emulator settings
	rx = QRegExp( ".*-smp\\s.*" );
	if( rx.exactMatch(all_help) )
	{
		// New Style?
		rx = QRegExp( ".*-smp\\s+.*\\[(.*)\\].*" );
		
		if( internalName == "qemu-system-x86_64" ) tmp_dev.PSO_SMP_Count = 255;
		else if( internalName == "qemu-system-sparc" ) tmp_dev.PSO_SMP_Count = 4;
		
		if( rx.exactMatch(all_help) )
		{
			if( rx.capturedTexts().count() > 1 )
			{
				if( rx.capturedTexts()[1].indexOf("cores")   != -1 ) tmp_dev.PSO_SMP_Cores = true;
				if( rx.capturedTexts()[1].indexOf("threads") != -1 ) tmp_dev.PSO_SMP_Threads = true;
				if( rx.capturedTexts()[1].indexOf("sockets") != -1 ) tmp_dev.PSO_SMP_Sockets = true;
				if( rx.capturedTexts()[1].indexOf("maxcpus") != -1 ) tmp_dev.PSO_SMP_MaxCPUs = true;
			}
		}
	}
	
	// -device
	rx = QRegExp( ".*-device\\s.*" );
	if( rx.exactMatch(all_help) )
	{
		tmp_dev.PSO_Device = true;
		
		// Get available devices list
		QString emulDevList = Get_Emulator_Output( path, QStringList() << "-device" << "?" );
		
		// Find usb-ehci support (USB 2.0)
		rx = QRegExp( ".*usb\\-ehci.*" );
		if( rx.exactMatch(emulDevList) ) tmp_dev.PSO_Device_USB_EHCI = true;
		
		// Find usb-xhci support (USB 3.0)
		rx = QRegExp( ".*usb\\-xhci.*" );
		if( rx.exactMatch(emulDevList) ) tmp_dev.PSO_Device_USB_XHCI = true;
	}
	
	// -drive
	rx = QRegExp( ".*-drive\\s+(\\[.*\\]).*-.*" );
	if( rx.exactMatch(all_help) )
	{
		tmp_dev.PSO_Drive = true;
		
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 )
		{
			// Parse drive options list
			QString drive_str = rx_list[ 1 ];
			
			if( drive_str.indexOf("file=") != -1 ) tmp_dev.PSO_Drive_File = true;
			if( drive_str.indexOf("if=") != -1 ) tmp_dev.PSO_Drive_If = true;
			if( drive_str.indexOf("bus=") != -1 ) tmp_dev.PSO_Drive_Bus_Unit = true;
			if( drive_str.indexOf("index=") != -1 ) tmp_dev.PSO_Drive_Index = true;
			if( drive_str.indexOf("media=") != -1 ) tmp_dev.PSO_Drive_Media = true;
			if( drive_str.indexOf("cyls=") != -1 ) tmp_dev.PSO_Drive_Cyls_Heads_Secs_Trans = true;
			if( drive_str.indexOf("snapshot=") != -1 ) tmp_dev.PSO_Drive_Snapshot = true;
			if( drive_str.indexOf("cache=") != -1 ) tmp_dev.PSO_Drive_Cache = true;
			if( drive_str.indexOf("aio=") != -1 ) tmp_dev.PSO_Drive_AIO = true;
			if( drive_str.indexOf("format=") != -1 ) tmp_dev.PSO_Drive_Format = true;
			if( drive_str.indexOf("serial=") != -1 ) tmp_dev.PSO_Drive_Serial = true;
			if( drive_str.indexOf("addr=") != -1 ) tmp_dev.PSO_Drive_ADDR = true;
			if( drive_str.indexOf("boot=") != -1 ) tmp_dev.PSO_Drive_Boot = true;
		}
	}
	
	// -boot [order=drives][,once=drives][,menu=on|off]
	rx = QRegExp( ".*-boot\\s+\\[order.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Boot_Order = true;
	
	// -g WxH[xDEPTH]
	rx = QRegExp( ".*-g\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Initial_Graphic_Mode = true;
	
	// -no-fd-bootchk
	rx = QRegExp( ".*-no-fd-bootchk\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_FB_Boot_Check = true;
	
	// -win2k-hack
	rx = QRegExp( ".*-win2k-hack\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Win2K_Hack = true;
	
	// -no-acpi
	rx = QRegExp( ".*-no-acpi\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_ACPI = true;
	
	// (old option -rtc-td-hack) -rtc *,drift=slew
	rx = QRegExp( ".*-rtc\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_RTC_TD_Hack = true;
	
	// -mtdblock
	rx = QRegExp( ".*-mtdblock\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_MTDBlock = true;
	
	// -sd
	rx = QRegExp( ".*-sd\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_SD = true;
	
	// -pflash
	rx = QRegExp( ".*-pflash\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_PFlash = true;
	
	// -name
	rx = QRegExp( ".*-name\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Name = true;
	
	// -curses
	rx = QRegExp( ".*-curses\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Curses = true;
	
	// -no-frame
	rx = QRegExp( ".*-no-frame\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_Frame = true;
	
	// -alt-grab
	rx = QRegExp( ".*-alt-grab\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Alt_Grab = true;
	
	// -no-quit
	rx = QRegExp( ".*-no-quit\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_Quit = true;
	
	// -sdl
	rx = QRegExp( ".*-sdl\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_SDL = true;
	
	// -portrait
	rx = QRegExp( ".*-portrait\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Portrait = true;
	
	// -no-shutdown
	rx = QRegExp( ".*-no-shutdown\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_Shutdown = true;
	
	// (old -startdate option) -rtc base=date
	rx = QRegExp( ".*-rtc\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Startdate = true;
	
	// -show-cursor
	rx = QRegExp( ".*-show-cursor\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Show_Cursor = true;
	
	// -bootp
	rx = QRegExp( ".*-bootp\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Bootp = true;

	// -netdev
	QString net_str = "";
	rx = QRegExp( ".*(-netdev\\s+.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		QStringList rx_list = rx.capturedTexts();
        
        /*for ( int i = 0; i < rx_list.count(); i++)
        {
            AQDebug(rx_list.at(i),"----------------------------------------------------------------------------");
        }*/

		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}

	// -net nic
	rx = QRegExp( ".*(-net\\s+nic.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}
	
	// -net user
	rx = QRegExp( ".*(-net\\s+user.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}
	
	// -net bridge
	rx = QRegExp( ".*(-net\\s+bridge.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}

	// -net tap
	rx = QRegExp( ".*(-net\\s+tap.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}
	
	// -net socket
	rx = QRegExp( ".*(-net\\s+socket.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}
	
	// -net vde
	rx = QRegExp( ".*(-net\\s+vde.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		tmp_dev.PSO_Net_type_vde = true;
		
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}
	
	// -net dump
	rx = QRegExp( ".*(-net\\s+dump.*)-net.*" );
	if( rx.exactMatch(all_help) )
	{
		tmp_dev.PSO_Net_type_dump = true;
		
		QStringList rx_list = rx.capturedTexts();
		if( rx_list.count() > 1 ) net_str += rx_list[ 1 ];
	}
	
	// Check Network Arguments
	if( net_str.isEmpty() )
	{
		AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				 "VM::Emulator_Version version, const QString &internalName )",
				 "net_str is Empty" );
	}
	else
	{
		// name
		rx = QRegExp( ".*name=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_name = true;
		
		// addr
		rx = QRegExp( ".*addr=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_addr = true;
		
		// vectors
		rx = QRegExp( ".*vectors=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_vectors = true;
		
		
		// net
		rx = QRegExp( ".*net=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_net = true;
		
		// host
		rx = QRegExp( ".*host=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_host = true;
		
		// restrict
		rx = QRegExp( ".*restrict=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_restrict = true;
		
		// dhcpstart
		rx = QRegExp( ".*dhcpstart=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_dhcpstart = true;
		
		// dns
		rx = QRegExp( ".*dns=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_dns = true;
		
		// tftp
		rx = QRegExp( ".*tftp=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_tftp = true;
		
		// bootfile
		rx = QRegExp( ".*bootfile=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_bootfile = true;
		
		// smb
		rx = QRegExp( ".*smb=.*" );
		if( rx.exactMatch(net_str) ) {
                tmp_dev.PSO_Net_smb = true;
        }

		// hostfwd
		rx = QRegExp( ".*hostfwd=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_hostfwd = true;
		
		// guestfwd
		rx = QRegExp( ".*guestfwd=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_guestfwd = true;
		
		
		// ifname
		rx = QRegExp( ".*ifname=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_ifname = true;
		
		// bridge
		rx = QRegExp( ".*br=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_bridge = true;

		// script
		rx = QRegExp( ".*script=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_script = true;
		
		// downscript
		rx = QRegExp( ".*downscript=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_downscript = true;
		
		// helper
		rx = QRegExp( ".*helper=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_helper = true;

		// sndbuf
		rx = QRegExp( ".*sndbuf=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_sndbuf = true;
		
		// vnet_hdr
		rx = QRegExp( ".*vnet_hdr=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_vnet_hdr = true;
		
		// vhost
		rx = QRegExp( ".*vhost=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_vhost = true;
		
		// vhostfd
		rx = QRegExp( ".*vhostfd=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_vhostfd = true;
		
		// listen
		rx = QRegExp( ".*listen=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_listen = true;
		
		// connect
		rx = QRegExp( ".*connect=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_connect = true;
		
		
		// mcast
		rx = QRegExp( ".*mcast=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_mcast = true;
		
		
		// sock
		rx = QRegExp( ".*sock=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_sock = true;
		
		// port
		rx = QRegExp( ".*port=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_port = true;
		
		// group
		rx = QRegExp( ".*group=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_group = true;
		
		// mode
		rx = QRegExp( ".*mode=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_mode = true;
		
		
		// file
		rx = QRegExp( ".*file=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_file = true;
		
		// len
		rx = QRegExp( ".*len=.*" );
		if( rx.exactMatch(net_str) ) tmp_dev.PSO_Net_len = true;
	}
	
	// -enable-kvm
	rx = QRegExp( ".*-enable-kvm\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Enable_KVM = true;
	
	// -no-kvm
	rx = QRegExp( ".*-no-kvm\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_KVM = true;
	
	// -no-kvm-irqchip
	rx = QRegExp( ".*-no-kvm-irqchip\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_KVM_IRQChip = true;
	
	// -no-kvm-pit
	rx = QRegExp( ".*-no-kvm-pit\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_KVM_Pit = true;
	
	// -no-kvm-pit-reinjection
	rx = QRegExp( ".*-no-kvm-pit-reinjection\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_No_KVM_Pit_Reinjection = true;
	
	// -enable-nesting
	rx = QRegExp( ".*-enable-nesting\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Enable_Nesting = true;
	
	// -kvm-shadow-memory
	rx = QRegExp( ".*kvm_shadow_mem=.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_KVM_Shadow_Memory = true;
	
	// -tftp
	rx = QRegExp( ".*-tftp\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_TFTP = true;

    if ( tmp_dev.PSO_Net_tftp == true )
    {  //hack, all this code should probably be rewritten or removed
       //but in the meantime this hopefully makes things work
       tmp_dev.PSO_TFTP = true;
    }

	// -smb
	rx = QRegExp( ".*-smb\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_SMB = true;

    if ( tmp_dev.PSO_Net_smb == true )
    {  //hack, all this code should probably be rewritten or removed
       //but in the meantime this hopefully makes things work
       tmp_dev.PSO_SMB = true;
    }

	// -std-vga
	rx = QRegExp( ".*-std-vga\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_Std_VGA = true;
	
	// -spice
	rx = QRegExp( ".*-spice\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_SPICE = true;
	
	// -qxl
	rx = QRegExp( ".*-qxl\\s.*" );
	if( rx.exactMatch(all_help) ) tmp_dev.PSO_QXL = true;
	
	// Base emulator
	// Get Default Devices List
	Available_Devices default_device;
	
	switch( version )
	{
		case VM::QEMU_2_0:
			default_device = Emulator_QEMU_2_0[ internalName ];
			break;
			
		default:
			AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
					 "VM::Emulator_Version version, const QString &internalName )",
					 "Emulator version invalid!" );
			*ok = false;
			return Available_Devices();
	}
	
	if( default_device.System.QEMU_Name.isEmpty() )
	{
		AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				 "VM::Emulator_Version version, const QString &internalName )",
				 "Cannot get system default device list!" );
		*ok = false;
		return Available_Devices();
	}
	
	tmp_dev.System.Caption = default_device.System.Caption;
	tmp_dev.System.QEMU_Name = internalName;
	
	AQDebug( "Start device search for:", internalName );
	
	QStringList args_list;
	QString tmp = "";
	
	// Get CPU Models
	args_list << "-cpu" << "?";
	QString cpu_list_str = Get_Emulator_Output( path, args_list );
	QTextStream *text_stream = new QTextStream( &cpu_list_str );
	
	do
	{
		tmp = text_stream->readLine();
		QString qemu_dev_name = "";
		
		if( tmp.indexOf("Available CPUs") != -1 ) continue;
		if( tmp.indexOf("Sparc") != -1 ) break; // FIXME Sparc -cpu not supported in this release
		
		QStringList regExpVariants;
		regExpVariants << ".*x86\\s+([\\w-]+).*|.*x86\\s+\\[([\\w-]+)\\].*"	// x86 pentium3 | x86 [core2duo]
					   << ".*PowerPC\\s+(.*)\\s+PVR.*"	// PowerPC 750 PVR 00080301
					   << ".*MIPS\\s+\\'(.*)\\'.*"		// MIPS '4Kc'
					   << ".*\\s*([\\w-]+).*";			// cortex-a9
		
		// Get QEMU ID String
		for( int lx = 0; lx < regExpVariants.count(); ++lx )
		{
			QRegExp tmp_rx = QRegExp( regExpVariants[lx] );
			if( tmp_rx.exactMatch(tmp) )
			{
				QStringList rx_list = tmp_rx.capturedTexts();
				if( rx_list.count() > 1 )
				{
					if( ! rx_list[1].isEmpty() )
						qemu_dev_name = rx_list[ 1 ];
					else if( rx_list.count() > 2 )
						qemu_dev_name = rx_list[ 2 ];
					break;
				}
				else
				{
					AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
							 "VM::Emulator_Version version, const QString &internalName )",
							 "Cannot get QEMU CPU ID string!" );
					continue;
				}
			}
			else continue;
		}
		
		// String empty?
		if( ! (qemu_dev_name.isEmpty() ||
			   qemu_dev_name.indexOf(QRegExp("/^\\S+$/"), 0) != -1) )
		{
			bool cpu_found = false;
			for( int ix = 0; ix < default_device.CPU_List.count(); ix++ )
			{
				if( qemu_dev_name == default_device.CPU_List[ix].QEMU_Name )
				{
					tmp_dev.CPU_List << default_device.CPU_List[ ix ];
					cpu_found = true;
				}
			}
			
			// No this device name in default list
			if( cpu_found == false )
				tmp_dev.CPU_List << Device_Map( qemu_dev_name, qemu_dev_name );
		}
		else continue;
	}
	while( ! tmp.isNull() );
	
	// No CPU's... Set Default List
	if( tmp_dev.CPU_List.count() < 2 )
	{
		tmp_dev.CPU_List = default_device.CPU_List; // FIXME Emul Version
		AQWarning( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				   "VM::Emulator_Version version, const QString &internalName )",
				   QString("Cannot get CPU's info from emulator \"%1\". Use default list").arg(path) );
	}
	
	// Get Machines Models
	args_list.clear();
	args_list << "-M" << "?";
	QString machines_list_str = Get_Emulator_Output( path, args_list );
	text_stream = new QTextStream( &machines_list_str );
	
	do
	{
		tmp = text_stream->readLine();
		Device_Map dev_map;
		
		// This description?
		if( tmp.startsWith("Supported machines are") ) continue;
		
		// Get QEMU ID String
		QRegExp tmp_rx = QRegExp( "([\\w-.]+)\\s+(\\S.*)" );
		if( tmp_rx.exactMatch(tmp) )
		{
			QStringList rx_list = tmp_rx.capturedTexts();
			if( rx_list.count() > 2 )
			{
				dev_map.Caption = rx_list[ 2 ];
				dev_map.QEMU_Name = rx_list[ 1 ];
			}
			else
			{
				AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
						 "VM::Emulator_Version version, const QString &internalName )",
						 "Cannot get QEMU Machine ID string!" );
				continue;
			}
		}
		
		if( ! (dev_map.QEMU_Name.isEmpty() || dev_map.QEMU_Name.indexOf(QRegExp("/^\\S+$/"), 0) != -1) )
		{
			bool machine_found = false;
			for( int ix = 0; ix < default_device.Machine_List.count(); ix++ )
			{
				if( dev_map.QEMU_Name == default_device.Machine_List[ix].QEMU_Name )
				{
					tmp_dev.Machine_List << default_device.Machine_List[ ix ];
					machine_found = true;
				}
			}
			
			// No this device name in default list
			if( machine_found == false ) tmp_dev.Machine_List << dev_map;
		}
		else continue;
	}
	while( ! tmp.isNull() );
	
	// No Machines... Set Default List
	if( tmp_dev.Machine_List.count() < 2 )
	{
		tmp_dev.Machine_List = default_device.Machine_List; // FIXME Emul Version
		AQWarning( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				   "VM::Emulator_Version version, const QString &internalName )",
				   QString("Cannot get machines info from emulator \"%1\". Use default list").arg(path) );
	}
	
	// -vga
	QString vga_list_string = "";
	text_stream = new QTextStream( &all_help );
	
	// Find -vga line
	do
	{
		vga_list_string = text_stream->readLine();
		if( vga_list_string.startsWith("-vga") ) break;
	}
	while( ! vga_list_string.isNull() );
	
	// Get video cards names
	rx = QRegExp( ".*-vga\\s+\\[(.*)\\].*" );
	if( rx.exactMatch(vga_list_string) )
	{
		// Parse vga devices list string
		if( rx.capturedTexts().count() > 1 )
		{
			QStringList vga_devices_list = rx.capturedTexts()[1].split( "|", QString::SkipEmptyParts );
			
			if( vga_devices_list.isEmpty() )
			{
				AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
						 "VM::Emulator_Version version, const QString &internalName )",
						 "VGA Devices List is Empty. Data is: \"" + rx.capturedTexts()[0] + "\"" );
			}
			else
			{
				for( int gx = 0; gx < vga_devices_list.count(); gx++ )
				{
					bool vga_found = false;
					
					for( int ix = 0; ix < default_device.Video_Card_List.count(); ix++ )
					{
						if( vga_devices_list[gx] == default_device.Video_Card_List[ix].QEMU_Name )
						{
							tmp_dev.Video_Card_List << default_device.Video_Card_List[ ix ];
							vga_found = true;
						}
					}
					
					// No this device name in default list
					if( vga_found == false ) tmp_dev.Video_Card_List << Device_Map( vga_devices_list[gx], vga_devices_list[gx] );
				}
			}
		}
		else
		{
			AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
					 "VM::Emulator_Version version, const QString &internalName )",
					 "Cannot parse VGA string regExp. Data is: \"" + rx.capturedTexts()[0] + "\"" );
		}
	}
	
	// Get Audio Cards Models
	args_list.clear();
	args_list << "-soundhw" << "?";
	QString audio_list_str = Get_Emulator_Output( path, args_list );
	text_stream = new QTextStream( &audio_list_str );
	
	do
	{
		tmp = text_stream->readLine();
		QString qemu_dev_name = "";
		
		// This target platform support soundhw?
		if( tmp.contains("not supported") ) break;
		
		// This description?
		if( tmp.isEmpty() ||
			tmp.startsWith("Valid sound card names") ||
			tmp.startsWith("-soundhw") ||
			tmp.indexOf(QRegExp("/^\\S+$/"), 0) != -1 ) continue;
		
		// Get QEMU ID String
		QRegExp tmp_rx = QRegExp( "([\\w-.]+)\\s+.*" );
		if( tmp_rx.exactMatch(tmp) )
		{
			QStringList rx_list = tmp_rx.capturedTexts();
			if( rx_list.count() > 1 ) qemu_dev_name = rx_list[ 1 ];
			else
			{
				AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
						 "VM::Emulator_Version version, const QString &internalName )",
						 "Cannot get QEMU Audio ID string!" );
				continue;
			}
		}
		
		if( qemu_dev_name == "sb16" ) tmp_dev.Audio_Card_List.Audio_sb16 = true;
		else if( qemu_dev_name == "es1370" ) tmp_dev.Audio_Card_List.Audio_es1370 = true;
		else if( qemu_dev_name == "gus" ) tmp_dev.Audio_Card_List.Audio_GUS = true;
		else if( qemu_dev_name == "adlib" ) tmp_dev.Audio_Card_List.Audio_Adlib = true;
		else if( qemu_dev_name == "pcspk" ) tmp_dev.Audio_Card_List.Audio_PC_Speaker = true;
		else if( qemu_dev_name == "ac97" ) tmp_dev.Audio_Card_List.Audio_AC97 = true;
		else if( qemu_dev_name == "hda" ) tmp_dev.Audio_Card_List.Audio_HDA = true;
		else if( qemu_dev_name == "cs4231a" ) tmp_dev.Audio_Card_List.Audio_cs4231a = true;
		else
		{
			if( ! qemu_dev_name.isEmpty() )
				AQWarning( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
						   "VM::Emulator_Version version, const QString &internalName )",
						   "Unregistred Sound Card Name: \"" + qemu_dev_name + "\"" );
			continue;
		}
	}
	while( ! tmp.isNull() );
	
	// Get Network Card Models
	args_list.clear();
	args_list << "-net" << "nic,model=?";
	
	QString net_list_str = Get_Emulator_Output( path, args_list );
	text_stream = new QTextStream( &net_list_str );
	
	do
	{
		tmp = text_stream->readLine();
		
		// This description?
		if( tmp.indexOf("models:") == -1 ) continue;
		
		// Get all models string
		QString all_models = tmp.mid( tmp.indexOf("models:") + QString("models:").count(), tmp.count() );
		all_models.remove( QChar(' ') ); // Remove all spaces
		
		QStringList net_cards_models = all_models.split( ',', QString::SkipEmptyParts );
		
		if( net_cards_models.isEmpty() )
		{
			AQError( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
					 "VM::Emulator_Version version, const QString &internalName )",
					 "Cannot get QEMU network card ID string!" );
			continue;
		}
		
		// Find names in devices list
		for( int ax = 0; ax < net_cards_models.count(); ax++ )
		{
			bool net_found = false;
			QString net_dev_str = "";
			
			for( int bx = 0; bx < default_device.Network_Card_List.count(); bx++ )
			{
				net_dev_str = net_cards_models[ ax ];
				
				if( net_dev_str == default_device.Network_Card_List[bx].QEMU_Name )
				{
					tmp_dev.Network_Card_List << default_device.Network_Card_List[ bx ];
					net_found = true;
					break;
				}
			}
			
			// No this device name in default list
			if( net_found == false ) tmp_dev.Network_Card_List << Device_Map( net_dev_str, net_dev_str );
		}
		
		break; // All Done
	}
	while( ! tmp.isNull() );
	
	// No Cards... Set Default List
	if( tmp_dev.Network_Card_List.count() < 2 )
	{
		tmp_dev.Network_Card_List = default_device.Network_Card_List; // FIXME Emul Version
		AQWarning( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
				   "VM::Emulator_Version version, const QString &internalName )",
				   QString("Cannot get net cards info from emulator \"%1\". Use default list").arg(path) );
	}
	
	// Return info
	AQDebug( "Available_Devices System_Info::Get_Emulator_Info( const QString &path, bool *ok,"
			 "VM::Emulator_Version version, const QString &internalName )",
			 "Done" );
	*ok = true;
	return tmp_dev;
}

QString System_Info::Get_Emulator_Help_Output( const QString &path )
{
	QString result = Get_Emulator_Output( path, QStringList("-h") );
	
	if( result.isEmpty() )
	{
		result = Get_Emulator_Output( path, QStringList("-nographic") );
		
		if( result.isEmpty() )
		{
			AQError( "QString System_Info::Get_Emulator_Help_Output( const QString &path )",
					 "Cannot get emulator help!" );
			return "";
		}
	}
	
	return result;
}

QString System_Info::Get_Emulator_Output( const QString &path, const QStringList &args )
{
	QProcess *qemu_pr = new QProcess();
	
	qemu_pr->start( path, args );
	
	if( ! qemu_pr->waitForFinished(2000) )
	{
		AQError( "QStringList System_Info::Get_Emulator_Output( const QString &path, const QStringList &args )",
				 QString("Time left. File: \"%1\" Args: \"%1\"").arg(path).arg(args.join(" ")) );
		
		qemu_pr->kill();
		return QString();
	}
	
	QString result = qemu_pr->readAllStandardOutput();
	result += qemu_pr->readAllStandardError(); // readAll() not read cerr messages...
	delete qemu_pr;
	
	return result;
}

const QList<VM_USB> &System_Info::Get_All_Host_USB()
{
	if( All_Host_USB.count() <= 0 ) Update_Host_USB();
	
	return All_Host_USB;
}

const QList<VM_USB> &System_Info::Get_Used_USB_List()
{
	return Used_Host_USB;
}

bool System_Info::Add_To_Used_USB_List( const VM_USB &device )
{
	if( Used_Host_USB.indexOf(device) == -1 )
	{
		Used_Host_USB << device;
		return true;
	}
	else return false;
}

bool System_Info::Delete_From_Used_USB_List( const VM_USB &device )
{
	return Used_Host_USB.removeOne( device );
}

#ifdef Q_OS_LINUX

bool System_Info::Update_Host_USB()
{
	QList<VM_USB> list;
	
	if( QFile::exists("/sys/bus/usb") )
	{
		if( Scan_USB_Sys(list) )
		{
			All_Host_USB = list;
			return true;
		}
	}
	
	if( QFile::exists("/proc/bus/usb/devices") )
	{
		if( Scan_USB_Proc(list) )
		{
			All_Host_USB = list;
			return true;
		}
	}
	
	// Error...
	AQError( "bool System_Info::Update_Host_USB()",
			 "Cannot read USB information from /sys, /proc, /dev!" );
	return false;
}

bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )
{
	list.clear();
	QDir dir("/sys/bus/usb/devices/");
	
	QStringList usb_dirs;
	QStringList all_usb_dirs = dir.entryList( QStringList("*"), QDir::Dirs, QDir::Name );
	
	// add only unique usb device folders
	QRegExp re_usbNum = QRegExp( "^usb\\d+$" ); // like: usb5
	QRegExp re_NumNum = QRegExp( "^\\d+[-]\\d+$" ); // like: 1-2
	QRegExp re_NumNumNum = QRegExp( "^\\d+[-]\\d+[.]\\d+$" ); // like: 1-2.1
	
	foreach( QString cur_dir, all_usb_dirs )
	{
		if( re_usbNum.exactMatch(cur_dir) ) usb_dirs << cur_dir;
		else if( re_NumNum.exactMatch(cur_dir) ) usb_dirs << cur_dir;
		else if( re_NumNumNum.exactMatch(cur_dir) ) usb_dirs << cur_dir;
		else continue;
	}
	
	if( usb_dirs.isEmpty() )
	{
		AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
				   "No USB Folders in /sys/bus/usb/devices/" );
		return false;
	}
	
	for( int dx = 0; dx < usb_dirs.count(); dx++ )
	{
		QString usb_path = "/sys/bus/usb/devices/" + usb_dirs[ dx ] + "/";
		QString data = "";
		VM_USB tmp_usb;
		
		// idVendor
		if( Read_SysFS_File(usb_path + "idVendor", data) )
		{
			tmp_usb.Set_Vendor_ID( data );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "Cannot read idVendor from /sys/bus/usb/devices/" );
			continue;
		}
		
		// idProduct
		if( Read_SysFS_File(usb_path + "idProduct", data) )
		{
			tmp_usb.Set_Product_ID( data );
		}
		else
		{
			AQError( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					 "Cannot read idProduct from /sys/bus/usb/devices/" );
			continue;
		}
		
		// manufacturer
		if( Read_SysFS_File(usb_path + "manufacturer", data) )
		{
			tmp_usb.Set_Manufacturer_Name( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read manufacturer from /sys/bus/usb/devices/" );
			tmp_usb.Set_Manufacturer_Name( "Unknown Manufacturer" );
		}
		
		// product
		if( Read_SysFS_File(usb_path + "product", data) )
		{
			tmp_usb.Set_Product_Name( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read product from /sys/bus/usb/devices/" );
			tmp_usb.Set_Product_Name( "Unknown Product" );
		}
		
		// speed
		if( Read_SysFS_File(usb_path + "speed", data) )
		{
			tmp_usb.Set_Speed( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read speed from /sys/bus/usb/devices/" );
		}
		
		// Serial Number
		if( Read_SysFS_File(usb_path + "serial", data) )
		{
			tmp_usb.Set_Serial_Number( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read serial from /sys/bus/usb/devices/" );
		}
		
		// Bus
		if( Read_SysFS_File(usb_path + "busnum", data) )
		{
			tmp_usb.Set_Bus( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read busnum from /sys/bus/usb/devices/" );
			continue;
		}
		
		// Addr
		if( Read_SysFS_File(usb_path + "devnum", data) )
		{
			tmp_usb.Set_Addr( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read devnum from /sys/bus/usb/devices/" );
			continue;
		}
		
		// DevPath
		if( Read_SysFS_File(usb_path + "devpath", data) )
		{
			tmp_usb.Set_DevPath( data );
		}
		else
		{
			AQWarning( "bool System_Info::Scan_USB_Sys( QList<VM_USB> &list )",
					   "Cannot read devpath from /sys/bus/usb/devices/" );
			continue;
		}
		
		// All Data Read
		tmp_usb.Set_Use_Host_Device( true );
		list << tmp_usb;
	}
	
	return true;
}

bool System_Info::Read_SysFS_File( const QString &path, QString &data )
{
	QFile sysfs_file( path );
	
	if( ! sysfs_file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		AQWarning( "bool System_Info::Read_SysFS_File( const QString &path, QString &data )",
				   QString("Cannot Open File \"%1\"!").arg(path) );
		return false;
	}
	
	QTextStream in( &sysfs_file );
	QString line = in.readAll();
	sysfs_file.close();
	
	if( line.isEmpty() )
	{
		AQWarning( "bool System_Info::Read_SysFS_File( const QString &path, QString &data )",
				   "File is Empty!" );
		return false;
	}
	else
	{
		line = line.remove( "\n" );
		data = line;
		return true;
	}
}

bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )
{
	list.clear();
	QFile proc_file( "/proc/bus/usb/devices" );
	
	if( ! proc_file.open(QIODevice::ReadOnly | QIODevice::Text) )
	{
		AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
				 "Cannot Open File \"/proc/bus/usb/devices\"! USB information not found!" );
		return false;
	}
	
	QTextStream in( &proc_file );
	QList<QStringList> linux_usb_dev;
	
	QString line = in.readLine();
	
	while( ! line.isNull() )
	{
		QStringList tmp_str;
		
		if( line.startsWith("T:") )
		{
			tmp_str << line;
			
			while( ! in.atEnd() )
			{
				line = in.readLine();
				
				if( line.startsWith("P:") ||
					line.startsWith("S:  Manufacturer") ||
					line.startsWith("S:  Product") ||
					line.startsWith("S:  SerialNumber") ) tmp_str << line;
				else if( line.startsWith("E:") ) break;
			}
			
			linux_usb_dev << tmp_str;
		}
		
		line = in.readLine();
	}
	
	proc_file.close();
	
	for( int ex = 0; ex < linux_usb_dev.count(); ++ex )
	{
		if( linux_usb_dev[ex].count() < 4 )
		{
			AQWarning( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
					   "linux_usb_dev[ex].count() < 4" );
		}
	}
	
	// T:  Bus=01 Lev=00 Prnt=00 Port=00 Cnt=00 Dev#=  1 Spd=480 MxCh= 6
	QRegExp busAddr = QRegExp( "T:[\\s]+Bus=([\\d]{2}).+Dev#=[\\s]*([\\d]+).+Spd=([\\d]{1,3}).*" );
	
	// P:  Vendor=0123 ProdID=abcd Rev= 0.01
	QRegExp idHex = QRegExp( "P:[\\s]+Vendor=([\\dabcdef]{4})[\\s]+ProdID=([\\dabcdef]{4}).+" );
	
	// S:  Manufacturer=Mega Cool Manufacturer
	QRegExp manufacturer = QRegExp( "S:[\\s]+Manufacturer=(.+)" );
	
	// S:  Product=Super USB Device
	QRegExp product = QRegExp( "S:[\\s]+Product=(.+)" );
	
	// S:  SerialNumber=0000:00:12.2
	QRegExp serialNumber = QRegExp( "S:[\\s]+SerialNumber=(.+)" );
	
	for( int ix = 0; ix < linux_usb_dev.count(); ix++ )
	{
		if( linux_usb_dev[ix].count() <= 0 )
		{
			AQWarning( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
					   "linux_usb_dev[ix].count() <= 0" );
			continue;
		}
		
		QStringList busAddr_list;
		QStringList idHex_list;
		QStringList manufacturer_list;
		QStringList product_list;
		QStringList serialNumber_list;
		
		for( int bx = 0; bx < linux_usb_dev[ix].count(); bx++ )
		{
			if( busAddr_list.count() <= 0 )
			{
				if( busAddr.exactMatch(linux_usb_dev[ix][bx]) )
				{
					busAddr_list = busAddr.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match busAddr! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( idHex_list.count() <= 0 )
			{
				if( idHex.exactMatch(linux_usb_dev[ix][bx]) )
				{
					idHex_list = idHex.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match idHex! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( manufacturer_list.count() <= 0 )
			{
				if( manufacturer.exactMatch(linux_usb_dev[ix][bx]) )
				{
					manufacturer_list = manufacturer.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match manufacturer! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( product_list.count() <= 0 )
			{
				if( product.exactMatch(linux_usb_dev[ix][bx]) )
				{
					product_list = product.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match product! String: " + linux_usb_dev[ix][bx] );
				}
			}
			
			if( serialNumber_list.count() <= 0 )
			{
				if( serialNumber.exactMatch(linux_usb_dev[ix][bx]) )
				{
					serialNumber_list = serialNumber.capturedTexts();
					continue;
				}
				else
				{
					AQError( "bool System_Info::Scan_USB_Proc( QList<VM_USB> &list )",
							 "Cannot Match serialNumber! String: " + linux_usb_dev[ix][bx] );
				}
			}
		}
		
		// Create VM_USB
		if( idHex_list.count() > 1 )
		{
			if( idHex_list[1] != "0000" && idHex_list[2] != "0000" )
			{
				VM_USB tmp_usb;
				
				tmp_usb.Set_Vendor_ID( idHex_list[1] );
				tmp_usb.Set_Product_ID( idHex_list[2] );
				
				if( manufacturer_list.count() > 0 )
				{
					tmp_usb.Set_Manufacturer_Name( manufacturer_list[1] );
				}
				
				if( product_list.count() > 0 )
				{
					tmp_usb.Set_Product_Name( product_list[1] );
				}
				
				if( busAddr_list.count() >= 3 )
				{
					QString busStr = (busAddr_list[1][0] == '0') ? QString(busAddr_list[1][1]) : busAddr_list[1];
					tmp_usb.Set_Bus( busStr );
					tmp_usb.Set_Addr( busAddr_list[2] );
					// FIXME add usb device path also
					tmp_usb.Set_Speed( busAddr_list[3] );
				}
				
				if( serialNumber_list.count() > 0 )
				{
					tmp_usb.Set_Serial_Number( serialNumber_list[1] );
				}
				
				list << tmp_usb;
			}
		}
	}
	
	return true;
}

#include <sys/sysinfo.h>
#include <QDir>
#include <QFileInfoList>

void System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )
{
	// proc fs is exists?
	if( QFile::exists("/proc/meminfo") )
	{
		QFile proc_file( "/proc/meminfo" );
		
		if( ! proc_file.open(QIODevice::ReadOnly | QIODevice::Text) )
		{
			AQDebug( "int System_Info::Get_Free_Memory_Size()",
					 "void System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )" );
		}
		else
		{
			QTextStream in( &proc_file );
			QString line = in.readLine();
			
			int all_mem, free_mem, cached_mem, buf_mem;
			all_mem = free_mem = cached_mem = buf_mem = -1;
			
			while( ! line.isNull() )
			{
				if( (all_mem != -1 ) && (free_mem != -1) && (cached_mem != -1) && (buf_mem != -1) )
				{
					allRAM = (int)(all_mem / 1024.0);
					freeRAM = (int)((free_mem + cached_mem + buf_mem) / 1024.0);
					return; // exit this
				}
				
				if( line.startsWith("MemTotal") )
				{
					QRegExp rx = QRegExp( "MemTotal:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else all_mem = res[1].toInt();
					}
					else break;
				}
				else if( line.startsWith("MemFree") )
				{
					QRegExp rx = QRegExp( "MemFree:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else free_mem = res[1].toInt();
					}
					else break;
				}
				else if( line.startsWith("Cached") )
				{
					QRegExp rx = QRegExp( "Cached:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else cached_mem = res[1].toInt();
					}
					else break;
				}
				else if( line.startsWith("Buffers") )
				{
					QRegExp rx = QRegExp( "Buffers:\\s+(\\d+)\\s+.*" );
					
					if( rx.exactMatch(line) )
					{
						QStringList res = rx.capturedTexts();
						
						if( res.count() != 2 ) break;
						else buf_mem = res[1].toInt();
					}
					else break;
				}
				
				line = in.readLine(); // Read next line
			}
		}
	}
	
	// If proc fs not exists
	struct sysinfo sysInfo;
	sysinfo( &sysInfo );
	
	allRAM = (int)(unsigned long)(sysInfo.totalram * sysInfo.mem_unit) / 1024.0 / 1024.0;
	freeRAM  = (int)(unsigned long)((sysInfo.freeram + sysInfo.bufferram) * sysInfo.mem_unit) / 1024.0 / 1024.0;
}

QStringList System_Info::Get_Host_FDD_List()
{
	// Find Floppy's
	QDir dev_dir( "/dev/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("fd*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	// Find CD-ROM's
	QDir dev_dir( "/dev/" );
	
	QStringList name_filters;
	name_filters << "cdrom*" << "sr*";
	
	QFileInfoList devices = dev_dir.entryInfoList( name_filters, QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[ d ].absoluteFilePath();
	}
	
	// Additional CDROM Devices (This Added By User)
	QSettings settings;
	int cdrom_count = settings.value( "Additional_CDROM_Devices/Count", "0" ).toString().toInt();
	
	for( int ix = 0; ix < cdrom_count; ix++ )
	{
		tmp_list << settings.value("Additional_CDROM_Devices/Device" + QString::number(ix), "").toString();
	}
	
	return tmp_list;
}

#endif // Linux

#ifdef Q_OS_FREEBSD

#include <sys/sysctl.h>
#include <QDir>
#include <QFileInfoList>

void System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )
{
	size_t all_ram = 0, free_ram = 0, cached_mem = 0;
	size_t len = sizeof( all_ram );
	
	if( (sysctlbyname("vm.stats.vm.v_page_count", &all_ram, &len, NULL, 0) == -1) || ! len )
	{
		all_ram = 0;
		AQError( "void  System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )",
				 "Cannot Get Information on Memory! (vm.stats.vm.v_page_count)" );
	}
	
	if( (sysctlbyname("vm.stats.vm.v_free_count", &free_ram, &len, NULL, 0) == -1) || ! len )
	{
		free_ram = 0;
		AQError( "void  System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )",
				 "Cannot Get Information on Memory! (vm.stats.vm.v_free_count)" );
	}
	
	if( (sysctlbyname("vm.stats.vm.v_cache_count", &cached_mem, &len, NULL, 0) == -1) || ! len )
	{
		cached_mem = 0;
		AQError( "void  System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )",
				 "Cannot Get Information on Memory! (vm.stats.vm.v_cache_count)" );
	}
	
	int page_size = getpagesize();
	
	all_ram *= page_size;
	free_ram *= page_size;
	cached_mem *= page_size;
	
	allRAM = (int)(all_ram / 1024.0 / 1024.0);
	freeRAM = (int)((free_ram + cached_mem) / 1024.0 / 1024.0);
}

QStringList System_Info::Get_Host_FDD_List()
{
	// Find Floppy's
	QDir dev_dir( "/dev/fd/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	// Find CD-ROM's
	QDir dev_dir( "/dev/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("acd*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

bool System_Info::Update_Host_USB()
{
	AQError( "System_Info::Update_Host_USB()",
			 "Not implemented!" );
	return false;
}

#endif // FreeBSD

#ifdef Q_OS_MAC

#include <sys/sysctl.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <mach/task_info.h>
#include <mach/task.h>
#include <QDir>
#include <QFileInfoList>

void System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )
{
	int mib[6]; 
	mib[0] = CTL_HW;
	mib[1] = HW_PAGESIZE;

	int page_size;
	size_t length;
	length = sizeof (page_size);
	if (sysctl (mib, 2, &page_size, &length, NULL, 0) < 0)
	{
				AQError( "void  System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )",
				 "Cannot Get Memory Pagesize! (sysctl)" );
	}

	mach_msg_type_number_t count = HOST_VM_INFO_COUNT;

	vm_statistics_data_t vmstat;
	if (host_statistics (mach_host_self (), HOST_VM_INFO, (host_info_t) &vmstat, &count) != KERN_SUCCESS)
	{
		AQError( "void  System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )",
				 "Cannot Get Information on Memory! (host_statistics)" );
	}
	
	double all_ram = vmstat.wire_count + vmstat.active_count + vmstat.inactive_count + vmstat.free_count;
	double free_ram = vmstat.free_count;
	double cached_ram = vmstat.inactive_count;
		
	all_ram *= page_size;
	free_ram *= page_size;
	cached_ram *= page_size;
	
	allRAM = (int)(all_ram / 1024.0 / 1024.0);
	freeRAM = (int)((free_ram + cached_ram) / 1024.0 / 1024.0);
}

QStringList System_Info::Get_Host_FDD_List()
{
	// Find Floppy's
	QDir dev_dir( "/dev/fd/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	// Find CD-ROM's
	QDir dev_dir( "/dev/" );
	QFileInfoList devices = dev_dir.entryInfoList( QStringList("acd*"), QDir::System, QDir::Name );
	
	QStringList tmp_list;
	
	for( int d = 0; d < devices.count(); ++d )
	{
		tmp_list << devices[d].absoluteFilePath();
	}
	
	return tmp_list;
}

bool System_Info::Update_Host_USB()
{
	AQError( "System_Info::Update_Host_USB()",
			 "Not implemented!" );
	return false;
}

#endif // macOS

#ifdef Q_OS_WIN32

#include <windows.h>
#include <tchar.h>

void System_Info::Get_Free_Memory_Size( int &allRAM, int &freeRAM )
{
	MEMORYSTATUS mem;
	memset( (void*)&mem, 0, sizeof(mem) );
	mem.dwLength = sizeof( mem );
	GlobalMemoryStatus( &mem );
	
	freeRAM = (int)mem.dwAvailPhys / 1024.0 / 1024.0;
	allRAM = (int)mem.dwTotalPhys / 1024.0 / 1024.0;
}

QStringList System_Info::Get_Host_FDD_List()
{
	DWORD len = GetLogicalDriveStrings( 0, NULL );
	TCHAR buf[ len ];
	int buf_size = sizeof(buf) / sizeof(TCHAR);
	GetLogicalDriveStrings( buf_size, buf );
	
	QStringList ret_list;
	QString tmp = "";
	for( int ix = 0; ix < buf_size-1; ix++ )
	{
		if( buf[ix] != NULL )
		{
			tmp += (char)buf[ix];
		}
		else
		{
			WCHAR *w = new WCHAR[ tmp.count() ];
			tmp.toWCharArray( w );
			UINT uDriveType = GetDriveType( w );
			delete w;
			
			if( uDriveType == DRIVE_REMOVABLE )
			{
				ret_list << tmp;
			}
			
			tmp = "";
		}
	}

	return ret_list;
}

QStringList System_Info::Get_Host_CDROM_List()
{
	DWORD len = GetLogicalDriveStrings( 0, NULL );
	TCHAR buf[ len ];
	int buf_size = sizeof(buf) / sizeof(TCHAR);
	GetLogicalDriveStrings( buf_size, buf );
	
	QStringList ret_list;
	QString tmp = "";
	for( int ix = 0; ix < buf_size-1; ix++ )
	{
		if( buf[ix] != NULL )
		{
			tmp += (char)buf[ix];
		}
		else
		{
			WCHAR *w = new WCHAR[ tmp.count() ];
			tmp.toWCharArray( w );
			UINT uDriveType = GetDriveType( w );
			delete w;
			
			if( uDriveType == DRIVE_CDROM )
			{
				ret_list << tmp;
			}
			
			tmp = "";
		}
	}
	
	return ret_list;
}

bool System_Info::Update_Host_USB()
{
	AQError( "System_Info::Update_Host_USB()",
			 "Not implemented!" );
	return false;
}

#endif // Windows
