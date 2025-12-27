/*
  Organ.cpp - Library for scanning organ keys and output via MIDI.
  Created by Matthew Foote, December 2025.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Organ.h"

//Organ::Organ(uint8_t *gOut, uint8_t n1, uint8_t *sOut, uint8_t n2, uint8_t *pOut, uint8_t n3, uint8_t *gsIn, uint8_t n4, uint8_t *pIn, uint8_t n5)
Organ::Organ(uint8_t *gOut, uint8_t *sOut, uint8_t *pOut, uint8_t *gsIn, uint8_t *pIn, void (*on)(byte, byte, byte), void (*off)(byte, byte, byte))
{
  uint8_t n1, n2, n4 = 6;
  uint8_t n3 = 11;
  uint8_t n5 = 7;

  memcpy(_greatOutput, gOut, n1);
  memcpy(_swellOutput, sOut, n2);
  memcpy(_pedalOutput, pOut, n3);
  memcpy(_gsInput,     gsIn, n4);
  memcpy(_pedalInput,   pIn, n5);

  _noteOn = on;
  _noteOff = off;
}

void Organ::begin()
{
  //Set up pin direction
  for(uint8_t i = 0; i < n1; i++) {
    pinMode(_greatOutput[i], OUTPUT);
  }

  for(uint8_t i = 0; i < n2; i++) {
    pinMode(_swellOutput[i], OUTPUT);
  }

  for(uint8_t i = 0; i < n3; i++) {
    pinMode(_pedalOutput[i], OUTPUT);
  }

  for(uint8_t i = 0; i < n4; i++) {
    pinMode(_gsInput[i], INPUT_PULLUP);
  }

  for(uint8_t i = 0; i < n5; i++) {
    pinMode(_pedalInput[i], INPUT_PULLUP);
  }
}

void Organ::scanGreat() {
     digitalWrite (_greatOutput[0], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONgreat (37);} else {turnOFFgreat (37);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONgreat (43);} else {turnOFFgreat (43);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONgreat (49);} else {turnOFFgreat (49);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONgreat (55);} else {turnOFFgreat (55);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONgreat (61);} else {turnOFFgreat (61);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONgreat (67);} else {turnOFFgreat (67);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONgreat (73);} else {turnOFFgreat (73);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONgreat (79);} else {turnOFFgreat (79);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONgreat (85);} else {turnOFFgreat (85);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONgreat (91);} else {turnOFFgreat (91);} 
     digitalWrite (_greatOutput[0], HIGH); 
     digitalWrite (_greatOutput[1], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONgreat (38);} else {turnOFFgreat (38);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONgreat (44);} else {turnOFFgreat (44);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONgreat (50);} else {turnOFFgreat (50);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONgreat (56);} else {turnOFFgreat (56);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONgreat (62);} else {turnOFFgreat (62);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONgreat (68);} else {turnOFFgreat (68);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONgreat (74);} else {turnOFFgreat (74);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONgreat (80);} else {turnOFFgreat (80);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONgreat (86);} else {turnOFFgreat (86);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONgreat (92);} else {turnOFFgreat (92);} 
     digitalWrite (_greatOutput[1], HIGH); 
     digitalWrite (_greatOutput[2], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONgreat (39);} else {turnOFFgreat (39);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONgreat (45);} else {turnOFFgreat (45);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONgreat (51);} else {turnOFFgreat (51);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONgreat (57);} else {turnOFFgreat (57);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONgreat (63);} else {turnOFFgreat (63);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONgreat (69);} else {turnOFFgreat (69);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONgreat (75);} else {turnOFFgreat (75);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONgreat (81);} else {turnOFFgreat (81);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONgreat (87);} else {turnOFFgreat (87);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONgreat (93);} else {turnOFFgreat (93);} 
     digitalWrite (_greatOutput[2], HIGH); 
     digitalWrite (_greatOutput[3], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONgreat (40);} else {turnOFFgreat (40);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONgreat (46);} else {turnOFFgreat (46);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONgreat (52);} else {turnOFFgreat (52);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONgreat (58);} else {turnOFFgreat (58);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONgreat (64);} else {turnOFFgreat (64);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONgreat (70);} else {turnOFFgreat (70);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONgreat (76);} else {turnOFFgreat (76);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONgreat (82);} else {turnOFFgreat (82);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONgreat (88);} else {turnOFFgreat (88);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONgreat (94);} else {turnOFFgreat (94);} 
     digitalWrite (_greatOutput[3], HIGH); 
     digitalWrite (_greatOutput[4], LOW);  
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONgreat (41);} else {turnOFFgreat (41);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONgreat (47);} else {turnOFFgreat (47);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONgreat (53);} else {turnOFFgreat (53);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONgreat (59);} else {turnOFFgreat (59);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONgreat (65);} else {turnOFFgreat (65);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONgreat (71);} else {turnOFFgreat (71);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONgreat (77);} else {turnOFFgreat (77);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONgreat (83);} else {turnOFFgreat (83);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONgreat (89);} else {turnOFFgreat (89);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONgreat (95);} else {turnOFFgreat (95);} 
     digitalWrite (_greatOutput[4], HIGH); 
     digitalWrite (_greatOutput[5], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[0]) == LOW) {turnONgreat (36);} else {turnOFFgreat (36);}   //bottom C
     if  (digitalRead(_gsInput[1]) == LOW) {turnONgreat (42);} else {turnOFFgreat (42);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONgreat (48);} else {turnOFFgreat (48);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONgreat (54);} else {turnOFFgreat (54);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONgreat (60);} else {turnOFFgreat (60);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONgreat (66);} else {turnOFFgreat (66);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONgreat (72);} else {turnOFFgreat (72);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONgreat (78);} else {turnOFFgreat (78);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONgreat (84);} else {turnOFFgreat (84);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONgreat (90);} else {turnOFFgreat (90);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONgreat (96);} else {turnOFFgreat (96);} 
     digitalWrite (_greatOutput[5], HIGH);   
     delayMicroseconds(17);
}

void Organ::scanSwell () {
     digitalWrite (_swellOutput[0], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONswell (37);} else {turnOFFswell (37);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONswell (43);} else {turnOFFswell (43);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONswell (49);} else {turnOFFswell (49);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONswell (55);} else {turnOFFswell (55);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONswell (61);} else {turnOFFswell (61);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONswell (67);} else {turnOFFswell (67);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONswell (73);} else {turnOFFswell (73);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONswell (79);} else {turnOFFswell (79);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONswell (85);} else {turnOFFswell (85);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONswell (91);} else {turnOFFswell (91);} 
     digitalWrite (_swellOutput[0], HIGH); 
     digitalWrite (_swellOutput[1], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONswell (38);} else {turnOFFswell (38);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONswell (44);} else {turnOFFswell (44);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONswell (50);} else {turnOFFswell (50);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONswell (56);} else {turnOFFswell (56);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONswell (62);} else {turnOFFswell (62);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONswell (68);} else {turnOFFswell (68);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONswell (74);} else {turnOFFswell (74);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONswell (80);} else {turnOFFswell (80);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONswell (86);} else {turnOFFswell (86);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONswell (92);} else {turnOFFswell (92);} 
     digitalWrite (_swellOutput[1], HIGH); 
     digitalWrite (_swellOutput[2], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONswell (39);} else {turnOFFswell (39);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONswell (45);} else {turnOFFswell (45);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONswell (51);} else {turnOFFswell (51);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONswell (57);} else {turnOFFswell (57);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONswell (63);} else {turnOFFswell (63);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONswell (69);} else {turnOFFswell (69);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONswell (75);} else {turnOFFswell (75);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONswell (81);} else {turnOFFswell (81);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONswell (87);} else {turnOFFswell (87);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONswell (93);} else {turnOFFswell (93);} 
     digitalWrite (_swellOutput[2], HIGH); 
     digitalWrite (_swellOutput[3], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONswell (40);} else {turnOFFswell (40);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONswell (46);} else {turnOFFswell (46);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONswell (52);} else {turnOFFswell (52);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONswell (58);} else {turnOFFswell (58);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONswell (64);} else {turnOFFswell (64);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONswell (70);} else {turnOFFswell (70);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONswell (76);} else {turnOFFswell (76);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONswell (82);} else {turnOFFswell (82);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONswell (88);} else {turnOFFswell (88);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONswell (94);} else {turnOFFswell (94);} 
     digitalWrite (_swellOutput[3], HIGH); 
     digitalWrite (_swellOutput[4], LOW);  
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[1]) == LOW) {turnONswell (41);} else {turnOFFswell (41);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONswell (47);} else {turnOFFswell (47);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONswell (53);} else {turnOFFswell (53);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONswell (59);} else {turnOFFswell (59);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONswell (65);} else {turnOFFswell (65);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONswell (71);} else {turnOFFswell (71);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONswell (77);} else {turnOFFswell (77);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONswell (83);} else {turnOFFswell (83);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONswell (89);} else {turnOFFswell (89);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONswell (95);} else {turnOFFswell (95);} 
     digitalWrite (_swellOutput[4], HIGH); 
     digitalWrite (_swellOutput[5], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_gsInput[0]) == LOW) {turnONswell (36);} else {turnOFFswell (36);}   //bottom C
     if  (digitalRead(_gsInput[1]) == LOW) {turnONswell (42);} else {turnOFFswell (42);}
     if  (digitalRead(_gsInput[2]) == LOW) {turnONswell (48);} else {turnOFFswell (48);}
     if  (digitalRead(_gsInput[3]) == LOW) {turnONswell (54);} else {turnOFFswell (54);}
     if  (digitalRead(_gsInput[4]) == LOW) {turnONswell (60);} else {turnOFFswell (60);}     
     if  (digitalRead(_gsInput[5]) == LOW) {turnONswell (66);} else {turnOFFswell (66);}
     if  (digitalRead(_gsInput[6]) == LOW) {turnONswell (72);} else {turnOFFswell (72);}
     if  (digitalRead(_gsInput[7]) == LOW) {turnONswell (78);} else {turnOFFswell (78);}
     if  (digitalRead(_gsInput[8]) == LOW) {turnONswell (84);} else {turnOFFswell (84);} 
     if  (digitalRead(_gsInput[9]) == LOW) {turnONswell (90);} else {turnOFFswell (90);}
     if  (digitalRead(_gsInput[10]) == LOW) {turnONswell (96);} else {turnOFFswell (96);} 
     digitalWrite (_swellOutput[5], HIGH);   
     delayMicroseconds(17);
}

void Organ::scanPedal() {
     digitalWrite (_pedalOutput[0], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_pedalInput[1]) == LOW) {turnONpedal (37);} else {turnOFFpedal (37);}
     if  (digitalRead(_pedalInput[2]) == LOW) {turnONpedal (43);} else {turnOFFpedal (43);}
     if  (digitalRead(_pedalInput[3]) == LOW) {turnONpedal (49);} else {turnOFFpedal (49);}
     if  (digitalRead(_pedalInput[4]) == LOW) {turnONpedal (55);} else {turnOFFpedal (55);}     
     if  (digitalRead(_pedalInput[5]) == LOW) {turnONpedal (61);} else {turnOFFpedal (61);}
     if  (digitalRead(_pedalInput[6]) == LOW) {turnONpedal (67);} else {turnOFFpedal (67);}
     digitalWrite (_pedalOutput[0], HIGH); 
     digitalWrite (_pedalOutput[1], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_pedalInput[1]) == LOW) {turnONpedal (38);} else {turnOFFpedal (38);}
     if  (digitalRead(_pedalInput[2]) == LOW) {turnONpedal (44);} else {turnOFFpedal (44);}
     if  (digitalRead(_pedalInput[3]) == LOW) {turnONpedal (50);} else {turnOFFpedal (50);}
     if  (digitalRead(_pedalInput[4]) == LOW) {turnONpedal (56);} else {turnOFFpedal (56);}     
     if  (digitalRead(_pedalInput[5]) == LOW) {turnONpedal (62);} else {turnOFFpedal (62);}
     digitalWrite (_pedalOutput[1], HIGH); 
     digitalWrite (_pedalOutput[2], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_pedalInput[1]) == LOW) {turnONpedal (39);} else {turnOFFpedal (39);}
     if  (digitalRead(_pedalInput[2]) == LOW) {turnONpedal (45);} else {turnOFFpedal (45);}
     if  (digitalRead(_pedalInput[3]) == LOW) {turnONpedal (51);} else {turnOFFpedal (51);}
     if  (digitalRead(_pedalInput[4]) == LOW) {turnONpedal (57);} else {turnOFFpedal (57);}     
     if  (digitalRead(_pedalInput[5]) == LOW) {turnONpedal (63);} else {turnOFFpedal (63);}
     digitalWrite (_pedalOutput[2], HIGH); 
     digitalWrite (_pedalOutput[3], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_pedalInput[1]) == LOW) {turnONpedal (40);} else {turnOFFpedal (40);}
     if  (digitalRead(_pedalInput[2]) == LOW) {turnONpedal (46);} else {turnOFFpedal (46);}
     if  (digitalRead(_pedalInput[3]) == LOW) {turnONpedal (52);} else {turnOFFpedal (52);}
     if  (digitalRead(_pedalInput[4]) == LOW) {turnONpedal (58);} else {turnOFFpedal (58);}     
     if  (digitalRead(_pedalInput[5]) == LOW) {turnONpedal (64);} else {turnOFFpedal (64);}
     digitalWrite (_pedalOutput[3], HIGH); 
     digitalWrite (_pedalOutput[4], LOW);
     delayMicroseconds(17);
     if  (digitalRead(_pedalInput[1]) == LOW) {turnONpedal (41);} else {turnOFFpedal (41);}
     if  (digitalRead(_pedalInput[2]) == LOW) {turnONpedal (47);} else {turnOFFpedal (47);}
     if  (digitalRead(_pedalInput[3]) == LOW) {turnONpedal (53);} else {turnOFFpedal (53);}
     if  (digitalRead(_pedalInput[4]) == LOW) {turnONpedal (59);} else {turnOFFpedal (59);}     
     if  (digitalRead(_pedalInput[5]) == LOW) {turnONpedal (65);} else {turnOFFpedal (65);}
     digitalWrite (_pedalOutput[4], HIGH); 
     digitalWrite (_pedalOutput[5], LOW);    
     delayMicroseconds(17);
     if  (digitalRead(_pedalInput[0]) == LOW) {turnONpedal (36);} else {turnOFFpedal (36);}   //bottom C
     if  (digitalRead(_pedalInput[1]) == LOW) {turnONpedal (42);} else {turnOFFpedal (42);}
     if  (digitalRead(_pedalInput[2]) == LOW) {turnONpedal (48);} else {turnOFFpedal (48);}
     if  (digitalRead(_pedalInput[3]) == LOW) {turnONpedal (54);} else {turnOFFpedal (54);}
     if  (digitalRead(_pedalInput[4]) == LOW) {turnONpedal (60);} else {turnOFFpedal (60);}     
     if  (digitalRead(_pedalInput[5]) == LOW) {turnONpedal (66);} else {turnOFFpedal (66);}
     digitalWrite (_pedalOutput[5], HIGH);   
}


//MIDI ON message is sent only if note is not already ON.
void Organ::turnONgreat(uint8_t noteNumber) {
    if(noteNumber < 100) {
        if(_greatDebounceArray[noteNumber] == 0) {
            _noteOn(2, noteNumber, 127);
            _greatDebounceArray[noteNumber] = debounceCount;
        }   
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void Organ::turnOFFgreat(uint8_t noteNumber) {
    if(noteNumber < 100) {
        if(_greatDebounceArray[noteNumber] == 1) {
            _noteOff(2, noteNumber, 0);
        }  
        if(_greatDebounceArray[noteNumber] > 0)
	    _greatDebounceArray[noteNumber] -- ;
    }
}

//MIDI ON message is sent only if note is not already ON.
void Organ::turnONswell(uint8_t noteNumber) {
    if(noteNumber < 100) {
        if(_swellDebounceArray[noteNumber] == 0) {
            _noteOn(1, noteNumber, 127);
            _swellDebounceArray[noteNumber] = debounceCount;
        }   
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void Organ::turnOFFswell(uint8_t noteNumber) {
    if(noteNumber < 100) {
        if(_swellDebounceArray[noteNumber] == 1) {
            _noteOff(1, noteNumber, 0);
        }  
        if(_swellDebounceArray[noteNumber] > 0)
	    _swellDebounceArray[noteNumber] -- ;       
    }
}

//MIDI ON message is sent only if note is not already ON.
void Organ::turnONpedal(uint8_t noteNumber) {
    if(noteNumber < 100) {
        if(_pedalDebounceArray[noteNumber] == 0) {
            _noteOn(3, noteNumber, 127);
            _pedalDebounceArray[noteNumber] = debounceCount;
        }
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void Organ::turnOFFpedal(uint8_t noteNumber) {
    if(noteNumber < 100) {
        if(_pedalDebounceArray[noteNumber] == 1) {
            _noteOff(3, noteNumber, 0);
        }
        if(_pedalDebounceArray[noteNumber] > 0)
	    _pedalDebounceArray[noteNumber] -- ;
    }
}

