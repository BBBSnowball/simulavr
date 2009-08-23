/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003   Klaus Rudolph		
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 *
 *  $Id$
 */

#include <iostream>
using namespace std;

#include "avrdevice.h"
#include "hwextirq.h"
#include "irqsystem.h"
#include "trace.h"

//#define NEW

#define    INT1         7
#define    INT0         6
HWExtIrq::HWExtIrq(AvrDevice *core, HWIrqSystem *i, PinAtPort p0, PinAtPort p1, unsigned int iv0, unsigned int iv1):
    Hardware(core),
    TraceValueRegister(core, "EXTIRQ"),
    irqSystem(i),
    pinI0(p0), pinI1(p1), vectorInt0(iv0), vectorInt1(iv1),
    gimsk_reg(this, "GIMSK",
              this, &HWExtIrq::GetGimsk, &HWExtIrq::SetGimsk),
    gifr_reg(this, "GIFR",
             this, &HWExtIrq::GetGifr, &HWExtIrq::SetGifr)
{
    //irqSystem->RegisterIrqPartner(this, iv0);
    //irqSystem->RegisterIrqPartner(this, iv1);
#ifdef NEW
    p0.GetPin().RegisterCallback(this);
    p1.GetPin().RegisterCallback(this);
#else
    core->AddToCycleList(this);
#endif
    Reset();
}

void HWExtIrq::Reset() {
    gimsk=0;
    gifr=0;
}


unsigned char  HWExtIrq::GetGimsk(){ return gimsk;}
unsigned char  HWExtIrq::GetGifr(){return gifr;}

void HWExtIrq::CheckForNewSetIrq(unsigned char giac) 
{
    if (giac&(1<<INT0)) { irqSystem->SetIrqFlag(this, vectorInt0); }
    if (giac&(1<<INT1)) { irqSystem->SetIrqFlag(this, vectorInt1); }
}

void HWExtIrq::CheckForNewClearIrq(unsigned char giac) 
{
    if (giac&(1<<INT0)) { irqSystem->ClearIrqFlag(vectorInt0); }
    if (giac&(1<<INT1)) { irqSystem->ClearIrqFlag(vectorInt1); }
}

void  HWExtIrq::SetGimsk(unsigned char val){ 
    unsigned char giacOld= gimsk&gifr;
    gimsk=val;
    unsigned char giacNew= gimsk&gifr;

    unsigned char changed=giacNew^giacOld;
    unsigned char setnew= changed&giacNew;
    unsigned char clearnew= changed& (~giacNew);

    CheckForNewSetIrq(setnew);
    CheckForNewClearIrq(clearnew);
}

void  HWExtIrq::SetGifr(unsigned char val){ 
    unsigned char giacOld= gimsk&gifr;
    val&=0xc0; 
    val^=0xc0; //invert the bits, if set 1 clear the flag
    gifr&=val;
    unsigned char giacNew= gimsk&gifr;

    unsigned char changed=giacNew^giacOld;
    unsigned char setnew= changed&giacNew;
    unsigned char clearnew= changed& (~giacNew);

    CheckForNewSetIrq(setnew);
    CheckForNewClearIrq(clearnew);
}

void HWExtIrq::SetMcucrCopy(unsigned char val) { mcucrCopy=val; }


void HWExtIrq::ClearIrqFlag(unsigned int vector) {
    if (vector==vectorInt0) { 
        gifr&=0xff-(1<<INT0);
        irqSystem->ClearIrqFlag(vectorInt0);
    } else if ( vector==vectorInt1) {
        gifr&=0xff-(1<<INT1);
        irqSystem->ClearIrqFlag(vectorInt1);
    }
}

//not longer in use!
unsigned int HWExtIrq::CpuCycle(){
#ifndef NEW
    PinStateHasChanged(NULL);
#endif
    return 0;    
}
void HWExtIrq::PinStateHasChanged(Pin *p) {
    switch (mcucrCopy&0x03) {
        case 0x00:
            if (pinI0==0) {
                gifr|=(1<<INT0);
                if (gimsk & (1<<INT0) ) irqSystem->SetIrqFlag(this, vectorInt0);
            }
            break;
        case 0x01:
            cerr << "Illegal State in mcucrCopy! for int0 sence control" << endl;
            break;
        case 0x02:
            if ((pinI0==0) && ( int0_old==1)) {
                int0_old=0;
                gifr|=(1<<INT0);
                if (gimsk & (1<<INT0) ) irqSystem->SetIrqFlag(this, vectorInt0);
            }
            break;
        case 0x03:
            if ((pinI0==1) && ( int0_old==0)) {
                int0_old=1;
                gifr|=(1<<INT0);
                if (gimsk & (1<<INT0) ) irqSystem->SetIrqFlag(this, vectorInt0);
            }
            break;
    }
    int0_old=pinI0;

    switch (mcucrCopy&0x0c) {
        case 0x00:
            if (pinI1==0) {
                gifr|=(1<<INT1);
                if (gimsk & (1<<INT1) ) irqSystem->SetIrqFlag(this, vectorInt1);
            }
            break;
        case 0x04:
            cerr << "Illegal State in mcucrCopy! for int1 sence control" << endl;
            break;
        case 0x08:
            if ((pinI1==0) && ( int1_old==1)) {
                int1_old=0;
                gifr|=(1<<INT1);
                if (gimsk & (1<<INT1) ) irqSystem->SetIrqFlag(this, vectorInt1);
            }
            break;
        case 0x0c:
            if ((pinI1==1) && ( int1_old==0)) {
                int1_old=1;
                gifr|=(1<<INT1);
                if (gimsk & (1<<INT1) ) irqSystem->SetIrqFlag(this, vectorInt1);
            }
            break;
    }
    int1_old=pinI1;
    //return 0;
}		

