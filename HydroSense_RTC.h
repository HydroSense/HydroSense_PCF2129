/*
 * HydroSense RTC (PCF2129)
 * Alan Marchiori
 * Bucknell University
 *
 * Based on the DS1307RTC.h - library for DS1307 RTC
 *
 * See the file "LICENSE" for the full license governing this code.
 */

#ifndef __pcf2129__
#define __pcf2129__

#include <Time.h>
#include <i2c_t3.h>

// someday we could make this a union with useful fields
// for now it's just the raw configuration
typedef struct {
	uint8_t config[3];
} HydroSense_RTC_status_t;

// library interface description
class HydroSense_RTC
{
  // user-accessible "public" interface
  public:
	HydroSense_RTC(uint8_t address);
    time_t get();
    bool status(HydroSense_RTC_status_t* stat);
    bool set(time_t t);
    bool read(tmElements_t &tm);
    bool write(tmElements_t &tm);
    bool chipPresent() { return exists; }

  private:
    uint8_t _address;
    bool exists;
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
};

#endif
 

