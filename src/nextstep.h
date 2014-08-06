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

#ifndef NEXTSTEP
#define NEXTSTEP

#include "systemclocktypes.h"

class Fraction {
public:
    typedef long long num_t;
private:

    num_t numerator, denominator;

    static num_t gcd(num_t a, num_t b);
public:
    Fraction(num_t numerator, num_t denominator=1);

    inline Fraction operator+ (const Fraction& f) const {
        return Fraction(numerator*f.denominator + f.numerator*denominator, denominator*f.denominator);
    }

    inline Fraction operator- (const Fraction& f) const {
        return Fraction(numerator*f.denominator - f.numerator*denominator, denominator*f.denominator);
    }

    inline Fraction operator* (const Fraction& f) const {
        return Fraction(numerator*f.numerator, denominator*f.denominator);
    }

    inline Fraction operator/ (const Fraction& f) const {
        return *this * f.Reciprocal();
    }

    inline Fraction Reciprocal() const {
        return Fraction(denominator, numerator);
    }

    inline num_t RoundToInt() const {
        return (numerator + denominator/2) / denominator;
    }

    inline double ToDouble() const {
        return numerator / (double)denominator;
    }

    inline num_t GetNumerator() const {
        return numerator;
    }

    inline num_t GetDenominator() const {
        return denominator;
    }
};

class NextStepGenerator {
    Fraction accumulated_error;
public:
    inline NextStepGenerator() : accumulated_error(0) { }

    SystemClockOffset Step(Fraction step_size);
};

#endif
