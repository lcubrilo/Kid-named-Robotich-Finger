#include <LiquidCrystal.h>
const int rs=D1, en=D2, d4=D4, d5=D5, d6=D6, d7=D7;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7); //select the pins used on the LCD panel

int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
// read the buttons
int read_LCD_buttons()
{
    adc_key_in = analogRead(0);      // read the value from the sensor 
    // my buttons when read are centered at these valies: 18, 236, 522, 802
    // that's roughly whole-number multiple of 256
    // rounded to closest odd multiple of 128 
    if (adc_key_in > 1408)
        return adc_key_in; // We make this the 1st option for speed reasons since it will be the most likely result
    
    if (adc_key_in < 128)
        return btnRIGHT;
    
    if (adc_key_in < 384)
        return btnUP;
    
    if (adc_key_in < 640)
        return btnDOWN; 
    
    if (adc_key_in < 896)
        return btnLEFT;
    
    if (adc_key_in < 1152) //never detected, max is 1024
        return btnSELECT;   
    
    return btnNONE;  // when all others fail, return this...
}

void setup(){
    lcd.begin(16, 2);
    lcd.clear();  
}
 
void loop()
{
    lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
    lcd.print(millis()/1000);      // display seconds elapsed since power-up


    lcd.setCursor(0,1);            // move to the begining of the second line
    lcd_key = read_LCD_buttons();  // read the buttons

    switch (lcd_key)               // depending on which button was pushed, we perform an action
    {
        case btnRIGHT:{
            lcd.print("RIGHT ");
            break;
        }
        case btnLEFT:{
            lcd.print("LEFT   ");
            break;
        }
        case btnUP:{
            lcd.print("UP    ");
            break;
        }
        case btnDOWN:{
            lcd.print("DOWN  ");
            break;
        }
        case btnSELECT:{
            lcd.print("SELECT");
            break;
        }
        case btnNONE:{
            lcd.print("NONE  ");
            break;
        }
        default:
        {
            lcd.print(lcd_key);
        }
    }
}

 