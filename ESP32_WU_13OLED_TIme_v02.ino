#include "SH1106Wire.h"    // https://github.com/squix78/esp8266-oled-ssd1306
// Include the UI lib
#include "OLEDDisplayUi.h" // Part of SSD1306/SH1106

#include <WiFi.h>
#include <ArduinoJson.h>   // https://github.com/bblanchon/ArduinoJson
#include "time.h"

// WU Icon names, there are 37, but many are the same image, so use the *icon when required, giving 10 basic icons to be used
// *snow            == chanceflurries == chancesnow == flurries == nt_chanceflurries == nt_chancesnow == nt_flurries == nt_snow
// *rain            == chancerain  == nt_chancerain  == nt_rain
// *sleet           == chancesleet == nt_chancesleet == nt_sleet
// *sunny           == clear
// *cloudy          == nt_cloudy
// *mostlysunny     == partlycloudy == partlysunny == mostlycloudy
// *fog             == nt_fog == nt_hazy == hazy
// *tstorms         == chancetstorms == nt_tstorms == nt_chancetstorms
// *nt_mostlycloudy == nt_mostlysunny == nt_partlycloudy == nt_partlysunny
// *nt_clear

#define icon_width  40
#define icon_height 40

// Define each of the *icons for display
const char snow_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 
  0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x01, 0x00, 0x00, 0x00, 
  0xFC, 0x00, 0x00, 0x00, 0xC0, 0x78, 0x0C, 0x00, 0x00, 0xD8, 0x30, 0x6C, 
  0x00, 0x00, 0xF8, 0x30, 0x7C, 0x00, 0x00, 0xF0, 0x31, 0x3E, 0x00, 0x00, 
  0xF8, 0x7F, 0x3F, 0x00, 0x00, 0x70, 0xFF, 0x7B, 0x00, 0x00, 0x00, 0xCC, 
  0x00, 0x00, 0x00, 0x00, 0xCC, 0x00, 0x00, 0x00, 0x78, 0xFF, 0x33, 0x00, 
  0x00, 0xF0, 0xFB, 0x7F, 0x00, 0x00, 0xF0, 0x31, 0x3E, 0x00, 0x00, 0xF8, 
  0x31, 0x7E, 0x40, 0x00, 0xD8, 0x70, 0x6C, 0xF0, 0x01, 0xC0, 0x78, 0x0C, 
  0xE0, 0x01, 0x80, 0xFC, 0x00, 0xE5, 0x0C, 0x00, 0xFC, 0x01, 0x4F, 0x3C, 
  0x00, 0x30, 0x00, 0xCF, 0x1E, 0x00, 0x20, 0x00, 0xFF, 0x3F, 0x00, 0x00, 
  0x00, 0xB0, 0x01, 0x00, 0x00, 0x00, 0xF1, 0x03, 0x00, 0x00, 0x00, 0xFF, 
  0x1F, 0x00, 0x00, 0x00, 0x4F, 0x1C, 0x00, 0x00, 0x00, 0xCF, 0x3C, 0x00, 
  0x00, 0x00, 0xE4, 0x09, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 
  0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char rain_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x0F, 0x00, 
  0x00, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x00, 0x78, 0x7A, 0x00, 0x00, 0x00, 
  0x18, 0xF4, 0x0E, 0x00, 0x00, 0x0C, 0xE0, 0x7F, 0x00, 0x00, 0x0C, 0xC0, 
  0x7B, 0x00, 0x00, 0x06, 0xA0, 0xEC, 0x00, 0xE0, 0x07, 0x40, 0xE1, 0x0F, 
  0xF0, 0x07, 0x80, 0xE0, 0x1F, 0x1C, 0x00, 0x00, 0x60, 0x3E, 0x0C, 0x00, 
  0x00, 0xA0, 0x79, 0x0C, 0x00, 0x00, 0x00, 0x74, 0x04, 0x00, 0x00, 0x00, 
  0x74, 0x06, 0x00, 0x00, 0x00, 0x48, 0x0C, 0x00, 0x00, 0x00, 0x78, 0x1C, 
  0x15, 0x45, 0x5A, 0x74, 0xF8, 0xFF, 0xFF, 0xFF, 0x3F, 0xF8, 0xFF, 0xFF, 
  0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x00, 0x0E, 0x07, 0x00, 0x00, 0x00, 
  0x8F, 0x07, 0x00, 0x00, 0x00, 0x8F, 0x07, 0x00, 0x00, 0x00, 0x87, 0x03, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 
  0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x1C, 
  0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char sleet_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0xE0, 0x3F, 0x00, 0x00, 0x00, 
  0xF0, 0x3D, 0x00, 0x00, 0x00, 0x38, 0x64, 0x00, 0x00, 0x00, 0x0C, 0xF8, 
  0x1F, 0x00, 0x00, 0x0C, 0xE0, 0x7F, 0x00, 0x00, 0x04, 0xC0, 0xF9, 0x00, 
  0xC0, 0x07, 0x40, 0xC5, 0x02, 0xF0, 0x07, 0x80, 0xE0, 0x0F, 0x18, 0x02, 
  0x00, 0xE0, 0x3D, 0x1C, 0x00, 0x00, 0x40, 0x79, 0x0C, 0x00, 0x00, 0x40, 
  0x72, 0x0C, 0x00, 0x00, 0x00, 0x68, 0x06, 0x00, 0x00, 0x00, 0x70, 0x04, 
  0x00, 0x00, 0x00, 0x6C, 0x1C, 0x40, 0xA0, 0x04, 0x74, 0xFC, 0x7F, 0xFF, 
  0xFF, 0x3F, 0xF0, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0x12, 0x08, 0x20, 0x00, 
  0x00, 0x40, 0x00, 0x08, 0x00, 0x00, 0xC0, 0x38, 0x0C, 0x00, 0x00, 0x60, 
  0x18, 0x06, 0x00, 0x00, 0x30, 0x18, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x83, 0x01, 0x00, 0x00, 
  0x0C, 0xC3, 0x00, 0x00, 0x00, 0x86, 0x41, 0x00, 0x00, 0x00, 0x82, 0x61, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char sunny_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xC0, 0x1C, 0x01, 
  0x00, 0x00, 0xC0, 0xDC, 0x01, 0x00, 0x00, 0xC6, 0xFF, 0x33, 0x00, 0x00, 
  0xFC, 0xFF, 0x1F, 0x00, 0x00, 0x7C, 0x6E, 0x3F, 0x00, 0x40, 0xFC, 0xB2, 
  0x1B, 0x01, 0xC0, 0x9F, 0x02, 0xFD, 0x01, 0x80, 0x1F, 0x48, 0xFC, 0x00, 
  0x80, 0x5B, 0x08, 0xF8, 0x00, 0x80, 0x03, 0x51, 0xE2, 0x02, 0xF0, 0x0F, 
  0x01, 0xF1, 0x07, 0xF0, 0xA1, 0xA8, 0x84, 0x07, 0xC0, 0x95, 0x40, 0x81, 
  0x01, 0xC0, 0x03, 0x2A, 0xF8, 0x03, 0xF8, 0x05, 0x01, 0xC5, 0x0F, 0xF8, 
  0x60, 0x28, 0x20, 0x0F, 0xE0, 0x03, 0x88, 0xF0, 0x03, 0xC0, 0x53, 0x12, 
  0xC5, 0x01, 0xE0, 0x88, 0xAA, 0x88, 0x03, 0xF0, 0x17, 0x00, 0xF1, 0x07, 
  0xF0, 0x17, 0x0A, 0xE8, 0x0B, 0x00, 0x53, 0xA0, 0xE1, 0x00, 0x80, 0x1F, 
  0x22, 0xFC, 0x00, 0xC0, 0x4F, 0x84, 0xFC, 0x01, 0xC0, 0x6D, 0x02, 0xFB, 
  0x01, 0x00, 0xFC, 0x76, 0x1F, 0x00, 0x00, 0xFC, 0xF7, 0x3F, 0x00, 0x00, 
  0xCE, 0xFF, 0x1B, 0x00, 0x00, 0xC0, 0xBD, 0x23, 0x00, 0x00, 0xC0, 0x9C, 
  0x01, 0x00, 0x00, 0x40, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char cloudy_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x00, 0x00, 
  0x00, 0xE0, 0x3F, 0x00, 0x00, 0x00, 0xF8, 0x7A, 0x00, 0x00, 0x00, 0x38, 
  0xF4, 0x0C, 0x00, 0x00, 0x0C, 0xE0, 0x3F, 0x00, 0x00, 0x0C, 0xA0, 0x77, 
  0x00, 0x00, 0x06, 0xE0, 0xF9, 0x00, 0xE0, 0x07, 0xC0, 0xE8, 0x07, 0xF0, 
  0x07, 0x00, 0xC0, 0x1F, 0x18, 0x00, 0x00, 0xA0, 0x3D, 0x0C, 0x00, 0x00, 
  0x60, 0x7A, 0x0C, 0x00, 0x00, 0x00, 0x74, 0x06, 0x00, 0x00, 0x00, 0x68, 
  0x04, 0x00, 0x00, 0x00, 0x68, 0x0E, 0x00, 0x00, 0x00, 0x64, 0x6C, 0xA6, 
  0x1A, 0x9A, 0x76, 0xF8, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 0xFF, 0xFF, 0xFF, 
  0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char mostlysunny_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x48, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x03, 
  0x00, 0x00, 0xC0, 0xF7, 0x03, 0x00, 0x00, 0xF0, 0xED, 0x0F, 0x00, 0x00, 
  0xF6, 0x10, 0x07, 0x00, 0xE0, 0x7F, 0x46, 0x1F, 0x00, 0xF0, 0x7F, 0x41, 
  0x1E, 0x00, 0x38, 0xE4, 0x55, 0x0E, 0x00, 0x1C, 0xE0, 0x3F, 0x3D, 0x00, 
  0x0C, 0xD0, 0xFF, 0x1E, 0x00, 0x0C, 0xE0, 0xE7, 0x0C, 0xA0, 0x06, 0x80, 
  0xD8, 0x1F, 0xF0, 0x07, 0x40, 0xE0, 0x1F, 0x38, 0x04, 0x00, 0xC0, 0x3F, 
  0x1C, 0x00, 0x00, 0xC0, 0x35, 0x0C, 0x00, 0x00, 0x00, 0x7A, 0x06, 0x00, 
  0x00, 0x00, 0x68, 0x04, 0x00, 0x00, 0x00, 0x70, 0x06, 0x00, 0x00, 0x00, 
  0x68, 0x8C, 0x01, 0x84, 0x82, 0x76, 0xFC, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 
  0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char fog_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x80, 0x0F, 0x00, 0x00, 0x00, 0xF0, 0x3F, 0x00, 0x00, 0x00, 0xF0, 0x7D, 
  0x00, 0x00, 0x00, 0x18, 0xF8, 0x0A, 0x00, 0x00, 0x0C, 0xD0, 0x3F, 0x00, 
  0x00, 0x0C, 0xA0, 0x77, 0x00, 0x00, 0x06, 0xE0, 0xFB, 0x00, 0xE0, 0x07, 
  0x40, 0xD8, 0x0F, 0xF0, 0x07, 0xC0, 0xE0, 0x1F, 0x1C, 0x00, 0x00, 0xE0, 
  0x3A, 0x0C, 0x00, 0x40, 0x40, 0x77, 0x0C, 0x00, 0xF8, 0x01, 0x74, 0x04, 
  0x00, 0xFE, 0x07, 0x68, 0x04, 0x00, 0x86, 0x7F, 0x78, 0x0E, 0x00, 0x03, 
  0xFE, 0x65, 0xDC, 0x56, 0x03, 0xFE, 0x7F, 0xFC, 0xFF, 0x03, 0x88, 0x3F, 
  0xF0, 0xFF, 0x01, 0x80, 0x3F, 0x00, 0x0C, 0x00, 0x80, 0x37, 0x00, 0x0C, 
  0x00, 0x00, 0x70, 0x00, 0x06, 0x00, 0x00, 0x78, 0x00, 0x0C, 0x00, 0x00, 
  0x70, 0x00, 0xFC, 0x7B, 0xDD, 0x3F, 0x00, 0xF8, 0xFF, 0xFF, 0x3F, 0x00, 
  0x80, 0x5A, 0x56, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char tstorms_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 
  0x07, 0x00, 0x00, 0x00, 0xE0, 0x1F, 0x00, 0x00, 0x00, 0xF0, 0x7E, 0x00, 
  0x00, 0x00, 0x18, 0x68, 0x02, 0x00, 0x00, 0x1C, 0xE0, 0x3F, 0x00, 0x00, 
  0x0C, 0xD0, 0x7B, 0x00, 0x00, 0x04, 0xA0, 0xED, 0x00, 0xA0, 0x07, 0x80, 
  0xD1, 0x01, 0xF0, 0x07, 0x40, 0xE0, 0x1F, 0x18, 0x04, 0x00, 0x80, 0x3D, 
  0x0C, 0x00, 0x00, 0xA0, 0x7A, 0x0C, 0x00, 0x00, 0x00, 0x66, 0x06, 0x00, 
  0x00, 0x00, 0x78, 0x04, 0x00, 0x00, 0x00, 0x60, 0x0C, 0x00, 0x00, 0x00, 
  0x74, 0x1C, 0x11, 0x14, 0x21, 0x78, 0xF8, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 
  0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0xD7, 0x00, 0x00, 0x00, 0x00, 0xEB, 
  0x00, 0x00, 0x00, 0x80, 0xF3, 0x03, 0x00, 0x00, 0x80, 0xE5, 0x03, 0x00, 
  0x00, 0x80, 0xC5, 0x01, 0x00, 0x00, 0xC0, 0xDD, 0x01, 0x00, 0x00, 0xC0, 
  0xEF, 0x00, 0x00, 0x00, 0xC0, 0x6D, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 
  0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 
  0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x07, 
  0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 
  0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char nt_mostlycloudy_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 
  0x00, 0x00, 0x00, 0x9E, 0x0F, 0x00, 0x00, 0x00, 0xBC, 0x1E, 0x00, 0x00, 
  0x00, 0x70, 0x3B, 0x00, 0x00, 0x00, 0x70, 0xFD, 0x00, 0x00, 0x00, 0xE0, 
  0xFF, 0x01, 0x00, 0x00, 0xE0, 0xC7, 0x03, 0x00, 0x00, 0xE0, 0x81, 0x7F, 
  0x00, 0x00, 0xE0, 0x00, 0xFF, 0x00, 0x00, 0xE8, 0x00, 0xDA, 0x01, 0x00, 
  0xFC, 0x00, 0xA2, 0x1F, 0x00, 0x8E, 0x00, 0x80, 0x3D, 0x00, 0x03, 0x00, 
  0x80, 0x39, 0x00, 0x03, 0x00, 0x00, 0x74, 0x00, 0x01, 0x00, 0x00, 0x30, 
  0x00, 0x03, 0x00, 0x00, 0x70, 0xC8, 0xAF, 0xAD, 0xB9, 0x3E, 0xFC, 0xFF, 
  0xFF, 0xFF, 0x1F, 0xFC, 0xFE, 0x7F, 0xA6, 0x05, 0x78, 0xA4, 0x1E, 0x00, 
  0x00, 0xF0, 0xDB, 0x0F, 0x00, 0x00, 0xC0, 0xFF, 0x03, 0x00, 0x00, 0x80, 
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

const char nt_clear_icon[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x01, 0x00, 0x00, 
  0xC0, 0xFF, 0x07, 0x00, 0x00, 0x00, 0xAF, 0x1F, 0x00, 0x00, 0x00, 0x5E, 
  0x3E, 0x00, 0x00, 0x00, 0x9C, 0x79, 0x00, 0x00, 0x00, 0x78, 0x76, 0x00, 
  0x00, 0x00, 0xF0, 0xF9, 0x00, 0x00, 0x00, 0x70, 0xE4, 0x01, 0x00, 0x00, 
  0x70, 0xC9, 0x01, 0x00, 0x00, 0xE0, 0xB6, 0x03, 0x00, 0x00, 0xE0, 0xAA, 
  0x03, 0x00, 0x00, 0xE0, 0x98, 0x03, 0x00, 0x00, 0xE0, 0xA5, 0x03, 0x00, 
  0x00, 0xE0, 0x5A, 0x03, 0x00, 0x00, 0x70, 0x4A, 0x07, 0x00, 0x00, 0x70, 
  0x95, 0x03, 0x00, 0x00, 0xF8, 0x5A, 0x03, 0x00, 0x00, 0x38, 0xA8, 0x03, 
  0x00, 0x00, 0x3C, 0xA9, 0x03, 0x00, 0x00, 0xCF, 0x96, 0x01, 0x00, 0x80, 
  0x5F, 0xD5, 0x01, 0x80, 0xFA, 0xAB, 0xEA, 0x01, 0xC0, 0xFF, 0x59, 0xF2, 
  0x00, 0x80, 0xBF, 0x86, 0x7D, 0x00, 0x00, 0x67, 0xAA, 0x7E, 0x00, 0x00, 
  0xBE, 0x54, 0x1E, 0x00, 0x00, 0xFC, 0xF3, 0x0F, 0x00, 0x00, 0xF0, 0xFF, 
  0x07, 0x00, 0x00, 0xC0, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

String apikey   = "your Weather Underground API key";
String country  = "your country" //e.g. "UK";
String city     = "your city"    //e.g. "LONDON";
String pws      = "your pws_id"  //e.g. "pws:KIDBOISE188" - Optional

const char* host     = "api.wunderground.com";
const char* ssid     = "your SSID";
const char* password = "password";

unsigned long        lastConnectionTime = 0;          // Last time you connected to the server, in milliseconds
const unsigned long  postingInterval = 30L*60L*1000L; // Delay between updates, in milliseconds, WU allows 500 requests per-day maximum, this sets it to every 30-mins or 48/day
 
String currCondString, time_str; // strings to hold received API weather data and current time

SH1106Wire display(0x3c, 5,4); // OLED display object definition (address, SDA, SCL)
OLEDDisplayUi ui     ( &display );

WiFiClient client; // wifi client object

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //See http://www.cplusplus.com/reference/ctime/strftime/
  time_str = asctime(&timeinfo); // Displays: Sat Jun 24 14:05:49 2017
  display->setTextAlignment(TEXT_ALIGN_CENTER); // The coordinates define the center of the screen!
  if (time_str.substring(0,3) =="Sun") display->drawString(64,0,"Sunday");
  if (time_str.substring(0,3) =="Mon") display->drawString(64,0,"Monday");
  if (time_str.substring(0,3) =="Tue") display->drawString(64,0,"Tuesday");
  if (time_str.substring(0,3) =="Wed") display->drawString(64,0,"Wednesday");
  if (time_str.substring(0,3) =="Thu") display->drawString(64,0,"Thursday");
  if (time_str.substring(0,3) =="Fri") display->drawString(64,0,"Friday");
  if (time_str.substring(0,3) =="Sat") display->drawString(64,0,"Saturday");
  display->drawString(18,53,time_str.substring(4,10));
  display->drawString(107,53,time_str.substring(11,19));
  display->drawLine(0, 12, DISPLAY_WIDTH-1, 12);
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //See http://www.cplusplus.com/reference/ctime/strftime/
  time_str = asctime(&timeinfo); // Displays: Sat Jun 24 14:05:49 2017
  display->setTextAlignment(TEXT_ALIGN_CENTER); // The coordinates define the center of the screen!
  display->drawString(x+64,y+25,time_str.substring(0,16));
  display->drawString(x+64,y+35,time_str.substring(20));
  display->setFont(ArialMT_Plain_10);
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  if(CurrCondObj(&currCondString)) {
    display->setFont(ArialMT_Plain_10);
    String icon = getWeatherIcon();
    // The icons are drawn within a 40x20 box, each position can be slighly diiferent depending on icon shape
    if (icon == "snow" ||
        icon == "chanceflurries" ||
        icon == "chancesnow" ||
        icon == "flurries" ||
        icon == "nt_chanceflurries" ||
        icon == "nt_chancesnow" ||
        icon == "nt_flurries" || 
        icon == "nt_snow") display->drawXbm(x+0,y+20, icon_width, icon_height, snow_icon);
    if (icon == "rain" ||
        icon == "chancerain" ||
        icon == "nt_chancerain" ||
        icon == "nt_rain") display->drawXbm(x+0,y+20, icon_width, icon_height, rain_icon);
    if (icon == "sleet" ||
        icon == "chancesleet" ||
        icon == "nt_chancesleet" ||
        icon == "nt_sleet") display->drawXbm(x+0,y+20, icon_width, icon_height, sleet_icon);
    if (icon == "sunny" || 
        icon == "clear") display->drawXbm(x+0,y+20, icon_width, icon_height, sunny_icon);
    if (icon == "cloudy" ||
        icon == "nt_cloudy") display->drawXbm(x+0,y+20, icon_width, icon_height, cloudy_icon);
    if (icon == "mostlysunny" ||
        icon == "partlycloudy" ||
        icon == "partlysunny" ||
        icon == "mostlycloudy") display->drawXbm(x+0,y+20, icon_width, icon_height, mostlysunny_icon);
    if (icon == "fog" || 
        icon == "nt_fog" ||
        icon == "nt_hazy" ||
        icon == "hazy") display->drawXbm(x+0,y+20, icon_width, icon_height, fog_icon);
    if (icon == "tstorms" ||
        icon == "chancetstorms" ||
        icon == "nt_tstorms" || 
        icon == "nt_chancetstorms") display->drawXbm(x+0,y+20, icon_width, icon_height, tstorms_icon);
    if (icon == "nt_mostlycloudy" ||
        icon == "nt_mostlysunny" || 
        icon == "nt_partlycloudy" ||
        icon == "nt_partlysunny") display->drawXbm(x+0,y+20, icon_width, icon_height, nt_mostlycloudy_icon);
    if (icon == "nt_clear") display->drawXbm(x+0,y+20, icon_width, icon_height, nt_clear_icon);
    //display->drawRect(0,23,40,33); // Icon alignment rectangle
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(x+128,y+12,"At: "+getLocalTime().substring(0,12)+getLocalTime().substring(14,22)); 
    display->drawString(x+128,y+24,getCurrWeather());
    display->drawString(x+128,y+36,getCurrC()+"°C / "+getRelHum()); // use getCurrF for fahrenheit
  }
}

void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  if(CurrCondObj(&currCondString)) {
    display->drawString(x+64,y+12,getWindDir()+" @ "+getWindMPH()+"mph");
    display->drawString(x+64,y+24,getPressure_mb()+"mB ["+display_Ptrend(getPressure_trend())+"]"); // '-' for falling '0' for no change and '+' for rising
    display->drawString(x+64,y+36,getDewPointC()+"°C Dewpoint");
  }
}

String display_Ptrend(String indicator){
  if (indicator == "+")return "Rising";
   else if (indicator == "-") return "Falling";
     else return "Steady";
}

void drawFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  if(CurrCondObj(&currCondString)) {
    display->drawString(x+64,y+15,"Visibility: "+getVisibility_mi()+" miles");
    display->drawString(x+64,y+30,getPrecipTodayString()+" Rain");
  }
}

// This array keeps function pointers to all frames frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4 };

// how many frames are there?
int frameCount = 4;

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { msOverlay };
int overlaysCount = 1;

void setup() { 
  Serial.begin(115200);
    // The ESP is capable of rendering 60fps in 80Mhz mode but that won't give you much time for anything else run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);
  // Customize the active and inactive symbol
//  ui.setActiveSymbol(activeSymbol);
//  ui.setInactiveSymbol(inactiveSymbol);
  // You can change this to TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(BOTTOM);
  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);
  // You can change the transition that is used SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);
  // Add frames
  ui.setFrames(frames, frameCount);
  // Add overlays
  ui.setOverlays(overlays, overlaysCount);
  // Initialising the UI will init the display too.
  ui.init();
  display.flipScreenVertically();
  Start_WiFi(ssid,password);
  configTime(1, 3600, "pool.ntp.org");
  lastConnectionTime = millis();
  Call_API(&currCondString);      // Get data with an API call and place response in a String
}
 
void loop() {
  int remainingTimeBudget = ui.update();
  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
    if (millis() - lastConnectionTime > postingInterval) {
      Call_API(&currCondString);      // Get data with an API call and place response in a String
      Serial.println(currCondString); // Display the response
      lastConnectionTime = millis();
    }
    delay(remainingTimeBudget);
  }
}
 
int Start_WiFi(const char* ssid, const char* password){
 int connAttempts = 0;
 Serial.println("\r\nConnecting to: "+String(ssid));
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED ) {
   delay(500);
   Serial.print(".");
   if(connAttempts > 20) return -5;
   connAttempts++;
 }
 Serial.println("WiFi connected\r\n");
 Serial.print("IP address: ");
 Serial.println(WiFi.localIP());
 return 1;
}
 
int Call_API(String* resultString) {
  client.stop();  // Clear any current connections
  Serial.println("Connecting to "+String(host)); // start a new connection
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
   Serial.println("Connection failed");
   return false;
  }
  // Weather Underground API address http://api.wunderground.com/api/YOUR_API_KEY/conditions/q/YOUR_STATE/YOUR_CITY.json
  // Weather Underground API address for PWS http://api.wunderground.com/api/YOUR_API_KEY/conditions/q/"+pws+".json
  if (pws != "your pws_id"){
    String url ="http://api.wunderground.com/api/"+apikey+"/conditions/q/"+pws+".json";
  }
  else
  {
  String url = "http://api.wunderground.com/api/"+apikey+"/conditions/q/"+country+"/"+city+".json";
  }
  Serial.println("Requesting URL: "+String(url));
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
  "Host: " + host + "\r\n" +
  "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Connection Timeout...Stopping");
      client.stop();
      return false;
    }
  }
  Serial.print("Receiving API weather data");
  while(client.available()) {
    *(resultString) = client.readStringUntil('\r');
    Serial.print(".");
  }
  Serial.println("\r\nClosing connection");
  return true;
}

bool  api_error;
const char* CurrentObservation_location_full;
const char* CurrentObservation_location_city;
const char* CurrentObservation_location_state;
const char* CurrentObservation_location_state_name;
const char* CurrentObservation_location_zip;
const char* CurrentObservation_location_country;
const char* CurrentObservation_location_country_iso3166;
const char* CurrentObservation_latitude;
const char* CurrentObservation_longitude;
const char* CurrentObservation_elevation;
const char* CurrentObservation_local_time_rfc822;
const char* CurrentObservation_observation_time;
const char* CurrentObservation_weather;
const char* CurrentObservation_temperature_string;
const char* CurrentObservation_temp_f;
const char* CurrentObservation_temp_c;
const char* CurrentObservation_dewpoint_f;
const char* CurrentObservation_dewpoint_c;
const char* CurrentObservation_heat_index_f;
const char* CurrentObservation_heat_index_c;
const char* CurrentObservation_windchill_f;
const char* CurrentObservation_windchill_c;
const char* CurrentObservation_relative_humidity;
const char* CurrentObservation_wind_string;
const char* CurrentObservation_wind_mph;
const char* CurrentObservation_wind_kph;
const char* CurrentObservation_wind_dir;
const char* CurrentObservation_wind_degrees;
const char* CurrentObservation_visibility_mi;
const char* CurrentObservation_visibility_km;
const char* CurrentObservation_pressure_mb;
const char* CurrentObservation_pressure_in;
const char* CurrentObservation_pressure_trend;
const char* CurrentObservation_precip_today_string;
const char* CurrentObservation_precip_today_in;
const char* CurrentObservation_precip_today_metric;
const char* CurrentObservation_icon;

bool  CurrCondObj(String* currCondString){
  api_error = false;
  // When using a StaticJsonBuffer you must allocate sufficient memory for the json string returned by the WU api
  Serial.println("Creating object...");
  DynamicJsonBuffer jsonBuffer(5*1024);
  // Create root object and parse the json file returned from the api. The API returns errors and these need to be checked to ensure successful decoding
  JsonObject& root = jsonBuffer.parseObject(*(currCondString));
  if(root.success()) {
    Serial.println("root object created...");
    JsonObject& CurrentObservation = root["current_observation"];
    if(CurrentObservation.success()) {
      Serial.println("CurrentObservation object created...");
      // Now process the  "display_location":
      JsonObject& CurrentObservation_display_location     = CurrentObservation["display_location"]; // Set root location
      
      CurrentObservation_location_full            = CurrentObservation_display_location["full"]; 
      CurrentObservation_location_city            = CurrentObservation_display_location["city"];
      CurrentObservation_location_state           = CurrentObservation_display_location["state"]; 
      CurrentObservation_latitude                 = CurrentObservation_display_location["latitude"]; 
      CurrentObservation_longitude                = CurrentObservation_display_location["longitude"]; 
      CurrentObservation_elevation                = CurrentObservation_display_location["elevation"]; 
      CurrentObservation_location_state_name      = CurrentObservation_display_location["state_name"]; 
      CurrentObservation_location_country         = CurrentObservation_display_location["country"]; 
      CurrentObservation_location_country_iso3166 = CurrentObservation_display_location["country_iso3166"]; 
      CurrentObservation_location_zip             = CurrentObservation_display_location["zip"]; 
      
      CurrentObservation_local_time_rfc822        = CurrentObservation["local_time_rfc822"]; 
      CurrentObservation_observation_time         = CurrentObservation["observation_time"]; 
      CurrentObservation_weather                  = CurrentObservation["weather"]; 
      CurrentObservation_temperature_string       = CurrentObservation["temperature_string"]; 
      CurrentObservation_temp_f                   = CurrentObservation["temp_f"]; 
      CurrentObservation_temp_c                   = CurrentObservation["temp_c"]; 
      CurrentObservation_dewpoint_f               = CurrentObservation["dewpoint_f"]; 
      CurrentObservation_dewpoint_c               = CurrentObservation["dewpoint_c"]; 
      CurrentObservation_heat_index_f             = CurrentObservation["heat_index_f"]; 
      CurrentObservation_heat_index_c             = CurrentObservation["heat_index_c"]; 
      CurrentObservation_windchill_f              = CurrentObservation["windchill_f"]; 
      CurrentObservation_windchill_c              = CurrentObservation["windchill_c"];       
      CurrentObservation_relative_humidity        = CurrentObservation["relative_humidity"]; 
      CurrentObservation_wind_string              = CurrentObservation["wind_string"]; 
      CurrentObservation_wind_mph                 = CurrentObservation["wind_mph"]; 
      CurrentObservation_wind_kph                 = CurrentObservation["wind_kph"]; 
      CurrentObservation_wind_dir                 = CurrentObservation["wind_dir"]; 
      CurrentObservation_wind_degrees             = CurrentObservation["wind_degrees"]; 
      CurrentObservation_pressure_mb              = CurrentObservation["pressure_mb"]; 
      CurrentObservation_pressure_in              = CurrentObservation["pressure_in"]; 
      CurrentObservation_pressure_trend           = CurrentObservation["pressure_trend"]; 
      CurrentObservation_precip_today_string      = CurrentObservation["precip_today_string"]; 
      CurrentObservation_precip_today_in          = CurrentObservation["precip_today_in"];
      CurrentObservation_precip_today_metric      = CurrentObservation["precip_today_metric"]; 
      CurrentObservation_visibility_mi            = CurrentObservation["visibility_mi"];
      CurrentObservation_visibility_km            = CurrentObservation["visibility_km"];
      CurrentObservation_icon                     = CurrentObservation["icon"]; 
      return true;
    }
    else
    {
      // if the object could not be created, then return an error make an API call the next time around
      Serial.println("Unable to obtain current observation object");
      api_error = true;
      return false;
    }
  }
  else
  {
    // if the root object could not be created, then return an error make an API call the next time around
    Serial.println("Unable to create a root object");
    api_error = true;
    return false;
  }
}

// Functions for weather data.
String getCountry()          {return CurrentObservation_location_country;}
String getCity()             {return CurrentObservation_location_city;}
String getCityState()        {return CurrentObservation_location_full;}
String getState()            {return CurrentObservation_location_state_name;}
String getZip()              {return CurrentObservation_location_zip;}
String getLatitude()         {return CurrentObservation_latitude;}
String getLongitude()        {return CurrentObservation_longitude;}
String getElevation()        {return CurrentObservation_elevation;}; 
String getLocalTime()        {return CurrentObservation_local_time_rfc822;}
String getObsTime()          {return CurrentObservation_observation_time;}
String getCurrWeather()      {return CurrentObservation_weather;}
String getCurrTempString()   {return CurrentObservation_temperature_string;}
String getCurrF()            {return CurrentObservation_temp_f;}
String getCurrC()            {return CurrentObservation_temp_c;}
String getDewPointF()        {return CurrentObservation_dewpoint_f;}
String getDewPointC()        {return CurrentObservation_dewpoint_c;}
String getHeatindexF()       {return CurrentObservation_heat_index_f;}
String getHeatindexC()       {return CurrentObservation_heat_index_c;}
String getWindchillF()       {return CurrentObservation_windchill_f;}
String getWindchillC()       {return CurrentObservation_windchill_c;}
String getRelHum()           {return CurrentObservation_relative_humidity;}
String getWindString()       {return CurrentObservation_wind_string;}
String getWindMPH()          {return CurrentObservation_wind_mph;}
String getWindKPH()          {return CurrentObservation_wind_kph;}
String getWindDir()          {return CurrentObservation_wind_dir;}
String getWindDegrees()      {return CurrentObservation_wind_degrees;}
String getPressure_mb()      {return CurrentObservation_pressure_mb;}
String getPressure_in()      {return CurrentObservation_pressure_in;}
String getPressure_trend()   {return CurrentObservation_pressure_trend;}
String getPrecipTodayString(){return CurrentObservation_precip_today_string;}
String getPrecipTodayInches(){return CurrentObservation_precip_today_in;}
String getPrecipTodayMet()   {return CurrentObservation_precip_today_metric;}
String getVisibility_mi()    {return CurrentObservation_visibility_mi;}
String getVisibility_km()    {return CurrentObservation_visibility_km;}
String getWeatherIcon()      {return CurrentObservation_icon;}

/* Typical API response:
{
  "response": {
  "version":"0.1",
  "termsofService":"http://www.wunderground.com/weather/api/d/terms.html",
  "features": {
  "conditions": 1
  }
  }
  , "CurrentObservation": {
    "image": {
    "url":"http://icons.wxug.com/graphics/wu2/logo_130x80.png",
    "title":"Weather Underground",
    "link":"http://www.wunderground.com"
    },
    "display_location": {
    "full":"Bath, United Kingdom",
    "city":"Bath",
    "state":"WIL",
    "state_name":"United Kingdom",
    "country":"UK",
    "country_iso3166":"GB",
    "zip":"00000",
    "magic":"58",
    "wmo":"03740",
    "latitude":"51.36999893",
    "longitude":"-2.14000010",
    "elevation":"64.0"
    },
    "observation_location": {
    "full":"Bath, Bath, ",
    "city":"Bath, Bath",
    "state":"",
    "country":"GB",
    "country_iso3166":"GB",
    "latitude":"51.364689",
    "longitude":"-2.129354",
    "elevation":"131 ft"
    },
    "estimated": {
    },
    "station_id":"IBATH2",
    "observation_time":"Last Updated on June 21, 9:00 AM BST",
    "observation_time_rfc822":"Wed, 21 Jun 2017 09:00:16 +0100",
    "observation_epoch":"1498032016",
    "local_time_rfc822":"Wed, 21 Jun 2017 09:09:36 +0100",
    "local_epoch":"1498032576",
    "local_tz_short":"BST",
    "local_tz_long":"Europe/London",
    "local_tz_offset":"+0100",
    "weather":"Clear",
    "temperature_string":"70.6 F (21.4 C)",
    "temp_f":70.6,
    "temp_c":21.4,
    "relative_humidity":"82%",
    "wind_string":"From the ENE at 2.3 MPH Gusting to 6.0 MPH",
    "wind_dir":"ENE",
    "wind_degrees":71,
    "wind_mph":2.3,
    "wind_gust_mph":"6.0",
    "wind_kph":3.7,
    "wind_gust_kph":"9.7",
    "pressure_mb":"1016",
    "pressure_in":"30.01",
    "pressure_trend":"-",
    "dewpoint_string":"65 F (18 C)",
    "dewpoint_f":65,
    "dewpoint_c":18,
    "heat_index_string":"NA",
    "heat_index_f":"NA",
    "heat_index_c":"NA",
    "windchill_string":"NA",
    "windchill_f":"NA",
    "windchill_c":"NA",
    "feelslike_string":"70.6 F (21.4 C)",
    "feelslike_f":"70.6",
    "feelslike_c":"21.4",
    "visibility_mi":"7",
    "visibility_km":"11",
    "solarradiation":"--",
    "UV":"-1","precip_1hr_string":"0.00 in ( 0 mm)",
    "precip_1hr_in":"0.00",
    "precip_1hr_metric":" 0",
    "precip_today_string":"0.00 in (0 mm)",
    "precip_today_in":"0.00",
    "precip_today_metric":"0",
    "soil_temp_f": "-88.0",
    "soil_moisture": "1.0",
    "leaf_wetness": "4.0",
    "icon":"clear",
    "icon_url":"http://icons.wxug.com/i/c/k/clear.gif",
    "forecast_url":"http://www.wunderground.com/global/stations/03740.html",
    "history_url":"http://www.wunderground.com/weatherstation/WXDailyHistory.asp?ID=IMELKSHA2",
    "ob_url":"http://www.wunderground.com/cgi-bin/findweather/getForecast?query=51.364689,-2.129354",
    "nowcast":""
  }
}
*/
