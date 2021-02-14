//
// Created by CLion.
// @author: SMITH Emmanuel <hey@emmanuel-smith.me>
// @site: https://emmanuel-smith.me
// Date: 14/02/2021
// Time: 11:31
//

#ifndef CONSUMPTIONSNIFFER_OLED_H
#define CONSUMPTIONSNIFFER_OLED_H

#include <Arduino.h>
//#include <Wire.h>
//#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>

class Oled {
private:
    Adafruit_SSD1306 *_screen;
    float            _whCount = 0;
    
    // --
    
    const char *getUnit() const;
    
    float getValue() const;
    
    uint8_t getPrecision();

public:
    static constexpr const char *UNIT_WH       = " Wh";
    static constexpr const char *UNIT_KWH      = " KWh";
    static constexpr const char *UNIT_MWH      = " MWh";
    static const uint8_t        PRECISION_10   = 2;
    static const uint8_t        PRECISION_100  = 1;
    static const uint8_t        PRECISION_1000 = 0;
    
    // --
    
    explicit Oled();
    
    // --
    void begin();
    
    void loop( uint16_t syncerCount );
    
    void print( const char *str, bool clear = true );
    
    void printLn( const char *str, bool clear = true );
    
    void printLn( const Printable &x, bool clear = true );
    
    void printLn( unsigned char b, bool clear = true );
    
    void whIncrease();
    
    void whReset();
};


#endif //CONSUMPTIONSNIFFER_OLED_H
