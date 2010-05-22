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
 */
 
#include "TextDisplay.h"

TextDisplay::TextDisplay() {
    _row = 0;
    _column = 0;
}
    
int TextDisplay::_putc(int value) {
    if(value == '\n') {
        _column = 0;
        _row++;
        if(_row >= rows()) {
            _row = 0;
        }
    } else {
        character(_column, _row, value);
        _column++;
        if(_column >= columns()) {
            _column = 0;
            _row++;
            if(_row >= rows()) {
                _row = 0;
            }
        }
    }
    return value;
}

// a basic cls implementation, that should generally be overwritten
// with a more efficient implementation
void TextDisplay::cls() {
    locate(0, 0);
    for(int i=0; i<columns()*rows(); i++) {
        putc(' ');
    }
}

void TextDisplay::locate(int column, int row) {
    _column = column;
    _row = row;
}

int TextDisplay::_getc() {
    return -1;
}
        
void TextDisplay::foreground(int colour) {
    _foreground = colour;
}

void TextDisplay::background(int colour) {
    _background = colour;
}
