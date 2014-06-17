/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *               2013, v01: WH, Added LCD types, fixed LCD address issues, added Cursor and UDCs 
 *               2013, v02: WH, Added I2C and SPI bus interfaces
 *               2013, v03: WH, Added support for LCD40x4 which uses 2 controllers   
 *               2013, v04: WH, Added support for Display On/Off, improved 4bit bootprocess  
 *               2013, v05: WH, Added support for 8x2B, added some UDCs  
 *               2013, v06: WH, Added support for devices that use internal DC/DC converters 
 *               2013, v07: WH, Added support for backlight and include portdefinitions for LCD2004 Module from DFROBOT
 *               2014, v08: WH, Refactored in Base and Derived Classes to deal with mbed lib change regarding 'NC' defined DigitalOut pins
 *               2014, v09: WH/EO, Added Class for Native SPI controllers such as ST7032 
 *               2014, v10: WH, Added Class for Native I2C controllers such as ST7032i, Added support for MCP23008 I2C portexpander, Added support for Adafruit module  
 *               2014, v11: WH, Added support for native I2C controllers such as PCF21XX, improved the _initCtrl() method to deal with differences between all supported controllers  
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
 * Currently supports 8x1, 8x2, 12x4, 16x1, 16x2, 16x4, 20x2, 20x4, 24x2, 24x4, 40x2 and 40x4 panels
 * Interface options include direct mbed pins, I2C portexpander (PCF8474/PCF8574A or MCP23008) or SPI bus shiftregister (74595). 
 * Supports some controllers with native I2C or SP interface. Supports some controllers that provide internal DC/DC converters for VLCD or VLED. 
 *
 * @code
 * #include "mbed.h"
 * #include "TextLCD.h"
 * 
 * // I2C Communication
 * I2C i2c_lcd(p28,p27); // SDA, SCL
 *
 * // SPI Communication
 * SPI spi_lcd(p5, NC, p7); // MOSI, MISO, SCLK
 *
 * //TextLCD lcd(p15, p16, p17, p18, p19, p20);                          // RS, E, D4-D7, LCDType=LCD16x2, BL=NC, E2=NC, LCDTCtrl=HD44780
 * //TextLCD_SPI lcd(&spi_lcd, p8, TextLCD::LCD40x4);                    // SPI bus, 74595 expander, CS pin, LCD Type  
 * TextLCD_I2C lcd(&i2c_lcd, 0x42, TextLCD::LCD20x4);                  // I2C bus, PCF8574 Slaveaddress, LCD Type
 * //TextLCD_I2C lcd(&i2c_lcd, 0x42, TextLCD::LCD16x2, TextLCD::WS0010); // I2C bus, PCF8574 Slaveaddress, LCD Type, Device Type
 * //TextLCD_SPI_N lcd(&spi_lcd, p8, p9);                                // SPI bus, CS pin, RS pin, LCDType=LCD16x2, BL=NC, LCDTCtrl=ST7032_3V3   
 * //TextLCD_I2C_N lcd(&i2c_lcd, ST7032_SA, TextLCD::LCD16x2, NC, TextLCD::ST7032_3V3); // I2C bus, Slaveaddress, LCD Type, BL=NC, LCDTCtrl=ST7032_3V3  
 * 
 * int main() {
 *   lcd.printf("Hello World!\n");
 * }
 * @endcode
 */


//Pin Defines for I2C PCF8574/PCF8574A or MCP23008 and SPI 74595 bus expander interfaces
//LCD and serial portexpanders should be wired accordingly 
//
//Select Hardware module (one option only)
#define DEFAULT        0
#define ADAFRUIT       1
#define DFROBOT        0


#if (DEFAULT==1)
//Definitions for default (WH) mapping between serial port expander pins and LCD controller
//This hardware supports the I2C bus expander (PCF8574/PCF8574A or MCP23008) and SPI bus expander (74595) interfaces
//See https://mbed.org/cookbook/Text-LCD-Enhanced
//
//Note: LCD RW pin must be connected to GND
//      E2 is used for LCD40x4 (second controller)
//      BL may be used to control backlight
#define D_LCD_PIN_D4   0
#define D_LCD_PIN_D5   1
#define D_LCD_PIN_D6   2
#define D_LCD_PIN_D7   3
#define D_LCD_PIN_RS   4
#define D_LCD_PIN_E    5
#define D_LCD_PIN_E2   6
#define D_LCD_PIN_BL   7

#define D_LCD_PIN_RW   D_LCD_PIN_E2

//Select I2C Portexpander type (one option only)
#define PCF8574        1
#define MCP23008       0
#endif

#if (ADAFRUIT==1)
//Definitions for Adafruit i2cspilcdbackpack mapping between serial port expander pins and LCD controller
//This hardware supports both an I2C expander (MCP23008) and an SPI expander (74595) selectable by a jumper.
//See http://www.ladyada.net/products/i2cspilcdbackpack
//
//Note: LCD RW pin must be kept LOW
//      E2 is not available on this hardware and so it does not support LCD40x4 (second controller)
//      BL is used to control backlight
#define D_LCD_PIN_0    0
#define D_LCD_PIN_RS   1
#define D_LCD_PIN_E    2
#define D_LCD_PIN_D4   3
#define D_LCD_PIN_D5   4
#define D_LCD_PIN_D6   5
#define D_LCD_PIN_D7   6
#define D_LCD_PIN_BL   7

#define D_LCD_PIN_E2   D_LCD_PIN_0

//Force I2C portexpander type
#define PCF8574        0
#define MCP23008       1
#endif

#if (DFROBOT==1)
//Definitions for DFROBOT LCD2004 Module mapping between serial port expander pins and LCD controller
//This hardware uses PCF8574 and is different from earlier/different Arduino I2C LCD displays
//See http://arduino-info.wikispaces.com/LCD-Blue-I2C
//
//Note: LCD RW pin must be kept LOW
//      E2 is not available on default Arduino hardware and so it does not support LCD40x4 (second controller)
//      BL is used to control backlight
#define D_LCD_PIN_RS   0
#define D_LCD_PIN_RW   1
#define D_LCD_PIN_E    2
#define D_LCD_PIN_BL   3
#define D_LCD_PIN_D4   4
#define D_LCD_PIN_D5   5
#define D_LCD_PIN_D6   6
#define D_LCD_PIN_D7   7

#define D_LCD_PIN_E2   D_LCD_PIN_RW

//Force I2C portexpander type
#define PCF8574        1
#define MCP23008       0
#endif

//Bitpattern Defines for I2C PCF8574/PCF8574A, MCP23008 and SPI 74595 Bus expanders
//
#define D_LCD_D4       (1<<D_LCD_PIN_D4)
#define D_LCD_D5       (1<<D_LCD_PIN_D5)
#define D_LCD_D6       (1<<D_LCD_PIN_D6)
#define D_LCD_D7       (1<<D_LCD_PIN_D7)
#define D_LCD_RS       (1<<D_LCD_PIN_RS)
#define D_LCD_E        (1<<D_LCD_PIN_E)
#define D_LCD_E2       (1<<D_LCD_PIN_E2)
#define D_LCD_BL       (1<<D_LCD_PIN_BL)
//#define D_LCD_RW       (1<<D_LCD_PIN_RW)

#define D_LCD_BUS_MSK  (D_LCD_D4 | D_LCD_D5 | D_LCD_D6 | D_LCD_D7)
#define D_LCD_BUS_DEF  0x00

/* PCF8574/PCF8574A I2C portexpander slave address */
#define PCF8574_SA0   0x40
#define PCF8574_SA1   0x42
#define PCF8574_SA2   0x44
#define PCF8574_SA3   0x46
#define PCF8574_SA4   0x48
#define PCF8574_SA5   0x4A
#define PCF8574_SA6   0x4C
#define PCF8574_SA7   0x4E

#define PCF8574A_SA0  0x70
#define PCF8574A_SA1  0x72
#define PCF8574A_SA2  0x74
#define PCF8574A_SA3  0x76
#define PCF8574A_SA4  0x78
#define PCF8574A_SA5  0x7A
#define PCF8574A_SA6  0x7C
#define PCF8574A_SA7  0x7E

/* MCP23008 I2C portexpander slave address */
#define MCP23008_SA0  0x40
#define MCP23008_SA1  0x42
#define MCP23008_SA2  0x44
#define MCP23008_SA3  0x46
#define MCP23008_SA4  0x48
#define MCP23008_SA5  0x4A
#define MCP23008_SA6  0x4C
#define MCP23008_SA7  0x4E


/* MCP23008 I2C portexpander internal registers */
#define IODIR         0x00
#define IPOL          0x01
#define GPINTEN       0x02
#define DEFVAL        0x03
#define INTCON        0x04
#define IOCON         0x05
#define GPPU          0x06
#define INTF          0x07
#define INTCAP        0x08
#define GPIO          0x09
#define OLAT          0x0A


/* ST7032I I2C slave address */
#define ST7032_SA     0x7C

/* PCF21XX I2C slave address */
#define PCF21XX_SA0   0x74
#define PCF21XX_SA1   0x76

/** Some sample User Defined Chars 5x7 dots */
const char udc_ae[] = {0x00, 0x00, 0x1B, 0x05, 0x1F, 0x14, 0x1F, 0x00};  //æ
const char udc_0e[] = {0x00, 0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00};  //ø
const char udc_ao[] = {0x0E, 0x0A, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00};  //å
const char udc_AE[] = {0x0F, 0x14, 0x14, 0x1F, 0x14, 0x14, 0x17, 0x00};  //Æ
const char udc_0E[] = {0x0E, 0x13, 0x15, 0x15, 0x15, 0x19, 0x0E, 0x00};  //Ø
const char udc_Ao[] = {0x0E, 0x0A, 0x0E, 0x11, 0x1F, 0x11, 0x11, 0x00};  //Å
const char udc_PO[] = {0x04, 0x0A, 0x0A, 0x1F, 0x1B, 0x1B, 0x1F, 0x00};  //Padlock Open
const char udc_PC[] = {0x1C, 0x10, 0x08, 0x1F, 0x1B, 0x1B, 0x1F, 0x00};  //Padlock Closed

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
const char udc_AC[]     = {0x0A, 0x0A, 0x1F, 0x11, 0x0E, 0x04, 0x04, 0x00};  // AC Power

//const char udc_smiley[] = {0x00, 0x0A, 0x00, 0x04, 0x11, 0x0E, 0x00, 0x00};  // Smiley
//const char udc_droopy[] = {0x00, 0x0A, 0x00, 0x04, 0x00, 0x0E, 0x11, 0x00};  // Droopey
//const char udc_note[]   = {0x01, 0x03, 0x05, 0x09, 0x0B, 0x1B, 0x18, 0x00};  // Note

//const char udc_bar_1[]  = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00};  // Bar 1
//const char udc_bar_2[]  = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00};  // Bar 11
//const char udc_bar_3[]  = {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x00};  // Bar 111
//const char udc_bar_4[]  = {0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x00};  // Bar 1111
//const char udc_bar_5[]  = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};  // Bar 11111

//const char udc_ch_1[]  =  {0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00};  // Hor bars 4
//const char udc_ch_2[]  =  {0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f, 0x00, 0x1f};  // Hor bars 4 (inverted)
//const char udc_ch_3[]  =  {0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15};  // Ver bars 3
//const char udc_ch_4[]  =  {0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a};  // Ver bars 3 (inverted)
//const char udc_ch_yr[] =  {0x08, 0x0f, 0x12, 0x0f, 0x0a, 0x1f, 0x02, 0x02};  // Year   (kana)
//const char udc_ch_mo[] =  {0x0f, 0x09, 0x0f, 0x09, 0x0f, 0x09, 0x09, 0x13};  // Month  (kana)
//const char udc_ch_dy[] =  {0x1f, 0x11, 0x11, 0x1f, 0x11, 0x11, 0x11, 0x1F};  // Day    (kana)
//const char udc_ch_mi[] =  {0x0C, 0x0a, 0x11, 0x1f, 0x09, 0x09, 0x09, 0x13};  // minute (kana)


/** A TextLCD interface for driving 4-bit HD44780-based LCDs
 *
 * @brief Currently supports 8x1, 8x2, 12x2, 12x4, 16x1, 16x2, 16x4, 20x2, 20x4, 24x2, 24x4, 40x2 and 40x4 panels
 *        Interface options include direct mbed pins, I2C portexpander (PCF8474/PCF8574A or MCP23008) or SPI bus shiftregister (74595) and some native I2C or SPI devices 
 *
 */
class TextLCD_Base : public Stream {
public:

    /** LCD panel format */
    enum LCDType {
        LCD8x1,     /**<  8x1 LCD panel */    
        LCD8x2,     /**<  8x2 LCD panel */          
        LCD8x2B,    /**<  8x2 LCD panel (actually 16x1) */                  
//        LCD12x1,    /**< 12x1 LCD panel */                          
//        LCD12x1B,   /**< 12x1 LCD panel, special mode PCF21XX */                                  
        LCD12x2,    /**< 12x2 LCD panel */                          
        LCD12x3B,   /**< 12x3 LCD panel, special mode PCF21XX */                                  
        LCD12x4,    /**< 12x4 LCD panel */                  
        LCD12x4B,   /**< 12x4 LCD panel, special mode PCF21XX */                                          
        LCD16x1,    /**< 16x1 LCD panel (actually 8x2) */          
        LCD16x2,    /**< 16x2 LCD panel (default) */
        LCD16x2B,   /**< 16x2 LCD panel alternate addressing */
//        LCD16x3B,   /**< 16x3 LCD panel, special mode ST7036 */                
        LCD16x4,    /**< 16x4 LCD panel */        
//        LCD20x1,    /**< 20x1 LCD panel */
        LCD20x2,    /**< 20x2 LCD panel */
        LCD20x4,    /**< 20x4 LCD panel */
        LCD24x4B,   /**< 24x4 LCD panel, special mode KS0078 */                                                  
        LCD24x1,    /**< 24x1 LCD panel */        
        LCD24x2,    /**< 24x2 LCD panel */        
        LCD40x2,    /**< 40x2 LCD panel */                
        LCD40x4     /**< 40x4 LCD panel, Two controller version */                        
    };

    /** LCD Controller Device */
    enum LCDCtrl {
        HD44780,     /**<  HD44780 (default)                           */    
        WS0010,      /**<  WS0010  OLED Controller, 4/8 bit            */    
        ST7036,      /**<  ST7036  3V3 with Booster, 4/8 bit, SPI      */   
        ST7032_3V3,  /**<  ST7032  3V3 with Booster, 4/8 bit, SPI, I2C */   
        ST7032_5V,   /**<  ST7032  5V no Booster, 4/8 bit, SPI, I2C    */           
        KS0078,      /**<  KS0078  24x3 support                        */                   
        PCF21XX_3V3, /**<  PCF21XX 3V3 with Booster, 4/8 bit, I2C      */                           
//        PCF21XX_5V   /**<  PCF21XX 5V no Booster, 4/8 bit, I2C         */        
    };


    /** LCD Cursor control */
    enum LCDCursor {
        CurOff_BlkOff = 0x00,  /**<  Cursor Off, Blinking Char Off */    
        CurOn_BlkOff  = 0x02,  /**<  Cursor On, Blinking Char Off */    
        CurOff_BlkOn  = 0x01,  /**<  Cursor Off, Blinking Char On */    
        CurOn_BlkOn   = 0x03   /**<  Cursor On, Blinking Char On */    
    };


    /** LCD Display control */
    enum LCDMode {
        DispOff = 0x00,  /**<  Display Off */    
        DispOn  = 0x04   /**<  Display On */            
    };

   /** LCD Backlight control */
    enum LCDBacklight {
        LightOff,        /**<  Backlight Off */    
        LightOn          /**<  Backlight On */            
    };


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

    /** Locate cursor to a screen column and row
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


    /** Clear the screen and locate to 0,0
     */
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
     * @param cursorMode  The Cursor mode (CurOff_BlkOff, CurOn_BlkOff, CurOff_BlkOn, CurOn_BlkOn)
     */
    void setCursor(LCDCursor cursorMode);     
    

    /** Set the Displaymode
     *
     * @param displayMode The Display mode (DispOff, DispOn)
     */
    void setMode(LCDMode displayMode);     

    /** Set the Backlight mode
     *
     *  @param backlightMode The Backlight mode (LightOff, LightOn)
     */
    void setBacklight(LCDBacklight backlightMode); 


    /** Set User Defined Characters
     *
     * @param unsigned char c   The Index of the UDC (0..7)
     * @param char *udc_data    The bitpatterns for the UDC (8 bytes of 5 significant bits)     
     */
    void setUDC(unsigned char c, char *udc_data);

//test
    void _initCtrl();    
    
protected:

   /** LCD controller select, mainly used for LCD40x4
     */
    enum _LCDCtrl_Idx {
        _LCDCtrl_0,  /*<  Primary */    
        _LCDCtrl_1,  /*<  Secondary */            
    };

    /** Create a TextLCD_Base interface
      * @brief Base class, can not be instantiated
      *
      * @param type  Sets the panel size/addressing mode (default = LCD16x2)
      * @param ctrl  LCD controller (default = HD44780)           
      */
    TextLCD_Base(LCDType type = LCD16x2, LCDCtrl ctrl = HD44780);

    
    // Stream implementation functions
    virtual int _putc(int value);
    virtual int _getc();

/** Low level method for LCD controller
  */
    void _init();    

/** Low level initialisation method for LCD controller
  */
//    void _initCtrl();    

/** Low level character address set method
  */  
    int  _address(int column, int row);
    
/** Low level cursor enable or disable method
  */  
    void _setCursor(LCDCursor show);

/** Low level method to store user defined characters for current controller
  */     
    void _setUDC(unsigned char c, char *udc_data);   

/** Low level method to restore the cursortype and display mode for current controller
  */     
    void _setCursorAndDisplayMode(LCDMode displayMode, LCDCursor cursorType);       
    

/** Low level nibble write operation to LCD controller (serial or parallel)
  */
    void _writeNibble(int value);
   
/** Low level command byte write operation to LCD controller.
  * Methods resets the RS bit and provides the required timing for the command.
  */
    void _writeCommand(int command);

/** Low level data byte write operation to LCD controller (serial or parallel).
  * Methods sets the RS bit and provides the required timing for the data.
  */   
    void _writeData(int data);


/** Pure Virtual Low level writes to LCD Bus (serial or parallel)
  * Set the Enable pin.
  */
    virtual void _setEnable(bool value) = 0;

/** Pure Virtual Low level writes to LCD Bus (serial or parallel)
  * Set the RS pin ( 0 = Command, 1 = Data).
  */   
    virtual void _setRS(bool value) = 0;  

/** Pure Virtual Low level writes to LCD Bus (serial or parallel)
  * Set the BL pin (0 = Backlight Off, 1 = Backlight On).
  */   
    virtual void _setBL(bool value) = 0;
    
/** Pure Virtual Low level writes to LCD Bus (serial or parallel)
  * Set the databus value (4 bit).
  */   
    virtual void _setData(int value) = 0;

/** Low level byte write operation to LCD controller (serial or parallel)
  * Depending on the RS pin this byte will be interpreted as data or command
  */
    virtual void _writeByte(int value);
    
//Display type
    LCDType _type;

//Display mode
    LCDMode _currentMode;

//Controller type 
    LCDCtrl _ctrl;    

//Controller select, mainly used for LCD40x4 
    _LCDCtrl_Idx _ctrl_idx;    

// Cursor
    int _column;
    int _row;
    LCDCursor _currentCursor;    
};

//--------- End TextLCD_Base -----------



//--------- Start TextLCD Bus -----------

/** Create a TextLCD interface for using regular mbed pins
  *
  */
class TextLCD : public TextLCD_Base {
public:    
    /** Create a TextLCD interface for using regular mbed pins
     *
     * @param rs    Instruction/data control line
     * @param e     Enable line (clock)
     * @param d4-d7 Data lines for using as a 4-bit interface
     * @param type  Sets the panel size/addressing mode (default = LCD16x2)
     * @param bl    Backlight control line (optional, default = NC)      
     * @param e2    Enable2 line (clock for second controller, LCD40x4 only)  
     * @param ctrl  LCD controller (default = HD44780)           
     */
    TextLCD(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7, LCDType type = LCD16x2, PinName bl = NC, PinName e2 = NC, LCDCtrl ctrl = HD44780);


   /** Destruct a TextLCD interface for using regular mbed pins
     *
     * @param  none
     * @return none
     */ 
    virtual ~TextLCD();

private:    

/** Implementation of pure Virtual Low level writes to LCD Bus (parallel)
  * Set the Enable pin.
  */
    virtual void _setEnable(bool value);

/** Implementation of pure Virtual Low level writes to LCD Bus (parallel)
  * Set the RS pin ( 0 = Command, 1 = Data).
  */   
    virtual void _setRS(bool value);  

/** Implementation of pure Virtual Low level writes to LCD Bus (parallel)
  * Set the BL pin (0 = Backlight Off, 1 = Backlight On).
  */   
    virtual void _setBL(bool value);
    
/** Implementation of pure Virtual Low level writes to LCD Bus (parallel)
  * Set the databus value (4 bit).
  */   
    virtual void _setData(int value);


/** Regular mbed pins bus
  */
    DigitalOut _rs, _e;
    BusOut _d;
    
/** Optional Hardware pins for the Backlight and LCD40x4 device
  * Default PinName value is NC, must be used as pointer to avoid issues with mbed lib and DigitalOut pins
  */
    DigitalOut *_bl, *_e2;       
};

    
//----------- End TextLCD ---------------


//--------- Start TextLCD_I2C -----------


/** Create a TextLCD interface using an I2C PCF8574 (or PCF8574A) or MCP23008 portexpander
  *
  */
class TextLCD_I2C : public TextLCD_Base {    
public:
   /** Create a TextLCD interface using an I2C PCF8574 (or PCF8574A) or MCP23008 portexpander
     *
     * @param i2c             I2C Bus
     * @param deviceAddress   I2C slave address (PCF8574 or PCF8574A, default = PCF8574_SA0 = 0x40)
     * @param type            Sets the panel size/addressing mode (default = LCD16x2)
     * @param ctrl            LCD controller (default = HD44780)                
     */
    TextLCD_I2C(I2C *i2c, char deviceAddress = PCF8574_SA0, LCDType type = LCD16x2, LCDCtrl ctrl = HD44780);

private:
    
/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the Enable pin.
  */
    virtual void _setEnable(bool value);

/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the RS pin ( 0 = Command, 1 = Data).
  */   
    virtual void _setRS(bool value);  

/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the BL pin (0 = Backlight Off, 1 = Backlight On).
  */   
    virtual void _setBL(bool value);
    
/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the databus value (4 bit).
  */   
    virtual void _setData(int value);
    
    
/** Write data to MCP23008 I2C portexpander
  *  @param reg register to write
  *  @param value data to write
  *  @return none     
  *
  */
    void _write_register (int reg, int value);     
  
//I2C bus
    I2C *_i2c;
    char _slaveAddress;
    
// Internal bus mirror value for serial bus only
    char _lcd_bus;   
    
};


//---------- End TextLCD_I2C ------------



//--------- Start TextLCD_SPI -----------


/** Create a TextLCD interface using an SPI 74595 portexpander
  *
  */
class TextLCD_SPI : public TextLCD_Base {    
public:
    /** Create a TextLCD interface using an SPI 74595 portexpander
     *
     * @param spi             SPI Bus
     * @param cs              chip select pin (active low)
     * @param type            Sets the panel size/addressing mode (default = LCD16x2)
     * @param ctrl            LCD controller (default = HD44780)                     
     */
    TextLCD_SPI(SPI *spi, PinName cs, LCDType type = LCD16x2, LCDCtrl ctrl = HD44780);


private:

/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the Enable pin.
  */
    virtual void _setEnable(bool value);

/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the RS pin ( 0 = Command, 1 = Data).
  */   
    virtual void _setRS(bool value);  

/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the BL pin (0 = Backlight Off, 1 = Backlight On).
  */   
    virtual void _setBL(bool value);
    
/** Implementation of pure Virtual Low level writes to LCD Bus (serial expander)
  * Set the databus value (4 bit).
  */   
    virtual void _setData(int value);
    
/** Implementation of Low level writes to LCD Bus (serial expander)
  * Set the CS pin (0 = select, 1 = deselect).
  */   
    virtual void _setCS(bool value);
    
///** Low level writes to LCD serial bus only (serial expander)
//  */
//    void _writeBus();      
   
// SPI bus        
    SPI *_spi;
    DigitalOut _cs;    
    
// Internal bus mirror value for serial bus only
    char _lcd_bus;   

};

//---------- End TextLCD_SPI ------------



//--------- Start TextLCD_SPI_N -----------

/** Create a TextLCD interface using a controller with native SPI interface
  *
  */
class TextLCD_SPI_N : public TextLCD_Base {    
public:
    /** Create a TextLCD interface using a controller with native SPI interface
     *
     * @param spi             SPI Bus
     * @param cs              chip select pin (active low)
     * @param rs              Instruction/data control line
     * @param type            Sets the panel size/addressing mode (default = LCD16x2)
     * @param bl              Backlight control line (optional, default = NC)  
     * @param ctrl            LCD controller (default = ST7032_3V3)                     
     */
    TextLCD_SPI_N(SPI *spi, PinName cs, PinName rs, LCDType type = LCD16x2, PinName bl = NC, LCDCtrl ctrl = ST7032_3V3);
    virtual ~TextLCD_SPI_N(void);

private:

/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the Enable pin.
  */
    virtual void _setEnable(bool value);

/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the RS pin ( 0 = Command, 1 = Data).
  */   
    virtual void _setRS(bool value);  

/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the BL pin (0 = Backlight Off, 1 = Backlight On).
  */   
    virtual void _setBL(bool value);
    
/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the databus value (4 bit).
  */   
    virtual void _setData(int value);

/** Low level writes to LCD serial bus only (serial native)
  */
    virtual void _writeByte(int value);

   
// SPI bus        
    SPI *_spi;
    DigitalOut _cs;    
    DigitalOut _rs;
    DigitalOut *_bl;

};

//---------- End TextLCD_SPI_N ------------


//--------- Start TextLCD_I2C_N -----------

/** Create a TextLCD interface using a controller with native I2C interface
  *
  */
class TextLCD_I2C_N : public TextLCD_Base {    
public:
    /** Create a TextLCD interface using a controller with native I2C interface
     *
     * @param i2c             I2C Bus
     * @param deviceAddress   I2C slave address (default = ST7032_SA = 0x7C)  
     * @param type            Sets the panel size/addressing mode (default = LCD16x2)
     * @param bl              Backlight control line (optional, default = NC)       
     * @param ctrl            LCD controller (default = ST7032_3V3)                     
     */
    TextLCD_I2C_N(I2C *i2c, char deviceAddress = ST7032_SA, LCDType type = LCD16x2, PinName bl = NC, LCDCtrl ctrl = ST7032_3V3);
    virtual ~TextLCD_I2C_N(void);

private:

/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the Enable pin.
  */
    virtual void _setEnable(bool value);

/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the RS pin ( 0 = Command, 1 = Data).
  */   
    virtual void _setRS(bool value);  

/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the BL pin (0 = Backlight Off, 1 = Backlight On).
  */   
    virtual void _setBL(bool value);
    
/** Implementation of pure Virtual Low level writes to LCD Bus (serial native)
  * Set the databus value (4 bit).
  */   
    virtual void _setData(int value);

/** Low level writes to LCD serial bus only (serial native)
  */
    virtual void _writeByte(int value);


//I2C bus
    I2C *_i2c;
    char _slaveAddress;
    
// controlbyte to select between data and command. Internal value for serial bus only
    char _controlbyte;   
    
    DigitalOut *_bl;    
};

//---------- End TextLCD_I2C_N ------------


#endif
