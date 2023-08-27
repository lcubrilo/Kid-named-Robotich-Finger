//------------------------------| Configuration and Initialization |------------------------------
#pragma region 

#include "config.h"

#include <LiquidCrystal.h>
const int rs=D1, en=D2, d4=D4, d5=D5, d6=D6, d7=D7;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7); //select the pins used on the LCD panel

#define btnNEXT   1
#define btnUP     3
#define btnDOWN   5
#define btnNONE   7

int lcd_key     = 0;
int previous_operation = btnNEXT;
int adc_key_in  = 0;

#include <Servo.h>
Servo motoric;

#include <ESP8266WiFi.h>
WiFiClient client;

#pragma endregion

//------------------------------| Main Application Flow |------------------------------
#pragma region 
void setup()
{
    Serial.begin(9600);

    lcd.begin(16, 2);
    lcd.clear();  
    
    motoric.attach(0, 500, 2400);

    // loading animation + servo wiggle/unstuck
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("LOADING");
    lcd.setCursor(0,1);
    lcd.print("[*");

    motoric.write(IDLE_POSITION);
    for(int i = 1; i < 4; i++)
    {
        motoric.write(PUSH_BUTTON);
        lcd.print("**");
        delay(600);
        motoric.write(IDLE_POSITION);
        lcd.print("**");
        delay(600);
    }
    lcd.print("*]");
    delay(500);
    clearLCD();
    delay(200);
}

int curr_cycle = -1;
int num_cycles = DEFAULT_NUM_CYCLES;
int dur_cycle = DEFAULT_DUR_CYCLES;
int time_since_last_click;
int time_since_last_blink = 0;

void loop()
{
    if (curr_cycle <= 0)
        userMenu(); // User selects number of cycles and length of cycles
    else
        clickingProcedure(); // User setup everything, running the clicking operations

    lcd_key = read_LCD_buttons(); // read input from the buttons
    handleButtonEvents(lcd_key);
}
#pragma endregion

//------------------------------| Shield LCD |------------------------------
#pragma region 

void clearLCD()
{
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
}

void showRemainingTime()
{
    lcd.setCursor(0,1);
    lcd.print("Tap soon ->");

    String output = formatTime_old();
    int n = output.length();
    lcd.setCursor(16-n, 1);
    lcd.print(output);
}

String formatTime_old()
{
    int time_elapse = (millis() - time_since_last_click)/1000;
    int seconds = dur_cycle*60 - time_elapse;
    int minutes = seconds/60; 

    if (seconds < 99)
        return "  " + String(seconds) + "s";

    seconds %= 60;
    String temp = String(seconds);
    if (seconds<10)
        temp = "0" + temp;
      
    temp = String(minutes) + ":" + temp;
    if(minutes<10)
      temp = "0" + temp;

    return temp;
}

void blinkCorner(){
    if(millis()-time_since_last_blink > 1500)
    {
        time_since_last_blink = millis();
        lcd.setCursor(15, 1);
        lcd.print(".");
    }
    else if (millis()-time_since_last_blink > 750)
    {
      lcd.setCursor(15, 1);
      lcd.print(" ");
    }
}

void userMenu()
{
    blinkCorner();
    if (curr_cycle == 0) // Still setting up the duration of cycle
    {
        lcd.setCursor(0, 0);
        lcd.print("Cycle length: ");

        lcd.setCursor(0, 1);
        if(dur_cycle < 10)
            lcd.print(" ");
        
        lcd.print(String(dur_cycle) + " mins");

    }
    else if (curr_cycle == -1)  // First setting up the number of cycles
    {
        lcd.setCursor(0, 0);
        lcd.print("Num cycles: ");

        lcd.setCursor(0, 1);
        if(num_cycles < 10)
            lcd.print(" ");

        lcd.print(String(num_cycles) + " times");
    }
}

#pragma endregion

//------------------------------| Shield Buttons |------------------------------
#pragma region 

int read_LCD_buttons()
{
    adc_key_in = analogRead(0);      // read the value from the sensor 
    // my buttons when read are centered at these valius: 18, 236, 522, 802
    // that's roughly EVEN multiples of 128 (0, 256, 512, 768)
    // so midpoints are going to be around ODD multiples of 128
    int coefficient = 128;
    
    //if (adc_key_in > 1408)
        //return adc_key_in; // We make this the 1st option for speed reasons since it will be the most likely result
    
    if (adc_key_in < btnNEXT * coefficient) // this one gives off smallest value
        return btnNEXT;
    
    if (adc_key_in < btnUP * coefficient) // etc in order of value size
        return btnUP;
    
    if (adc_key_in < btnDOWN * coefficient)
        return btnDOWN; 
    
    //if (adc_key_in < btnNONE * coefficient)
    return btnNONE;  
}

void handleButtonEvents(int lcd_key)
{
    int inc_amount;
    if (lcd_key == previous_operation) // triggers if holding same button, or mashing quick
        inc_amount = 5;
    else // if changing between buttons or clicking slowly
        inc_amount = 1;
    
    previous_operation = lcd_key;

    switch (lcd_key)               // depending on which button was pushed, we perform an action
    {
        case btnNEXT:{ // used for moving from setup1 to setup2 to running
            clearLCD();
            lcd.setCursor(0,1);
            curr_cycle++;

            if (curr_cycle == 1) // first button click commenced
            {
                lcd.print("STARTING...");
                delay(400);
                time_since_last_click = millis();
                motoric.write(PUSH_BUTTON);
                delay(ONE_SECOND);
                motoric.write(IDLE_POSITION);
            }

            else // selected first number, now the second
            {
                lcd.print("DALJE ");
            }
            delay(600);
            clearLCD();
            break;
        }
        case btnUP:{ //used for increasing number in setup1 and setup2
            if (curr_cycle == 0)
                dur_cycle += inc_amount;
            else if (curr_cycle == -1)
                num_cycles += inc_amount;
            delay(600);
            break;
        }
        case btnDOWN:{ // used for decreasing
            if (curr_cycle == 0){
                if (dur_cycle <= 5) // to slow down changes, even if mashed
                    inc_amount = 1;              
                if (dur_cycle > inc_amount)
                    dur_cycle -= inc_amount;
            }
            else if (curr_cycle == -1){
                if (num_cycles <= 5) // to slow down changes, even if mashed
                      inc_amount = 1;   
                if (num_cycles > inc_amount)
                    num_cycles -= inc_amount;
            }
            delay(600);
            break;
        }
    }
}
#pragma endregion

//------------------------------| Servo Motor Component/Module |------------------------------
#pragma region 

void clickingProcedure()
{
    lcd.setCursor(0,0);
    lcd.print("Cycle [" + String(curr_cycle) + "/" + String(num_cycles) + "]");

    int sec_elapsed = (millis() - time_since_last_click)/1000;
    int sec_remaining = 60*dur_cycle - sec_elapsed;

    if( sec_remaining > 0 ){
        showRemainingTime();
        return;
    }

    motoric.write(PUSH_BUTTON);
    delay(ONE_SECOND);
    motoric.write(IDLE_POSITION);

    time_since_last_click = millis();

    curr_cycle++;
    if (curr_cycle < num_cycles) // we must do more cycles, exit loop
        return;
    else 
        finishedProcedure();  
}

void finishedProcedure()
{
    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("Done all " + String(num_cycles) + "cycles");
    lcd.setCursor(0,1);
    int time_min = int(millis()/60000);
    int time_hr = int(time_min/60); time_min = time_min % 60;
    String time_output = String(time_hr) + "h" + String(time_min) + "m";

    lcd.print("Total " + time_output);
    while(true)
    {
        delay(ONE_SECOND);
        lcd.setCursor(15,1);
        lcd.print(".");
        delay(ONE_SECOND); 
        lcd.setCursor(15,1);
        lcd.print(" ");
    }
}
#pragma endregion

//------------------------------| Future WiFi Operations |------------------------------
#pragma region
void WIFIsetup()
{
    /* Connecting to the WiFi
    int retries = 0;
    while ((WiFi.status() != WL_CONNECTED)) {
        retries++;
        delay(ONE_SECOND);
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
#pragma endregion