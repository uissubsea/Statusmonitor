#include <SimpleTimer.h>
#include <wire.h>
#include <EEPROM.h>

int humidity_front_pin = 1;//Must be set to a reasonable and available analog pin
int humidity_back_pin = 2;//Must be set to a reasonable and available analog pin
int resetswitch_pin = 3;//Must be set to a reasonable and available digital pin
int go_to_surface_pin = 13;//Must be set to a reasonable and available digital pin
int voltage_pin = 5; //This is the pin where you read the voltage input from the shunt resistor. Must be a analog input.
int temp_pin = 6; //Pin for reading temperature sensor.
SimpleTimer timer; 


volatile double voltage = 24; //Can be measured and implemented in watt integration code.
volatile int alarm_temperature = 70; //This should be a safe temperature for the batteries to operate under
volatile int initial_watt_hours = 500;
volatile int minimum_battery = 25; // Should represent a 5% value of the batteries becoming empty
volatile int humidity_alarm_value = 1; //Needs to be updated to sensible value
volatile double ampere_constant = 1; // This is the constant for calculating the amp draw from the voltage across a shunt resistor 
double battery_temp;
double dt;
double watt_minutes_left; //bÃ¸r settes til double
double humidity_front = 0;
double humidity_back = 0;
boolean temp_alarm_trigged = 0;
boolean humidity_alarm_trigged = 0;
boolean battery_low_triggered = 0;
double pastMillis = 0;


void setup()
{
//Should setup values for i2c sensors here
//SPI pins for display etc must be setup here	
pinMode(humidity_front_pin, INPUT);
pinMode(humidity_back_pin, INPUT);
pinMode(resetswitch_pin, INPUT);
pinMode(go_to_surface_pin, OUTPUT);
digitalWrite(go_to_surface_pin, HIGH);
timer.setInterval(3000, save_to_eeprom);//Must be set to 30000!

digitalWrite(go_to_surface_pin, HIGH);
Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
Serial.println("Setup OK, reading memory:");
read_from_eeprom();
}


void loop()
{
  timer.run();
        update_watt_minutes_left();
	check_temp();
	check_humidity();
	check_switches();
	update_display();
}



void update_display()
{

}

void check_switches()
{

        	if (digitalRead(resetswitch_pin))
	{
        Serial.println("Resetpin pushed, resetting value:");
	watt_minutes_left = (float)initial_watt_hours*3600;
        Serial.println("Watt_minutes_left");
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
	dt = millis()-pastMillis;
	pastMillis = millis();
	double ampere = analogRead(voltage_pin)*ampere_constant/(1024*5);
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
	battery_temp = analogRead(temp_pin);
	if (battery_temp > alarm_temperature)
	{
		temp_alarm_trigged = 1;
	}
}

