double watt_hours_left = read_from_eeprom();

void setup()
{
	volatile double initial_watt_hours = 500;
	volatile int humidity_ararm_value = 30; //Needs to be updatet to sensable value
	volatile double ampere_constant = 1; //Needs to be updatet to sensable value
	
}


void loop()
{
	boolean humidity_alarm = check_humidity();
	boolean temp_alarm = check_temp();
	
	double ampere = check_ampere();
	akkumulate_ampere(ampere);

	if (resetswitch_pushed){
		watt_hours_left = initial_watt_hours;
	}

	//If time > 30 sek
	//Save 
}

boolead check_humidity(){
	if (humidity.get()< humidity_ararm_value){
		return false;
	}
	return true;
}

double check_ampere(){
	double ampere = voltage_pin.read()*ampere_constant;
	return ampere
}

double read_from_eeprom(){
	return//Read ffrom eeprom memory
}