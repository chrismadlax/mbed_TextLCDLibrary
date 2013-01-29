/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *               2013, WH, Updated LCD types and fixed lcd address issues
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "TextLCD.h"
#include "mbed.h"

TextLCD::TextLCD(PinName rs, PinName e, PinName d4, PinName d5,
                 PinName d6, PinName d7, LCDType type) : _rs(rs),
        _e(e), _d(d4, d5, d6, d7),
        _type(type) {

    _e  = 1;
    _rs = 0;            // command mode

    wait(0.015);        // Wait 15ms to ensure powered up

    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    for (int i=0; i<3; i++) {
        writeByte(0x3);
        wait(0.00164);  // this command takes 1.64ms, so wait for it
    }
    writeByte(0x2);     // 4-bit mode
    wait(0.000040f);    // most instructions take 40us

    writeCommand(0x28); // Function set 001 BW N F - -
    writeCommand(0x0C);
    writeCommand(0x06); // Cursor Direction and Display Shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes
    cls();
}


void TextLCD::character(int column, int row, int c) {
    int addr = getAddress(column, row);
    
    writeCommand(0x80 | addr);
    writeData(c);
}


void TextLCD::cls() {
    writeCommand(0x01); // cls, and set cursor to 0
    wait(0.00164f);     // This command takes 1.64 ms
    locate(0, 0);
}

void TextLCD::locate(int column, int row) {
    _column = column;
    _row = row;
}

int TextLCD::_putc(int value) {
    if (value == '\n') {
        _column = 0;
        _row++;
        if (_row >= rows()) {
            _row = 0;
        }
    } else {
        character(_column, _row, value);
        _column++;
        if (_column >= columns()) {
            _column = 0;
            _row++;
            if (_row >= rows()) {
                _row = 0;
            }
        }
    }
    return value;
}

int TextLCD::_getc() {
    return -1;
}

void TextLCD::writeByte(int value) {
    _d = value >> 4;
    wait(0.000040f); // most instructions take 40us
    _e = 0;
    wait(0.000040f);
    _e = 1;
    _d = value >> 0;
    wait(0.000040f);
    _e = 0;
    wait(0.000040f);  // most instructions take 40us
    _e = 1;
}

void TextLCD::writeCommand(int command) {
    _rs = 0;
    writeByte(command);
}

void TextLCD::writeData(int data) {
    _rs = 1;
    writeByte(data);
}


#if (0)
// This is the original method.
// It is confusing since it returns the memoryaddress or-ed with the set memorycommand 0x80.
// Left it in here for compatibility with older code. New applications should use getAddress() instead.
// 
int TextLCD::address(int column, int row) {
    switch (_type) {
        case LCD20x4:
            switch (row) {
                case 0:
                    return 0x80 + column;
                case 1:
                    return 0xc0 + column;
                case 2:
                    return 0x94 + column;
                case 3:
                    return 0xd4 + column;
            }
        case LCD16x2B:
            return 0x80 + (row * 40) + column;
        case LCD16x2:
        case LCD20x2:
        default:
            return 0x80 + (row * 0x40) + column;
    }
}
#endif


// This replaces the original method.
// Left it in here for compatibility with older code. New applications should use getAddress() instead.
int TextLCD::address(int column, int row) {
  return 0x80 | getAddress(column, row);
}

// This is new method to return the memory address based on row, column and displaytype.
//
int TextLCD::getAddress(int column, int row) {

    switch (_type) {
        case LCD8x1:
            return 0x00 + column;                        
    
        case LCD16x4:
            switch (row) {
                case 0:
                    return 0x00 + column;
                case 1:
                    return 0x40 + column;
                case 2:
                    return 0x10 + column;
                case 3:
                    return 0x50 + column;
            }

        case LCD20x4:
            switch (row) {
                case 0:
                    return 0x00 + column;
                case 1:
                    return 0x40 + column;
                case 2:
                    return 0x14 + column;
                case 3:
                    return 0x54 + column;
            }

// Not sure about this one, seems wrong.
        case LCD16x2B:      
            return 0x00 + (row * 40) + column;
      
        case LCD8x2:        
        case LCD16x2:
        case LCD20x2:
        case LCD24x2:        
            return 0x00 + (row * 0x40) + column;
            
// Should never get here.
        default:            
            return 0x00;        
    }
}


// Added for consistency. Set row, colum and update memoryaddress.
//
void TextLCD::setAddress(int column, int row) {

    locate(column, row);
    
    int addr = getAddress(column, row);
    
    writeCommand(0x80 | addr);
}

int TextLCD::columns() {
    switch (_type) {
        case LCD8x1:
        case LCD8x2:
            return 8;

        case LCD16x2:
        case LCD16x2B:
        case LCD16x4:        
            return 16;
            
        case LCD20x2:
        case LCD20x4:
            return 20;

        case LCD24x2:
            return 24;        
        
// Should never get here.
        default:
            return 0;
    }
}

int TextLCD::rows() {
    switch (_type) {
        case LCD8x1: 
            return 1;           

        case LCD8x2:        
        case LCD16x2:
        case LCD16x2B:
        case LCD20x2:
        case LCD24x2:        
            return 2;
                    
        case LCD16x4:
        case LCD20x4:
            return 4;
      
        default:
            return 0;        
    }
}
