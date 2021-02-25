//
// Created by CLion.
// @author: SMITH Emmanuel <hey@emmanuel-smith.me>
// @site: https://emmanuel-smith.me
// Date: 12/02/2021
// Time: 07:40
//

#include "Syncer.h"

void Syncer::send() {
    resetRequest();
    
    sprintf( _data, REQUEST_DATA_TEMPLATE, SYNC_NAME, _whCounter );
    sprintf( _request, REQUEST_TEMPLATE,
             SYNC_ENDPOINT_URI,
             SYNC_NAME,
             SYNC_ENDPOINT_HOST,
             strlen( _data ),
             _data );
    
    _client->write( _request );
    
    uint8_t cAvailable = _client->available();
    
    while ( !cAvailable ) {
        _result    = RESULT_SUCCESS;
        cAvailable = _client->available();
        
        if ( ++_waitingCount > WAITING_CYCLE ) {
            _client->stop();
            _result = RESULT_ERROR_SEND;
            break;
        }
    }

//    Serial.print( "send | " );
//    Serial.print( "_whCounter: " );
//    Serial.print( _whCounter );
//    Serial.print( " | _waitingCount: " );
//    Serial.print( _waitingCount );
//    Serial.print( " | _result: " );
//    Serial.println( _result );
    
    if ( _result == RESULT_SUCCESS )
        _whCounter = 0;
}

void Syncer::receive() {
    if ( resultIsNotAnError() ) {
        resetRequest();

//        uint8_t cAvailable = _client->available();
        int8 cRead = _client->read();
    
        do {
            _response[ _responseI++ ] = ( char ) cRead;
            cRead = _client->read();
//            cAvailable = _client->available();
        
            if ( ++_waitingCount > WAITING_CYCLE ) {
                _result = RESULT_ERROR_RECEIVE;
                _client->stop();
                break;
            }
        
        } while ( cRead != -1 );
    
        if ( resultIsNotAnError() && strstr( _response, RESPONSE_HEADER_OK ) != nullptr )
            _result = RESULT_SUCCESS;

//        Serial.print( "receive | " );
//        Serial.print( _response );
//        Serial.print( " | _waitingCount: " );
//        Serial.print( _waitingCount );
//        Serial.print( " | _result: " );
//        Serial.println( _result );
    }
}

bool Syncer::resultIsNotAnError() const {
    return _result >= RESULT_NONE;
}

void Syncer::resetRequest() {
    _cycleCounter = 0;
    
    memset( _request, 0, strlen( _request ) );
    memset( _data, 0, strlen( _data ) );
    memset( _response, 0, strlen( _response ) );
    
    _responseI    = 0;
    _waitingCount = 0;
    _result       = RESULT_NONE;
}

// ---

Syncer::Syncer( const uint8_t &ledPin ) : _ledPin( ledPin ) {
    _client = new WiFiClient();
}

int8_t Syncer::connect() {
    return _client->connect( SYNC_ENDPOINT_HOST, SYNC_ENDPOINT_PORT );
}

bool Syncer::reconnect() {
    if ( !_client->connected() ) {
        _client->stop();
        return connect();
    }
    
    return true;
}

void Syncer::sync() {
    _cycleCounter++;

//    Serial.println( _cycleCounter );
    
    if ( enableToSync() ) {
        digitalWrite( _ledPin, HIGH );
        
        resetRequest();
        
        if ( reconnect() ) {
            send();
            receive();
        }

//        Serial.print( "_client->connected(): " );
//        Serial.print( _client->connected() );
//        Serial.print( " | _result: " );
//        Serial.println( _result );
        
        digitalWrite( _ledPin, LOW );
    }
}

// ---

bool Syncer::enableToSync() const {
    return _cycleCounter >= Syncer::CYCLE && _whCounter > 0;
}


void Syncer::increaseWhCounter() {
    _whCounter++;
}

// ---

uint16_t Syncer::getWhCounter() const {
    return _whCounter;
}
