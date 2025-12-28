/*
  Organ.h - Library for scanning organ keys and output via MIDI.
  Created by Matthew Foote, December 2025.
  Released into the public domain.
*/

#ifndef Organ_h
#define Organ_h

#define NUM_KEYBOARDS 5

#include "Arduino.h"

class Organ
{
  public:
    //Organ(uint8_t *gOut, uint8_t *sOut, uint8_t *pOut, uint8_t *gsIn, uint8_t *pIn, void (*on)(byte, byte, byte), void (*off)(byte, byte, byte));
    Organ(void (*on)(byte, byte, byte), void (*off)(byte, byte, byte));
    void setKeyboard(uint8_t num, uint8_t offset, uint8_t *in, uint8_t n1, uint8_t *out, uint8_t n2);
    void begin();
    //void scanOrgan();
    void scanGreat();
    void scanSwell();
    void scanPedal();
    //void scanPistons();
  private:
    uint8_t n1, n2, n3, n4, n5;

    uint8_t debounceCount = 3;

    uint8_t _greatOutput[15] = {};
    uint8_t _swellOutput[15] = {};
    uint8_t _pedalOutput[15] = {};
    uint8_t _gsInput[15]     = {};
    uint8_t _pedalInput[15]  = {};

    uint8_t _pinsIn  [NUM_KEYBOARDS][15] = {};
    uint8_t _pinsOut [NUM_KEYBOARDS][15] = {};

    uint8_t _inCnt  [NUM_KEYBOARDS] = {};
    uint8_t _outCnt [NUM_KEYBOARDS] = {};
    uint8_t _offset [NUM_KEYBOARDS] = {};

    uint8_t _debounceArray [NUM_KEYBOARDS][70];          //holds debounce count for each switch

    void kbdOn(uint8_t kbd, uint8_t noteNumber);
    void kbdOff(uint8_t kbd, uint8_t noteNumber);

    void scan(uint8_t kbd);

    void (*_noteOn)(byte, byte, byte);
    void (*_noteOff)(byte, byte, byte);
};
#endif
