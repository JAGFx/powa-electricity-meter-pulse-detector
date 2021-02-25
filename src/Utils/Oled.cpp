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
const char *Oled::getUnit() const {
    if ( _whCount > 1000000 ) return UNIT_MWH;
    else if ( _whCount > 1000 ) return UNIT_KWH;
    else return UNIT_WH;
}

/**
 * Format value to fit the value on the OLED screen
 *
 * @return float
 */
float Oled::getValue() const {
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
    _screen = new Adafruit_SSD1306( SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT, &Wire );
}

// --

void Oled::begin() {
    _screen->begin( SSD1306_SWITCHCAPVCC, 0x3C ); // Address 0x3C for 128x32
    _screen->setTextSize( 1 );
    _screen->setTextColor( SSD1306_WHITE );
    printLn( "Init..." );
}

void Oled::loop( uint16_t syncerCount ) {
    // FIXME: Crash on it
    _screen->clearDisplay();
    _screen->setCursor( 0, 0 );
    _screen->setTextSize( 3 );
    _screen->print( getValue(), getPrecision() );
    _screen->setTextSize( 2 );
    _screen->println( getUnit() );
    _screen->setCursor( 0, 25 );
    _screen->setTextSize( 1 );
    _screen->print( "Sync:" );
    _screen->print( syncerCount );
    _screen->print( " WH:" );
    _screen->println( _whCount, 0 );
    _screen->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param str
 * @param clear
 */
void Oled::print( const char *str, bool clear ) {
    if ( clear ) {
        _screen->clearDisplay();
        _screen->setCursor( 0, 0 );
    }
    
    _screen->print( str );
    _screen->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param str
 * @param clear
 */
void Oled::printLn( const char *str, bool clear ) {
    if ( clear ) {
        _screen->clearDisplay();
        _screen->setCursor( 0, 0 );
    }
    
    _screen->println( str );
    _screen->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param x
 * @param clear
 */
void Oled::printLn( const Printable &x, bool clear ) {
    if ( clear ) {
        _screen->clearDisplay();
        _screen->setCursor( 0, 0 );
    }
    
    _screen->println( x );
    _screen->display();
}

/**
 * Method to whReset oled screen and display a new text
 *
 * @param b
 * @param clear
 */
void Oled::printLn( unsigned char b, bool clear ) {
    if ( clear ) {
        _screen->clearDisplay();
        _screen->setCursor( 0, 0 );
    }
    
    _screen->println( b );
    _screen->display();
}

void Oled::whIncrease() {
    _whCount++;
}

void Oled::whReset() {
    _whCount = 0;
}
