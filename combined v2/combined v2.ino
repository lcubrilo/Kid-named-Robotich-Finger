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

    clearLCD();
    lcd.setCursor(0,0);
    lcd.print("LOADING");
    lcd.setCursor(0,1);
    lcd.print("[*");

    motoric.write(0);
    for(int i = 1; i < 4; i++)
    {
        motoric.write(180);
        lcd.print("**");
        delay(250);
        motoric.write(0);
        lcd.print("**");
        delay(250);
    }
    lcd.print("*]");
    delay(500);
    clearLCD();
    delay(200);
}

int curr_cycle = -1;
int num_cycles = 20;
int dur_cycle = 30;
int start_time;

void clearLCD()
{
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
}

String formatTime_old()
{
    int seconds = (millis()-start_time)/1000;
    int minutes = seconds/60; 
    int hours = minutes/60;

    if (seconds < 99)
        return String(seconds) + "s";

    else{
        seconds %= 60;
        String temp = String(seconds) + "s";
        if (temp.length()==2)
            temp = "0" + temp;

        if (minutes < 99)
            return String(minutes) + "m" + temp;

        minutes %= 60;
        temp = String(minutes) + "m";
        if (temp.length() == 5)
            temp = "0" + temp;
        
        return String(hours) + "h" + temp;
        
    }
}

void showElapsedTime()
{
    String output = formatTime_old();
    int n = output.length();
    lcd.setCursor(16-n, 1);
    lcd.print(output);
}

int previous_operation = btnNEXT;
int inc_amount;

void loop()
{
    showElapsedTime();
    String status = "";

    
    if (curr_cycle > 0)  // Everything setted up, running the clicking operations
    {
        lcd.setCursor(0,0);
        lcd.print("Cycle [" + String(curr_cycle) + "/" + String(num_cycles) + "]");
        motoric.write(180);
        delay(1000);
        motoric.write(0);
        for (int i = 0; i < dur_cycle; i++){
            for(int j = 0; j < 60; j++){
                if (i == dur_cycle - 1 && j == 59)
                    continue;                    
                delay(1000);
                showElapsedTime();
            }
        }
        curr_cycle++;
        if (curr_cycle < num_cycles)
            return;
        else
        {
            motoric.write(180);
            delay(1000);
            motoric.write(0);
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
                delay(1000);
                lcd.setCursor(15,1);
                lcd.print(".");
                delay(1000);
                lcd.setCursor(15,1);
                lcd.print(" ");
            }
        }      
    }

    if (curr_cycle == 0) // Still setting up the duration of cycle
    {
        lcd.setCursor(0,0);  
        lcd.print("Cycle length: ");
        lcd.setCursor(0,1);
        lcd.print(String(dur_cycle) + " mins");
    }
    else if (curr_cycle == -1)  // First setting up the number of cycles
    {
        lcd.setCursor(0,0);  
        lcd.print("Num cycles: ");
        lcd.setCursor(0,1);
        lcd.print(String(num_cycles) + " times");
    }    

    lcd_key = read_LCD_buttons();  // read input from the buttons

    // This if-else statement reduces amount of same-button mashing. If you go over, getting back in steps of 1 is easy
    
    if (lcd_key == previous_operation) // triggers if holding same button, or mashing quick
        inc_amount = 5;
    else // if changing between buttons or clicking slowly
        inc_amount = 1;

    switch (lcd_key)               // depending on which button was pushed, we perform an action
    {
        case btnNEXT:{ // used for moving from setup1 to setup2 to running
            clearLCD();
            lcd.setCursor(0,1);
            curr_cycle++;
            if (curr_cycle == 1)
            {
                start_time = millis();
                lcd.print("STARTING..."+String(start_time));
                delay(400);
            }
            else
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
                if (dur_cycle <= 5)
                    inc_amount = 1;              
                if (dur_cycle > inc_amount)
                    dur_cycle -= inc_amount;
            }
            else if (curr_cycle == -1){
                if (num_cycles <= 5)
                      inc_amount = 1;   
                if (num_cycles > inc_amount)
                    num_cycles -= inc_amount;
            }
            delay(600);
            break;
        }
    }

    previous_operation = lcd_key;
}


 