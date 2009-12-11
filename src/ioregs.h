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

#ifndef IOREGS
#define IOREGS

#include "traceval.h"
#include "avrdevice.h"

class HWRampz: public Hardware, public TraceValueRegister {
    protected:
        unsigned char rampz;

    public:
        HWRampz(AvrDevice *core);
        void Reset(){rampz=0;}
        unsigned char GetRampz() { return rampz; }
        void SetRampz(unsigned char val) { rampz=val; }

        IOReg<HWRampz> rampz_reg;
};

#endif
