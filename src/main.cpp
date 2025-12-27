/*
Allen Keyboards plus Pedal plus Expression Scan
 ==============================================
This sketch is designed to scan a pair of keyboards as well as the pedal salvaged from a MDC 20 Allen organ.
The original wiring is retained but since the Allen is active HIGH and the Arduino is active LOW, the scanning
procedure is reversed. There is also provision for scanning an expression pedal.

Referring to the pin numbers on Allen's schematic for this organ, the Swell is scanned by sequentially bringing 
pins 52, 63,69,54,64 and 62 LOW (Arduino pins 22 - 27). 
Pins 16, 20, 18, 22, 24, 26, 28, 30, 32,34 and 36 (Arduino pins 36 - 46)
are then scanned to determine which keys are closed. LOW = switch closed. Output on Ch. 3

Referring to the pin numbers on Allen's schematic for this organ, the Great is scanned by sequentially bringing 
pins 48, 53,57, 55, 59 and 61 LOW (Arduino pins 28 - 33). 
Pins 16, 20, 18, 22, 24, 26, 28, 30, 32,34 and 36 (Arduino pins 36 - 46)
are then scanned to determine which keys are closed. LOW = switch closed. Output on Ch. 2

Referring to the pin numbers on Allen's schematic for this organ, the Pedal is scanned by sequentially bringing 
pins 67, 65, 58, 60, 68 and 66 LOW (Arduino pins 14 - 19). 
Pins 16, 20, 18, 22, 24, 26, and 28 (Arduino pins 62 - 68) (**Arduino Due 47 - 53**)
are then scanned to determine which keys are closed. LOW = switch closed. Output on Ch. 1
Note! the wires running to pins 16, 20, 18, 22, 24, 26, and 28 on the pedal are to be detached from the 
corresponding pins on the Great and Swell.

Piston inputs for 1,2,3,4,5 and GC are Arduino pins 0 - 5. Output on Ch. 4

Piston inputs for 1 - 10, GC, and set are matrixed. arduino pins A8 - A11 (62 - 65) are inputs, A5 - A7 (59 - 61) are outputs.

Pin 69: Reserved for analog input. The controllerArray values will have to be edited to
reflect the voltage range put out by device device. Connect 15k (+/-) pot across Arduino's
5V and Ground. Centre tap goes to pin 69.Input must never exceed 5V.
Note: ***** Unused analog inputs must be grounded to avoid spurious control messages *****
                
Equipment: Arduino Mega with one MIDI shield 
The connections for the pedal are partially covered by the MIDI shield. Either the MIDI shield can be mounted off 
the Arduino or right angle connectors can be used to bring out the connections.

 created 2021 June 1
 modified 2023 July 27
 by John Coenraads 
 modified 2025 November
 by Matthew Foote
 */

#define version "1.1.2"

#include "Arduino.h"
#include <USB-MIDI.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <Wire.h>
#include <Bounce2.h>
//#include <DueTimer.h>
//#include <Scheduler.h>
#include "Organ.h"
#include <LiquidCrystal_I2C.h>

// Declarations==========================================

#define MAC_BOOT_TIME		25000	//30 seconds
#define SAMPLE_LOAD_TIME	45000	//50 seconds

//Counters (old Fortran habit)
int i, j, k;

unsigned long lastDraw, lastExp, lastScan, trnspReset;

byte noteStatus;
//byte noteNumber;        // low C = 36
byte noteVelocity;

byte debounceCount = 3;           //Note ON if count > 0, OFF if count = 0
byte pistonDebounceArray [100];         //holds debounce count for each Piston switch

const char lcdArray[81] = "  St. John Cantius  "
			  "Pist:      Trans:   "
			  "                    "
			  "Swll:       Cres:   ";

const char lcdLoad0[81] = " The Organ Project  "
			  " 2025 Matthew Foote "
			  "                    "
			  "For the Glory of God";

const char lcdLoad1[81] = "                    "
			  "                    "
			  "   Starting up...   "
			  "                    ";

const char lcdLoad2[81] = "                    "
			  "                    "
			  " Loading Organ Files"
			  "                    ";

// (C)
byte newCharCopyright[8] = {
  B11111,
  B10001,
  B10101,
  B10111,
  B10101,
  B10001,
  B11111,
  B00000
};

bool stopsState[70];

byte swellPos, crescPos = 0;

volatile int transpose = 0;
volatile byte piston = 0;
volatile byte noPedal = 0;
volatile byte loaded = 0;

//Pin definitions
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

const byte pedSwitch  = 35;	//switch for changing pedal function LOW = swell HIGH = cresc
const byte pwrSwitch  = 55;	//power switch LOW = off HIGH = on
const byte trnspUpBtn = 6;	//Transpose pins. keep track 'cause we'll do some transpose features in Arduino
const byte trnspDnBtn = 7;
const byte trnspUpLgt = 8;	//Transpose lights. keep track 'cause we'll do some transpose features in Arduino
const byte trnspDnLgt = 9;
const byte panicBtn   = 13;
const byte initOvride = A0;

byte swellOut[] = {22, 23, 24, 25, 26, 27};
byte greatOut[] = {28, 29, 30, 31, 32, 33};
byte pedalOut[] = {14, 15, 16, 17, 18, 19};
byte gsIn[]     = {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46};
byte pedalIn[]  = {47, 48, 49, 50, 51, 52, 53};


//Function declarations
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=
//void loop1();
void initializeComputer();
void scanKeys();
void scanPistons();
void scanTranspose();
void turnOFFpiston(byte noteNumber);
void turnONpiston(byte noteNumber);
void scanExpression();
void noteOff(byte channel, byte pitch, byte velocity);
void noteOn(byte channel, byte pitch, byte velocity);
void controlChange(byte channel, byte control, byte value);
void OnMidiSysEx(byte* data, unsigned length);
void OnNoteOn(byte channel, byte note, byte velocity);
void OnNoteOff(byte channel, byte note, byte velocity);
void drawDisplay();
void lights();
byte countDigits(int num);

//Objects
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=

LiquidCrystal_I2C lcd(0x27,  20, 4);

Bounce trnspUp = Bounce();
Bounce trnspDn = Bounce();
Bounce2::Button panic = Bounce2::Button();

USBMIDI_CREATE_DEFAULT_INSTANCE();

Organ organ(greatOut, swellOut, pedalOut, gsIn, pedalIn, noteOn, noteOff);


//Initialize =========================================================
void setup() {
    //Serial.begin(9600);

    //Initialize  output (normally high)
    for(i = 22; i < 34; i++) {
        pinMode (i, OUTPUT);  
        digitalWrite (i, HIGH);
    }
    
    for(i = 14; i < 20; i++) {
        pinMode (i, OUTPUT);  
        digitalWrite (i, HIGH);
    }
    
    for(i = 59; i < 62; i++) {
        pinMode (i, OUTPUT);  
        digitalWrite (i, HIGH);
    }

    for(i = 10; i < 13; i++) {
	pinMode (i, OUTPUT);
        digitalWrite (i, LOW);
    }

    //Initialize input. Normally high (via internal pullups)
    for(i = 36; i < 54; i++) {
	pinMode (i, INPUT_PULLUP);
    }
    
    /*for(i = 47; i < 53; i++) {
        pinMode (i, INPUT_PULLUP);
    }*/

    for(i = 0; i < 6; i++) {
        pinMode (i, INPUT_PULLUP);
    }
        
    for(i = 62; i < 66; i++) {
        pinMode (i, INPUT_PULLUP);
    }
        
    for(i = 56; i < 59; i++) {
        pinMode (i, INPUT_PULLUP);
    }
        
    //Initialize debounce count arrays to zero
    /*for(i= 0; i < 100; i++) {
        greatDebounceArray[i] = 0;
        swellDebounceArray[i] = 0;
        pedalDebounceArray[i] = 0;
        pistonDebounceArray[i] = 0; 
    }*/

    //pinMode(pedSwitch, INPUT_PULLUP);
    //pinMode(pwrSwitch, INPUT_PULLUP);
    pinMode(trnspUpBtn, INPUT_PULLUP);
    pinMode(trnspDnBtn, INPUT_PULLUP);
    pinMode(panicBtn, INPUT_PULLUP);
    pinMode(initOvride, INPUT_PULLUP);
    pinMode(trnspUpLgt, OUTPUT);
    pinMode(trnspDnLgt, OUTPUT);

    organ.begin();

    Keyboard.begin();
    Mouse.begin();

    trnspUp.attach(trnspUpBtn);
    trnspUp.interval(6);
    trnspDn.attach(trnspDnBtn);
    trnspDn.interval(6);
    panic.attach(panicBtn);
    panic.interval(6);
    panic.setPressedState(LOW);

    Wire.begin();
    Wire.setTimeout(200);

    lcd.init();
    lcd.backlight();
    lcd.createChar(0, newCharCopyright);
    
    lcd.setCursor(0,0);
    lcd.print(lcdLoad0);
    lcd.setCursor(0,2);
    lcd.write(byte(0));
    lcd.noCursor();
    delay(4000);

    //Check for power off reset to initialise computer
    if(digitalRead(initOvride) == 0) {
        initializeComputer();
    }

    lcd.setCursor(0,0);
    lcd.print(lcdArray);
    lcd.noCursor();

    MIDI.begin(1);
    MIDI.setHandleSystemExclusive(OnMidiSysEx);
    MIDI.setHandleNoteOn(OnNoteOn);
    MIDI.setHandleNoteOff(OnNoteOff);

    //Timer3.attachInterrupt(scanKeys);
    //Timer3.start(4000); // Calls every 2.5ms 400x/sec (400Hz)
}

//Main Loops ===========================================================
void loop() {
  trnspUp.update();
  trnspDn.update();
  panic.update();

  MIDI.read();

  //power on mac
  //loop until confirmation message / for certain time
    //display starting info on lcd

  //change running var
  //while(digitalRead(pwrSwitch) == HIGH) {
  //while(1) {
    //update lcd

  if((micros() - lastScan) > 4000) {
    scanKeys();
    lastScan = micros();
  }

  if((millis() - lastDraw) > 300) {
    drawDisplay();
    lights();
    lastDraw = millis();
    //yield();
  }
    //manage stops
  scanPistons(); 
  scanTranspose();

    //scanGreatAndPedal();
    //scanSwell();

  //yield();
    //manage pedal
  if((millis() - lastExp) > 400) {
    scanExpression();
    lastExp = millis();
    //yield();
  }

  if(panic.pressed()) {
    //send 'ESC'
    Keyboard.press(KEY_ESC);
    delay(10);
    Keyboard.releaseAll();

    //Switch to GrandOrgue App
    Keyboard.press(KEY_LEFT_GUI);
    delay(10);
    Keyboard.press(' ');
    delay(300);
    Keyboard.releaseAll();
    delay(600);
    Keyboard.print("GrandOrgue");
    delay(700);
    Keyboard.press(KEY_RETURN);
    delay(10);
    Keyboard.releaseAll();
    delay(10);

    //send 'ESC'
    Keyboard.press(KEY_ESC);
    delay(10);
    Keyboard.releaseAll();
  }

}

void scanKeys() {
    if(!noPedal) {
        organ.scanPedal();
        organ.scanGreat();
    }
    else {
        organ.scanGreat();
        delayMicroseconds(150);
    }
    organ.scanSwell();
    yield();
}

/*void loop1() {
    __disable_irq();
    //scanGreat();
    scanGreatAndPedal();
    scanSwell();
    //scanPedal();
    __enable_irq();
    yield();
}*/

void initializeComputer() {
    lcd.setCursor(0,0);
    lcd.print(lcdLoad1);
    char displayLine[5];
    unsigned long initTime = millis();
    while((millis() - initTime) < MAC_BOOT_TIME) {
        lcd.setCursor(9,2);
        byte time = map((millis() - initTime), 0, MAC_BOOT_TIME, 0, 100);
        sprintf(displayLine, "%d%%", time);
        lcd.print(displayLine);
        delay(400);
    }

    Keyboard.press(KEY_LEFT_GUI);
    delay(10);
    Keyboard.press(' ');
    delay(300);
    Keyboard.releaseAll();
    delay(600);
    Keyboard.print("GrandOrgue");
    delay(700);
    Keyboard.press(KEY_RETURN);
    delay(10);
    Keyboard.releaseAll();

    lcd.setCursor(0,0);
    lcd.print(lcdLoad2);
    initTime = millis();
    while((millis() - initTime) < SAMPLE_LOAD_TIME) {
        lcd.setCursor(9,2);
        byte time = map((millis() - initTime), 0, SAMPLE_LOAD_TIME, 0, 100);
        sprintf(displayLine, "%d%%", time);
        lcd.print(displayLine);
        delay(500);
        //if(loaded)
            //break;
    }

    //while(!loaded);
}
        

//*******************************************************************************************

void scanPistons() {  
    for (j = 0; j < 6; j++) {
        if(digitalRead(j) == LOW) {turnONpiston (j);} else {turnOFFpiston (j);} 
    }    

    for (j = 56; j < 59; j++) {
        if(digitalRead(j) == LOW) {turnONpiston (j);} else {turnOFFpiston (j);} 
    }    

    digitalWrite (59, LOW);  
    //delay(1);
    delayMicroseconds(17);
    if  (digitalRead(62) == LOW) {turnONpiston (6);} else {turnOFFpiston (6);}
    if  (digitalRead(63) == LOW) {turnONpiston (7);} else {turnOFFpiston (7);}
    if  (digitalRead(64) == LOW) {turnONpiston (8);} else {turnOFFpiston (8);}
    if  (digitalRead(65) == LOW) {turnONpiston (9);} else {turnOFFpiston (9);}     
    digitalWrite (59, HIGH); 
    digitalWrite (60, LOW);    
    //delay(1);
    delayMicroseconds(17);
    if  (digitalRead(62) == LOW) {turnONpiston (10);} else {turnOFFpiston (10);}
    if  (digitalRead(63) == LOW) {turnONpiston (11);} else {turnOFFpiston (11);}
    if  (digitalRead(64) == LOW) {turnONpiston (12);} else {turnOFFpiston (12);}
    if  (digitalRead(65) == LOW) {turnONpiston (13);} else {turnOFFpiston (13);}     
    digitalWrite (60, HIGH); 
    digitalWrite (61, LOW);    
    //delay(1);
    delayMicroseconds(17);
    if  (digitalRead(62) == LOW) {turnONpiston (14);} else {turnOFFpiston (14);}
    if  (digitalRead(63) == LOW) {turnONpiston (15);} else {turnOFFpiston (15);}
    if  (digitalRead(64) == LOW) {turnONpiston (16);} else {turnOFFpiston (16);}
    if  (digitalRead(65) == LOW) {turnONpiston (17);} else {turnOFFpiston (17);}     
    digitalWrite (61, HIGH); 
        
}

void scanTranspose() {
    byte value1 = trnspUp.read();
    byte value2 = trnspDn.read();
    int dir;

    if((millis() - trnspReset) > 800) {
        if(value1 == LOW && value2 == LOW) {
            //reset transpose
            if(transpose > 0)
                dir = 0;
            else
                dir = 1;

            for(j = 0; j < abs(transpose); j++) {
                noteOn(5, 20 + dir, 127);
                delay(2);
                noteOff(5, 20 + dir, 0);
                delay(2);
            }
            trnspReset = millis();
        }
        else {
            if(value1 == LOW) {turnONpiston(20);} else {turnOFFpiston(20);}
            if(value2 == LOW) {turnONpiston(21);} else {turnOFFpiston(21);}
        }
    }
}

//MIDI ON message is sent only if note is not already ON.
void turnONpiston(byte noteNumber) {
    if(noteNumber < 100) {
        if(pistonDebounceArray[noteNumber] == 0) {
            noteOn(5, noteNumber, 127);
            pistonDebounceArray[noteNumber] = debounceCount;
        }   
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void turnOFFpiston(byte noteNumber) {
    if(noteNumber < 100) {
        if(pistonDebounceArray[noteNumber] == 1) {
            noteOff(5, noteNumber, 0);
        }  

        if(pistonDebounceArray[noteNumber] > 0)
            pistonDebounceArray[noteNumber] -- ;
    }
}

void scanExpression() {
    byte newCrescPos, newSwellPos;
    //if(digitalRead(pedSwitch)) {
        newSwellPos = analogRead(A1) >> 3;
    //}
    //else {
    //    newCrescPos = map(analogRead(A1), PED_MIN, PED_MAX, 0, 127);
    //}

    //if(newSwellPos > (swellPos + 1) || newSwellPos < (swellPos - 1)) {
    if(newSwellPos != swellPos) {
        //send swell info
        controlChange(5, 11, map(newSwellPos, 1, 127, 35, 127));
        swellPos = newSwellPos;
    }

    /*if(crescPos != newCrescPos) {
        //send cresc info
        controlChange(5, 12, newCrescPos);
        crescPos = newCrescPos;
    }*/

}

void noteOn(byte channel, byte pitch, byte velocity) {
  //midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  //MidiUSB.sendMIDI(noteOn);
  delayMicroseconds(80);
  MIDI.sendNoteOn(pitch, velocity, channel);
  yield();
}

void noteOff(byte channel, byte pitch, byte velocity) {
  //midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  //MidiUSB.sendMIDI(noteOff);
  delayMicroseconds(80);
  MIDI.sendNoteOff(pitch, velocity, channel);
  yield();
}

void controlChange(byte channel, byte control, byte value) {
  //midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  //MidiUSB.sendMIDI(event);
  delayMicroseconds(20);
  MIDI.sendControlChange(control, value, channel);
}

void OnMidiSysEx(byte* data, unsigned length) {
  //Signal to the rest of the program that GrandOrgue is loaded
  //if(!loaded)
    //loaded = 1;

  char buf[4] = {0};

  if(data[3] == 0x01) {	//Message we're looking for is hex 0x01
    memcpy(&buf, &data[4], 3);
    transpose = atoi(buf);
  }
}

void OnNoteOn(byte channel, byte note, byte velocity) {
    if(note < 15)
        piston = note;
    else if(note >= 20 && note <= 22)
        digitalWrite(note - 10, HIGH);

    if(note == 20)
        noPedal = 1;
}

void OnNoteOff(byte channel, byte note, byte velocity) {
    if(note >= 20 && note <= 22)
        digitalWrite(note - 10, LOW);

    if(note == 20)
        noPedal = 0;
}

void drawDisplay() {
        lcd.setCursor(9, 3);
        lcd.print("<-");
    /*if(digitalRead(pedSwitch)) {
        swellPos = analogRead(A1) >> 3;
        lcd.setCursor(9, 3);
        lcd.print("<-");
    }
    else {
        crescPos = analogRead(A1) >> 3;
        lcd.setCursor(9, 3);
        lcd.print("->");
    }*/

    //lcd.setCursor((8 - countDigits(swellPos)), 3);
    char displayValue[4];

    sprintf(displayValue, "%03d", swellPos);
    lcd.setCursor(5, 3);
    lcd.print(displayValue);
    //lcd.setCursor((20 - countDigits(crescPos)), 3);

    sprintf(displayValue, "%03d", crescPos);
    lcd.setCursor(17, 3);
    lcd.print(displayValue);

    lcd.setCursor(5, 2);
    if(countDigits(piston) == 1)
        lcd.print('0');
    lcd.print(piston);

    lcd.setCursor(17, 2);
    if(transpose < 0) {lcd.print('-');} else {lcd.print('+');}
    lcd.print(abs(transpose));
    if(countDigits(transpose) == 1)
        lcd.print(' ');
    lcd.noCursor();
}

void lights() {
    if(transpose == 0) {
        digitalWrite(trnspUpLgt, LOW);
        digitalWrite(trnspDnLgt, LOW);
    }
    else if(transpose >= 1) {
        digitalWrite(trnspUpLgt, HIGH);
        digitalWrite(trnspDnLgt, LOW);
    }
    else if(transpose <= -1) {
        digitalWrite(trnspUpLgt, LOW);
        digitalWrite(trnspDnLgt, HIGH);
    }
}

byte countDigits(int num) {
  if (num == 0) return 1;

  byte count = 0;
  while (num != 0) {
    num /= 10;
    count++;
  }
  return count;
}
