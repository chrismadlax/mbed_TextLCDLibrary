/* mbed TextLCD Library, 4-bit 2x16
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
 
#include "TextLCD.h"
#include "mbed.h"

/*
 * useful info found at http://www.a-netz.de/lcd.en.php
 *
 * Initialisation
 * ==============
 *
 * After attaching the supply voltage/after a reset, the display needs to be brought in to a defined state
 *
 * - wait approximately 15 ms so the display is ready to execute commands
 * - Execute the command 0x30 ("Display Settings") three times (wait 1,64ms after each command, the busy flag cannot be queried now). 
 * - The display is in 8 bit mode, so if you have only connected 4 data pins you should only transmit the higher nibble of each command.
 * - If you want to use the 4 bit mode, now you can execute the command to switch over to this mode now.
 * - Execute the "clear display" command
 *
 * Timing
 * ======
 *
 * Nearly all commands transmitted to the display need 40us for execution. 
 * Exceptions are the commands "Clear Display and Reset" and "Set Cursor to Start Position" 
 * These commands need 1.64ms for execution. These timings are valid for all displays working with an 
 * internal clock of 250kHz. But I do not know any displays that use other frequencies. Any time you 
 * can use the busy flag to test if the display is ready to accept the next command.
 * 
 * _e is kept high apart from calling clock
 * _rw is kept 0 (write) apart from actions that uyse it differently
 * _rs is set by the data/command writes
 */

TextLCD::TextLCD(PinName rs, PinName rw, PinName e, PinName d0, PinName d1, 
    PinName d2, PinName d3) : _rw(rw), _rs(rs), 
    _e(e), _d(d0, d1, d2, d3) {

    _rw = 0;
    _e  = 1;
    _rs = 0; // command mode

    // Should theoretically wait 15ms, but most things will be powered up pre-reset
    // so i'll disable that for the minute. If implemented, could wait 15ms post reset
    // instead
    // wait(0.015); 
        
    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    for(int i=0; i<3; i++) {
        writeByte(0x3);
        wait(0.00164);      // this command takes 1.64ms, so wait for it
    }
    writeByte(0x2); // 4-bit mode
            
    writeCommand(0x28);    // Function set 001 BW N F - -  
    writeCommand(0x0C);
    writeCommand(0x6);  //  Cursor Direction and Display Shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes    
    cls();
}

void TextLCD::character(int column, int row, int c) {
      int address = 0x80 + (row * 40) + column; // memory starts at 0x80, and is 40 chars long per row
      writeCommand(address);           
      writeData(c); 
}

int TextLCD::columns() {
    return 16;
}

int TextLCD::rows() { 
    return 2; 
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
