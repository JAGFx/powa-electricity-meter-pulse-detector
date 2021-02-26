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
    resetResultSend();
    
    sprintf( _data, REQUEST_DATA_TEMPLATE, SYNC_NAME, _whCounter );
    sprintf( _request, REQUEST_TEMPLATE,
             SYNC_ENDPOINT_URI,
             SYNC_ENDPOINT_HOST,
             strlen( _data ),
             _data );
    
    _client->write( _request );
    
    uint8_t cAvailable = _client->available();
    
    while ( !cAvailable ) {
        _resultSend = RESULT_SUCCESS;
        cAvailable  = _client->available();
    
        if ( ++_waitingCount > WAITING_CYCLE ) {
            _client->stop();
            _resultSend = RESULT_ERROR_SEND;
            break;
        }
    }

//    Serial.print( "send | " );
//    Serial.print( "_whCounter: " );
//    Serial.print( _whCounter );
//    Serial.print( " | _waitingCount: " );
//    Serial.println( _waitingCount );
}

void Syncer::receive() {
    if ( _resultSend >= RESULT_NONE ) {
        resetRequest();
        resetResultReceive();
        
        int8 cRead = _client->read();
        
        do {
            _response[ _responseI++ ] = ( char ) cRead;
            cRead = _client->read();
            
            if ( ++_waitingCount > WAITING_CYCLE ) {
                _resultReceive = RESULT_ERROR_RECEIVE;
                _client->stop();
                break;
            }
            
        } while ( cRead != -1 && cRead != '\n' );
        
        if ( _resultReceive >= RESULT_NONE && strstr( _response, RESPONSE_HEADER_OK ) != nullptr )
            _resultReceive = RESULT_SUCCESS;

//        Serial.print( "receive | " );
//        Serial.print( _response );
//        Serial.print( " | _waitingCount: " );
//        Serial.println( _waitingCount );
    }
}

void Syncer::resetRequest() {
    _cycleCounter = 0;
    
    memset( _request, 0, strlen( _request ) );
    memset( _data, 0, strlen( _data ) );
    memset( _response, 0, strlen( _response ) );
    
    _responseI    = 0;
    _waitingCount = 0;
}

void Syncer::resetResultSend() {
    _resultSend = RESULT_NONE;
}

void Syncer::resetResultReceive() {
    _resultReceive = RESULT_NONE;
}

void Syncer::resetWhCounter() {
    if ( _resultSend == RESULT_SUCCESS && _resultReceive == RESULT_ERROR_RECEIVE )
        _whCounter = 0;
    
    if ( _resultSend == RESULT_SUCCESS && _resultReceive == RESULT_SUCCESS )
        _whCounter = 0;
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
        resetResultSend();
        resetResultReceive();
    
        if ( reconnect() ) {
            send();
            receive();
            resetWhCounter();
        }

//        Serial.print( "_client->connected(): " );
//        Serial.print( _client->connected() );
//        Serial.print( " | _resultSend: " );
//        Serial.println( _resultSend );
//        Serial.print( " | _resultReceive: " );
//        Serial.println( _resultReceive );
    
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
