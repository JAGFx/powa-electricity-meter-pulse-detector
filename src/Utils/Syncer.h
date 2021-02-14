//
// Created by CLion.
// @author: SMITH Emmanuel <hey@emmanuel-smith.me>
// @site: https://emmanuel-smith.me
// Date: 12/02/2021
// Time: 07:40
//

#ifndef CONSUMPTIONSNIFFER_SYNCER_H
#define CONSUMPTIONSNIFFER_SYNCER_H

#include <Arduino.h>
#include <WiFi.h>
#include "../Settings.h"

class Syncer {
private:
    uint8_t    _ledPin;
    WiFiClient *_client;
    
    // --
    
    uint16_t _cycleCounter  = 0;
    uint16_t  _whCounter     = 0;
    char     _request[128]  = { 0 };
    char     _data[16]      = { 0 };
    char     _response[310] = { 0 };
    uint16_t _responseI     = 0;
    uint16_t _waitingCount  = 0;
    int8_t   _result        = RESULT_NONE;
    
    // --
    
    void send();
    
    void receive();
    
    bool resultIsNotAnError() const;
    
    bool resetRequest();

public:
    static const uint16_t CYCLE         = 50; // Test
//    static const uint16_t CYCLE         = 500;
    static const uint16_t WAITING_CYCLE = 3500;
    
    static const int8_t RESULT_ERROR_RECEIVE = -2;
    static const int8_t RESULT_ERROR_SEND    = -1;
    static const int8_t RESULT_NONE          = 0;
    static const int8_t RESULT_SUCCESS       = 1;
    
    static constexpr const char* RESPONSE_HEADER_OK    = "HTTP/1.1 200";
    static constexpr const char* REQUEST_DATA_TEMPLATE = "%s;%d";
    static constexpr const char* REQUEST_TEMPLATE      = "POST %s%s HTTP/1.1\n"
                                              "Host: %s\n"
                                              "Content-Type: text/plain\n"
                                              "Content-Length: %d\n"
                                              "\n"
                                              "%s\n";
    
    // --
    
    explicit Syncer( const uint8_t &ledPin );
    
    // --
    
    int8_t connect();
    
    bool reconnect();
    
    uint8_t sync();
    
    // ---
    
    bool enableToSync() const;
    
    void addCycle();
    
    // ---
    
    uint16_t getWhCounter() const;
    
};


#endif //CONSUMPTIONSNIFFER_SYNCER_H
