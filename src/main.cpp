#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <BH1750.h>
#include <WiFi.h>

#include <Settings.h>
#include <Utils/Syncer.h>

// ----------------------
// ---- Modules

// -- OLED Scren
#define DISPLAY_UNIT_WH " Wh"
#define DISPLAY_UNIT_KWH " KWh"
#define DISPLAY_UNIT_MWH " MWh"
#define DISPLAY_PRECISION_10 2
#define DISPLAY_PRECISION_100 1
#define DISPLAY_PRECISION_1000 0

Adafruit_SSD1306 display = Adafruit_SSD1306( 128, 32, &Wire );

// -- Luminosity sensor
BH1750        luxSensor; // instantiate a sensor event object
const uint8_t LUX_OFFSET = 0;
uint8_t       lastLux    = -1;

// -- Reset wh count
#define RST_WH_BUTTON_PIN 34

// -- Led WH pulse
#define WH_PULSE_LED_PIN 12

// -- Wifi
#define WIFI_MAX_TRY 10

IPAddress ip( 192, 168, 1, 70 );
IPAddress gateway( 192, 168, 1, 1 );
IPAddress subnet( 255, 255, 255, 0 );
uint8_t   wifiCounterTry = 0;

// -- Sync
#define SYNC_LED_PIN 2

Syncer syncer( SYNC_LED_PIN );

// ---- ./Modules
// ----------------------


volatile float whCount = 0;

// ----------------------
// ---- Common method

/**
 * Detect a pulse from you electricity meter. Each pulse = 1 Wh in France
 */
void detectPulseChange() {
    // ---- Sync
    syncer.addCycle();
    
    // ---- Lux
    uint8_t currentLux = ( int ) luxSensor.readLightLevel();
    
    if ( ( currentLux - LUX_OFFSET ) > 0 && currentLux != lastLux ) {
        digitalWrite( WH_PULSE_LED_PIN, HIGH );

//        whCount++;
        whCount += 126;
        lastLux = currentLux;
    
        // --- Sync here
        syncer.sync();
        // --- ./Sync here-
        
        digitalWrite( WH_PULSE_LED_PIN, LOW );
        // --- ./Process here
    }
}

/**
 * Return the unit Wh, KWh or MWh
 *
 * @return string Unit for the current whCount value
 */
const char *getUnit() {
    if ( whCount <= 1000 ) return DISPLAY_UNIT_WH;
    else if ( whCount <= 100000 ) return DISPLAY_UNIT_KWH;
    else return DISPLAY_UNIT_MWH;
}

/**
 * Format value to fit the value on the OLED screen
 *
 * @return float
 */
float getValue() {
    if ( whCount < 1000 ) return whCount;
    else if ( whCount < 100000 ) return whCount / 1000;
    else return whCount / 10000;
}

/**
 * Get the float precision to fit the value on OLED screen
 *
 * @return int
 */
int getPrecision() {
    if ( getValue() < 10 ) return DISPLAY_PRECISION_10;
    else if ( getValue() < 100 ) return DISPLAY_PRECISION_100;
//    else if ( getValue() < 1000 ) return 1;
    else return DISPLAY_PRECISION_1000;
}

/**
 * Method to reset oled screen and display a new text
 *
 * @param str
 * @param clear
 */
void oledPrint( const char *str, bool clear = true ) {
    if ( clear ) {
        display.clearDisplay();
        display.setCursor( 0, 0 );
    }
    
    display.print( str );
    display.display();
}

/**
 * Method to reset oled screen and display a new text
 *
 * @param str
 * @param clear
 */
void oledPrintLn( const char *str, bool clear = true ) {
    if ( clear ) {
        display.clearDisplay();
        display.setCursor( 0, 0 );
    }
    
    display.println( str );
    display.display();
}

/**
 * Method to reset oled screen and display a new text
 *
 * @param x
 * @param clear
 */
void oledPrintLn( const Printable &x, bool clear = true ) {
    if ( clear ) {
        display.clearDisplay();
        display.setCursor( 0, 0 );
    }
    
    display.println( x );
    display.display();
}

/**
 * Method to reset oled screen and display a new text
 *
 * @param b
 * @param clear
 */
void oledPrintLn( unsigned char b, bool clear = true ) {
    if ( clear ) {
        display.clearDisplay();
        display.setCursor( 0, 0 );
    }
    
    display.println( b );
    display.display();
}

// ---- ./Common method
// ----------------------


// ----------------------
// ---- Interrupt

/**
 * Interupt for reset pressed button
 */
void IRAM_ATTR onResetValue() {
    whCount       = 0;
    lastLux       = 0;
    
    syncer.reset();
}

// ---- ./Interrupt
// ----------------------

// ----- Minimal functions

void setup() {
    Serial.begin( 115200 );
    delay( 1500 );
    
    // ---- Reset WH count button
    pinMode( RST_WH_BUTTON_PIN, INPUT_PULLDOWN );
    attachInterrupt( RST_WH_BUTTON_PIN, onResetValue, RISING );
    
    // ---- LED Pulse
    pinMode( WH_PULSE_LED_PIN, OUTPUT );
    digitalWrite( WH_PULSE_LED_PIN, LOW );
    
    // ---- OLED Screen
    display.begin( SSD1306_SWITCHCAPVCC, 0x3C ); // Address 0x3C for 128x32
    display.setTextSize( 1 );
    display.setTextColor( SSD1306_WHITE );
    oledPrintLn( "Init..." );
    delay( 500 );
    
    // ---- Lux sensor
    oledPrintLn( "Sensor init..." );
    luxSensor.begin();
    delay( 500 );
    
    // ---- Wifi
    oledPrintLn( "Wifi init..." );
    WiFi.config( ip, gateway, subnet );
    WiFi.begin( WIFI_SSID, WIFI_PSWD );
    
    while ( WiFi.status() != WL_CONNECTED ) {
        oledPrint( "Wifi connecting #" );
        oledPrintLn( ++wifiCounterTry, false );
        
        if ( wifiCounterTry > WIFI_MAX_TRY ) {
            wifiCounterTry = 0;
            WiFi.begin( WIFI_SSID, WIFI_PSWD );
            oledPrintLn( "Wifi: Reset" );
        }
        
        delay( 500 );
    }
    
    oledPrint( "Wifi: " );
    oledPrintLn( WiFi.localIP(), false );
    delay( 1000 );
    
    // ---- Sync
    pinMode( SYNC_LED_PIN, OUTPUT );
    digitalWrite( SYNC_LED_PIN, LOW );
    
    syncer.connect();
    
    display.setTextSize( 3 );
}

void loop() {
    // ---- Wifi connexion
    if ( WiFi.status() != WL_CONNECTED )
        ESP.restart();
    
    // ---- Lux senor
    detectPulseChange();
    
    // ---- Display WH count
    display.clearDisplay();
    display.setCursor( 0, 0 );
    display.setTextSize( 3 );
    display.print( getValue(), getPrecision() );
    display.setTextSize( 2 );
    display.println( getUnit() );
    display.setCursor( 0, 25 );
    display.setTextSize( 1 );
    display.print( "Sync:" );
    display.print( syncer.getWhCounter() );
    display.print( " WH:" );
    display.println( whCount, 0 );
    display.display(); // actually display all of the above1
    
    delay( 100 );
//    Serial.println( ESP.getFreeHeap() );
}
