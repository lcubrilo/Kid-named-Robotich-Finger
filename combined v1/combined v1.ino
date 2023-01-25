#include <LiquidCrystal.h>
const int rs=D1, en=D2, d4=D4, d5=D5, d6=D6, d7=D7;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7); //select the pins used on the LCD panel

int lcd_key     = 0;
int adc_key_in  = 0;

#define btnNEXT   0
#define btnUP     1
#define btnDOWN   2
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
        return btnNEXT;
    
    if (adc_key_in < 384)
        return btnUP;
    
    if (adc_key_in < 640)
        return btnDOWN; 
    
    if (adc_key_in < 896)
        return btnNONE;  // when all others fail, return this...
}

void LCDsetup(){
    lcd.begin(16, 2);
    lcd.clear();  
}


//////////////////////////////////////////////////

#include <Servo.h>
Servo motoric;

#include <ESP8266WiFi.h>
WiFiClient client;
const char *ssid = "HUAWEI P10 lite";
const char *password = "knin2022";

void WIFISERVOsetup(){
    // Opening the serial connection
    Serial.begin(9600);
    
    // Connecting to the servo at pin 15
    motoric.attach(15);

    /* Connecting to the WiFi
    WiFiClient client; int retries = 0;
    while ((WiFi.status() != WL_CONNECTED)) {
        retries++;
        delay(1000);
        Serial.print(".");
        if (retries > 14) {
          Serial.println(F("WiFi connection FAILED"));
          break;
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(F("WiFi connected!"));
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
    }*/
}

////////////////////////////////////////////////////////////

void setup()
{
    LCDsetup();
    WIFISERVOsetup();
}

int servoPosition = 90;
int servoMax = 180;
int servoMin = 0;

void loop()
{
    
    lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
    lcd.print(millis()/1000);      // display seconds elapsed since power-up


    lcd.setCursor(0,1);            // move to the begining of the second line
    lcd_key = read_LCD_buttons();  // read the buttons

    switch (lcd_key)               // depending on which button was pushed, we perform an action
    {
        case btnNEXT:{
            lcd.print("DALJE ");
            //delay(1000);
            break;
        }
        case btnUP:{
            lcd.print("GORE  ");
            if (servoPosition < servoMax)
                servoPosition += 5;
            motoric.write(180);
            //delay(1000);
            break;
        }
        case btnDOWN:{
            lcd.print("DOLE  ");
            if (servoPosition > servoMin)
                servoPosition -= 5;
            motoric.write(0);
            //delay(1000);
            break;
        }
        case btnNONE:{
            lcd.print("NISTA ");
            break;
        }
        default:
        {
            lcd.print(lcd_key);
        }
    }
}


 