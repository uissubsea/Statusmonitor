#include <Adafruit_MCP9808.h>
#include <SimpleTimer.h>
#include <wire.h>
#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>

//Digital pin layout:
int resetswitch_pin = 3;
int go_to_surface_pin = 13;	//Must be set to a reasonable and available digital pin

//Analog pin layout:
int shunt_pin = 0;			//This is the pin where you read the voltage input from the shunt resistor. Must be a analog input.
int humidity_front_pin = 1;
int humidity_back_pin = 2;
int voltage_pin = 3;		//This is the pin where you read the voltage input from voltage divider, from the battery v++. 42 V(max) = 5 V
//I2C bus: Pin 4 is SDA
//I2C bus: Pin 5 is SCL
int cell_voltage_pin = 6;	//Reads the value of each batterycell througt a decoder. 

SimpleTimer timer; 
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
volatile double cell_voltage_alarm = 3.1;
volatile int alarm_temperature = 50;	// This should be a safe temperature for the batteries to operate under
volatile int initial_watt_hours = 530;	// Total capasity of batteries.
volatile int minimum_battery = 25;		// Should represent a 5% value of the batteries becoming empty
volatile int humidity_alarm_value = 1;	// Needs to be updated to sensible value
volatile double ampere_calibration_constant = 1;	// This is the constant for calculating the amp draw from the voltage across a shunt resistor 
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
double lowest_cell_voltage = 3.7;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void setup()
{
	//Connect I2C pins to A4 (SDA)and A5 (SCL).
	pinMode(humidity_front_pin, INPUT);
	pinMode(humidity_back_pin, INPUT);
	pinMode(resetswitch_pin, INPUT);
	pinMode(cell_voltage_pin, INPUT);
	pinMode(go_to_surface_pin, OUTPUT);
	digitalWrite(go_to_surface_pin, HIGH);
	timer.setInterval(3000, save_to_eeprom);	//Must be set to 30000!
	timer.setInterval(200, update_display);		//Must be set to 30000!
	digitalWrite(go_to_surface_pin, HIGH);		//May be convertet into serial data..
	Serial.begin(9600);							//Opens serial port, sets data rate to 9600 bps
	read_from_eeprom();							//Loading previous variable batery capasity.
	lcd.begin(16, 2);
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("   UiS Subsea   ");
	if (!tempsensor.begin())
	{
		Serial.println("Couldn't find MCP9808!");
		lcd.setCursor(0, 1);
		lcd.print("Couldn't find MCP9808!");
		delay(1500);
	}
	else
	{
		Serial.println("MCP9808 connected");
		lcd.setCursor(0, 1);
		lcd.print("MCP9808 connected");
		delay(1500);
	}
lcd.setCursor(0, 1);
lcd.print("Initial setup ok");
Serial.println("Initial setup ok");
delay(2500);
}


void loop()
{
	timer.run();
	update_watt_minutes_left();
	check_battery_cells();
	check_temp();
	check_humidity();
	check_switches();
}



void update_display()
{
	//Must make methode for rolling text, and implement folliowing variables: Volt, Highest temp, Battery %, Ampere, Minimum cell voltage
	lcd.setCursor(0, 0); //Start at character 0 on line 0
	lcd.print("Temp:");
	lcd.setCursor(9, 0);
	lcd.print("Battery:");
	lcd.setCursor(0, 1); //Start at character 0 on line 0
	lcd.print(tempsensor.readTempC());
	lcd.setCursor(9, 1);
	int percent = (int)(watt_minutes_left / 3600) / initial_watt_hours * 100; //Must be tested.
	lcd.print(percent + "%");
}

void check_battery_cells()
{
	for (int i = 0; i < 5; i++)
		{
			//dekoder/mux position = i;
			if (lowest_cell_voltage > analogRead(cell_voltage_pin)/1024*5)
			{
			lowest_cell_voltage = analogRead(cell_voltage_pin)/1024*5;
			}
		}
}
void check_switches()
{

	if (digitalRead(resetswitch_pin))
	{
		watt_minutes_left = (float)initial_watt_hours*3600;
	}
	if (humidity_alarm_trigged && temp_alarm_trigged && battery_low_triggered && (lowest_cell_voltage < cell_voltage_alarm))
	{
		digitalWrite(go_to_surface_pin, LOW);
		//Alternative, send variables and alarm via bus/ip,
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
	dt = millis()-pastMillis;			//Calculates timestamp for integration
	pastMillis = millis();				//Reset counter value
	double ampere = analogRead(shunt_pin)*ampere_calibration_constant / (1024 * 5);
	double watt = ampere * voltage;
	watt_minutes_left = watt_minutes_left - (watt*dt/(1000));
	if (watt_minutes_left < minimum_battery)
	{
		battery_low_triggered = 1;
	}
}

void read_from_eeprom()
{       
    Serial.println("Reading eeprom memory:");
	lcd.setCursor(0, 1);
	lcd.write("Reading memory:");
	int mod = EEPROM.read(0);//Read from eeprom memory
    int base = EEPROM.read(1);
    int watt_hours_left = base*256+mod;
    watt_minutes_left = (double)watt_hours_left*3600;
	delay(1000);
    Serial.println(watt_hours_left);
	lcd.setCursor(0, 1);
	lcd.write("WattHours: " + watt_hours_left);
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
    Serial.println(watt_hours_left);
	}


void check_temp()
{
	battery_temp = tempsensor.readTempC();
	if (battery_temp > alarm_temperature)
	{
		temp_alarm_trigged = 1;
	}
}