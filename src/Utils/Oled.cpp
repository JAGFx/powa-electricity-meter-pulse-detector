//
// Created by CLion.
// @author: SMITH Emmanuel <hey@emmanuel-smith.me>
// @site: https://emmanuel-smith.me
// Date: 14/02/2021
// Time: 11:31
//

#include "Oled.h"

/**
 * Return the unit Wh, KWh or MWh
 *
 * @return string Unit for the current whCount value
 */
const char *Oled::getUnit() {
    if ( _whCount > 1000000 ) return UNIT_MWH;
    else if ( _whCount > 1000 ) return UNIT_KWH;
    else return UNIT_WH;
}

/**
 * Format value to fit the value on the OLED screen
 *
 * @return float
 */
float Oled::getValue() {
    if ( _whCount > 1000000 ) return _whCount / 1000000;
    else if ( _whCount > 1000 ) return _whCount / 1000;
    else return _whCount;
}

/**
 * Get the float precision to fit the value on OLED screen
 *
 * @return int
 */
uint8_t Oled::getPrecision() {
    if ( getValue() < 10 ) return PRECISION_10;
    else if ( getValue() < 100 ) return PRECISION_100;
//    else if ( getValue() < 1000 ) return 1;
    else return PRECISION_1000;
}

// --

Oled::Oled() {
    _display = new Adafruit_SSD1306( 128, 32, &Wire );
}

// --

void Oled::begin() {
    _display->begin( SSD1306_SWITCHCAPVCC, 0x3C ); // Address 0x3C for 128x32
    _display->setTextSize( 1 );
    _display->setTextColor( SSD1306_WHITE );
    printLn( "Init..." );
}

void Oled::loop( uint8_t syncerCount ) {
    _display->clearDisplay();
    _display->setCursor( 0, 0 );
    _display->setTextSize( 3 );
    _display->print( getValue(), getPrecision() );
    _display->setTextSize( 2 );
    _display->println( getUnit() );
    _display->setCursor( 0, 25 );
    _display->setTextSize( 1 );
    _display->print( "Sync:" );
    _display->print( syncerCount );
    _display->print( " WH:" );
    _display->println( _whCount, 0 );
    _display->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param str
 * @param clear
 */
void Oled::print( const char *str, bool clear ) {
    if ( clear ) {
        _display->clearDisplay();
        _display->setCursor( 0, 0 );
    }
    
    _display->print( str );
    _display->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param str
 * @param clear
 */
void Oled::printLn( const char *str, bool clear ) {
    if ( clear ) {
        _display->clearDisplay();
        _display->setCursor( 0, 0 );
    }
    
    _display->println( str );
    _display->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param x
 * @param clear
 */
void Oled::printLn( const Printable &x, bool clear ) {
    if ( clear ) {
        _display->clearDisplay();
        _display->setCursor( 0, 0 );
    }
    
    _display->println( x );
    _display->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param b
 * @param clear
 */
void Oled::printLn( unsigned char b, bool clear ) {
    if ( clear ) {
        _display->clearDisplay();
        _display->setCursor( 0, 0 );
    }
    
    _display->println( b );
    _display->display();
}

void Oled::whIncrease() {
    _whCount++;
}

void Oled::whReset() {
    _whCount = 0;
}
