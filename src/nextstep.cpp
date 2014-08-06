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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 ****************************************************************************
 *
 *  $Id$
 */

#include "nextstep.h"


Fraction::num_t Fraction::gcd(num_t a, num_t b) {
    if (a == 0 || b == 0)
        return 1;

    // see https://en.wikipedia.org/wiki/Greatest_common_divisor#Using_Euclid.27s_algorithm
    while (a != b) {
        if (a > b)
            a -= b;
        else
            b -= a;
    }
    return a;
}

Fraction::Fraction(num_t numerator, num_t denominator) {
    num_t sign = 1;
    if (numerator < 0) {
        sign = -sign;
        numerator = -numerator;
    }
    if (denominator < 0) {
        sign = -sign;
        denominator = -denominator;
    }
    num_t x = gcd(numerator, denominator);
    this->numerator = sign*numerator/x;
    this->denominator = denominator/x;
}

SystemClockOffset NextStepGenerator::Step(Fraction step_size) {
    SystemClockOffset actual_step_size = (step_size + accumulated_error).RoundToInt();
    accumulated_error = (step_size + accumulated_error) - actual_step_size;
    return actual_step_size;
}
