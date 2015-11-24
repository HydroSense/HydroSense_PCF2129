#include <Arduino.h>
#include <Time.h>
#include <i2c_t3.h>
#include <HydroSense_RTC.h>

const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

tmElements_t tm;
HydroSense_RTC RTC = HydroSense_RTC( 0x51 );

void setup() {
  bool parse=false;
  bool config=false;

  Serial.begin(9600);

  while (!Serial) delay (100);

  Wire.begin( I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );

  // get the date and time the compiler was run
  if (getDate(__DATE__) && getTime(__TIME__)) {
    parse = true;
  }

  if (parse) {
    Serial.print("HydroSense_RTC setting Time=");
    Serial.print(__TIME__);
    Serial.print(", Date=");
    Serial.println(__DATE__);

    if (RTC.write(tm)) {
        config = true;
    } else {
		Serial.println("HydroSense_RTC Communication Error :-{");
		Serial.println("Please check your circuitry");
    }
  } else {
    Serial.print("Could not parse info from the compiler, Time=\"");
    Serial.print(__TIME__);
    Serial.print("\", Date=\"");
    Serial.print(__DATE__);
    Serial.println("\"");
  }
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

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}

