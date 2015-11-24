#include <Arduino.h>
#include <Time.h>
#include <i2c_t3.h>
#include <HydroSense_RTC.h>


HydroSense_RTC RTC = HydroSense_RTC( 0x51 );


void setup(){
	Serial.begin(9600);
	Wire.begin( I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );

	Serial.println("HydroSense_RTC Read Test");
	Serial.println("-------------------");
}

void loop() {
	tmElements_t tm;
	HydroSense_RTC_status_t stats;

	if (RTC.read(tm)) {
		Serial.print("Ok, Time = ");
		print2digits(tm.Hour);
		Serial.write(':');
		print2digits(tm.Minute);
		Serial.write(':');
		print2digits(tm.Second);
		Serial.print(", Date (D/M/Y) = ");
		Serial.print(tm.Day);
		Serial.write('/');
		Serial.print(tm.Month);
		Serial.write('/');
		Serial.print(tmYearToCalendar(tm.Year));
		Serial.println();

		if (RTC.status(&stats)){
			//Serial.println(stats.config[0], HEX);
			//Serial.println(stats.config[1], HEX);
			//Serial.println(stats.config[2], HEX);
		}else{
			Serial.println("HydroSense_RTC read status error!  Please check the circuitry.");
			Serial.println();
		}
	} else {
		if (RTC.chipPresent()) {
			Serial.println("The HydroSense_RTC is stopped.  Please run the SetTime");
			Serial.println("example to initialize the time and begin running.");
			Serial.println();
		} else {
			Serial.println("HydroSense_RTC read error!  Please check the circuitry.");
			Serial.println();
		}

		delay(3000);
	}

	delay(1000);
}
	
void print2digits(int number) {
	if (number >= 0 && number < 10) {
		Serial.write('0');
	}
	Serial.print(number);
}
