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

#include "Arduino.h"
#include <USB-MIDI.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <Wire.h>
#include <Bounce2.h>
//#include <DueTimer.h>
//#include <Scheduler.h>
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

const byte debounceCount = 3;           //Note ON if count > 0, OFF if count = 0
byte greatDebounceArray [100];          //holds debounce count for each Great switch
byte swellDebounceArray [100];          //holds debounce count for each Swell switch
byte pedalDebounceArray [100];         //holds debounce count for each Piston switch
byte pistonDebounceArray [100];         //holds debounce count for each Piston switch

const char lcdArray[81] = "  St. John Cantius  "
			  "Pist:      Trans:   "
			  "                    "
			  "Swll:       Cres:   ";

const char lcdLoad0[81] = " The Organ Project  "
			  "     2025 GPLv3     "
			  "                    "
			  "   Matthew Foote    ";

const char lcdLoad1[81] = "                    "
			  "                    "
			  "   Starting up...   "
			  "                    ";

const char lcdLoad2[81] = "                    "
			  "                    "
			  " Loading Organ File "
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

LiquidCrystal_I2C lcd(0x27,  20, 4);

Bounce trnspUp = Bounce();
Bounce trnspDn = Bounce();
Bounce2::Button panic = Bounce2::Button();

USBMIDI_CREATE_DEFAULT_INSTANCE();

//Pin definitions
const byte pedSwitch  = 35;	//switch for changing pedal function LOW = swell HIGH = cresc
const byte pwrSwitch  = 55;	//power switch LOW = off HIGH = on
const byte trnspUpBtn = 6;	//Transpose pins. keep track 'cause we'll do some transpose features in Arduino
const byte trnspDnBtn = 7;
const byte trnspUpLgt = 8;	//Transpose lights. keep track 'cause we'll do some transpose features in Arduino
const byte trnspDnLgt = 9;
const byte panicBtn   = 13;
const byte initOvride = A0;

//Function declarations
//void loop1();
void initializeComputer();
void scanKeys();
void scanGreat();
void scanGreatAndPedal();
void turnOFFgreat(byte noteNumber);
void turnONgreat(byte noteNumber);
void scanSwell();
void turnOFFswell(byte noteNumber);
void turnONswell(byte noteNumber);
void scanPedal();
void turnOFFpedal(byte noteNumber);
void turnONpedal(byte noteNumber);
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
    for(i= 0; i < 100; i++) {
        greatDebounceArray[i] = 0;
        swellDebounceArray[i] = 0;
        pedalDebounceArray[i] = 0;
        pistonDebounceArray[i] = 0; 
    }

    //pinMode(pedSwitch, INPUT_PULLUP);
    //pinMode(pwrSwitch, INPUT_PULLUP);
    pinMode(trnspUpBtn, INPUT_PULLUP);
    pinMode(trnspDnBtn, INPUT_PULLUP);
    pinMode(panicBtn, INPUT_PULLUP);
    pinMode(initOvride, INPUT_PULLUP);
    pinMode(trnspUpLgt, OUTPUT);
    pinMode(trnspDnLgt, OUTPUT);

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
    lcd.setCursor(4,2);
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

  if((micros() - lastScan) > 5000) {
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
        scanGreatAndPedal();
    }
    else {
        scanGreat();
        delayMicroseconds(150);
    }
    scanSwell();
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

//Scan Great keyboard, convert to MIDI and output via port 0, channel 2.
void scanGreat() {
     digitalWrite (28, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (37);} else {turnOFFgreat (37);}
     if  (digitalRead(38) == LOW) {turnONgreat (43);} else {turnOFFgreat (43);}
     if  (digitalRead(39) == LOW) {turnONgreat (49);} else {turnOFFgreat (49);}
     if  (digitalRead(40) == LOW) {turnONgreat (55);} else {turnOFFgreat (55);}     
     if  (digitalRead(41) == LOW) {turnONgreat (61);} else {turnOFFgreat (61);}
     if  (digitalRead(42) == LOW) {turnONgreat (67);} else {turnOFFgreat (67);}
     if  (digitalRead(43) == LOW) {turnONgreat (73);} else {turnOFFgreat (73);}
     if  (digitalRead(44) == LOW) {turnONgreat (79);} else {turnOFFgreat (79);} 
     if  (digitalRead(45) == LOW) {turnONgreat (85);} else {turnOFFgreat (85);}
     if  (digitalRead(46) == LOW) {turnONgreat (91);} else {turnOFFgreat (91);} 
     digitalWrite (28, HIGH); 
     digitalWrite (29, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (38);} else {turnOFFgreat (38);}
     if  (digitalRead(38) == LOW) {turnONgreat (44);} else {turnOFFgreat (44);}
     if  (digitalRead(39) == LOW) {turnONgreat (50);} else {turnOFFgreat (50);}
     if  (digitalRead(40) == LOW) {turnONgreat (56);} else {turnOFFgreat (56);}     
     if  (digitalRead(41) == LOW) {turnONgreat (62);} else {turnOFFgreat (62);}
     if  (digitalRead(42) == LOW) {turnONgreat (68);} else {turnOFFgreat (68);}
     if  (digitalRead(43) == LOW) {turnONgreat (74);} else {turnOFFgreat (74);}
     if  (digitalRead(44) == LOW) {turnONgreat (80);} else {turnOFFgreat (80);} 
     if  (digitalRead(45) == LOW) {turnONgreat (86);} else {turnOFFgreat (86);}
     if  (digitalRead(46) == LOW) {turnONgreat (92);} else {turnOFFgreat (92);} 
     digitalWrite (29, HIGH); 
     digitalWrite (30, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (39);} else {turnOFFgreat (39);}
     if  (digitalRead(38) == LOW) {turnONgreat (45);} else {turnOFFgreat (45);}
     if  (digitalRead(39) == LOW) {turnONgreat (51);} else {turnOFFgreat (51);}
     if  (digitalRead(40) == LOW) {turnONgreat (57);} else {turnOFFgreat (57);}     
     if  (digitalRead(41) == LOW) {turnONgreat (63);} else {turnOFFgreat (63);}
     if  (digitalRead(42) == LOW) {turnONgreat (69);} else {turnOFFgreat (69);}
     if  (digitalRead(43) == LOW) {turnONgreat (75);} else {turnOFFgreat (75);}
     if  (digitalRead(44) == LOW) {turnONgreat (81);} else {turnOFFgreat (81);} 
     if  (digitalRead(45) == LOW) {turnONgreat (87);} else {turnOFFgreat (87);}
     if  (digitalRead(46) == LOW) {turnONgreat (93);} else {turnOFFgreat (93);} 
     digitalWrite (30, HIGH); 
     digitalWrite (31, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (40);} else {turnOFFgreat (40);}
     if  (digitalRead(38) == LOW) {turnONgreat (46);} else {turnOFFgreat (46);}
     if  (digitalRead(39) == LOW) {turnONgreat (52);} else {turnOFFgreat (52);}
     if  (digitalRead(40) == LOW) {turnONgreat (58);} else {turnOFFgreat (58);}     
     if  (digitalRead(41) == LOW) {turnONgreat (64);} else {turnOFFgreat (64);}
     if  (digitalRead(42) == LOW) {turnONgreat (70);} else {turnOFFgreat (70);}
     if  (digitalRead(43) == LOW) {turnONgreat (76);} else {turnOFFgreat (76);}
     if  (digitalRead(44) == LOW) {turnONgreat (82);} else {turnOFFgreat (82);} 
     if  (digitalRead(45) == LOW) {turnONgreat (88);} else {turnOFFgreat (88);}
     if  (digitalRead(46) == LOW) {turnONgreat (94);} else {turnOFFgreat (94);} 
     digitalWrite (31, HIGH); 
     digitalWrite (32, LOW);  
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (41);} else {turnOFFgreat (41);}
     if  (digitalRead(38) == LOW) {turnONgreat (47);} else {turnOFFgreat (47);}
     if  (digitalRead(39) == LOW) {turnONgreat (53);} else {turnOFFgreat (53);}
     if  (digitalRead(40) == LOW) {turnONgreat (59);} else {turnOFFgreat (59);}     
     if  (digitalRead(41) == LOW) {turnONgreat (65);} else {turnOFFgreat (65);}
     if  (digitalRead(42) == LOW) {turnONgreat (71);} else {turnOFFgreat (71);}
     if  (digitalRead(43) == LOW) {turnONgreat (77);} else {turnOFFgreat (77);}
     if  (digitalRead(44) == LOW) {turnONgreat (83);} else {turnOFFgreat (83);} 
     if  (digitalRead(45) == LOW) {turnONgreat (89);} else {turnOFFgreat (89);}
     if  (digitalRead(46) == LOW) {turnONgreat (95);} else {turnOFFgreat (95);} 
     digitalWrite (32, HIGH); 
     digitalWrite (33, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(36) == LOW) {turnONgreat (36);} else {turnOFFgreat (36);}   //bottom C
     if  (digitalRead(37) == LOW) {turnONgreat (42);} else {turnOFFgreat (42);}
     if  (digitalRead(38) == LOW) {turnONgreat (48);} else {turnOFFgreat (48);}
     if  (digitalRead(39) == LOW) {turnONgreat (54);} else {turnOFFgreat (54);}
     if  (digitalRead(40) == LOW) {turnONgreat (60);} else {turnOFFgreat (60);}     
     if  (digitalRead(41) == LOW) {turnONgreat (66);} else {turnOFFgreat (66);}
     if  (digitalRead(42) == LOW) {turnONgreat (72);} else {turnOFFgreat (72);}
     if  (digitalRead(43) == LOW) {turnONgreat (78);} else {turnOFFgreat (78);}
     if  (digitalRead(44) == LOW) {turnONgreat (84);} else {turnOFFgreat (84);} 
     if  (digitalRead(45) == LOW) {turnONgreat (90);} else {turnOFFgreat (90);}
     if  (digitalRead(46) == LOW) {turnONgreat (96);} else {turnOFFgreat (96);} 
     digitalWrite (33, HIGH);   
     delayMicroseconds(17);
}

//Scan Great keyboard nad pedal, convert to MIDI
void scanGreatAndPedal() {
     digitalWrite (28, LOW);    
     digitalWrite (14, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (37);} else {turnOFFgreat (37);}
     if  (digitalRead(38) == LOW) {turnONgreat (43);} else {turnOFFgreat (43);}
     if  (digitalRead(39) == LOW) {turnONgreat (49);} else {turnOFFgreat (49);}
     if  (digitalRead(40) == LOW) {turnONgreat (55);} else {turnOFFgreat (55);}     
     if  (digitalRead(41) == LOW) {turnONgreat (61);} else {turnOFFgreat (61);}
     if  (digitalRead(42) == LOW) {turnONgreat (67);} else {turnOFFgreat (67);}
     if  (digitalRead(43) == LOW) {turnONgreat (73);} else {turnOFFgreat (73);}
     if  (digitalRead(44) == LOW) {turnONgreat (79);} else {turnOFFgreat (79);} 
     if  (digitalRead(45) == LOW) {turnONgreat (85);} else {turnOFFgreat (85);}
     if  (digitalRead(46) == LOW) {turnONgreat (91);} else {turnOFFgreat (91);} 
     if  (digitalRead(48) == LOW) {turnONpedal (37);} else {turnOFFpedal (37);}
     if  (digitalRead(49) == LOW) {turnONpedal (43);} else {turnOFFpedal (43);}
     if  (digitalRead(50) == LOW) {turnONpedal (49);} else {turnOFFpedal (49);}
     if  (digitalRead(51) == LOW) {turnONpedal (55);} else {turnOFFpedal (55);}     
     if  (digitalRead(52) == LOW) {turnONpedal (61);} else {turnOFFpedal (61);}
     if  (digitalRead(53) == LOW) {turnONpedal (67);} else {turnOFFpedal (67);}
     digitalWrite (28, HIGH); 
     digitalWrite (29, LOW);    
     digitalWrite (14, HIGH); 
     digitalWrite (15, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (38);} else {turnOFFgreat (38);}
     if  (digitalRead(38) == LOW) {turnONgreat (44);} else {turnOFFgreat (44);}
     if  (digitalRead(39) == LOW) {turnONgreat (50);} else {turnOFFgreat (50);}
     if  (digitalRead(40) == LOW) {turnONgreat (56);} else {turnOFFgreat (56);}     
     if  (digitalRead(41) == LOW) {turnONgreat (62);} else {turnOFFgreat (62);}
     if  (digitalRead(42) == LOW) {turnONgreat (68);} else {turnOFFgreat (68);}
     if  (digitalRead(43) == LOW) {turnONgreat (74);} else {turnOFFgreat (74);}
     if  (digitalRead(44) == LOW) {turnONgreat (80);} else {turnOFFgreat (80);} 
     if  (digitalRead(45) == LOW) {turnONgreat (86);} else {turnOFFgreat (86);}
     if  (digitalRead(46) == LOW) {turnONgreat (92);} else {turnOFFgreat (92);} 
     if  (digitalRead(48) == LOW) {turnONpedal (38);} else {turnOFFpedal (38);}
     if  (digitalRead(49) == LOW) {turnONpedal (44);} else {turnOFFpedal (44);}
     if  (digitalRead(50) == LOW) {turnONpedal (50);} else {turnOFFpedal (50);}
     if  (digitalRead(51) == LOW) {turnONpedal (56);} else {turnOFFpedal (56);}     
     if  (digitalRead(52) == LOW) {turnONpedal (62);} else {turnOFFpedal (62);}
     digitalWrite (29, HIGH); 
     digitalWrite (30, LOW);    
     digitalWrite (15, HIGH); 
     digitalWrite (16, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (39);} else {turnOFFgreat (39);}
     if  (digitalRead(38) == LOW) {turnONgreat (45);} else {turnOFFgreat (45);}
     if  (digitalRead(39) == LOW) {turnONgreat (51);} else {turnOFFgreat (51);}
     if  (digitalRead(40) == LOW) {turnONgreat (57);} else {turnOFFgreat (57);}     
     if  (digitalRead(41) == LOW) {turnONgreat (63);} else {turnOFFgreat (63);}
     if  (digitalRead(42) == LOW) {turnONgreat (69);} else {turnOFFgreat (69);}
     if  (digitalRead(43) == LOW) {turnONgreat (75);} else {turnOFFgreat (75);}
     if  (digitalRead(44) == LOW) {turnONgreat (81);} else {turnOFFgreat (81);} 
     if  (digitalRead(45) == LOW) {turnONgreat (87);} else {turnOFFgreat (87);}
     if  (digitalRead(46) == LOW) {turnONgreat (93);} else {turnOFFgreat (93);} 
     if  (digitalRead(48) == LOW) {turnONpedal (39);} else {turnOFFpedal (39);}
     if  (digitalRead(49) == LOW) {turnONpedal (45);} else {turnOFFpedal (45);}
     if  (digitalRead(50) == LOW) {turnONpedal (51);} else {turnOFFpedal (51);}
     if  (digitalRead(51) == LOW) {turnONpedal (57);} else {turnOFFpedal (57);}     
     if  (digitalRead(52) == LOW) {turnONpedal (63);} else {turnOFFpedal (63);}
     digitalWrite (30, HIGH); 
     digitalWrite (31, LOW);    
     digitalWrite (16, HIGH); 
     digitalWrite (17, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (40);} else {turnOFFgreat (40);}
     if  (digitalRead(38) == LOW) {turnONgreat (46);} else {turnOFFgreat (46);}
     if  (digitalRead(39) == LOW) {turnONgreat (52);} else {turnOFFgreat (52);}
     if  (digitalRead(40) == LOW) {turnONgreat (58);} else {turnOFFgreat (58);}     
     if  (digitalRead(41) == LOW) {turnONgreat (64);} else {turnOFFgreat (64);}
     if  (digitalRead(42) == LOW) {turnONgreat (70);} else {turnOFFgreat (70);}
     if  (digitalRead(43) == LOW) {turnONgreat (76);} else {turnOFFgreat (76);}
     if  (digitalRead(44) == LOW) {turnONgreat (82);} else {turnOFFgreat (82);} 
     if  (digitalRead(45) == LOW) {turnONgreat (88);} else {turnOFFgreat (88);}
     if  (digitalRead(46) == LOW) {turnONgreat (94);} else {turnOFFgreat (94);} 
     if  (digitalRead(48) == LOW) {turnONpedal (40);} else {turnOFFpedal (40);}
     if  (digitalRead(49) == LOW) {turnONpedal (46);} else {turnOFFpedal (46);}
     if  (digitalRead(50) == LOW) {turnONpedal (52);} else {turnOFFpedal (52);}
     if  (digitalRead(51) == LOW) {turnONpedal (58);} else {turnOFFpedal (58);}     
     if  (digitalRead(52) == LOW) {turnONpedal (64);} else {turnOFFpedal (64);}
     digitalWrite (31, HIGH); 
     digitalWrite (32, LOW);  
     digitalWrite (17, HIGH); 
     digitalWrite (18, LOW);  
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONgreat (41);} else {turnOFFgreat (41);}
     if  (digitalRead(38) == LOW) {turnONgreat (47);} else {turnOFFgreat (47);}
     if  (digitalRead(39) == LOW) {turnONgreat (53);} else {turnOFFgreat (53);}
     if  (digitalRead(40) == LOW) {turnONgreat (59);} else {turnOFFgreat (59);}     
     if  (digitalRead(41) == LOW) {turnONgreat (65);} else {turnOFFgreat (65);}
     if  (digitalRead(42) == LOW) {turnONgreat (71);} else {turnOFFgreat (71);}
     if  (digitalRead(43) == LOW) {turnONgreat (77);} else {turnOFFgreat (77);}
     if  (digitalRead(44) == LOW) {turnONgreat (83);} else {turnOFFgreat (83);} 
     if  (digitalRead(45) == LOW) {turnONgreat (89);} else {turnOFFgreat (89);}
     if  (digitalRead(46) == LOW) {turnONgreat (95);} else {turnOFFgreat (95);} 
     if  (digitalRead(48) == LOW) {turnONpedal (41);} else {turnOFFpedal (41);}
     if  (digitalRead(49) == LOW) {turnONpedal (47);} else {turnOFFpedal (47);}
     if  (digitalRead(50) == LOW) {turnONpedal (53);} else {turnOFFpedal (53);}
     if  (digitalRead(51) == LOW) {turnONpedal (59);} else {turnOFFpedal (59);}     
     if  (digitalRead(52) == LOW) {turnONpedal (65);} else {turnOFFpedal (65);}
     digitalWrite (32, HIGH); 
     digitalWrite (33, LOW);    
     digitalWrite (18, HIGH); 
     digitalWrite (19, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(36) == LOW) {turnONgreat (36);} else {turnOFFgreat (36);}   //bottom C
     if  (digitalRead(37) == LOW) {turnONgreat (42);} else {turnOFFgreat (42);}
     if  (digitalRead(38) == LOW) {turnONgreat (48);} else {turnOFFgreat (48);}
     if  (digitalRead(39) == LOW) {turnONgreat (54);} else {turnOFFgreat (54);}
     if  (digitalRead(40) == LOW) {turnONgreat (60);} else {turnOFFgreat (60);}     
     if  (digitalRead(41) == LOW) {turnONgreat (66);} else {turnOFFgreat (66);}
     if  (digitalRead(42) == LOW) {turnONgreat (72);} else {turnOFFgreat (72);}
     if  (digitalRead(43) == LOW) {turnONgreat (78);} else {turnOFFgreat (78);}
     if  (digitalRead(44) == LOW) {turnONgreat (84);} else {turnOFFgreat (84);} 
     if  (digitalRead(45) == LOW) {turnONgreat (90);} else {turnOFFgreat (90);}
     if  (digitalRead(46) == LOW) {turnONgreat (96);} else {turnOFFgreat (96);} 
     if  (digitalRead(47) == LOW) {turnONpedal (36);} else {turnOFFpedal (36);}   //bottom C
     if  (digitalRead(48) == LOW) {turnONpedal (42);} else {turnOFFpedal (42);}
     if  (digitalRead(49) == LOW) {turnONpedal (48);} else {turnOFFpedal (48);}
     if  (digitalRead(50) == LOW) {turnONpedal (54);} else {turnOFFpedal (54);}
     if  (digitalRead(51) == LOW) {turnONpedal (60);} else {turnOFFpedal (60);}     
     if  (digitalRead(52) == LOW) {turnONpedal (66);} else {turnOFFpedal (66);}
     digitalWrite (33, HIGH);   
     digitalWrite (19, HIGH);   
     delayMicroseconds(17);
}

//MIDI ON message is sent only if note is not already ON.
void turnONgreat(byte noteNumber) {
    if(noteNumber < 100) {
        if(greatDebounceArray[noteNumber] == 0) {
            noteOn(2, noteNumber, 127);
            greatDebounceArray[noteNumber] = debounceCount;
        }   
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void turnOFFgreat(byte noteNumber) {
    if(noteNumber < 100) {
        if(greatDebounceArray[noteNumber] == 1) {
            noteOff(2, noteNumber, 0);
        }  

        if(greatDebounceArray[noteNumber] > 0)
	    greatDebounceArray[noteNumber] -- ;
    }
}

//Scan Swell keyboard, convert to MIDI and output via port 0, channel 3.
void scanSwell () {
     digitalWrite (22, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONswell (37);} else {turnOFFswell (37);}
     if  (digitalRead(38) == LOW) {turnONswell (43);} else {turnOFFswell (43);}
     if  (digitalRead(39) == LOW) {turnONswell (49);} else {turnOFFswell (49);}
     if  (digitalRead(40) == LOW) {turnONswell (55);} else {turnOFFswell (55);}     
     if  (digitalRead(41) == LOW) {turnONswell (61);} else {turnOFFswell (61);}
     if  (digitalRead(42) == LOW) {turnONswell (67);} else {turnOFFswell (67);}
     if  (digitalRead(43) == LOW) {turnONswell (73);} else {turnOFFswell (73);}
     if  (digitalRead(44) == LOW) {turnONswell (79);} else {turnOFFswell (79);} 
     if  (digitalRead(45) == LOW) {turnONswell (85);} else {turnOFFswell (85);}
     if  (digitalRead(46) == LOW) {turnONswell (91);} else {turnOFFswell (91);} 
     digitalWrite (22, HIGH); 
     digitalWrite (23, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONswell (38);} else {turnOFFswell (38);}
     if  (digitalRead(38) == LOW) {turnONswell (44);} else {turnOFFswell (44);}
     if  (digitalRead(39) == LOW) {turnONswell (50);} else {turnOFFswell (50);}
     if  (digitalRead(40) == LOW) {turnONswell (56);} else {turnOFFswell (56);}     
     if  (digitalRead(41) == LOW) {turnONswell (62);} else {turnOFFswell (62);}
     if  (digitalRead(42) == LOW) {turnONswell (68);} else {turnOFFswell (68);}
     if  (digitalRead(43) == LOW) {turnONswell (74);} else {turnOFFswell (74);}
     if  (digitalRead(44) == LOW) {turnONswell (80);} else {turnOFFswell (80);} 
     if  (digitalRead(45) == LOW) {turnONswell (86);} else {turnOFFswell (86);}
     if  (digitalRead(46) == LOW) {turnONswell (92);} else {turnOFFswell (92);} 
     digitalWrite (23, HIGH); 
     digitalWrite (24, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONswell (39);} else {turnOFFswell (39);}
     if  (digitalRead(38) == LOW) {turnONswell (45);} else {turnOFFswell (45);}
     if  (digitalRead(39) == LOW) {turnONswell (51);} else {turnOFFswell (51);}
     if  (digitalRead(40) == LOW) {turnONswell (57);} else {turnOFFswell (57);}     
     if  (digitalRead(41) == LOW) {turnONswell (63);} else {turnOFFswell (63);}
     if  (digitalRead(42) == LOW) {turnONswell (69);} else {turnOFFswell (69);}
     if  (digitalRead(43) == LOW) {turnONswell (75);} else {turnOFFswell (75);}
     if  (digitalRead(44) == LOW) {turnONswell (81);} else {turnOFFswell (81);} 
     if  (digitalRead(45) == LOW) {turnONswell (87);} else {turnOFFswell (87);}
     if  (digitalRead(46) == LOW) {turnONswell (93);} else {turnOFFswell (93);} 
     digitalWrite (24, HIGH); 
     digitalWrite (25, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONswell (40);} else {turnOFFswell (40);}
     if  (digitalRead(38) == LOW) {turnONswell (46);} else {turnOFFswell (46);}
     if  (digitalRead(39) == LOW) {turnONswell (52);} else {turnOFFswell (52);}
     if  (digitalRead(40) == LOW) {turnONswell (58);} else {turnOFFswell (58);}     
     if  (digitalRead(41) == LOW) {turnONswell (64);} else {turnOFFswell (64);}
     if  (digitalRead(42) == LOW) {turnONswell (70);} else {turnOFFswell (70);}
     if  (digitalRead(43) == LOW) {turnONswell (76);} else {turnOFFswell (76);}
     if  (digitalRead(44) == LOW) {turnONswell (82);} else {turnOFFswell (82);} 
     if  (digitalRead(45) == LOW) {turnONswell (88);} else {turnOFFswell (88);}
     if  (digitalRead(46) == LOW) {turnONswell (94);} else {turnOFFswell (94);} 
     digitalWrite (25, HIGH); 
     digitalWrite (26, LOW);  
     delayMicroseconds(17);
     if  (digitalRead(37) == LOW) {turnONswell (41);} else {turnOFFswell (41);}
     if  (digitalRead(38) == LOW) {turnONswell (47);} else {turnOFFswell (47);}
     if  (digitalRead(39) == LOW) {turnONswell (53);} else {turnOFFswell (53);}
     if  (digitalRead(40) == LOW) {turnONswell (59);} else {turnOFFswell (59);}     
     if  (digitalRead(41) == LOW) {turnONswell (65);} else {turnOFFswell (65);}
     if  (digitalRead(42) == LOW) {turnONswell (71);} else {turnOFFswell (71);}
     if  (digitalRead(43) == LOW) {turnONswell (77);} else {turnOFFswell (77);}
     if  (digitalRead(44) == LOW) {turnONswell (83);} else {turnOFFswell (83);} 
     if  (digitalRead(45) == LOW) {turnONswell (89);} else {turnOFFswell (89);}
     if  (digitalRead(46) == LOW) {turnONswell (95);} else {turnOFFswell (95);} 
     digitalWrite (26, HIGH); 
     digitalWrite (27, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(36) == LOW) {turnONswell (36);} else {turnOFFswell (36);}   //bottom C
     if  (digitalRead(37) == LOW) {turnONswell (42);} else {turnOFFswell (42);}
     if  (digitalRead(38) == LOW) {turnONswell (48);} else {turnOFFswell (48);}
     if  (digitalRead(39) == LOW) {turnONswell (54);} else {turnOFFswell (54);}
     if  (digitalRead(40) == LOW) {turnONswell (60);} else {turnOFFswell (60);}     
     if  (digitalRead(41) == LOW) {turnONswell (66);} else {turnOFFswell (66);}
     if  (digitalRead(42) == LOW) {turnONswell (72);} else {turnOFFswell (72);}
     if  (digitalRead(43) == LOW) {turnONswell (78);} else {turnOFFswell (78);}
     if  (digitalRead(44) == LOW) {turnONswell (84);} else {turnOFFswell (84);} 
     if  (digitalRead(45) == LOW) {turnONswell (90);} else {turnOFFswell (90);}
     if  (digitalRead(46) == LOW) {turnONswell (96);} else {turnOFFswell (96);} 
     digitalWrite (27, HIGH);   
     delayMicroseconds(17);
}

//MIDI ON message is sent only if note is not already ON.
void turnONswell(byte noteNumber) {
    if(noteNumber < 100) {
        if(swellDebounceArray[noteNumber] == 0) {
            noteOn(1, noteNumber, 127);
            swellDebounceArray[noteNumber] = debounceCount;
        }   
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void turnOFFswell(byte noteNumber) {
    if(noteNumber < 100) {
        if(swellDebounceArray[noteNumber] == 1) {
            noteOff(1, noteNumber, 0);
        }  
        if(swellDebounceArray[noteNumber] > 0)
	    swellDebounceArray[noteNumber] -- ;       
    }
}

//Scan Pedal, convert to MIDI and output via port 0, channel 1.
void scanPedal() {
     digitalWrite (14, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(48) == LOW) {turnONpedal (37);} else {turnOFFpedal (37);}
     if  (digitalRead(49) == LOW) {turnONpedal (43);} else {turnOFFpedal (43);}
     if  (digitalRead(50) == LOW) {turnONpedal (49);} else {turnOFFpedal (49);}
     if  (digitalRead(51) == LOW) {turnONpedal (55);} else {turnOFFpedal (55);}     
     if  (digitalRead(52) == LOW) {turnONpedal (61);} else {turnOFFpedal (61);}
     if  (digitalRead(53) == LOW) {turnONpedal (67);} else {turnOFFpedal (67);}
     digitalWrite (14, HIGH); 
     digitalWrite (15, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(48) == LOW) {turnONpedal (38);} else {turnOFFpedal (38);}
     if  (digitalRead(49) == LOW) {turnONpedal (44);} else {turnOFFpedal (44);}
     if  (digitalRead(50) == LOW) {turnONpedal (50);} else {turnOFFpedal (50);}
     if  (digitalRead(51) == LOW) {turnONpedal (56);} else {turnOFFpedal (56);}     
     if  (digitalRead(52) == LOW) {turnONpedal (62);} else {turnOFFpedal (62);}
     digitalWrite (15, HIGH); 
     digitalWrite (16, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(48) == LOW) {turnONpedal (39);} else {turnOFFpedal (39);}
     if  (digitalRead(49) == LOW) {turnONpedal (45);} else {turnOFFpedal (45);}
     if  (digitalRead(50) == LOW) {turnONpedal (51);} else {turnOFFpedal (51);}
     if  (digitalRead(51) == LOW) {turnONpedal (57);} else {turnOFFpedal (57);}     
     if  (digitalRead(52) == LOW) {turnONpedal (63);} else {turnOFFpedal (63);}
     digitalWrite (16, HIGH); 
     digitalWrite (17, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(48) == LOW) {turnONpedal (40);} else {turnOFFpedal (40);}
     if  (digitalRead(49) == LOW) {turnONpedal (46);} else {turnOFFpedal (46);}
     if  (digitalRead(50) == LOW) {turnONpedal (52);} else {turnOFFpedal (52);}
     if  (digitalRead(51) == LOW) {turnONpedal (58);} else {turnOFFpedal (58);}     
     if  (digitalRead(52) == LOW) {turnONpedal (64);} else {turnOFFpedal (64);}
     digitalWrite (17, HIGH); 
     digitalWrite (18, LOW);  
     delayMicroseconds(17);
     if  (digitalRead(48) == LOW) {turnONpedal (41);} else {turnOFFpedal (41);}
     if  (digitalRead(49) == LOW) {turnONpedal (47);} else {turnOFFpedal (47);}
     if  (digitalRead(50) == LOW) {turnONpedal (53);} else {turnOFFpedal (53);}
     if  (digitalRead(51) == LOW) {turnONpedal (59);} else {turnOFFpedal (59);}     
     if  (digitalRead(52) == LOW) {turnONpedal (65);} else {turnOFFpedal (65);}
     digitalWrite (18, HIGH); 
     digitalWrite (19, LOW);    
     delayMicroseconds(17);
     if  (digitalRead(47) == LOW) {turnONpedal (36);} else {turnOFFpedal (36);}   //bottom C
     if  (digitalRead(48) == LOW) {turnONpedal (42);} else {turnOFFpedal (42);}
     if  (digitalRead(49) == LOW) {turnONpedal (48);} else {turnOFFpedal (48);}
     if  (digitalRead(50) == LOW) {turnONpedal (54);} else {turnOFFpedal (54);}
     if  (digitalRead(51) == LOW) {turnONpedal (60);} else {turnOFFpedal (60);}     
     if  (digitalRead(52) == LOW) {turnONpedal (66);} else {turnOFFpedal (66);}
     digitalWrite (19, HIGH);   
}

//MIDI ON message is sent only if note is not already ON.
void turnONpedal(byte noteNumber) {
    if(noteNumber < 100) {
        if(pedalDebounceArray[noteNumber] == 0) {
            noteOn(3, noteNumber, 127);
            pedalDebounceArray[noteNumber] = debounceCount;
        }
    }
}

//MIDI OFF message is sent only if note is not already OFF.
void turnOFFpedal(byte noteNumber) {
    if(noteNumber < 100) {
        if(pedalDebounceArray[noteNumber] == 1) {
            noteOff(3, noteNumber, 0);
        }
        if(pedalDebounceArray[noteNumber] > 0)
	    pedalDebounceArray[noteNumber] -- ;
    }
}

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
