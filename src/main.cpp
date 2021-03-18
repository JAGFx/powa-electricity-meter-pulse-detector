#include <Arduino.h>

#include <ESP8266WiFi.h>

#include <Settings.h>
#include <Utils/Syncer.h>
#include <Utils/Oled.h>

// ----------------------
// ---- Modules

// -- OLED Scren
Oled oled;

// -- Luminosity sensor
#define LUX_PIN A0
#define LUX_DEBOUNCE_CYCLE 10
#define LUX_OFFSET 10

uint8_t luxCycle         = 0;

// -- Reset wh count
#define RST_WH_BUTTON_PIN D6

// -- Led WH pulse
#define WH_PULSE_LED_PIN D7

// -- Wifi
#define WIFI_MAX_TRY 10

IPAddress ip( 192, 168, 1, 70 );
IPAddress gateway( 192, 168, 1, 1 );
IPAddress subnet( 255, 255, 255, 0 );
uint8_t   wifiCounterTry = 0;

// -- Sync
#define SYNC_LED_PIN D8

Syncer syncer( SYNC_LED_PIN );

// ---- ./Modules
// ----------------------


// ----------------------
// ---- Common method

/**
 * Detect a pulse from you electricity meter. Each pulse = 1 Wh in France
 */
void detectPulseChange() {
    // --- Lux
    uint8_t currentLux = analogRead( LUX_PIN );
//    Serial.print( currentLux );
//    Serial.print( " | " );
//    Serial.println( luxCycle );
    
    if ( luxCycle++ > LUX_DEBOUNCE_CYCLE && currentLux > LUX_OFFSET ) {
        luxCycle = 0;
        digitalWrite( WH_PULSE_LED_PIN, HIGH );
        
        // --- Process here
        oled.whIncrease();
        syncer.increaseWhCounter();
        delay( 50 );
        // --- ./Process here
        
        digitalWrite( WH_PULSE_LED_PIN, LOW );
    }
    
    // --- Sync
    syncer.sync();
}

// ---- ./Common method
// ----------------------


// ----------------------
// ---- Interrupt

/**
 * Interupt for whReset pressed button
 */
void IRAM_ATTR onResetValue() {
    luxCycle = 0;
    oled.whReset();
}

// ---- ./Interrupt
// ----------------------

// ----- Minimal functions

void setup() {
    Serial.begin( 115200 );
    delay( 1500 );
    
    // ---- Reset WH count button
    pinMode( RST_WH_BUTTON_PIN, INPUT );
    attachInterrupt( RST_WH_BUTTON_PIN, onResetValue, RISING );
    
    // ---- LED Pulse
    pinMode( WH_PULSE_LED_PIN, OUTPUT );
    digitalWrite( WH_PULSE_LED_PIN, LOW );
    
    // ---- OLED Screen
    oled.begin();
    delay( 500 );
    
    // ---- Lux sensor
    oled.printLn( "Sensor init..." );
    pinMode( LUX_PIN, INPUT );
    delay( 500 );
    
    // ---- Wifi
    oled.printLn( "Wifi init..." );
    WiFi.config( ip, gateway, subnet );
    WiFi.begin( WIFI_SSID, WIFI_PSWD );
    
    while ( WiFi.status() != WL_CONNECTED ) {
        oled.print( "Wifi connecting #" );
        oled.printLn( ++wifiCounterTry, false );
        
        if ( wifiCounterTry > WIFI_MAX_TRY ) {
            wifiCounterTry = 0;
            WiFi.begin( WIFI_SSID, WIFI_PSWD );
            oled.printLn( "Wifi: Reset" );
        }
        
        delay( 500 );
    }
    
    oled.print( "Wifi: " );
    oled.printLn( WiFi.localIP(), false );
    delay( 1000 );
    
    // ---- Sync
    pinMode( SYNC_LED_PIN, OUTPUT );
    digitalWrite( SYNC_LED_PIN, LOW );
    
    syncer.connect();
}

void loop() {
    // ---- Wifi connexion
    if ( WiFi.status() != WL_CONNECTED )
        ESP.restart();
    
    // ---- Lux senor
    detectPulseChange();
    
    // ---- Display WH count
    oled.loop( syncer.getWhCounter() );
    
    delay( 100 );
//    Serial.println( ESP.getFreeHeap() );
}
