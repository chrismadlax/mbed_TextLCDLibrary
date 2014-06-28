/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 *               2013, v01: WH, Added LCD types, fixed LCD address issues, added Cursor and UDCs 
 *               2013, v02: WH, Added I2C and SPI bus interfaces  
 *               2013, v03: WH, Added support for LCD40x4 which uses 2 controllers 
 *               2013, v04: WH, Added support for Display On/Off, improved 4bit bootprocess
 *               2013, v05: WH, Added support for 8x2B, added some UDCs   
 *               2013, v06: WH, Added support for devices that use internal DC/DC converters 
 *               2013, v07: WH, Added support for backlight and include portdefinitions for LCD2004 Module from DFROBOT 
 *               2014, v08: WH, Refactored in Base and Derived Classes to deal with mbed lib change regarding 'NC' defined pins 
 *               2014, v09: WH/EO, Added Class for Native SPI controllers such as ST7032 
 *               2014, v10: WH, Added Class for Native I2C controllers such as ST7032i, Added support for MCP23008 I2C portexpander, Added support for Adafruit module  
 *               2014, v11: WH, Added support for native I2C controllers such as PCF21XX, Improved the _initCtrl() method to deal with differences between all supported controllers  
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

//For Testing only
//DigitalOut led1(LED1);
//DigitalOut led2(LED2);
//  led2=!led2;    


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

    
/** Create a TextLCD_Base interface
  *
  * @param type  Sets the panel size/addressing mode (default = LCD16x2)
  * @param ctrl  LCD controller (default = HD44780)           
  */
TextLCD_Base::TextLCD_Base(LCDType type, LCDCtrl ctrl) : _type(type), _ctrl(ctrl) {
    
  // Extract LCDType data  

  // Columns encoded in b7..b0
  _nr_cols = (_type & 0xFF);          

  // Rows encoded in b15..b8  
  _nr_rows = ((_type >> 8) & 0xFF);  

  // Addressing mode encoded in b19..b16  
  _addr_mode = _type & LCD_T_ADR_MSK;
}


/**  Init the LCD Controller(s)
  *  Clear display 
  */
void TextLCD_Base::_init() {
  
  // Select and configure second LCD controller when needed
  if(_type==LCD40x4) {
    _ctrl_idx=_LCDCtrl_1;        // Select 2nd controller   
    _initCtrl();                 // Init 2nd controller   
  }
    
  // Select and configure primary LCD controller
  _ctrl_idx=_LCDCtrl_0;          // Select primary controller  
  _initCtrl();                   // Init primary controller

  // Reset Cursor location
  _row=0;
  _column=0;
  
} 

/**  Init the LCD controller
  *  4-bit mode, number of lines, fonttype, no cursor etc
  *
  *  Note: some configurations are commented out because they have not yet been tested due to lack of hardware
  */
void TextLCD_Base::_initCtrl() {

    this->_setRS(false); // command mode
    
    wait_ms(20);         // Wait 20ms to ensure powered up

    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)    
    for (int i=0; i<3; i++) {
        _writeNibble(0x3);
        wait_ms(15);     // This command takes 1.64ms, so wait for it 
    }
    _writeNibble(0x2);   // 4-bit mode
    wait_us(40);         // most instructions take 40us

    // Display is now in 4-bit mode
    // Note: 4 bit mode is ignored for native SPI and I2C devices
   
    // Device specific initialisations: DC/DC converter to generate VLCD or VLED, number of lines etc
    switch (_ctrl) {
      case KS0078:

          // Initialise Display configuration
          switch (_type) {
            case LCD8x1:         //8x1 is a regular 1 line display
            case LCD8x2B:        //8x2B is a special case of 16x1
//            case LCD12x1:                                
            case LCD16x1:                                            
//            case LCD20x1:
            case LCD24x1:
              _writeCommand(0x20); // Function set 001 DL N RE(0) DH REV
                                   //   DL=0  (4 bits bus)             
                                   //    N=0  (1 line)                                   
                                   //   RE=0  (Dis. Extended Regs, special mode for KS0078)
                                   //   DH=0  (Disp shift=disable, special mode for KS0078)                                
                                   //   REV=0 (Reverse=Normal, special mode for KS0078)



              break;                                

//            case LCD12x3D:         // Special mode for KS0078            
//            case LCD12x3D1:        // Special mode for KS0078            
//            case LCD12x4D:         // Special mode for KS0078            
//            case LCD16x3D:             
//            case LCD16x4D:            
//            case LCD24x3D:         // Special mode for KS0078
//            case LCD24x3D1:        // Special mode for KS0078
            case LCD24x4D:         // Special mode for KS0078

              _writeCommand(0x2A); // Function set 001 DL N RE(0) DH REV
                                   //   DL=0  (4 bits bus)             
                                   //    N=1  (Dont care for KS0078 in 4-line mode)
                                   //   RE=0  (Dis. Extended Regs, special mode for KS0078)
                                   //   DH=1  (Disp shift=enable, special mode for KS0078)                                
                                   //   REV=0 (Reverse=Normal, special mode for KS0078)

              _writeCommand(0x2E); // Function set 001 DL N RE(1) BE 0
                                   //   DL=0  (4 bits bus)             
                                   //    N=1  (Dont care for KS0078 in 4-line mode)
                                   //   RE=1  (Ena Extended Regs, special mode for KS0078)
                                   //   BE=1  (Blink Enable, CG/SEG RAM, special mode for KS0078)                                
                                   //    X=0 (Reverse, special mode for KS0078)

              _writeCommand(0x09); // Ext Function set 0000 1 FW BW NW
                                   //   FW=0  (5-dot font, special mode for KS0078)
                                   //   BW=0  (Cur BW invert disable, special mode for KS0078)
                                   //   NW=1  (4 Line, special mode for KS0078)                                

              _writeCommand(0x2A); // Function set 001 DL N RE(0) DH REV
                                   //   DL=0  (4 bits bus)             
                                   //    N=1  (Dont care for KS0078 in 4 line mode)
                                   //   RE=0  (Dis. Extended Regs, special mode for KS0078)
                                   //   DH=1  (Disp shift enable, special mode for KS0078)                                
                                   //   REV=0 (Reverse normal, special mode for KS0078)
              break;                                
              
            default:
              // All other LCD types are initialised as 2 Line displays (including LCD16x1C and LCD40x4)            
              _writeCommand(0x28); // Function set 001 DL N RE(0) DH REV
                                   //   DL=0  (4 bits bus)             
                                   //   Note: 4 bit mode is ignored for native SPI and I2C devices                                                                    
                                   //    N=1  (2 lines)                                   
                                   //   RE=0  (Dis. Extended Regs, special mode for KS0078)
                                   //   DH=0  (Disp shift=disable, special mode for KS0078)                                
                                   //   REV=0 (Reverse=Normal, special mode for KS0078)

              break;
          } // switch type

          break; // case KS0078 Controller
              
      case ST7032_3V3:
          // ST7032 controller: Initialise Voltage booster for VLCD. VDD=3V3

          // Initialise Display configuration
          switch (_type) {
            case LCD8x1:         //8x1 is a regular 1 line display
            case LCD8x2B:        //8x2B is a special case of 16x1
//            case LCD12x1:                                
            case LCD16x1:                                            
//            case LCD20x1:                    
            case LCD24x1:                    
              _writeCommand(0x21);    //FUNCTION SET 4 bit, N=0 1-line display mode, 5*7dot, Select Instruction Set = 1
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices

              _writeCommand(0x1C);    //Internal OSC frequency adjustment Framefreq=183HZ, bias will be 1/4 

              _writeCommand(0x73);    //Contrast control low byte

              _writeCommand(0x57);    //booster circuit is turned on. /ICON display off. /Contrast control high byte
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x6C);    //Follower control
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x20);    //FUNCTION SET 4 bit, N=0 1-line display mode, 5*7dot, Return to Instruction Set = 0                  
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices                                                                          
              break;  

            case LCD12x3D:            // Special mode for PCF2116
            case LCD12x3D1:           // Special mode for PCF2116
            case LCD12x4D:            // Special mode for PCF2116
            case LCD24x4D:            // Special mode for KS0078
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  

            default:
              // All other LCD types are initialised as 2 Line displays        
              _writeCommand(0x29);    //FUNCTION SET 4 bit, N=1 2-line display mode, 5*7dot, Select Instruction Set = 1
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices

              _writeCommand(0x1C);    //Internal OSC frequency adjustment Framefreq=183HZ, bias will be 1/4 

              _writeCommand(0x73);    //Contrast control low byte

              _writeCommand(0x57);    //booster circuit is turned on. /ICON display off. /Contrast control high byte
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x6C);    //Follower control
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x28);    //FUNCTION SET 4 bit, N=1 2-line display mode, 5*7dot, Return to Instruction Set = 0                  
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices                                                                          
          } // switch type    
                                     
          break; // case ST7032_3V3 Controller

      case ST7032_5V:
          // ST7032 controller: Disable Voltage booster for VLCD. VDD=5V      

          // Initialise Display configuration
          switch (_type) {
            case LCD8x1:         //8x1 is a regular 1 line display
            case LCD8x2B:        //8x2B is a special case of 16x1
//            case LCD12x1:                                
            case LCD16x1:                                            
//            case LCD20x1:                    
            case LCD24x1:                    
              _writeCommand(0x21);    //FUNCTION SET 4 bit, N=0 1-line display mode, 5*7dot, Select Instruction Set = 1
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices

              _writeCommand(0x1C);    //Internal OSC frequency adjustment Framefreq=183HZ, bias will be 1/4 

              _writeCommand(0x73);    //Contrast control low byte

              _writeCommand(0x53);    //booster circuit is turned off. /ICON display off. /Contrast control high byte
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x6C);    //Follower control
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x20);    //FUNCTION SET 4 bit, N=0 1-line display mode, 5*7dot, Return to Instruction Set = 0                  
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices                                                                          
              break;  

            case LCD12x3D:            // Special mode for PCF2116
            case LCD12x3D1:           // Special mode for PCF2116
            case LCD12x4D:            // Special mode for PCF2116
            case LCD24x4D:            // Special mode for KS0078
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  

            default:
              // All other LCD types are initialised as 2 Line displays        
              _writeCommand(0x29);    //FUNCTION SET 4 bit, N=1 2-line display mode, 5*7dot, Select Instruction Set = 1
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices

              _writeCommand(0x1C);    //Internal OSC frequency adjustment Framefreq=183HZ, bias will be 1/4 

              _writeCommand(0x73);    //Contrast control low byte

              _writeCommand(0x53);    //booster circuit is turned off. /ICON display off. /Contrast control high byte
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x6C);    //Follower control
              wait_ms(10);            // Wait 10ms to ensure powered up
          
              _writeCommand(0x28);    //FUNCTION SET 4 bit, N=1 2-line display mode, 5*7dot, Return to Instruction Set = 0                  
                                      //Note: 4 bit mode is ignored for native SPI and I2C devices                                                                          
          } // switch type    
                                    
          break; // case ST7032_5V Controller

      case ST7036:
          // ST7036 controller: Initialise Voltage booster for VLCD. VDD=5V
          // Note: supports 1,2 or 3 lines
          
          // Initialise Display configuration
          switch (_type) {
            case LCD8x1:         //8x1 is a regular 1 line display
            case LCD8x2B:        //8x2D is a special case of 16x1
//            case LCD12x1:                                
            case LCD16x1:   
            case LCD24x1:                                                                         
              _writeCommand(0x21);    // 4-bit Databus, N=0 1 Line, DH=0 5x7font, IS2,IS1 = 01 Select Instruction Set = 1
              wait_ms(30);            // > 26,3ms 
              _writeCommand(0x14);    // Bias: 1/5, 1 or 2-Lines LCD 
//              _writeCommand(0x15);    // Bias: 1/5, 3-Lines LCD           
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x55);    // Icon off, Booster on, Set Contrast C5, C4
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x6D);    // Voltagefollower On, Ampl ratio Rab2, Rab1, Rab0
              wait_ms(200);           // > 200ms!
              _writeCommand(0x78);    // Set Contrast C3, C2, C1, C0
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x20);    // Return to Instruction Set = 0
              wait_ms(50);      
              break;  
#if(0)
//            case LCD12x3:                                            
            case LCD16x3:                                            
              _writeCommand(0x29);    // 4-bit Databus, N=1 2 Line, DH=0 5x7font, IS2,IS1 = 01 Select Instruction Set = 1
              wait_ms(30);            // > 26,3ms 
//              _writeCommand(0x14);    // Bias: 1/5, 1 or 2-Lines LCD 
              _writeCommand(0x15);    // Bias: 1/5, 3-Lines LCD           
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x55);    // Icon off, Booster on, Set Contrast C5, C4
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x6D);    // Voltagefollower On, Ampl ratio Rab2, Rab1, Rab0
              wait_ms(200);           // > 200ms!
              _writeCommand(0x78);    // Set Contrast C3, C2, C1, C0
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x28);    // Return to Instruction Set = 0
              wait_ms(50);      
              break;  
#endif

            case LCD12x3D:            // Special mode for PCF2116
            case LCD12x3D1:           // Special mode for PCF2116
            case LCD12x4D:            // Special mode for PCF2116
            case LCD24x4D:            // Special mode for KS0078
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  

            default:
              // All other LCD types are initialised as 2 Line displays (including LCD16x1C and LCD40x4)       
              _writeCommand(0x29);    // 4-bit Databus, N=1 2 Line, DH=0 5x7font, IS2,IS1 = 01 Select Instruction Set = 1              
              wait_ms(30);            // > 26,3ms 
              _writeCommand(0x14);    // Bias: 1/5, 2-Lines LCD 
//             _writeCommand(0x15);    // Bias: 1/5, 3-Lines LCD           
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x55);    // Icon off, Booster on, Set Contrast C5, C4
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x6D);    // Voltagefollower On, Ampl ratio Rab2, Rab1, Rab0
              wait_ms(200);           // > 200ms!
              _writeCommand(0x78);    // Set Contrast C3, C2, C1, C0
              wait_ms(30);            // > 26,3ms
              _writeCommand(0x28);    // Return to Instruction Set = 0
              wait_ms(50);      
          } // switch type
          
          break; // case ST7036 Controller
          
      case PCF2113_3V3:
          // PCF2113 controller: Initialise Voltage booster for VLCD. VDD=3V3
          // Note1: The PCF21XX family of controllers has several types that dont have an onboard voltage generator for V-LCD.
          //        You must supply this LCD voltage externally and not enable VGen.
          // Note2: The early versions of PCF2116 controllers (eg PCF2116C) can not generate sufficiently negative voltage for the LCD at a VDD of 3V3. 
          //        You must supply this voltage externally and not enable VGen or you must use a higher VDD (e.g. 5V) and enable VGen.
          //        More recent versions of the controller (eg PCF2116K) have an improved VGen that will work with 3V3.
          // Note3: See datasheet, members of the PCF21XX family support different numbers of rows/columns. Not all can support 3 or 4 rows.
          // Note4: See datasheet, you can also disable VGen by connecting Vo to VDD. VLCD will then be used directly as LCD voltage.
          // Note5: PCF2113 is different wrt to VLCD generator !           
          // Note6: See datasheet, the PCF21XX-C and PCF21XX-K use a non-standard character set. This may result is strange text when not corrected..
                
          // Initialise Display configuration
          switch (_type) {
//            case LCD12x1:                                
            case LCD24x1:                    
              _writeCommand(0x21);    //FUNCTION SET 4 bit, M=0 1-line/24 chars display mode, extended IS
                                      //Note: 4 bit mode is ignored for I2C mode              
              _writeCommand(0x9F);    //Set VLCD A : VGen for Chars and Icons
              _writeCommand(0xDF);    //Set VLCD B : VGen for Icons Only
              _writeCommand(0x20);    //FUNCTION SET 4 bit, M=0 1-line/24 chars display mode
//              _writeCommand(0x24);    //FUNCTION SET 4 bit, M=1 2-line/12 chars display mode, standard IS
              
              wait_ms(10);            // Wait 10ms to ensure powered up                                                    
              break;  

//Tested OK for PCF2113
//Note: PCF2113 is different wrt to VLCD generator ! 
            case LCD12x2:                    
              _writeCommand(0x21);    //FUNCTION SET 4 bit, M=0 1-line/24 chars display mode, extended IS
                                      //Note: 4 bit mode is ignored for I2C mode              
              _writeCommand(0x9F);    //Set VLCD A : VGen for Chars and Icons
              _writeCommand(0xDF);    //Set VLCD B : VGen for Icons Only
//              _writeCommand(0x20);    //FUNCTION SET 4 bit, M=0 1-line/24 chars display mode
              _writeCommand(0x24);    //FUNCTION SET 4 bit, M=1 2-line/12 chars display mode, standard IS
              
              wait_ms(10);            // Wait 10ms to ensure powered up                                                    
              break;  
             
            default:
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  
                         
          } // switch type    

          break; // case PCF2113_3V3 Controller



      case PCF2116_3V3:
          // PCF2116 controller: Initialise Voltage booster for VLCD. VDD=3V3
          // Note1: The PCF21XX family of controllers has several types that dont have an onboard voltage generator for V-LCD.
          //        You must supply this LCD voltage externally and not enable VGen.
          // Note2: The early versions of PCF2116 controllers (eg PCF2116C) can not generate sufficiently negative voltage for the LCD at a VDD of 3V3. 
          //        You must supply this voltage externally and not enable VGen or you must use a higher VDD (e.g. 5V) and enable VGen.
          //        More recent versions of the controller (eg PCF2116K) have an improved VGen that will work with 3V3.
          // Note3: See datasheet, members of the PCF21XX family support different numbers of rows/columns. Not all can support 3 or 4 rows.
          // Note4: See datasheet, you can also disable VGen by connecting Vo to VDD. VLCD will then be used directly as LCD voltage.
          // Note5: PCF2113 is different wrt to VLCD generator !           
          // Note6: See datasheet, the PCF21XX-C and PCF21XX-K use a non-standard character set. This may result is strange text when not corrected..
                  
          // Initialise Display configuration
          switch (_type) {
//            case LCD12x1:
//            case LCD12x2:                                                                            
            case LCD24x1:                    
              _writeCommand(0x22);    //FUNCTION SET 4 bit, N=0/M=0 1-line/24 chars display mode, G=1 VGen on                               
                                      //Note: 4 bit mode is ignored for I2C mode
              wait_ms(10);            // Wait 10ms to ensure powered up                                                    
              break;  

            case LCD12x3D:                                
            case LCD12x3D1:                                
            case LCD12x4D:
              _writeCommand(0x2E);    //FUNCTION SET 4 bit, N=1/M=1 4-line/12 chars display mode, G=1 VGen on                               
                                      //Note: 4 bit mode is ignored for I2C mode              
              wait_ms(10);            // Wait 10ms to ensure powered up                                                    
              break;  

            case LCD24x2:
              _writeCommand(0x2A);    //FUNCTION SET 4 bit, N=1/M=0 2-line/24 chars display mode, G=1 VGen on
                                      //Note: 4 bit mode is ignored for I2C mode
              wait_ms(10);            // Wait 10ms to ensure powered up   

            default:
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  
            
          } // switch type    

          break; // case PCF2116_3V3 Controller

//      case PCF21XX_5V:
          // PCF21XX controller: No Voltage generator for VLCD. VDD=5V
//@TODO                            

      case WS0010:         
          // WS0010 OLED controller: Initialise DC/DC Voltage converter for LEDs
          // Note1: Identical to RS0010  
          // Note2: supports 1 or 2 lines (and 16x100 graphics)
          //        supports 4 fonts (English/Japanese (default), Western European-I, English/Russian, Western European-II)
                           // Cursor/Disp shift set 0001 SC RL  0 0
                           //
                           // Mode and Power set    0001 GC PWR 1 1                           
                           //  GC  = 0 (Graph Mode=1, Char Mode=0)             
                           //  PWR = 1 (DC/DC On/Off)
   
//@Todo: This may be needed to enable a warm reboot
          //_writeCommand(0x13);   // DC/DC off              
          //wait_ms(10);           // Wait 10ms to ensure powered down                  
          _writeCommand(0x17);   // DC/DC on        
          wait_ms(10);           // Wait 10ms to ensure powered up        

          // Initialise Display configuration
          switch (_type) {                    
            case LCD8x1:         //8x1 is a regular 1 line display
            case LCD8x2B:        //8x2B is a special case of 16x1
//            case LCD12x1:                                
            case LCD16x1:                                            
            case LCD24x1:
              _writeCommand(0x20); // Function set 001 DL N F FT1 FT0
                                   //  DL=0  (4 bits bus)             
                                   //   N=0  (1 line)
                                   //   F=0  (5x7 dots font)
                                   //  FT=00 (00 = Engl/Jap, 01 = WestEur1, 10 = Engl/Russian, 11 = WestEur2
              break;  

            case LCD12x3D:            // Special mode for PCF2116
            case LCD12x3D1:           // Special mode for PCF2116
            case LCD12x4D:            // Special mode for PCF2116
            case LCD24x4D:            // Special mode for KS0078
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  

            default:
              // All other LCD types are initialised as 2 Line displays (including LCD16x1C and LCD40x4)       
              _writeCommand(0x28); // Function set 001 DL N F FT1 FT0
                                   //  DL=0  (4 bits bus)
                                   //   N=1  (2 lines)
                                   //   F=0  (5x7 dots font)
                                   //  FT=00 (00 = Engl/Jap, 01 = WestEur1, 10 = Engl/Russian, 11 = WestEur2


              break;
           } // switch type
           
           break; // case WS0100 Controller
           
        default:
          // Devices fully compatible to HD44780 that do not use any DC/DC Voltage converters but external VLCD

          // Initialise Display configuration
          switch (_type) {
            case LCD8x1:         //8x1 is a regular 1 line display
            case LCD8x2B:        //8x2B is a special case of 16x1
//            case LCD12x1:                                
            case LCD16x1:                                            
//            case LCD20x1:                                                        
            case LCD24x1:
//            case LCD40x1:            
              _writeCommand(0x20); // Function set 001 DL N F - -
                                   //  DL=0 (4 bits bus)             
                                   //   N=0 (1 line)
                                   //   F=0 (5x7 dots font)
              break;                                
                                                  

//            case LCD12x3D:            // Special mode for PCF2116
//            case LCD12x3D1:           // Special mode for PCF2116
//            case LCD12x4D:            // Special mode for PCF2116
//            case LCD24x3D:            // Special mode for KS0078
            case LCD24x4D:            // Special mode for KS0078
              error("Error: LCD Controller type does not support this Display type\n\r"); 
              break;  

            // All other LCD types are initialised as 2 Line displays (including LCD16x1C and LCD40x4)
            default:
              _writeCommand(0x28); // Function set 001 DL N F - -
                                   //  DL=0 (4 bits bus)
                                   //  Note: 4 bit mode is ignored for native SPI and I2C devices                                 
                                   //   N=1 (2 lines)
                                   //   F=0 (5x7 dots font, only option for 2 line display)
                                   //    -  (Don't care)                                
            
              break;
          } // switch type

          break; // case default Controller
          
    } // switch Controller specific initialisations 
    

    // Controller general initialisations                                          
    _writeCommand(0x01); // cls, and set cursor to 0
    wait_ms(10);         // The CLS command takes 1.64 ms.
                         // Since we are not using the Busy flag, Lets be safe and take 10 ms  

    _writeCommand(0x02); // Return Home 
                         //   Cursor Home, DDRAM Address to Origin

    _writeCommand(0x06); // Entry Mode 0000 0 1 I/D S 
                         //   Cursor Direction and Display Shift
                         //   I/D=1 (Cur incr)
                         //     S=0 (No display shift)                        

    _writeCommand(0x14); // Cursor or Display shift 0001 S/C R/L x x 
                         //   S/C=0 Cursor moves
                         //   R/L=1 Right
                         // 

//    _writeCommand(0x0C); // Display Ctrl 0000 1 D C B
//                         //   Display On, Cursor Off, Blink Off   
    setCursor(CurOff_BlkOff);     
    setMode(DispOn);     
}


/** Clear the screen, Cursor home. 
  */
void TextLCD_Base::cls() {

  // Select and configure second LCD controller when needed
  if(_type==LCD40x4) {
    _ctrl_idx=_LCDCtrl_1; // Select 2nd controller

    // Second LCD controller Cursor always Off
    _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);

    // Second LCD controller Clearscreen
    _writeCommand(0x01);  // cls, and set cursor to 0    
    wait_ms(10);          // The CLS command takes 1.64 ms.
                          // Since we are not using the Busy flag, Lets be safe and take 10 ms
  
    _ctrl_idx=_LCDCtrl_0; // Select primary controller
  }
  
  // Primary LCD controller Clearscreen
  _writeCommand(0x01);    // cls, and set cursor to 0
  wait_ms(10);            // The CLS command takes 1.64 ms.
                          // Since we are not using the Busy flag, Lets be safe and take 10 ms

  // Restore cursormode on primary LCD controller when needed
  if(_type==LCD40x4) {
    _setCursorAndDisplayMode(_currentMode,_currentCursor);     
  }
                   
  setAddress(0, 0);  // Reset Cursor location
                     // Note: this is needed because some displays (eg PCF21XX) don't use line 0 in the '3 Line' mode.   
}

/** Locate cursor to a screen column and row
  *
  * @param column  The horizontal position from the left, indexed from 0
  * @param row     The vertical position from the top, indexed from 0
  */ 
void TextLCD_Base::locate(int column, int row) {
    
   // setAddress() does all the heavy lifting:
   //   check column and row sanity, 
   //   switch controllers for LCD40x4 if needed
   //   switch cursor for LCD40x4 if needed
   //   set the new memory address to show cursor at correct location
   setAddress(column, row);
       
}
   

/** Write a single character (Stream implementation)
  */
int TextLCD_Base::_putc(int value) {
  int addr;
    
    if (value == '\n') {
      //No character to write
      
      //Update Cursor      
      _column = 0;
      _row++;
      if (_row >= rows()) {
        _row = 0;
      }      
    }
    else {
      //Character to write      
      _writeData(value); 
              
      //Update Cursor
      _column++;
      if (_column >= columns()) {
        _column = 0;
        _row++;
        if (_row >= rows()) {
          _row = 0;
        }
      }          
    } //else

    //Set next memoryaddress, make sure cursor blinks at next location
    addr = getAddress(_column, _row);
    _writeCommand(0x80 | addr);
            
    return value;
}


// get a single character (Stream implementation)
int TextLCD_Base::_getc() {
    return -1;
}


// Write a nibble using the 4-bit interface
void TextLCD_Base::_writeNibble(int value) {

// Enable is Low
    this->_setEnable(true);        
    this->_setData(value & 0x0F);   // Low nibble
    wait_us(1); // Data setup time        
    this->_setEnable(false);    
    wait_us(1); // Datahold time

// Enable is Low
}


// Write a byte using the 4-bit interface
void TextLCD_Base::_writeByte(int value) {

// Enable is Low
    this->_setEnable(true);          
    this->_setData(value >> 4);   // High nibble
    wait_us(1); // Data setup time    
    this->_setEnable(false);   
    wait_us(1); // Data hold time
    
    this->_setEnable(true);        
    this->_setData(value >> 0);   // Low nibble
    wait_us(1); // Data setup time        
    this->_setEnable(false);    
    wait_us(1); // Datahold time

// Enable is Low

}

// Write a command byte to the LCD controller
void TextLCD_Base::_writeCommand(int command) {

    this->_setRS(false);        
    wait_us(1);  // Data setup time for RS       
    
    this->_writeByte(command);   
    wait_us(40); // most instructions take 40us            
}

// Write a data byte to the LCD controller
void TextLCD_Base::_writeData(int data) {

    this->_setRS(true);            
    wait_us(1);  // Data setup time for RS 
        
    this->_writeByte(data);
    wait_us(40); // data writes take 40us                
}


#if (0)
// This is the original _address() method.
// It is confusing since it returns the memoryaddress or-ed with the set memorycommand 0x80.
// Left it in here for compatibility with older code. New applications should use getAddress() instead.
// 
int TextLCD_Base::_address(int column, int row) {
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


// This replaces the original _address() method.
// Left it in here for compatibility with older code. New applications should use getAddress() instead.
int TextLCD_Base::_address(int column, int row) {
  return 0x80 | getAddress(column, row);
}

#if(0)
// This is new method to return the memory address based on row, column and displaytype.
//
/** Return the memoryaddress of screen column and row location
   *
   * @param column  The horizontal position from the left, indexed from 0
   * @param row     The vertical position from the top, indexed from 0
   * @param return  The memoryaddress of screen column and row location
   *
   *  Note: some configurations are commented out because they have not yet been tested due to lack of hardware   
   */
int TextLCD_Base::getAddress(int column, int row) {

    switch (_type) {
        case LCD8x1:
//        case LCD12x1:        
//        case LCD16x1B:        
//        case LCD20x1:        
        case LCD24x1:
//        case LCD40x1:                        
            return 0x00 + column;                        

        case LCD16x1:
            // LCD16x1 is a special layout of LCD8x2
            if (column<8) 
              return 0x00 + column;                        
            else   
              return 0x40 + (column - 8);                        

        case LCD8x2D:
            // LCD8x2B is a special layout of LCD16x1
            if (row==0) 
              return 0x00 + column;                        
            else   
              return 0x08 + column;                        

        case LCD8x2:               
        case LCD12x2:                
        case LCD16x2:
        case LCD20x2:
        case LCD24x2:        
        case LCD40x2:                
            return 0x00 + (row * 0x40) + column;

// Not sure about this one, seems wrong.
// Left in for compatibility with original library
        case LCD16x2B:      
            return 0x00 + (row * 40) + column;
    

// Special mode for ST7036 
//        case LCD16x3:

// Special mode for PCF2116 
        case LCD12x3B:
            //Display bottom three rows of four
            switch (row) {
                case 0:
                    return 0x20 + column;
                case 1:
                    return 0x40 + column;
                case 2:
                    return 0x60 + column;
            }

#if(0)
        case LCD12x3C:
            //Display top three rows of four        
            switch (row) {
                case 0:
                    return 0x00 + column;
                case 1:
                    return 0x20 + column;
                case 2:
                    return 0x40 + column;
            }
#endif

        case LCD12x4:
            switch (row) {
                case 0:
                    return 0x00 + column;
                case 1:
                    return 0x40 + column;
                case 2:
                    return 0x0C + column;
                case 3:
                    return 0x4C + column;
            }

// Special mode for PCF2116 (and KS0078) 
        case LCD12x4B:
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
        case LCD24x4B:
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

        case LCD40x4:                
          // LCD40x4 is a special case since it has 2 controllers
          // Each controller is configured as 40x2
          if (row<2) { 
            // Test to see if we need to switch between controllers  
            if (_ctrl_idx != _LCDCtrl_0) {

              // Second LCD controller Cursor Off
              _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);    

              // Select primary controller
              _ctrl_idx = _LCDCtrl_0;

              // Restore cursormode on primary LCD controller
              _setCursorAndDisplayMode(_currentMode, _currentCursor);    
            }           
            
            return 0x00 + (row * 0x40) + column;          
          }
          else {

            // Test to see if we need to switch between controllers  
            if (_ctrl_idx != _LCDCtrl_1) {
              // Primary LCD controller Cursor Off
              _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);    

              // Select secondary controller
              _ctrl_idx = _LCDCtrl_1;

              // Restore cursormode on secondary LCD controller
              _setCursorAndDisplayMode(_currentMode, _currentCursor);    
            }           
                                   
            return 0x00 + ((row-2) * 0x40) + column;          
          } 
            
// Should never get here.
        default:            
            return 0x00;        
    }
}

#else

//Test of Addressing Mode encoded in LCDType

// This is new method to return the memory address based on row, column and displaytype.
//
/** Return the memoryaddress of screen column and row location
   *
   * @param column  The horizontal position from the left, indexed from 0
   * @param row     The vertical position from the top, indexed from 0
   * @param return  The memoryaddress of screen column and row location
   *
   *  Note: some configurations are commented out because they have not yet been tested due to lack of hardware   
   */
int TextLCD_Base::getAddress(int column, int row) {

    switch (_addr_mode) {

        case LCD_T_A:
          //Default addressing mode for 1, 2 and 4 rows (except 40x4)
          //The two available rows are split and stacked on top of eachother. Addressing for 3rd and 4th line continues where lines 1 and 2 were split.          
          //Displays top rows when less than four are used.          
          switch (row) {
            case 0:
              return 0x00 + column;
            case 1:
              return 0x40 + column;
            case 2:
              return 0x00 + _nr_cols + column;
            case 3:
              return 0x40 + _nr_cols + column;
            // Should never get here.
            default:            
              return 0x00;                    
            }
          
        case LCD_T_B:
          // LCD8x2B is a special layout of LCD16x1
          if (row==0) 
            return 0x00 + column;                        
          else   
//            return _nr_cols + column;                                    
            return 0x08 + column;                        

        case LCD_T_C:
          // LCD16x1C is a special layout of LCD8x2
          if (column<8) 
            return 0x00 + column;                        
          else   
            return 0x40 + (column - 8);                        

// Not sure about this one, seems wrong.
// Left in for compatibility with original library
//        case LCD16x2B:      
//            return 0x00 + (row * 40) + column;
  

        case LCD_T_D:
          //Alternate addressing mode for 3 and 4 row displays (except 40x4). Used by PCF21XX, KS0078
          //The 4 available rows start at a hardcoded address.                    
          //Displays top rows when less than four are used.
          switch (row) {
            case 0:
              return 0x00 + column;
            case 1:
              return 0x20 + column;
            case 2:
              return 0x40 + column;
            case 3:
              return 0x60 + column;
            // Should never get here.
            default:            
              return 0x00;                    
            }

        case LCD_T_D1:
          //Alternate addressing mode for 3 row displays. Used by PCF21XX, KS0078
          //The 4 available rows start at a hardcoded address.                              
          //Skips top row of 4 row display and starts display at row 1
          switch (row) {
            case 0:
              return 0x20 + column;
            case 1:
              return 0x40 + column;
            case 2:
              return 0x60 + column;
            // Should never get here.
            default:            
              return 0x00;                    
            }
        
        case LCD_T_E:                
          // LCD40x4 is a special case since it has 2 controllers.
          // Each controller is configured as 40x2 (Type A)
          if (row<2) { 
            // Test to see if we need to switch between controllers  
            if (_ctrl_idx != _LCDCtrl_0) {

              // Second LCD controller Cursor Off
              _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);    

              // Select primary controller
              _ctrl_idx = _LCDCtrl_0;

              // Restore cursormode on primary LCD controller
              _setCursorAndDisplayMode(_currentMode, _currentCursor);    
            }           
            
            return 0x00 + (row * 0x40) + column;          
          }
          else {

            // Test to see if we need to switch between controllers  
            if (_ctrl_idx != _LCDCtrl_1) {
              // Primary LCD controller Cursor Off
              _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);    

              // Select secondary controller
              _ctrl_idx = _LCDCtrl_1;

              // Restore cursormode on secondary LCD controller
              _setCursorAndDisplayMode(_currentMode, _currentCursor);    
            }           
                                   
            return 0x00 + ((row-2) * 0x40) + column;          
          } 
            
        // Should never get here.
        default:            
            return 0x00;        
    }
}


#endif



/** Set the memoryaddress of screen column and row location
  *
  * @param column  The horizontal position from the left, indexed from 0
  * @param row     The vertical position from the top, indexed from 0
  */
void TextLCD_Base::setAddress(int column, int row) {
   
// Sanity Check column
    if (column < 0) {
      _column = 0;
    }
    else if (column >= columns()) {
      _column = columns() - 1;
    } else _column = column;
    
// Sanity Check row
    if (row < 0) {
      _row = 0;
    }
    else if (row >= rows()) {
      _row = rows() - 1;
    } else _row = row;
    
    
// Compute the memory address
// For LCD40x4:  switch controllers if needed
//               switch cursor if needed
    int addr = getAddress(_column, _row);
    
    _writeCommand(0x80 | addr);
}


/** Return the number of columns
  *
  * @param return  The number of columns
  *
  * Note: some configurations are commented out because they have not yet been tested due to lack of hardware     
  */   
int TextLCD_Base::columns() {
    
  // Columns encoded in b7..b0
  //return (_type & 0xFF);          
  return _nr_cols;          
  
#if(0)    
    switch (_type) {
        case LCD8x1:
        case LCD8x2:
        case LCD8x2B:                
            return 8;
        
        case LCD12x2:        
        case LCD12x3B:                
//        case LCD12x3C:                        
        case LCD12x4:        
        case LCD12x4B:                
            return 12;        

        case LCD16x1:        
        case LCD16x2:
        case LCD16x2B:
//        case LCD16x3:                        
        case LCD16x4:        
            return 16;
            
//        case LCD20x1:
        case LCD20x2:
        case LCD20x4:
            return 20;

        case LCD24x1:
        case LCD24x2:
//        case LCD24x3B:                
        case LCD24x4B:        
            return 24;        

//        case LCD40x1:        
        case LCD40x2:
        case LCD40x4:
            return 40;        
        
// Should never get here.
        default:
            return 0;
    }
#endif    
}

/** Return the number of rows
  *
  * @param return  The number of rows
  *
  * Note: some configurations are commented out because they have not yet been tested due to lack of hardware     
  */
int TextLCD_Base::rows() {

  // Rows encoded in b15..b8  
  //return ((_type >> 8) & 0xFF); 
  return _nr_rows;          
    
#if(0)    
    switch (_type) {
        case LCD8x1: 
        case LCD16x1:         
//        case LCD20x1:                 
        case LCD24x1:                 
//        case LCD40x1:                         
            return 1;           

        case LCD8x2:  
        case LCD8x2B:                        
        case LCD12x2:                      
        case LCD16x2:
        case LCD16x2B:
        case LCD20x2:
        case LCD24x2:        
        case LCD40x2:                
            return 2;

        case LCD12x3B:                
//        case LCD12x3C:                        
//        case LCD16x3:                
//        case LCD24x3B:                
            return 3;
                    
        case LCD12x4:        
        case LCD12x4B:                
        case LCD16x4:
        case LCD20x4:
        case LCD24x4B:        
        case LCD40x4:
            return 4;

// Should never get here.      
        default:
            return 0;        
    }
#endif    
}

/** Set the Cursormode
  *
  * @param cursorMode  The Cursor mode (CurOff_BlkOff, CurOn_BlkOff, CurOff_BlkOn, CurOn_BlkOn)
  */
void TextLCD_Base::setCursor(LCDCursor cursorMode) { 

  // Save new cursor mode, needed when 2 controllers are in use or when display is switched off/on
  _currentCursor = cursorMode;
    
  // Configure only current LCD controller
  _setCursorAndDisplayMode(_currentMode, _currentCursor);
    
}

/** Set the Displaymode
  *
  * @param displayMode The Display mode (DispOff, DispOn)
  */
void TextLCD_Base::setMode(LCDMode displayMode) { 

  // Save new displayMode, needed when 2 controllers are in use or when cursor is changed
  _currentMode = displayMode;
    
  // Select and configure second LCD controller when needed
  if(_type==LCD40x4) {
    if (_ctrl_idx==_LCDCtrl_0) {      
      // Configure primary LCD controller
      _setCursorAndDisplayMode(_currentMode, _currentCursor);

      // Select 2nd controller
      _ctrl_idx=_LCDCtrl_1;
  
      // Configure secondary LCD controller    
      _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);

      // Restore current controller
      _ctrl_idx=_LCDCtrl_0;       
    }
    else {
      // Select primary controller
      _ctrl_idx=_LCDCtrl_0;
    
      // Configure primary LCD controller
      _setCursorAndDisplayMode(_currentMode, CurOff_BlkOff);
       
      // Restore current controller
      _ctrl_idx=_LCDCtrl_1;

      // Configure secondary LCD controller    
      _setCursorAndDisplayMode(_currentMode, _currentCursor);
    }
  }
  else {
    // Configure primary LCD controller
    _setCursorAndDisplayMode(_currentMode, _currentCursor);
  }       
}


/** Low level method to restore the cursortype and display mode for current controller
  */     
void TextLCD_Base::_setCursorAndDisplayMode(LCDMode displayMode, LCDCursor cursorType) { 
    
    // Configure current LCD controller       
    _writeCommand(0x08 | displayMode | cursorType);
}

/** Set the Backlight mode
  *
  *  @param backlightMode The Backlight mode (LightOff, LightOn)
  */
void TextLCD_Base::setBacklight(LCDBacklight backlightMode) {

    if (backlightMode == LightOn) {
      this->_setBL(true);
    }
    else {
      this->_setBL(false);    
    }
} 

/** Set User Defined Characters
  *
  * @param unsigned char c   The Index of the UDC (0..7)
  * @param char *udc_data    The bitpatterns for the UDC (8 bytes of 5 significant bits)     
  */
void TextLCD_Base::setUDC(unsigned char c, char *udc_data) {
  
  // Select and configure second LCD controller when needed
  if(_type==LCD40x4) {
    _LCDCtrl_Idx current_ctrl_idx = _ctrl_idx; // Temp save current controller
   
    // Select primary controller     
    _ctrl_idx=_LCDCtrl_0;
    
    // Configure primary LCD controller
    _setUDC(c, udc_data);

    // Select 2nd controller
    _ctrl_idx=_LCDCtrl_1;
  
    // Configure secondary LCD controller    
    _setUDC(c, udc_data);

    // Restore current controller
    _ctrl_idx=current_ctrl_idx;       
  }
  else {
    // Configure primary LCD controller
    _setUDC(c, udc_data); 
  }
    
}

/** Low level method to store user defined characters for current controller
  */     
void TextLCD_Base::_setUDC(unsigned char c, char *udc_data) {
  
  // Select CG RAM for current LCD controller
  _writeCommand(0x40 + ((c & 0x07) << 3)); //Set CG-RAM address,
                                           //8 sequential locations needed per UDC
  // Store UDC pattern 
  for (int i=0; i<8; i++) {
    _writeData(*udc_data++);
  }
   
  //Select DD RAM again for current LCD controller
  int addr = getAddress(_column, _row);
  _writeCommand(0x80 | addr);  
}

//--------- End TextLCD_Base -----------


//--------- Start TextLCD Bus -----------

/* Create a TextLCD interface for using regular mbed pins
 *
 * @param rs     Instruction/data control line
 * @param e      Enable line (clock)
 * @param d4-d7  Data lines for using as a 4-bit interface
 * @param type   Sets the panel size/addressing mode (default = LCD16x2)
 * @param bl     Backlight control line (optional, default = NC)  
 * @param e2     Enable2 line (clock for second controller, LCD40x4 only) 
 * @param ctrl   LCD controller (default = HD44780)   
 */ 
TextLCD::TextLCD(PinName rs, PinName e,
                 PinName d4, PinName d5, PinName d6, PinName d7,
                 LCDType type, PinName bl, PinName e2, LCDCtrl ctrl) :
                 TextLCD_Base(type, ctrl), 
                 _rs(rs), _e(e), _d(d4, d5, d6, d7) {

  // The hardware Backlight pin is optional. Test and make sure whether it exists or not to prevent illegal access.
  if (bl != NC) {
    _bl = new DigitalOut(bl);   //Construct new pin 
    _bl->write(0);              //Deactivate    
  }
  else {
    // No Hardware Backlight pin       
    _bl = NULL;                 //Construct dummy pin     
  }  

  // The hardware Enable2 pin is only needed for LCD40x4. Test and make sure whether it exists or not to prevent illegal access.
  if (e2 != NC) {
    _e2 = new DigitalOut(e2);   //Construct new pin 
    _e2->write(0);              //Deactivate    
  }
  else {
    // No Hardware Enable pin       
    _e2 = NULL;                 //Construct dummy pin     
  }  
                                                                           
  _init();
}

/** Destruct a TextLCD interface for using regular mbed pins
  *
  * @param  none
  * @return none
  */ 
TextLCD::~TextLCD() {
   if (_bl != NULL) {delete _bl;}  // BL pin
   if (_e2 != NULL) {delete _e2;}  // E2 pin
}


/** Set E pin (or E2 pin)
  * Used for mbed pins, I2C bus expander or SPI shiftregister
  * Default PinName value for E2 is NC, must be used as pointer to avoid issues with mbed lib and DigitalOut pins
  *   @param  value true or false
  *   @return none 
  */
void TextLCD::_setEnable(bool value) {

  if(_ctrl_idx==_LCDCtrl_0) {
    if (value) {
      _e  = 1;    // Set E bit 
    }  
    else { 
      _e  = 0;    // Reset E bit  
    }  
  }    
  else { 
    if (value) {
      if (_e2 != NULL) {_e2->write(1);}  //Set E2 bit
    }  
    else { 
      if (_e2 != NULL) {_e2->write(0);}  //Reset E2 bit     
    }  
  }    
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD::_setRS(bool value) {

  if (value) {
    _rs  = 1;    // Set RS bit 
  }  
  else  {
    _rs  = 0;    // Reset RS bit 
  }  
}    

/** Set BL pin
  * Used for mbed pins, I2C bus expander or SPI shiftregister
  * Default PinName value is NC, must be used as pointer to avoid issues with mbed lib and DigitalOut pins
  *   @param  value true or false
  *   @return none  
  */
void TextLCD::_setBL(bool value) {

  if (value) {
    if (_bl != NULL) {_bl->write(1);}  //Set BL bit
  }  
  else { 
    if (_bl != NULL) {_bl->write(0);}  //Reset BL bit  
  }  

}    

// Place the 4bit data on the databus
// Used for mbed pins, I2C bus expander or SPI shifregister
void TextLCD::_setData(int value) {
  _d = value & 0x0F;   // Write Databits 
}    
    
//----------- End TextLCD ---------------


//--------- Start TextLCD_I2C -----------

/** Create a TextLCD interface using an I2C PC8574 (or PCF8574A) or MCP23008 portexpander
  *
  * @param i2c             I2C Bus
  * @param deviceAddress   I2C slave address (PCF8574, PCF8574A or MCP23008, default = 0x40)
  * @param type            Sets the panel size/addressing mode (default = LCD16x2)
  * @param ctrl            LCD controller (default = HD44780)    
  */
TextLCD_I2C::TextLCD_I2C(I2C *i2c, char deviceAddress, LCDType type, LCDCtrl ctrl) :
                         TextLCD_Base(type, ctrl), 
                         _i2c(i2c){
                              
  _slaveAddress = deviceAddress & 0xFE;

  // Setup the I2C bus
  // The max bitrate for PCF8574 is 100kbit, the max bitrate for MCP23008 is 400kbit, 
//  _i2c->frequency(100000);
  
#if (MCP23008==1)
  // MCP23008 portexpander Init
  _write_register(IODIR,   0x00);  // All outputs
  _write_register(IPOL,    0x00);  // No reverse polarity 
  _write_register(GPINTEN, 0x00);  // No interrupt 
  _write_register(DEFVAL,  0x00);  // Default value to compare against for interrupts
  _write_register(INTCON,  0x00);  // No interrupt on changes 
  _write_register(IOCON,   0x00);  // Interrupt polarity   
  _write_register(GPPU,    0x00);  // No Pullup 
  _write_register(INTF,    0x00);  //    
  _write_register(INTCAP,  0x00);  //    
  _write_register(GPIO,    0x00);  // Output/Input pins   
  _write_register(OLAT,    0x00);  // Output Latch  
    
  // Init the portexpander bus
  _lcd_bus = D_LCD_BUS_DEF;
  
  // write the new data to the portexpander
  _write_register(GPIO, _lcd_bus);      
#else
  // PCF8574 of PCF8574A portexpander

  // Init the portexpander bus
  _lcd_bus = D_LCD_BUS_DEF;

  // write the new data to the portexpander
  _i2c->write(_slaveAddress, &_lcd_bus, 1);    
#endif

  _init();    
}

// Set E pin (or E2 pin)
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_I2C::_setEnable(bool value) {

  if(_ctrl_idx==_LCDCtrl_0) {
    if (value) {
      _lcd_bus |= D_LCD_E;     // Set E bit 
    }  
    else {                    
      _lcd_bus &= ~D_LCD_E;    // Reset E bit                     
    }  
  }
  else {
    if (value) {
      _lcd_bus |= D_LCD_E2;    // Set E2 bit 
    }  
    else {
      _lcd_bus &= ~D_LCD_E2;   // Reset E2bit                     
    }  
  }    

#if (MCP23008==1)
  // MCP23008 portexpander
  
  // write the new data to the portexpander
  _write_register(GPIO, _lcd_bus);      
#else
  // PCF8574 of PCF8574A portexpander

  // write the new data to the I2C portexpander
  _i2c->write(_slaveAddress, &_lcd_bus, 1);    
#endif
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_I2C::_setRS(bool value) {

  if (value) {
    _lcd_bus |= D_LCD_RS;    // Set RS bit 
  }  
  else {                    
    _lcd_bus &= ~D_LCD_RS;   // Reset RS bit                     
  }

#if (MCP23008==1)
  // MCP23008 portexpander
  
  // write the new data to the portexpander
  _write_register(GPIO, _lcd_bus);      
#else
  // PCF8574 of PCF8574A portexpander

  // write the new data to the I2C portexpander
  _i2c->write(_slaveAddress, &_lcd_bus, 1);    
#endif                  
}    

// Set BL pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_I2C::_setBL(bool value) {

  if (value) {
    _lcd_bus |= D_LCD_BL;    // Set BL bit 
  }  
  else {                    
    _lcd_bus &= ~D_LCD_BL;   // Reset BL bit                     
  }
  
#if (MCP23008==1)
  // MCP23008 portexpander
  
  // write the new data to the portexpander
  _write_register(GPIO, _lcd_bus);      
#else
  // PCF8574 of PCF8574A portexpander

  // write the new data to the I2C portexpander
  _i2c->write(_slaveAddress, &_lcd_bus, 1);    
#endif                 
}    


// Place the 4bit data on the databus
// Used for mbed pins, I2C bus expander or SPI shifregister
void TextLCD_I2C::_setData(int value) {
  int data;

  // Set bit by bit to support any mapping of expander portpins to LCD pins
  
  data = value & 0x0F;
  if (data & 0x01){
    _lcd_bus |= D_LCD_D4;   // Set Databit 
  }  
  else { 
    _lcd_bus &= ~D_LCD_D4;  // Reset Databit
  }  

  if (data & 0x02){
    _lcd_bus |= D_LCD_D5;   // Set Databit 
  }  
  else {
    _lcd_bus &= ~D_LCD_D5;  // Reset Databit
  }  

  if (data & 0x04) {
    _lcd_bus |= D_LCD_D6;   // Set Databit 
  }  
  else {                    
    _lcd_bus &= ~D_LCD_D6;  // Reset Databit
  }  

  if (data & 0x08) {
    _lcd_bus |= D_LCD_D7;   // Set Databit 
  }  
  else {
    _lcd_bus &= ~D_LCD_D7;  // Reset Databit
  }  
                    
#if (MCP23008==1)
  // MCP23008 portexpander
  
  // write the new data to the portexpander
  _write_register(GPIO, _lcd_bus);      
#else
  // PCF8574 of PCF8574A portexpander

  // write the new data to the I2C portexpander
  _i2c->write(_slaveAddress, &_lcd_bus, 1);    
#endif
                 
}    

// Write data to MCP23008 I2C portexpander
void TextLCD_I2C::_write_register (int reg, int value) {
  char data[] = {reg, value};
    
  _i2c->write(_slaveAddress, data, 2); 
}

//---------- End TextLCD_I2C ------------


//--------- Start TextLCD_I2C_N ---------

 /** Create a TextLCD interface using a controller with native I2C interface
   *
   * @param i2c             I2C Bus
   * @param deviceAddress   I2C slave address (default = 0x7C)  
   * @param type            Sets the panel size/addressing mode (default = LCD16x2)
   * @param bl              Backlight control line (optional, default = NC)     
   * @param ctrl            LCD controller (default = ST7032_3V3)                     
   */
TextLCD_I2C_N::TextLCD_I2C_N(I2C *i2c, char deviceAddress, LCDType type, PinName bl, LCDCtrl ctrl) : 
                               TextLCD_Base(type, ctrl), 
                               _i2c(i2c){
  
  _slaveAddress = deviceAddress & 0xFE;
  
  // Setup the I2C bus
  // The max bitrate for ST7032i is 400kbit, lets stick to default here
  _i2c->frequency(100000);
//  _i2c->frequency(50000);
       
  // The hardware Backlight pin is optional. Test and make sure whether it exists or not to prevent illegal access.
  if (bl != NC) {
    _bl = new DigitalOut(bl);   //Construct new pin 
    _bl->write(0);              //Deactivate    
  }
  else {
    // No Hardware Backlight pin       
    _bl = NULL;                 //Construct dummy pin     
  }  
  
#if(0)
  //Sanity check
  switch (_ctrl) {
    case ST7032_3V3:
    case ST7032_5V:    
    case PCF21XX_3V3:    
//    case PCF21XX_5V:        
      _init();
      break;
    
    default: 
      error("Error: LCD Controller type does not support native I2C interface\n\r"); 
  }  
#endif

  //Sanity check
  if (_ctrl & LCD_C_I2C) {
    _init();      
  }
  else {
    error("Error: LCD Controller type does not support native I2C interface\n\r");           
  }

}

TextLCD_I2C_N::~TextLCD_I2C_N() {
   if (_bl != NULL) {delete _bl;}  // BL pin
}

// Not used in this mode
void TextLCD_I2C_N::_setEnable(bool value) {
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister and native I2C or SPI
void TextLCD_I2C_N::_setRS(bool value) {
// The controlbyte defines the meaning of the next byte. This next byte can either be data or command.
// Start Slaveaddress+RW  b7 b6 b5 b4 b3 b2 b1 b0   b7...........b0  Stop
//                        Co RS RW  0  0  0  0  0   command or data
//
//   C0=1 indicates that another controlbyte will follow after the next data or command byte 
//   RS=1 means that next byte is data, RS=0 means that next byte is command
//   RW=0 means write to controller. RW=1 means that controller will be read from after the next command. 
//        Many native I2C controllers dont support this option and it is not used by this lib. 
//

  if (value) {
    _controlbyte = 0x40; // Next byte is data, No more control bytes will follow
  }
  else {
    _controlbyte = 0x00; // Next byte is command, No more control bytes will follow     
  }
}    

// Set BL pin
void TextLCD_I2C_N::_setBL(bool value) {
    if (_bl) {
        _bl->write(value);   
    }    
}    
    
// Not used in this mode
void TextLCD_I2C_N::_setData(int value) {
}    

// Write a byte using I2C
void TextLCD_I2C_N::_writeByte(int value) {
// The controlbyte defines the meaning of the next byte. This next byte can either be data or command.
// Start Slaveaddress+RW  b7 b6 b5 b4 b3 b2 b1 b0   b7...........b0  Stop
//                        Co RS RW  0  0  0  0  0   command or data
//
//   C0=1 indicates that another controlbyte will follow after the next data or command byte 
//   RS=1 means that next byte is data, RS=0 means that next byte is command
//   RW=0 means write to controller. RW=1 means that controller will be read from after the next command. 
//        Many native I2C controllers dont support this option and it is not used by this lib. 
//
  char data[] = {_controlbyte, value};
    
  _i2c->write(_slaveAddress, data, 2); 
}

//-------- End TextLCD_I2C_N ------------


//--------- Start TextLCD_SPI -----------

 /** Create a TextLCD interface using an SPI 74595 portexpander
   *
   * @param spi             SPI Bus
   * @param cs              chip select pin (active low)
   * @param type            Sets the panel size/addressing mode (default = LCD16x2)
   * @param ctrl            LCD controller (default = HD44780)      
   */
TextLCD_SPI::TextLCD_SPI(SPI *spi, PinName cs, LCDType type, LCDCtrl ctrl) :
                         TextLCD_Base(type, ctrl), 
                         _spi(spi),        
                         _cs(cs) {      
        
  // Setup the spi for 8 bit data, low steady state clock,
  // rising edge capture, with a 500KHz or 1MHz clock rate  
  _spi->format(8,0);
  _spi->frequency(500000);    
  //_spi.frequency(1000000);    

  // Init the portexpander bus
  _lcd_bus = D_LCD_BUS_DEF;
  
  // write the new data to the portexpander
  _setCS(false);  
  _spi->write(_lcd_bus);   
  _setCS(true);  

  _init();   
}

// Set E pin (or E2 pin)
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI::_setEnable(bool value) {

  if(_ctrl_idx==_LCDCtrl_0) {
    if (value) {
      _lcd_bus |= D_LCD_E;     // Set E bit 
    }  
    else {                    
      _lcd_bus &= ~D_LCD_E;    // Reset E bit                     
    }  
  }
  else {
    if (value) {
      _lcd_bus |= D_LCD_E2;    // Set E2 bit 
    }  
    else {
      _lcd_bus &= ~D_LCD_E2;   // Reset E2 bit                     
    }  
  }
                  
  // write the new data to the SPI portexpander
  _setCS(false);  
  _spi->write(_lcd_bus);   
  _setCS(true);    
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI::_setRS(bool value) {

  if (value) {
    _lcd_bus |= D_LCD_RS;    // Set RS bit 
  }  
  else {                    
    _lcd_bus &= ~D_LCD_RS;   // Reset RS bit                     
  }
     
  // write the new data to the SPI portexpander
  _setCS(false);  
  _spi->write(_lcd_bus);   
  _setCS(true);     
}    

// Set BL pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI::_setBL(bool value) {

  if (value) {
    _lcd_bus |= D_LCD_BL;    // Set BL bit 
  }  
  else {
    _lcd_bus &= ~D_LCD_BL;   // Reset BL bit                     
  }
      
  // write the new data to the SPI portexpander
  _setCS(false);  
  _spi->write(_lcd_bus);   
  _setCS(true);      
}    

// Place the 4bit data on the databus
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI::_setData(int value) {
  int data;

  // Set bit by bit to support any mapping of expander portpins to LCD pins
    
  data = value & 0x0F;
  if (data & 0x01) {
    _lcd_bus |= D_LCD_D4;   // Set Databit 
  }  
  else {                    
    _lcd_bus &= ~D_LCD_D4;  // Reset Databit                     
  }
  
  if (data & 0x02) {
    _lcd_bus |= D_LCD_D5;   // Set Databit 
  }  
  else {
    _lcd_bus &= ~D_LCD_D5;  // Reset Databit                     
  }
  
  if (data & 0x04) {
    _lcd_bus |= D_LCD_D6;   // Set Databit 
  }  
  else {
    _lcd_bus &= ~D_LCD_D6;  // Reset Databit                     
  }
  
  if (data & 0x08) {
    _lcd_bus |= D_LCD_D7;   // Set Databit 
  }  
  else {
    _lcd_bus &= ~D_LCD_D7;  // Reset Databit
  }  
                    
  // write the new data to the SPI portexpander
  _setCS(false);  
  _spi->write(_lcd_bus);   
  _setCS(true);          
}    

// Set CS line.
// Only used for SPI bus
void TextLCD_SPI::_setCS(bool value) {

  if (value) {   
    _cs  = 1;    // Set CS pin 
  }  
  else {
    _cs  = 0;    // Reset CS pin 
  }
}

//---------- End TextLCD_SPI ------------


//--------- Start TextLCD_SPI_N ---------

 /** Create a TextLCD interface using a controller with a native SPI4 interface
   *
   * @param spi             SPI Bus
   * @param cs              chip select pin (active low)
   * @param rs              Instruction/data control line
   * @param type            Sets the panel size/addressing mode (default = LCD16x2)
   * @param bl              Backlight control line (optional, default = NC)  
   * @param ctrl            LCD controller (default = ST7032_3V3) 
   */       
TextLCD_SPI_N::TextLCD_SPI_N(SPI *spi, PinName cs, PinName rs, LCDType type, PinName bl, LCDCtrl ctrl) :
                             TextLCD_Base(type, ctrl), 
                             _spi(spi),        
                             _cs(cs),
                             _rs(rs) {      
        
  // Setup the spi for 8 bit data, low steady state clock,
  // rising edge capture, with a 500KHz or 1MHz clock rate  
  _spi->format(8,0);
  _spi->frequency(1000000);    
  
  // The hardware Backlight pin is optional. Test and make sure whether it exists or not to prevent illegal access.
  if (bl != NC) {
    _bl = new DigitalOut(bl);   //Construct new pin 
    _bl->write(0);              //Deactivate    
  }
  else {
    // No Hardware Backlight pin       
    _bl = NULL;                 //Construct dummy pin     
  }  

#if(0)
  //Sanity check
  switch (_ctrl) {
    case ST7032_3V3:
    case ST7032_5V:    
    case WS0010:        
      _init();
      break;
    
    default: 
      error("Error: LCD Controller type does not support native SPI4 interface\n\r"); 
  }     
#endif

  //Sanity check
  if (_ctrl & LCD_C_SPI4) {
    _init();      
  }
  else {
    error("Error: LCD Controller type does not support native SPI4 interface\n\r");           
  }
}

TextLCD_SPI_N::~TextLCD_SPI_N() {
   if (_bl != NULL) {delete _bl;}  // BL pin
}

// Not used in this mode
void TextLCD_SPI_N::_setEnable(bool value) {
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI_N::_setRS(bool value) {
    _rs = value;
}    

// Set BL pin
void TextLCD_SPI_N::_setBL(bool value) {
    if (_bl) {
        _bl->write(value);   
    }    
}    

// Not used in this mode
void TextLCD_SPI_N::_setData(int value) {
}    

// Write a byte using SPI
void TextLCD_SPI_N::_writeByte(int value) {
    _cs = 0;
    wait_us(1);
    _spi->write(value);
    wait_us(1);
    _cs = 1;
}
  
//-------- End TextLCD_SPI_N ------------



#if(0)
//Code checked out on logic analyser. Not yet tested on hardware..

//-------- Start TextLCD_SPI_N_3_9 --------

 /** Create a TextLCD interface using a controller with a native SPI3 9 bits interface
   *
   * @param spi             SPI Bus
   * @param cs              chip select pin (active low)
   * @param type            Sets the panel size/addressing mode (default = LCD16x2)
   * @param bl              Backlight control line (optional, default = NC)  
   * @param ctrl            LCD controller (default = AIP31068) 
   */       
TextLCD_SPI_N_3_9::TextLCD_SPI_N_3_9(SPI *spi, PinName cs, LCDType type, PinName bl, LCDCtrl ctrl) :
                                     TextLCD_Base(type, ctrl), 
                                     _spi(spi),        
                                     _cs(cs) {      
        
  // Setup the spi for 9 bit data, low steady state clock,
  // rising edge capture, with a 500KHz or 1MHz clock rate  
  _spi->format(9,0);
  _spi->frequency(1000000);    
  
  // The hardware Backlight pin is optional. Test and make sure whether it exists or not to prevent illegal access.
  if (bl != NC) {
    _bl = new DigitalOut(bl);   //Construct new pin 
    _bl->write(0);              //Deactivate    
  }
  else {
    // No Hardware Backlight pin       
    _bl = NULL;                 //Construct dummy pin     
  }  

  //Sanity check
  if (_ctrl & LCD_C_SPI3_9) {
    _init();      
  }
  else {
    error("Error: LCD Controller type does not support native SPI3 9 bits interface\n\r");           
  }
}

TextLCD_SPI_N_3_9::~TextLCD_SPI_N_3_9() {
   if (_bl != NULL) {delete _bl;}  // BL pin
}

// Not used in this mode
void TextLCD_SPI_N_3_9::_setEnable(bool value) {
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI_N_3_9::_setRS(bool value) {
// The controlbits define the meaning of the next byte. This next byte can either be data or command.
//   b8  b7...........b0 
//   RS  command or data
//
//   RS=1 means that next byte is data, RS=0 means that next byte is command
//

  if (value) {
    _controlbyte = 0x01; // Next byte is data
  }
  else {
    _controlbyte = 0x00; // Next byte is command
  }
   
}    

// Set BL pin
void TextLCD_SPI_N_3_9::_setBL(bool value) {
    if (_bl) {
        _bl->write(value);   
    }    
}    

// Not used in this mode
void TextLCD_SPI_N_3_9::_setData(int value) {
}    

// Write a byte using SPI3 9 bits mode
void TextLCD_SPI_N_3_9::_writeByte(int value) {
    _cs = 0;
    wait_us(1);
    _spi->write( (_controlbyte << 8) | (value & 0xFF));
    wait_us(1);
    _cs = 1;
}
  
//------- End TextLCD_SPI_N_3_9 -----------
#endif


#if(0)
//Code checked out on logic analyser. Not yet tested on hardware..

//------- Start TextLCD_SPI_N_3_10 --------

 /** Create a TextLCD interface using a controller with a native SPI3 10 bits interface
   *
   * @param spi             SPI Bus
   * @param cs              chip select pin (active low)
   * @param type            Sets the panel size/addressing mode (default = LCD16x2)
   * @param bl              Backlight control line (optional, default = NC)  
   * @param ctrl            LCD controller (default = AIP31068) 
   */       
TextLCD_SPI_N_3_10::TextLCD_SPI_N_3_10(SPI *spi, PinName cs, LCDType type, PinName bl, LCDCtrl ctrl) :
                                       TextLCD_Base(type, ctrl), 
                                       _spi(spi),        
                                       _cs(cs) {      
        
  // Setup the spi for 10 bit data, low steady state clock,
  // rising edge capture, with a 500KHz or 1MHz clock rate  
  _spi->format(10,0);
  _spi->frequency(1000000);    
  
  // The hardware Backlight pin is optional. Test and make sure whether it exists or not to prevent illegal access.
  if (bl != NC) {
    _bl = new DigitalOut(bl);   //Construct new pin 
    _bl->write(0);              //Deactivate    
  }
  else {
    // No Hardware Backlight pin       
    _bl = NULL;                 //Construct dummy pin     
  }  

  //Sanity check
  if (_ctrl & LCD_C_SPI3_10) {
    _init();      
  }
  else {
    error("Error: LCD Controller type does not support native SPI3 10 bits interface\n\r");           
  }
}

TextLCD_SPI_N_3_10::~TextLCD_SPI_N_3_10() {
   if (_bl != NULL) {delete _bl;}  // BL pin
}

// Not used in this mode
void TextLCD_SPI_N_3_10::_setEnable(bool value) {
}    

// Set RS pin
// Used for mbed pins, I2C bus expander or SPI shiftregister
void TextLCD_SPI_N_3_10::_setRS(bool value) {
// The controlbits define the meaning of the next byte. This next byte can either be data or command.
//   b9 b8  b7...........b0 
//   RS RW  command or data
//
//   RS=1 means that next byte is data, RS=0 means that next byte is command
//   RW=0 means that next byte is writen, RW=1 means that next byte is read (not used in this lib)
//

  if (value) {
    _controlbyte = 0x02; // Next byte is data
  }
  else {
    _controlbyte = 0x00; // Next byte is command
  }
   
}    

// Set BL pin
void TextLCD_SPI_N_3_10::_setBL(bool value) {
    if (_bl) {
        _bl->write(value);   
    }    
}    

// Not used in this mode
void TextLCD_SPI_N_3_10::_setData(int value) {
}    

// Write a byte using SPI3 10 bits mode
void TextLCD_SPI_N_3_10::_writeByte(int value) {
    _cs = 0;
    wait_us(1);
    _spi->write( (_controlbyte << 8) | (value & 0xFF));
    wait_us(1);
    _cs = 1;
}
  
//------- End TextLCD_SPI_N_3_10 ----------
#endif