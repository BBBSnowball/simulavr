/*
 ****************************************************************************
 *
 * simulavr - A simulator for the Atmel AVR family of microcontrollers.
 * Copyright (C) 2001, 2002, 2003   Klaus Rudolph       
 * Copyright (C) 2009 Onno Kortmann
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ****************************************************************************
 *
 *  $Id$
 */
#include <iostream>
#include <cstdlib>
#include "specialmem.h"
#include "avrerror.h"

using namespace std;

RWWriteToFile::RWWriteToFile(TraceValueRegister *registry,
                             const string &tracename,
                             const string &filename):
    RWMemoryMember(registry, tracename),
    os(filename=="-" ? cout : ofs)
{
    if(filename != "-")
        ofs.open(filename.c_str());
}

void RWWriteToFile::set(unsigned char val) {
    os << val;
    os.flush();
}

unsigned char RWWriteToFile::get() const {
    avr_warning("Invalid read access to RWWriteToFile register.");
    return 0;
} 

RWReadFromFile::RWReadFromFile(TraceValueRegister *registry,
                               const string &tracename,
                               const string &filename):
    RWMemoryMember(registry, tracename),
    is((filename=="-") ? cin : ifs)
{
    if(filename != "-")
        ifs.open(filename.c_str());
}

void RWReadFromFile::set(unsigned char val) {
    avr_warning("Invalid write access to RWWriteToFile register with value %d.", (int)val);
}

unsigned char RWReadFromFile::get() const { 
    char val;
    is.get(val);
    return val; 
}


RWFifo::RWFifo(TraceValueRegister *registry,
               const std::string &tracename,
               bool verbose) :
    RWMemoryMember(registry, tracename),
    readEmptyWarningFirstTime(true),
    verbose(verbose) {
}

const uint8_t* RWFifo::getInputData()   const { return &input_buffer.front(); }

const size_t   RWFifo::getInputLength() const { return input_buffer.size(); }

void RWFifo::skipInput(size_t amount) {
  if (amount > input_buffer.size())
    amount = input_buffer.size();
  input_buffer.erase(input_buffer.begin(), input_buffer.begin()+amount);
}

void RWFifo::appendOutput(const uint8_t* data, size_t length) {
  for (size_t i=0; i<length; i++) {
    output_buffer.push_back(data[i]);
  }
}

const size_t RWFifo::getUnprocessedOutputLength() const { return output_buffer.size(); }

unsigned char RWFifo::get() const {
    if (!output_buffer.empty()) {
        uint8_t value = output_buffer.front();
        output_buffer.erase(output_buffer.begin());
        return value;
    } else {
        if (readEmptyWarningFirstTime) {
            avr_message("The AVR program is reading from an empty RWFifo. This warning won't be reported again.");
            readEmptyWarningFirstTime = false;
        }
        return (uint8_t)(-1);
    }
}

void RWFifo::set(unsigned char c) {
    input_buffer.push_back(c);

    if (verbose) {
        avr_message("%s: '%c' (%d)", tracename.c_str(), c, c);
    }
}


RWExit::RWExit(TraceValueRegister *registry,
               const string &tracename) :
    RWMemoryMember(registry, tracename) {}


void RWExit::set(unsigned char c) {
    avr_message("Exiting at simulated program request (write)");
    DumpManager::Instance()->stopApplication();
    sysConHandler.ExitApplication(c); 
}

unsigned char RWExit::get() const {
    avr_message("Exiting at simulated program request (read)");
    DumpManager::Instance()->stopApplication();
    sysConHandler.ExitApplication(0); 
    return 0;
}

RWAbort::RWAbort(TraceValueRegister *registry,
                 const string &tracename) :
    RWMemoryMember(registry, tracename) {}

void RWAbort::set(unsigned char c) {
    avr_warning("Aborting at simulated program request (write)");
    DumpManager::Instance()->stopApplication();
    sysConHandler.AbortApplication(c);
}

unsigned char RWAbort::get() const {
    avr_warning("Aborting at simulated program request (read)");
    DumpManager::Instance()->stopApplication();
    sysConHandler.AbortApplication(0);
    return 0;
}

