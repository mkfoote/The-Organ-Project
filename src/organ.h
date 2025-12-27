/*
  Organ.h - Library for scanning organ keys and output via MIDI.
  Created by Matthew Foote, December 2025.
  Released into the public domain.
*/

#ifndef Organ_h
#define Organ_h

#include "Arduino.h"

class Organ
{
  public:
    Organ(uint8_t *gOut, uint8_t *sOut, uint8_t *pOut, uint8_t *gsIn, uint8_t *pIn, void (*on)(byte, byte, byte), void (*off)(byte, byte, byte));
    void begin();
    void scanOrgan();
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
    //int _pin;

    uint8_t _greatDebounceArray [100];          //holds debounce count for each Great switch
    uint8_t _swellDebounceArray [100];          //holds debounce count for each Swell switch
    uint8_t _pedalDebounceArray [100];          //holds debounce count for each Piston switch
    uint8_t _pistonDebounceArray [50];          //holds debounce count for each Piston switch

    void turnONgreat(uint8_t noteNumber);
    void turnOFFgreat(uint8_t noteNumber);

    void turnONswell(uint8_t noteNumber);
    void turnOFFswell(uint8_t noteNumber);

    void turnONpedal(uint8_t noteNumber);
    void turnOFFpedal(uint8_t noteNumber);

    //void turnONpiston(uint8_t noteNumber);
    //void turnOFFpiston(uint8_t noteNumber);

    void (*_noteOn)(byte, byte, byte);
    void (*_noteOff)(byte, byte, byte);
};
#endif
