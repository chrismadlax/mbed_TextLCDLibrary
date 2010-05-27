/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford
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

#ifndef MBED_TEXTLCD_H
#define MBED_TEXTLCD_H

#include "mbed.h"
class TextLCD : public Stream {
public:

    // the different LCDs and addressing modes
    /** Select the type of LCD */
    enum LCDType {
        LCD16x2
        , LCD16x2B
        , LCD20x2
        , LCD20x4
    };

    TextLCD(PinName rs, PinName e, PinName d0, PinName d1, PinName d2, PinName d3, LCDType type = LCD16x2);
    // int putc(int c) inherited from Stream
    // int printf(...) inherited from Stream
    void character(int column, int row, int c);
    void locate(int column, int row);
    void cls();
    
    int rows();
    int columns();  
    
    
protected:

    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

    // internal tx functions
    void writeByte(int value);
    void writeCommand(int command);
    void writeData(int data);
    int address(int column, int row);

    DigitalOut _rs, _e;
    BusOut _d;
    LCDType _type;

    // current row/column
    int _column;
    int _row;
};

#endif
