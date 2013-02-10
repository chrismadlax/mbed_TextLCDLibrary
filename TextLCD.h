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

#ifndef MBED_TEXTLCD_H
#define MBED_TEXTLCD_H

#include "mbed.h"

/** A TextLCD interface for driving 4-bit HD44780-based LCDs
 *
 * Currently supports 8x1, 8x2, 16x2, 16x4, 20x2, 20x4, 24x2, 24x4 and 40x2 panels
 *
 * @code
 * #include "mbed.h"
 * #include "TextLCD.h"
 * 
 * TextLCD lcd(p15, p16, p17, p18, p19, p20); // RS, E, D4-D7 
 * 
 * int main() {
 *     lcd.printf("Hello World!\n");
 * }
 * @endcode
 */


//Pin Defines for I2C PCF8574 and SPI 74595 Bus
//LCD and serial portexpanders should be wired accordingly 
//Note: LCD RW pin must be connected to GND
//      E2 may be used for future expansion to LCD40x4
//      BL may be used for future expansion to control backlight
//
#define D_LCD_PIN_D4   0x00
#define D_LCD_PIN_D5   0x01
#define D_LCD_PIN_D6   0x02
#define D_LCD_PIN_D7   0x03
#define D_LCD_PIN_RS   0x04
#define D_LCD_PIN_E    0x05
#define D_LCD_PIN_E2   0x06
#define D_LCD_PIN_BL   0x07

#define D_LCD_BUS_MSK  0x0F

//Bitpattern Defines for I2C PCF8574 and SPI 74595 Bus
//
#define D_LCD_D4       (1<<D_LCD_PIN_D4)
#define D_LCD_D5       (1<<D_LCD_PIN_D5)
#define D_LCD_D6       (1<<D_LCD_PIN_D6)
#define D_LCD_D7       (1<<D_LCD_PIN_D7)
#define D_LCD_RS       (1<<D_LCD_PIN_RS)
#define D_LCD_E        (1<<D_LCD_PIN_E)
#define D_LCD_E2       (1<<D_LCD_PIN_E2)
#define D_LCD_BL       (1<<D_LCD_PIN_BL)



/** Some sample User Defined Chars 5x7 dots */
const char udc_ae[] = {0x00, 0x00, 0x1B, 0x05, 0x1F, 0x14, 0x1F, 0x00};  //æ
const char udc_0e[] = {0x00, 0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00};  //ø
const char udc_ao[] = {0x0E, 0x0A, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00};  //å
const char udc_AE[] = {0x0F, 0x14, 0x14, 0x1F, 0x14, 0x14, 0x17, 0x00};  //Æ
const char udc_0E[] = {0x0E, 0x13, 0x15, 0x15, 0x15, 0x19, 0x0E, 0x00};  //Ø
const char udc_AA[] = {0x0E, 0x0A, 0x0E, 0x11, 0x1F, 0x11, 0x11, 0x00};  //Å

const char udc_0[]  = {0x18, 0x14, 0x12, 0x11, 0x12, 0x14, 0x18, 0x00};  // |>
const char udc_1[]  = {0x03, 0x05, 0x09, 0x11, 0x09, 0x05, 0x03, 0x00};  // <|
const char udc_2[]  = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00};  // |
const char udc_3[]  = {0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00};  // ||
const char udc_4[]  = {0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x00};  // |||
const char udc_5[]  = {0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x00};  // =
const char udc_6[]  = {0x15, 0x0a, 0x15, 0x0a, 0x15, 0x0a, 0x15, 0x00};  // checkerboard
const char udc_7[]  = {0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x10, 0x00};  // \

const char udc_degr[]   = {0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00};  // Degree symbol

const char udc_TM_T[]   = {0x1F, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00};  // Trademark T
const char udc_TM_M[]   = {0x11, 0x1B, 0x15, 0x11, 0x00, 0x00, 0x00, 0x00};  // Trademark M

//const char udc_Bat_Hi[] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};  // Battery Full
//const char udc_Bat_Ha[] = {0x0E, 0x11, 0x13, 0x17, 0x1F, 0x1F, 0x1F, 0x00};  // Battery Half
//const char udc_Bat_Lo[] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00};  // Battery Low
const char udc_Bat_Hi[] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};  // Battery Full
const char udc_Bat_Ha[] = {0x0E, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};  // Battery Half
const char udc_Bat_Lo[] = {0x0E, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x1F, 0x00};  // Battery Low

const char udc_bar_1[]  = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00};  // Bar 1
const char udc_bar_2[]  = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00};  // Bar 11
const char udc_bar_3[]  = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x00};  // Bar 111
const char udc_bar_4[]  = {0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x00};  // Bar 1111
const char udc_bar_5[]  = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};  // Bar 11111


/** A TextLCD interface for driving 4-bit HD44780-based LCDs
 *
 * Currently supports 8x1, 8x2, 16x1, 16x2, 16x4, 20x2, 20x4, 24x2, 24x4 and 40x2 panels
 *
 */
class TextLCD : public Stream {
public:

    /** LCD panel format */
    enum LCDType {
        LCD8x1,     /**<  8x1 LCD panel */    
        LCD8x2,     /**<  8x2 LCD panel */          
        LCD16x1,    /**< 16x1 LCD panel (actually 8x2) */          
        LCD16x2,    /**< 16x2 LCD panel (default) */
        LCD16x2B,   /**< 16x2 LCD panel alternate addressing */
        LCD16x4,    /**< 16x4 LCD panel */        
        LCD20x2,    /**< 20x2 LCD panel */
        LCD20x4,    /**< 20x4 LCD panel */
        LCD24x2,    /**< 24x2 LCD panel */        
        LCD24x4,    /**< 24x4 LCD panel, special mode KS0078 */                
        LCD40x2     /**< 40x2 LCD panel */                
    };

    /** LCD Cursor control */
    enum LCDCursor {
        CurOff_BlkOff,  /**<  Cursor Off, Blinking Char Off */    
        CurOn_BlkOff,   /**<  Cursor On, Blinking Char Off */    
        CurOff_BlkOn,   /**<  Cursor Off, Blinking Char On */    
        CurOn_BlkOn,    /**<  Cursor On, Blinking Char On */    
    };


    /** Create a TextLCD interface for using regural mbed pins
     *
     * @param rs    Instruction/data control line
     * @param e     Enable line (clock)
     * @param d4-d7 Data lines for using as a 4-bit interface
     * @param type  Sets the panel size/addressing mode (default = LCD16x2)
     */
    TextLCD(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7, LCDType type = LCD16x2);
    
    /** Create a TextLCD interface using an I2C PC8574 portexpander
     *
     * @param i2c             I2C Bus
     * @param deviceAddress   I2C slave address (PCF8574)
     * @param type            Sets the panel size/addressing mode (default = LCD16x2)
     */
    TextLCD(I2C *i2c, char deviceAddress, LCDType type = LCD16x2);


    /** Create a TextLCD interface using an SPI 74595 portexpander
     *
     * @param spi             SPI Bus
     * @param cs              chip select pin (active low)
     * @param type            Sets the panel size/addressing mode (default = LCD16x2)
     */
    TextLCD(SPI *spi, PinName cs, LCDType type = LCD16x2);


#if DOXYGEN_ONLY
    /** Write a character to the LCD
     *
     * @param c The character to write to the display
     */
    int putc(int c);

    /** Write a formated string to the LCD
     *
     * @param format A printf-style format string, followed by the
     *               variables to use in formating the string.
     */
    int printf(const char* format, ...);
#endif

    /** Locate to a screen column and row
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
    void locate(int column, int row);


    /** Return the memoryaddress of screen column and row location
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     * @param return  The memoryaddress of screen column and row location
     */
    int  getAddress(int column, int row);    
    
    
    /** Set the memoryaddress of screen column and row location
     *
     * @param column  The horizontal position from the left, indexed from 0
     * @param row     The vertical position from the top, indexed from 0
     */
    void setAddress(int column, int row);        


    /** Clear the screen and locate to 0,0 */
    void cls();

    /** Return the number of rows
     *
     * @param return  The number of rows
     */
    int rows();
    
    /** Return the number of columns
     *
     * @param return  The number of columns
     */  
    int columns();  

    /** Set the Cursormode
     *
     * @param show    The Cursor mode (CurOff_BlkOff, CurOn_BlkOff, CurOff_BlkOn, CurOn_BlkOn)
     */
    void setCursor(LCDCursor show);     


    /** Set User Defined Characters
     *
     * @param unsigned char c   The Index of the UDC (0..7)
     * @param char *udc_data    The bitpatterns for the UDC (8 bytes of 5 significant bits)     
     */
    void setUDC(unsigned char c, char *udc_data);

protected:
   /* LCD Bus control */
    enum _LCDBus {
        _PinBus,  /*<  Regular mbed pins */    
        _I2CBus,  /*<  I2C PCF8574 Portexpander */    
        _SPIBus   /*<  SPI 74595 */    
    };

    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

    void _init();    
    int  _address(int column, int row);
    void _character(int column, int row, int c);
    
//Low level writes to LCD Bus (serial or parallel)
    void _setEnable(bool value);
    void _setRS(bool value);  
    void _setData(int value);
    void _setCS(bool value);
    
//Low level writes to LCD serial bus only
    void _writeBus();      

//Low level writes to LCD
    void _writeByte(int value);
    void _writeCommand(int command);
    void _writeData(int data);
  
// Regular mbed pins bus
    DigitalOut _rs, _e;
    BusOut _d;
    
// I2C bus    
    I2C *_i2c;
    unsigned char _slaveAddress;
    
// SPI bus        
    SPI *_spi;
    DigitalOut _cs;    
    
//Bus Interface type    
    _LCDBus _busType;

// Internal bus mirror value for serial only
    char _lcd_bus;   
    
//Display type
    LCDType _type;

// Cursor
    int _column;
    int _row;
    LCDCursor _cursor;    
};

#endif
