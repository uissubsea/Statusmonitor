#include <SimpleTimer.h>
#include <wire.h>

int humidity_front_pin = //Must be set to a reasonable and available analog pin
int humidity_back_pin = //Must be set to a reasonable and available analog pin
int resetswitch_pin = //Must be set to a reasonable and available digital pin
int go_to_surface_pin = //Must be set to a reasonable and available digital pin
SimpleTimer timer;


volatile int voltage = 24; //Can be measured and implemented in watt integration code.
volatile int alarm_temperature = 70; //This should be a safe temperature for the batteries to operate under
volatile int initial_watt_hours = 500;
volatile int minimum_battery = 25; // Should represent a 5% value of the batteries becoming empty
volatile int humidity_alarm_value = 30; //Needs to be updated to sensible value
volatile double ampere_constant = 1; //Needs to be updated to sensible value
double battery_temp;
double watt_hours_left = read_from_eeprom();
double humidity_front = 0;
double humidity_back = 0;
boolean temp_alarm_trigged = 0;
boolean humidity_alarm_trigged = 0;
unsigned long pastMillis = 0;


void setup()
{

	//Should setup values for i2c sensors here
pinMode(humidity_front_pin, INPUT)
pinMode(humidity_back_pin, INPUT)
pinMode(resetswitch_pin, INPUT)
pinMode(go_to_surface_pin, OUTPUT)
//SPI pins for display etc must be setup here	
timer.setInterval(30000, save_to_eeprom());
timer.setInterval(10, update_watt_hours_left());
digitalWrite(go_to_surface_pin, FALSE);
}


void loop()
{
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
		watt_hours_left = initial_watt_hours;
	}

	if (humidity_alarm_trigged && temp_alarm_trigged && battery_low_triggered)
	{
		digitalWrite(go_to_surface_pin, TRUE);
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

void update_watt_hours_left()
{
	dt = millis()-pastMillis;
	pastMillis = millis();
	double ampere = voltage_pin.read()*ampere_constant;
	double watt = ampere * voltage;
	watt_hours_left -= watt*dt;
	if (watt_hours_left < minimum_battery)
	{
		battery_low_triggered = 1;
	}
}

double read_from_eeprom()
{
	return//Read ffrom eeprom memory
}

void check_temp()
{
	Wire.read();
	battery_temp = analogRead(temp_pin);
	if (battery_temp > alarm_temperature){
		temp_alarm_trigged = 1
	}
}