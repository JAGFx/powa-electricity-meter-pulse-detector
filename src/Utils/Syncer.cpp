//
// Created by CLion.
// @author: SMITH Emmanuel <hey@emmanuel-smith.me>
// @site: https://emmanuel-smith.me
// Date: 12/02/2021
// Time: 07:40
//

#include "Syncer.h"

void Syncer::send() {
    sprintf( _data, "%s;%d", SYNC_NAME, _whCounter );
    sprintf( _request, "POST %s%s HTTP/1.1\n"
                       "Host: %s\n"
                       "Content-Type: text/plain\n"
                       "Content-Length: %d\n"
                       "\n"
                       "%s\n",
             SYNC_ENDPOINT_URI,
             SYNC_NAME,
             SYNC_ENDPOINT_HOST,
             strlen( _data ),
             _data );
    
    _client->write( _request );
//    _client->println();
    
    _waitingCount = 0;
    _result       = RESULT_NONE;
    
    while ( _client->available() == 0 ) {
        _result = RESULT_SUCCESS;
        
//        ++_waitingCount;

        if ( ++_waitingCount > WAITING_CYCLE ) {
            _client->stop();
            _result = RESULT_ERROR_SEND;
            break;
        }
    }
    
    Serial.println( _waitingCount );
    if ( _result == RESULT_SUCCESS )
        _whCounter = 0;
}

void Syncer::receive() {
    if ( resultIsNotAnError() ) {
        _responseI    = 0;
        _waitingCount = 0;
        _result       = RESULT_NONE;
        
        while ( _client->available() ) {
            _result = RESULT_SUCCESS;
            _response[ _responseI++ ] = ( char ) _client->read();
            
//            ++_waitingCount;

            if ( ++_waitingCount > WAITING_CYCLE ) {
                _result = RESULT_ERROR_RECEIVE;
                _client->stop();
                break;
            }
        }
        
        if ( resultIsNotAnError() && strstr( _response, "HTTP/1.1 200" ) != NULL )
            _result = RESULT_NONE;
        
    }
}

bool Syncer::resultIsNotAnError() const {
    return _result >= RESULT_NONE;
}

// ---

Syncer::Syncer( const uint8_t &ledPin ) : _ledPin( ledPin ) {
    _client = new WiFiClient();
}

int8_t Syncer::connect() {
    return _client->connect( SYNC_ENDPOINT_HOST, SYNC_ENDPOINT_PORT );
}

int8_t Syncer::reconnect() {
    if ( !_client->connected() ) {
        _client->stop();
        return connect();
    }
    
    return 0;
}

uint8_t Syncer::sync() {
    _whCounter++;
    
    if ( enableToSync() ) {
        _cycleCounter = 0;
        _waitingCount = 0;
    
        digitalWrite( _ledPin, HIGH );
        
        reconnect();
        send();
        receive();
        
        Serial.println( _result );
        
    } else
        delay( 50 );
    
    digitalWrite( _ledPin, LOW );
    
    return _result;
}

bool Syncer::reset() {
    _whCounter = 0;
    return true;
}

// ---

bool Syncer::enableToSync() const {
    return _cycleCounter >= Syncer::CYCLE;
}

void Syncer::addCycle() {
    _cycleCounter++;
}

// ---

uint8_t Syncer::getWhCounter() const {
    return _whCounter;
}
