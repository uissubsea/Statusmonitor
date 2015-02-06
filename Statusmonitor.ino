#include wire

volatile int alarm_temperature = 70; //This should be a safe temperature for the batteries to operate under
volatile double initial_watt_hours = 500;
volatile int humidity_alarm_value = 30; //Needs to be updated to sensible value
volatile double ampere_constant = 1; //Needs to be updated to sensible value
double battery_temp;
double watt_hours_left = read_from_eeprom();
double humidity_front = 0;
double humidity_back = 0;
boolean temp_alarm_trigged = 0;
boolean humidity_alarm_trigged = 0;


void setup()
{

	//Should setup values for i2c sensors here
	
}


void loop()
{
	double ampere = check_ampere();
	akkumulate_ampere(ampere);
	check_temp();
	check_humidity();
	if (resetswitch_pushed){
		watt_hours_left = initial_watt_hours;
	}

	//If time > 30 sek
	//Save 
}

void check_humidity()
{
	humidity_front = analogRead(humidity_front_pin);
	humidity_back = analogRead(humidity_back_pin);
	if (humidity_front > humidity_alarm_value){
	humidity_alarm_trigged = 1;
	} else if (humidity_back > humidity_alarm_value){
		humidity_alarm_trigged = 1;
	}
}

double check_ampere()
{
	double ampere = voltage_pin.read()*ampere_constant;
	return ampere
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