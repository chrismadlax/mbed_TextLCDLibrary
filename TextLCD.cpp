/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *               2013, v01: WH, Added LCD types, fixed LCD address issues, added Cursor and UDCs 
 *               2013, v02: WH, Added I2C and SPI bus interfaces  
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

TextLCD::TextLCD(PinName rs, PinName e,
                 PinName d4, PinName d5, PinName d6, PinName d7,
                 LCDType type): _rs(rs), _e(e),
                                _d(d4, d5, d6, d7),
                                _cs(NC),                                                
                                _type(type) {


  _busType = _PinBus;

  _init();

}


TextLCD::TextLCD(I2C *i2c, char deviceAddress, LCDType type) :
        _rs(NC), _e(NC), _d(NC),
        _cs(NC),                
        _i2c(i2c),
        _type(type) {
        
  _slaveAddress = deviceAddress;
  _busType = _I2CBus;

  
  // Init the portexpander bus
  _lcd_bus = 0x80;
  
  // write the new data to the portexpander
  _i2c->write(_slaveAddress, &_lcd_bus, 1);    

  _init();
    
}


TextLCD::TextLCD(SPI *spi, PinName cs, LCDType type) :
        _rs(NC), _e(NC), _d(NC),
        _spi(spi),
        _cs(cs),        
        _type(type) {
        
  _busType = _SPIBus;

  // Setup the spi for 8 bit data, low steady state clock,
  // rising edge capture, with a 500KHz or 1MHz clock rate  
  _spi->format(8,0);
  _spi->frequency(500000);    
  //_spi.frequency(1000000);    


  // Init the portexpander bus
  _lcd_bus = 0x80;
  
  // write the new data to the portexpander
  _setCS(false);  
  _spi->write(_lcd_bus);   
  _setCS(true);  
  
  _init();
    
}


/*  Init the LCD controller
 *  4-bit mode, number of lines, no cursor etc
 *  Clear display 
 */
void TextLCD::_init() {
//    _e  = 1;  
//    _rs = 0;            // command mode

    _setEnable(true);    
    _setRS(false);      // command mode
    
//    wait(0.015);        // Wait 15ms to ensure powered up
    wait_ms(15);        // Wait 15ms to ensure powered up

    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    for (int i=0; i<3; i++) {
        _writeByte(0x3);
//        wait(0.00164);  // this command takes 1.64ms, so wait for it
        wait_ms(10);    // this command takes 1.64ms, so wait for it 
    }
    _writeByte(0x2);     // 4-bit mode
//    wait(0.000040f);     // most instructions take 40us
    wait_us(40);         // most instructions take 40us
    
    // Display is now in 4-bit mode
    switch (_type) {
        case LCD8x1:
            _writeCommand(0x20); // Function set 001 BW N F - -
                                 //   N=0 (1 line)
                                 //   F=0 (5x7 dots font)
            break;                                
            
        case LCD24x4:
            // Special mode for KS0078
            _writeCommand(0x2A); // Function set 001 BW N RE DH REV
                                 //    N=1  (Dont care for KS0078)
                                 //   RE=0  (Extended Regs, special mode for KS0078)
                                 //   DH=1  (Disp shift, special mode for KS0078)                                
                                 //   REV=0 (Reverse, special mode for KS0078)

            _writeCommand(0x2E); // Function set 001 BW N RE DH REV
                                 //    N=1  (Dont care for KS0078)
                                 //   RE=1  (Ena Extended Regs, special mode for KS0078)
                                 //   DH=1  (Disp shift, special mode for KS0078)                                
                                 //   REV=0 (Reverse, special mode for KS0078)

            _writeCommand(0x09); // Ext Function set 0000 1 FW BW NW
                                 //   FW=0  (5-dot font, special mode for KS0078)
                                 //   BW=0  (Cur BW invert disable, special mode for KS0078)
                                 //   NW=1  (4 Line, special mode for KS0078)                                

            _writeCommand(0x2A); // Function set 001 BW N RE DH REV
                                 //    N=1  (Dont care for KS0078)
                                 //   RE=0  (Dis. Extended Regs, special mode for KS0078)
                                 //   DH=1  (Disp shift, special mode for KS0078)                                
                                 //   REV=0 (Reverse, special mode for KS0078)
            break;
                                            
        default:
            _writeCommand(0x28); // Function set 001 BW N F - -
                                 //   N=1 (2 lines)
                                 //   F=0 (5x7 dots font)
                                 //    -  (Don't care)                                
            
            break;
    }

    _writeCommand(0x06); // Entry Mode 0000 01 CD S 
                         //   Cursor Direction and Display Shift
                         //   CD=1 (Cur incr)
                         //   S=0  (No display shift)                        

//    _writeCommand(0x0C); // Display Ctrl 0000 1 D C B
//                         //   Display On, Cursor Off, Blink Off
    setCursor(TextLCD::CurOff_BlkOff);  
    
    cls();    
}


void TextLCD::_character(int column, int row, int c) {
    int addr = getAddress(column, row);
    
    _writeCommand(0x80 | addr);
    _writeData(c);
}


void TextLCD::cls() {
    _writeCommand(0x01); // cls, and set cursor to 0
//    wait(0.00164f);      // This command takes 1.64 ms
    wait_ms(10);     // The CLS command takes 1.64 ms.
                     // Since we are not using the Busy flag, Lets be safe and take 10 ms
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
        _character(_column, _row, value);
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


void TextLCD::_setEnable(bool value) {

  switch(_busType) {
    case _PinBus : 
                    if (value)
                      _e  = 1;    // Set E bit 
                    else  
                      _e  = 0;    // Reset E bit  

                    break;  
    
    case _I2CBus : 
                   if (value)
                     _lcd_bus |= D_LCD_E;     // Set E bit 
                   else                     
                     _lcd_bus &= ~D_LCD_E;    // Reset E bit                     

                   // write the new data to the portexpander
                   _i2c->write(_slaveAddress, &_lcd_bus, 1);    
                   
                   break;  
    
    case _SPIBus :
                   if (value)
                     _lcd_bus |= D_LCD_E;     // Set E bit 
                   else                     
                     _lcd_bus &= ~D_LCD_E;    // Reset E bit                     
         
                   // write the new data to the portexpander
                   _setCS(false);  
                   _spi->write(_lcd_bus);   
                   _setCS(true);  
  
                   break;
  }
}    

void TextLCD::_setRS(bool value) {

  switch(_busType) {
    case _PinBus : 
                    if (value)
                      _rs  = 1;    // Set RS bit 
                    else  
                      _rs  = 0;    // Reset RS bit 

                    break;  
    
    case _I2CBus : 
                   if (value)
                     _lcd_bus |= D_LCD_RS;    // Set RS bit 
                   else                     
                     _lcd_bus &= ~D_LCD_RS;   // Reset RS bit                     

                   // write the new data to the portexpander
                   _i2c->write(_slaveAddress, &_lcd_bus, 1);    
                   
                   break;
                       
    case _SPIBus :
                   if (value)
                     _lcd_bus |= D_LCD_RS;    // Set RS bit 
                   else                     
                     _lcd_bus &= ~D_LCD_RS;   // Reset RS bit                     
      
                   // write the new data to the portexpander
                   _setCS(false);  
                   _spi->write(_lcd_bus);   
                   _setCS(true);  
     
                   break;
  }

}    

void TextLCD::_setData(int value) {
  int data;
  
  switch(_busType) {
    case _PinBus : 
                    _d = value & 0x0F;   // Write Databits 

                    break;  
    
    case _I2CBus : 
                    data = value & 0x0F;
                    if (data & 0x01)
                      _lcd_bus |= D_LCD_D4;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D4;  // Reset Databit                     

                    if (data & 0x02)
                      _lcd_bus |= D_LCD_D5;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D5;  // Reset Databit                     

                    if (data & 0x04)
                      _lcd_bus |= D_LCD_D6;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D6;  // Reset Databit                     

                    if (data & 0x08)
                      _lcd_bus |= D_LCD_D7;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D7;  // Reset Databit                     
                    
                    // write the new data to the portexpander
                    _i2c->write(_slaveAddress, &_lcd_bus, 1);  
                   
                    break;                    
                    
    case _SPIBus :
    
                    data = value & 0x0F;
                    if (data & 0x01)
                      _lcd_bus |= D_LCD_D4;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D4;  // Reset Databit                     

                    if (data & 0x02)
                      _lcd_bus |= D_LCD_D5;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D5;  // Reset Databit                     

                    if (data & 0x04)
                      _lcd_bus |= D_LCD_D6;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D6;  // Reset Databit                     

                    if (data & 0x08)
                      _lcd_bus |= D_LCD_D7;   // Set Databit 
                    else                     
                      _lcd_bus &= ~D_LCD_D7;  // Reset Databit                     
                    
                   // write the new data to the portexpander
                   _setCS(false);  
                   _spi->write(_lcd_bus);   
                   _setCS(true);  
        
                   break;
  }

}    


// Set CS line. Only used for SPI bus
void TextLCD::_setCS(bool value) {

  if (value)
    _cs  = 1;    // Set CS pin 
  else  
    _cs  = 0;    // Reset CS pin 

}



void TextLCD::_writeByte(int value) {
//    _d = value >> 4;
    _setData(value >> 4);
//    wait(0.000040f); // most instructions take 40us
    wait_us(40); // most instructions take 40us    
//    _e = 0;
    _setEnable(false);
//    wait(0.000040f);
    wait_us(40); // most instructions take 40us        
//    _e = 1;
    _setEnable(true);        
//    _d = value >> 0;
    _setData(value >> 0);    
//    wait(0.000040f);
    wait_us(40); // most instructions take 40us        
//    _e = 0;
    _setEnable(false);    
//    wait(0.000040f);  // most instructions take 40us
    wait_us(40); // most instructions take 40us        
//    _e = 1;
    _setEnable(true);    
}

void TextLCD::_writeCommand(int command) {
//    _rs = 0;
    _setRS(false);        
    _writeByte(command);
}

void TextLCD::_writeData(int data) {
//    _rs = 1;
    _setRS(true);            
    _writeByte(data);
}


#if (0)
// This is the original method.
// It is confusing since it returns the memoryaddress or-ed with the set memorycommand 0x80.
// Left it in here for compatibility with older code. New applications should use getAddress() instead.
// 
int TextLCD::_address(int column, int row) {
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
int TextLCD::_address(int column, int row) {
  return 0x80 | getAddress(column, row);
}

// This is new method to return the memory address based on row, column and displaytype.
//
int TextLCD::getAddress(int column, int row) {

    switch (_type) {
        case LCD8x1:
            return 0x00 + column;                        
    
        case LCD16x1:
            // LCD16x1 is a special layout of LCD8x2
            if (column<8) 
              return 0x00 + column;                        
            else   
              return 0x40 + (column - 8);                        

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

// Special mode for KS0078
        case LCD24x4:
            switch (row) {
                case 0:
                    return 0x00 + column;
                case 1:
                    return 0x20 + column;
                case 2:
                    return 0x40 + column;
                case 3:
                    return 0x60 + column;
            }

// Not sure about this one, seems wrong.
        case LCD16x2B:      
            return 0x00 + (row * 40) + column;
      
        case LCD8x2:        
        case LCD16x2:
        case LCD20x2:
        case LCD24x2:        
        case LCD40x2:                
            return 0x00 + (row * 0x40) + column;
            
// Should never get here.
        default:            
            return 0x00;        
    }
}


// Added for consistency. Set row, column and update memoryaddress.
//
void TextLCD::setAddress(int column, int row) {

    locate(column, row);
    
    int addr = getAddress(column, row);
    
    _writeCommand(0x80 | addr);
}

int TextLCD::columns() {
    switch (_type) {
        case LCD8x1:
        case LCD8x2:
            return 8;

        case LCD16x1:        
        case LCD16x2:
        case LCD16x2B:
        case LCD16x4:        
            return 16;
            
        case LCD20x2:
        case LCD20x4:
            return 20;

        case LCD24x2:
        case LCD24x4:        
            return 24;        

        case LCD40x2:
            return 40;        
        
// Should never get here.
        default:
            return 0;
    }
}

int TextLCD::rows() {
    switch (_type) {
        case LCD8x1: 
        case LCD16x1:         
            return 1;           

        case LCD8x2:        
        case LCD16x2:
        case LCD16x2B:
        case LCD20x2:
        case LCD24x2:        
        case LCD40x2:                
            return 2;
                    
        case LCD16x4:
        case LCD20x4:
        case LCD24x4:        
            return 4;

// Should never get here.      
        default:
            return 0;        
    }
}


void TextLCD::setCursor(TextLCD::LCDCursor show) { 
    
    switch (show) {
      case CurOff_BlkOff : _writeCommand(0x0C); // Cursor off and Blink Off
                           wait_us(40);
                           _cursor = show;
                           break;

      case CurOn_BlkOff   : _writeCommand(0x0E); // Cursor on and Blink Off
                           wait_us(40);  
                           _cursor = show;
                           break;

      case CurOff_BlkOn :  _writeCommand(0x0D); // Cursor off and Blink On
                           wait_us(40);
                           _cursor = show;
                           break;

      case CurOn_BlkOn   : _writeCommand(0x0F); // Cursor on and Blink char
                           wait_us(40);  
                           _cursor = show;
                           break;

// Should never get here.
      default : 
                           break;
                      
    }

}


void TextLCD::setUDC(unsigned char c, char *udc_data) {
  _writeCommand(0x40 + ((c & 0x07) << 3)); //Set CG-RAM address

  for (int i=0; i<8; i++) {
    _writeData(*udc_data++);
  }
}


