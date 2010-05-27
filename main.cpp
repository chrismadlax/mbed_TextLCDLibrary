// Write to all HD44780 TextLCD RAM locations, sford
//
// A quick hack to write to all the display RAM locations
// in an HD44780 display, to identify which location maps
// to which character.
//
// Instructions:
//  - Change TextLCD pinout as appropriate so it works
//  - Run, and it should fill the screen with characters
//  - Identify what characters are at the start of each row
//
// To determine what address each line starts at, you subtract the 
// ascii code for '0'(48) from the character at the start of each line
//  - see http://www.asciitable.com/
// 
// e.g.
//   +----------------+
//   |0123456789:;<=>?| first char = '0' (48)
//   |XYZ....         | first char = 'X' (88)
//   +----------------+
//  
// So in this case, the RAM offsets are 0 and 40

#include "mbed.h"
#include "TextLCD.h"
DigitalOut zero(p11);
TextLCD lcd(p10, p12, p15, p16, p29, p30, TextLCD::LCD16x2B);

int main() {
    lcd.printf("hithere");
    wait(1);
    lcd.putc('i');
    wait(1);
    lcd.cls();
    wait(1);
    lcd.locate(4, 1);    
    wait(1);
    lcd.printf("jdlkjfksj");
    for(int i=0; i<30; i++) {
        lcd.putc('A' + i);
        wait(0.3);
    }
}
