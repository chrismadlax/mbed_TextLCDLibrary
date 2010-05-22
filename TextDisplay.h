/* mbed TextDisplay Library Base Class
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
 *
 * A common base class for driving Text displays, providing useful functionality
 * that can be shared by all text lcds. To port a new display, derive from this 
 * class and implement the constructor (setup the display), character (put a 
 * character at a location), rows and columns (number of rows/cols) functions.
 * Everything else (locate, printf, putc, cls) will come for free
 *
 * The model is the display will wrap at the right and bottom, so you can
 * keep writing and will always get valid characters. The location is 
 * maintained internally to the class to make this easier
 */

#ifndef MBED_TEXTDISPLAY_H
#define MBED_TEXTDISPLAY_H

#include "mbed.h"

class TextDisplay : public Stream {
public:

    // functions to be implementated in the derived class
    TextDisplay();
    virtual void character(int column, int row, int c) = 0;
    virtual int rows() = 0;
    virtual int columns() = 0;
    
    // functions that come for free, but can be overwritten
    virtual void cls();
    virtual void locate(int column, int row);
    virtual void foreground(int colour);
    virtual void background(int colour);
    
    // other stdio-type functions
    // putc() inherited from Stream
    // printf() is inherited from Stream
    
protected:

    virtual int _putc(int value);
    virtual int _getc();

    // character location
    short _column;
    short _row;

    // colours
    int _foreground;
    int _background;
};

#endif
