#include <Adafruit_MCP9808.h>
#include <SimpleTimer.h>
#include <wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

int humidity_front_pin = 1;
int humidity_back_pin = 2;
int resetswitch_pin = 3;
int go_to_surface_pin = 13;//Must be set to a reasonable and available digital pin
int voltage_pin = 3; //This is the pin where you read the voltage input from the shunt resistor. Must be a analog input.
int shunt_pin = 0; //This is the pin where you read the voltage input from the shunt resistor. Must be a analog input.


//I2C:
//Pin A4 is SDA
//Pin A5 is SCL

SimpleTimer timer; 
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

volatile int alarm_temperature = 70; //This should be a safe temperature for the batteries to operate under
volatile int initial_watt_hours = 500;
volatile int minimum_battery = 25; // Should represent a 5% value of the batteries becoming empty
volatile int humidity_alarm_value = 1; //Needs to be updated to sensible value
volatile double ampere_constant = 1; // This is the constant for calculating the amp draw from the voltage across a shunt resistor 
double voltage = 0;
double battery_temp;
double dt;
double watt_minutes_left; //bÃ¸r settes til double
double humidity_front = 0;
double humidity_back = 0;
boolean temp_alarm_trigged = 0;
boolean humidity_alarm_trigged = 0;
boolean battery_low_triggered = 0;
double pastMillis = 0;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{

	lcd.begin(16, 2);

//Connect I2C pins to A4 (SDA)and A5 (SCL).
pinMode(humidity_front_pin, INPUT);
pinMode(humidity_back_pin, INPUT);
pinMode(resetswitch_pin, INPUT);
pinMode(go_to_surface_pin, OUTPUT);
digitalWrite(go_to_surface_pin, HIGH);
timer.setInterval(3000, save_to_eeprom);//Must be set to 30000!
timer.setInterval(200, update_display);//Must be set to 30000!


digitalWrite(go_to_surface_pin, HIGH);
Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
Serial.println("Setup OK, reading memory:");
read_from_eeprom();
if (!tempsensor.begin()) {
	Serial.println("Couldn't find MCP9808!");
	while (1);
}
Serial.println("MCP9808 connected");
}


void loop()
{
	timer.run();
	update_watt_minutes_left();
	check_temp();
	check_humidity();
	check_switches();
}



void update_display()
{
	lcd.clear();
	lcd.setCursor(0, 0); //Start at character 0 on line 0
//	lcd.print("Volt:");
	lcd.print("Temp: ♥");
	lcd.setCursor(8, 0);
	lcd.print("WattHl:");
	lcd.setCursor(0, 1); //Start at character 0 on line 0
//	lcd.print(voltage);
	lcd.print(tempsensor.readTempC());
	lcd.setCursor(8, 1);
	lcd.print((double)watt_minutes_left / 3600);
}

void check_switches()
{

        	if (digitalRead(resetswitch_pin))
	{
	watt_minutes_left = (float)initial_watt_hours*3600;
	}
	if (humidity_alarm_trigged && temp_alarm_trigged && battery_low_triggered)
	{
		digitalWrite(go_to_surface_pin, LOW);
	}

}

void check_humidity()
{
	humidity_front = analogRead(humidity_front_pin);
	humidity_back = analogRead(humidity_back_pin);
		if (humidity_front > humidity_alarm_value)
		{
		humidity_alarm_trigged = 1;
		} 
		else if (humidity_back > humidity_alarm_value)
		{
		humidity_alarm_trigged = 1;
		}
}

void update_watt_minutes_left()
{
	voltage = analogRead(voltage_pin);
	dt = millis()-pastMillis;
	pastMillis = millis();
	double ampere = analogRead(shunt_pin)*ampere_constant/(1024*5);
	double watt = ampere * voltage;
	watt_minutes_left = watt_minutes_left - (watt*dt/(1000));
	if (watt_minutes_left < minimum_battery)
	{
		battery_low_triggered = 1;
	}
        }

void read_from_eeprom()
{       
        Serial.println("Reading eeprom:");
        int mod = EEPROM.read(0);//Read from eeprom memory
        int base = EEPROM.read(1);
        int watt_hours_left = base*256+mod;
        watt_minutes_left = (double)watt_hours_left*3600;
        Serial.println(watt_hours_left);
}

void save_to_eeprom()
{
        Serial.println("Saving to eerpom!:");
  
		int watt_hours_left = watt_minutes_left/3600;
        Serial.println(watt_hours_left);
        int mod = (int)(watt_hours_left%256);
        int base = (int)(watt_hours_left/256); 
		EEPROM.write(0, mod);//Save to eeprom memory position 0, arduino has total of 512byte eeprom and is rated to 100 000 cycles (May have to be very careful here)
        EEPROM.write(1, base);
        Serial.println("Saved to eeprom ok:");
        Serial.println(watt_hours_left);
		Serial.println("Reading out from eeprom to confirm:");
        int mod_confirm = EEPROM.read(0);//Read from eeprom memory
        int base_confirm = EEPROM.read(1);
        int watt_hours_left_confirm = base_confirm*256+mod_confirm;
        Serial.println(watt_hours_left_confirm);

}


void check_temp()
{
	battery_temp = tempsensor.readTempC();
	if (battery_temp > alarm_temperature)
	{
		temp_alarm_trigged = 1;
	}
}