void setup()
{
	volatile double initial_watt_hours = 500
	volatile int humidity_ararm_value = 30 //Needs to be updatet to sensable value

	watt_hours_left = read_from_eeprom();
}


void loop()
{
	boolean humidity_alarm = check_humidity();
	boolean temp_alarm = check_temp();
	double voltage_drop = check_voltage_drop();
	akkumulate_ampere(coltage_drop);

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

double read_from_eeprom(){

	return
}