#include <MIDI.h>

//rotary encoder Pins - DO NOT CHANGE!
const int encoderPin1 = 2;
const int encoderPin2 = 3;

//shift register Pins
const int dataPin = 8;
const int latchPin = 9;
const int clockPin = 4;

//pins of all 16 rows
const int row_1 = 22;
const int row_2 = 24;
const int row_3 = 26;
const int row_4 = 28;
const int row_5 = 30;
const int row_6 = 32;
const int row_7 = 34;
const int row_8 = 36;
const int row_9 = 40;
const int row_10 = 42;
const int row_11 = 44;
const int row_12 = 46;
const int row_13 = 48;
const int row_14 = 50;
const int row_15 = 52;
const int row_16 = 54;

const int note_1 = 36;               //set Midi notes
const int note_2 = 38;               //reference Midi mapping: http://newt.phys.unsw.edu.au/jw/notes.html
const int note_3 = 40;
const int note_4 = 42;
const int note_5 = 44;
const int note_6 = 46;
const int note_7 = 48;
const int note_8 = 50;
const int note_9 = 52;
const int note_10 = 54;
const int note_11 = 56;
const int note_12 = 58;
const int note_13 = 60;
const int note_14 = 62;
const int note_15 = 64;
const int note_16 = 65;

const int columns = 64;              //set number of columns to play (2-64)
const int rotSens = 10;              //set sensibility of encoder, 100 == 1 rotation

const int modeSwitch = 11;           //switch for AUTO MODE
 
const int duration = 500;            //set duration for tone length

unsigned int speedDelay = 75;        //set default speed if start with auto mode
unsigned long timeold;
int speedCount = 0;
int speedCalcNum = 10;               //set after how many columns we calculate the speed

byte byte1;                          //data bytes for shift register
byte byte2;
byte byte3;        
byte byte4;
byte byte5;        
byte byte6;
byte byte7;        
byte byte8;

int pos[8] = {1,2,4,8,16,32,64,128};    
int LEDPos = 1;                      //starts with first column

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;

void setup() {
  
  MIDI.begin(4);                     //configure MIDI and Serial communication
  Serial.begin(115200);
  
  pinMode(dataPin, OUTPUT);          //configure shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  pinMode(encoderPin1, INPUT);       //configure encoder
  pinMode(encoderPin2, INPUT);
  
  pinMode(row_1, INPUT);             //configure rows
  pinMode(row_2, INPUT);
  pinMode(row_3, INPUT);
  pinMode(row_4, INPUT);
  pinMode(row_5, INPUT);
  pinMode(row_6, INPUT);
  pinMode(row_7, INPUT);
  pinMode(row_8, INPUT);
  pinMode(row_9, INPUT);
  pinMode(row_10, INPUT);
  pinMode(row_11, INPUT);
  pinMode(row_12, INPUT);
  pinMode(row_13, INPUT);
  pinMode(row_14, INPUT);
  pinMode(row_15, INPUT);
  pinMode(row_16, INPUT);

  digitalWrite(encoderPin1, HIGH);    //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH);    //turn pullup resistor on

  //call updateEncoder() when any high/low changed seen
  //on interrupt 0 (pin 2), or interrupt 1 (pin 3) 
  attachInterrupt(0, updateEncoder, CHANGE); 
  attachInterrupt(1, updateEncoder, CHANGE);
  
  pinMode(modeSwitch, INPUT);
      
  updateShiftRegister();             //reset shift register, start at LEDPos = 1

}

void loop(){ 
 
 //AUTO MODE
 if(digitalRead(modeSwitch) == HIGH){
    for(LEDPos; LEDPos < columns+2; LEDPos++)
        delay(speedDelay),
        updateShiftRegister(),
        checkMIDI();
   }
   
  //MANUAL MODE
   else
   {
   if(encoderValue > rotSens)
      LEDPos = LEDPos++, speedCount = speedCount++, encoderValue = 0;
      
    else if(encoderValue < -rotSens)
      LEDPos = LEDPos--, encoderValue = 0;
    
    
    
 //calculate rotSpeedDelay
 if(speedCount > speedCalcNum)
   speedDelay = (millis()-timeold)/speedCalcNum,
   speedCount = 0,
   timeold = millis();
   
   }
 //LEDPos loop    
    if(LEDPos < 1)
       LEDPos = columns;
 
    if(LEDPos > columns)
       LEDPos = 1;
       
    updateShiftRegister();
    checkMIDI();
    
    //END of LOOP
 }
 

void updateEncoder(){
  int MSB = digitalRead(encoderPin1);      //MSB = most significant bit
  int LSB = digitalRead(encoderPin2);      //LSB = least significant bit

  int encoded = (MSB << 1) |LSB;           //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

if(sum == 13 || sum == 4 || sum == 2 || sum == 11) encoderValue ++;
if(sum == 14 || sum == 7 || sum == 1 || sum == 8 ) encoderValue --;

  lastEncoded = encoded;                   //store this value for next time
}



void updateShiftRegister()
{      
       //reset bytes
       byte1 = 0;        
       byte2 = 0;
       byte3 = 0;        
       byte4 = 0;
       byte5 = 0;        
       byte6 = 0;
       byte7 = 0;        
       byte8 = 0;

     //convert LEDPos for shift register
     if(LEDPos < 9 && LEDPos > 0)
        byte1 = pos[LEDPos-1];
     if(17-LEDPos < 9 && 17-LEDPos > 0)
        byte2 = pos[LEDPos-9];
     if(25-LEDPos < 9 && 25-LEDPos > 0)
        byte3 = pos[LEDPos-17];
     if(33-LEDPos < 9 && 33-LEDPos > 0)
        byte4 = pos[LEDPos-25];
     if(41-LEDPos < 9 && 41-LEDPos > 0)
        byte5 = pos[LEDPos-33];
     if(49-LEDPos < 9 && 49-LEDPos > 0)
        byte6 = pos[LEDPos-41];
     if(57-LEDPos < 9 && 57-LEDPos > 0)
        byte7 = pos[LEDPos-49];
     if(65-LEDPos < 9 && 65-LEDPos > 0)
        byte8 = pos[LEDPos-57];
        
       //send data to shift register
       digitalWrite(latchPin, LOW); 
       
        shiftOut(dataPin, clockPin, MSBFIRST, byte8);         
        shiftOut(dataPin, clockPin, MSBFIRST, byte7);
        shiftOut(dataPin, clockPin, MSBFIRST, byte6);  
        shiftOut(dataPin, clockPin, MSBFIRST, byte5);
        shiftOut(dataPin, clockPin, MSBFIRST, byte4);         
        shiftOut(dataPin, clockPin, MSBFIRST, byte3);
        shiftOut(dataPin, clockPin, MSBFIRST, byte2);         
        shiftOut(dataPin, clockPin, MSBFIRST, byte1);
       
       digitalWrite(latchPin, HIGH);  
        
}

void checkMIDI()
{
  //send Midi notes
  if(digitalRead(row_1) == HIGH)
     MIDI.sendNoteOn(note_1,127,1);
  
  if(digitalRead(row_2) == HIGH)
     MIDI.sendNoteOn(note_2,127,1);    

  if(digitalRead(row_3) == HIGH)
     MIDI.sendNoteOn(note_3,127,1); 
     
  if(digitalRead(row_4) == HIGH)
     MIDI.sendNoteOn(note_4,127,1); 
  
  
  //stop sending Midi notes
  delay(duration);
  MIDI.sendNoteOff(note_1,0,1);
  MIDI.sendNoteOff(note_2,0,1); 
  MIDI.sendNoteOff(note_3,0,1); 
  MIDI.sendNoteOff(note_4,0,1);
     
}
