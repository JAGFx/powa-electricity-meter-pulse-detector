#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_SSD1306.h>
#include <BH1750.h>

// ----------------------
// ---- Modules

// -- OLED Scren
Adafruit_SSD1306 display = Adafruit_SSD1306( 128, 32, &Wire );

// -- Luminosity sensor
BH1750    luxSensor; // instantiate a sensor event object
const int LUX_OFFSET     = 0;
int       lastLux        = -1;

// -- Reset wh count
#define RST_WH_DEBOUNCE_TIME 250
#define RST_WH_BUTTON_PIN 34
volatile uint32_t resetDebouncerTimer = 0;
uint32_t          resetButtonCount    = 0;

// -- Led WH pulse
#define WH_PULSE_LED_PIN 12
int           ledWhPulseState    = LOW;
unsigned long lastPulseMillis    = 0;
const long    ledWhPulseInterval = 50;
bool          scheduleLedWhPulse = false;

// ---- ./Modules
// ----------------------

volatile float whCount = 0;

// ----------------------
// ---- Common method

/**
 * Detect a pulse from you electricity meter. Each pulse = 1 Wh in France
 */
void detectPulseChange() {
    int currentLux = ( int ) luxSensor.readLightLevel();
    
    if ( ( currentLux - LUX_OFFSET ) > 0 && currentLux != lastLux ) {
        currentLux++;
        lastLux            = currentLux;
        scheduleLedWhPulse = true;
    }
}

/**
 * Return the unit Wh, KWh or MWh
 *
 * @return string Unit for the current whCount value
 */
const char *getUnit() {
    if ( whCount <= 1000 ) return "Wh";
    else if ( whCount <= 10000 ) return "KWh";
    else return "MWh";
}

/**
 * Format value to fit the value on the OLED screen
 *
 * @return float
 */
float getValue() {
    if ( whCount < 1000 ) return whCount;
    else if ( whCount < 10000 ) return whCount / 1000;
    else return whCount / 10000;
}

/**
 * Get the float precision to fit the value on OLED screen
 *
 * @return int
 */
int getPrecision() {
    if ( getValue() < 10 ) return 3;
    else if ( getValue() < 100 ) return 2;
    else if ( getValue() < 1000 ) return 1;
    else return 0;
}

/**
 * Update the state of the led pulse
 */
void updateLedPulseState() {
    unsigned long currentMillis = millis();
    
    if ( currentMillis - lastPulseMillis >= ledWhPulseInterval && scheduleLedWhPulse ) {
        // save the lastLux time you blinked the LED
        lastPulseMillis = currentMillis;
        
        // if the LED is off turn it on and vice-versa:
        if ( ledWhPulseState == LOW )
            ledWhPulseState = HIGH;
        
        else {
            ledWhPulseState    = LOW;
            scheduleLedWhPulse = false;
        }
        
        // set the LED with the ledWhPulseState of the variable:
        digitalWrite( WH_PULSE_LED_PIN, ledWhPulseState );
    }
}

// ---- ./Common method
// ----------------------


// ----------------------
// ---- Interrupt

/**
 * Interupt for reset pressed button
 */
void IRAM_ATTR onResetValue() {
    if ( millis() - RST_WH_DEBOUNCE_TIME >= resetDebouncerTimer ) {
        resetDebouncerTimer = millis();
        resetButtonCount += 1;
        whCount             = 0;
//        Serial.printf( "Button has been pressed %u times\n", resetButtonCount );
    }
}

// ---- ./Interrupt
// ----------------------

// ----- Minimal functions

void setup() {
    Serial.begin( 9600 );
    
    // ---- Reset WH count button
    pinMode( RST_WH_BUTTON_PIN, INPUT_PULLDOWN );
    attachInterrupt( RST_WH_BUTTON_PIN, onResetValue, RISING );
    
    // ---- LED Pulse
    pinMode( WH_PULSE_LED_PIN, OUTPUT );
    
    // ---- Lux sensor
    luxSensor.begin( BH1750::CONTINUOUS_LOW_RES_MODE );
    
    // ---- OLED Screen
    display.begin( SSD1306_SWITCHCAPVCC, 0x3C ); // Address 0x3C for 128x32
    display.display();
    delay( 1000 );
    display.clearDisplay();
    display.display();
    display.setTextSize( 3 );
    display.setTextColor( SSD1306_WHITE );
    display.setCursor( 0, 0 );
    display.println( "Init..." );
    display.setCursor( 0, 0 );
    display.display(); // actually display all of the above
    delay( 2000 );
}

void loop() {
    // ---- Lux senor
    detectPulseChange();
    
    // ---- Display WH count
    display.clearDisplay();
    display.setTextSize( 3 );
    display.print( getValue(), getPrecision() );
    display.setTextSize( 2 );
    display.println( getUnit() );
    display.setCursor( 0, 0 );
    display.display(); // actually display all of the above1
    delay( 100 );
    
    // ---- Reset button
    if ( resetButtonCount >= 5 ) {
        detachInterrupt( RST_WH_BUTTON_PIN );
        // reset click counter to avoid re-enter here
        resetButtonCount = 0;
    }
    
    // ---- Update pulse LED
    updateLedPulseState();
}
