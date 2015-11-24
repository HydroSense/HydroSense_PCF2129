/*
 * HydroSense RTC (PCF2129)
 * Alan Marchiori
 * Bucknell University
 *
 * Based on the HydroSense_RTC.h - library for DS1307 RTC
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#include "HydroSense_RTC.h"

HydroSense_RTC::HydroSense_RTC(uint8_t address)
{
  // Wire.begin();
  // user must start wire library before calling me.
  _address = address;
  exists = false;
}
  
// PUBLIC FUNCTIONS
time_t HydroSense_RTC::get()   // Acquire data from buffer and convert to time_t
{
  tmElements_t tm;
  if (read(tm) == false) return 0;
  return(makeTime(tm));
}

 bool HydroSense_RTC::status(HydroSense_RTC_status_t* stats)
{
  Wire.beginTransmission(_address);
  Wire.write((uint8_t)0x0);
  if (Wire.endTransmission() != 0) {
	exists = false;
	return false;
  }
  exists = true;

  Wire.requestFrom(_address, 3);
  if (Wire.available() < 3) return false;

  stats->config[0] = Wire.read();
  stats->config[1] = Wire.read();
  stats->config[2] = Wire.read();

  return true;
}

bool HydroSense_RTC::set(time_t t)
{
  tmElements_t tm;
  breakTime(t, tm);
  tm.Second |= 0x80;  // stop the clock 
  write(tm); 
  tm.Second &= 0x7f;  // start the clock
  write(tm); 
}

// Acquire data from the RTC chip in BCD format
bool HydroSense_RTC::read(tmElements_t &tm)
{
  uint8_t sec;
  Wire.beginTransmission(_address);
  Wire.write((uint8_t)0x3);
  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;

  // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
  Wire.requestFrom(_address, (uint8_t) tmNbrFields);
  if (Wire.available() < tmNbrFields) return false;

  sec = Wire.read();
  tm.Second = bcd2dec(sec & 0x7f);   
  tm.Minute = bcd2dec(Wire.read() );
  tm.Hour =   bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock

  tm.Day = bcd2dec(Wire.read() );

  tm.Wday = bcd2dec(Wire.read() ) + 1; // adjust for sunday=1/0 difference

  tm.Month = bcd2dec(Wire.read() ) - 1; // adjust for jan =1/0 difference
  tm.Year = y2kYearToTm((bcd2dec(Wire.read())));

  if (sec & 0x80) return false; // clock is halted
  return true;
}

bool HydroSense_RTC::write(tmElements_t &tm)
{
  Wire.beginTransmission(_address);

  Wire.write((uint8_t)0x3); // reset register pointer

  Wire.write(dec2bcd(tm.Second)) ;   
  Wire.write(dec2bcd(tm.Minute));
  Wire.write(dec2bcd(tm.Hour));      // sets 24 hour format

  Wire.write(dec2bcd(tm.Day));
  Wire.write(dec2bcd(tm.Wday - 1)); // pcf2129 uses sunday=0
  Wire.write(dec2bcd(tm.Month + 1)); // pcf2129 uses jan = 1
  Wire.write(dec2bcd(tmYearToY2k(tm.Year))); 

  if (Wire.endTransmission() != 0) {
    exists = false;
    return false;
  }
  exists = true;
  return true;
}

// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t HydroSense_RTC::dec2bcd(uint8_t num)
{
  return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t HydroSense_RTC::bcd2dec(uint8_t num)
{
  return ((num/16 * 10) + (num % 16));
}

