/*
  Organ.cpp - Library for scanning organ keys and output via MIDI.
  Created by Matthew Foote, December 2025.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Organ.h"

//Organ::Organ(uint8_t *gOut, uint8_t n1, uint8_t *sOut, uint8_t n2, uint8_t *pOut, uint8_t n3, uint8_t *gsIn, uint8_t n4, uint8_t *pIn, uint8_t n5)
//Organ::Organ(uint8_t *gOut, uint8_t *sOut, uint8_t *pOut, uint8_t *gsIn, uint8_t *pIn, void (*on)(byte, byte, byte), void (*off)(byte, byte, byte))
Organ::Organ(void (*on)(byte, byte, byte), void (*off)(byte, byte, byte)) {
    _noteOn = on;
    _noteOff = off;
}

void Organ::begin() {
    for(uint8_t i = 0; i < NUM_KEYBOARDS; i++) {
        for(uint8_t j = 0; j < 15; j++) {
            if(_pinsIn[i][j] != 0)
                pinMode(_pinsIn[i][j], INPUT_PULLUP);

            if(_pinsOut[i][j] != 0)
                pinMode(_pinsOut[i][j], OUTPUT);
        }
    } 
}

void Organ::setKeyboard(uint8_t num, uint8_t offset, uint8_t *in, uint8_t n1, uint8_t *out, uint8_t n2) {
    memcpy(_pinsIn[num], in, n1);
    memcpy(_pinsOut[num], out, n2);
    _inCnt[num]  = n1;
    _outCnt[num] = n2;
    _offset[num] = offset;
}

void Organ::scan(uint8_t kbd) {
    for(uint8_t i = 0; i < _outCnt[kbd]; i++) {
        digitalWrite(_pinsOut[kbd][i], LOW);
        for(uint8_t j = 0; j < _inCnt[kbd]; j++) {
            //Should work. Only problem might be from scanning _pinsIn[X][0] too much
            if(digitalRead(_pinsIn[kbd][j]) == LOW) {
                kbdOn(kbd, (_offset[kbd] + i + (j * 6)));
            }
            else {
                kbdOff(kbd, (_offset[kbd] + i + (j * 6)));
            }
        }
        digitalWrite(_pinsOut[kbd][i], HIGH);
    }

}

void Organ::kbdOn(uint8_t kbd, uint8_t noteNumber) {
    if(noteNumber < 70) {
        if(_debounceArray[kbd][noteNumber] == 0) {
            _noteOn(kbd, noteNumber, 127);
            _debounceArray[kbd][noteNumber] = debounceCount;
        }
    }
}

void Organ::kbdOff(uint8_t kbd, uint8_t noteNumber) {
    if(noteNumber < 70) {
        if(_debounceArray[kbd][noteNumber] == 1) {
            _noteOff(kbd, noteNumber, 0);
        }
        if(_debounceArray[kbd][noteNumber] > 0)
	    _debounceArray[kbd][noteNumber] -- ;
    }
}

void Organ::scanGreat() {
    scan(1);
}

void Organ::scanSwell() {
    scan(2);
}

void Organ::scanPedal() {
    scan(0);
}

