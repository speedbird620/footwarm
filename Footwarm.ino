      /*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.

  To upload to your Gemma or Trinket:
  1) Select the proper board from the Tools->Board Menu
  2) Select USBtinyISP from the Tools->Programmer
  3) Plug in the Gemma/Trinket, make sure you see the green LED lit
  4) For windows, install the USBtiny drivers
  5) Press the button on the Gemma/Trinket - verify you see
     the red LED pulse. This means it is ready to receive data
  6) Click the upload button above within 10 seconds
*/

// Defenition I/O:s
int PinOut = 1;        // To MOSFET, pin 3 @ATtiny841
int inPlus = 8;     // Button notch up, pin 11 @ATtiny841
int inMinus = 10;   // Button notch down, pin 13 @ATtiny841
int voltIN = A1;    // Analog voltage battery in, pin 12 @ATtiny841
int Led1 = 7;       // Pin 10 @ATtiny841
int Led2 = 6;       // Pin 9 @ATtiny841
int Led3 = 5;       // Pin 8 @ATtiny841
int Led4 = 4;       // Pin 7 @ATtiny841
int Led5 = 3;       // Pin 6 @ATtiny841
int Led6 = 2;       // Pin 5 @ATtiny841
int Pin10 = 0;       // Pin 2 @ATtiny841

// Parameters
bool Init = HIGH;
float BattVoltage = 0;
int StepWidth = 100;
int PulseWidth = 0;
bool LowVoltage = LOW;
int FlankPlusMillis = 0;
int FlankMinusMillis = 0;
int ButtonTime = 600;
int Sync = 0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(PinOut, OUTPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);
  pinMode(Led5, OUTPUT);
  pinMode(Led6, OUTPUT);
  pinMode(inPlus, INPUT);
  pinMode(inMinus, INPUT);
}

// the loop routine runs over and over again forever:
void loop() {

  //if (Init) {
  //  Init = LOW;
  //}

  BattVoltage = (analogRead(voltIN) * (8.95/512));           // 512@8.95v

  if (BattVoltage < 7.6){
    LowVoltage = HIGH;
  }

  while (not digitalRead(inPlus) and not digitalRead(inMinus)){
    BattVoltage = (analogRead(voltIN) * (8.95/512));           // 512@8.95v
    LowVoltage = LOW;
    digitalWrite(Led1, (BattVoltage < 7)); 
    digitalWrite(Led2, (BattVoltage >= 7 and BattVoltage < 10 )); 
    digitalWrite(Led3, (BattVoltage >= 10 and BattVoltage < 12 )); 
    digitalWrite(Led4, (BattVoltage >= 12 and BattVoltage < 12.5 )); 
    digitalWrite(Led5, (BattVoltage >= 12.5 and BattVoltage < 14 )); 
    digitalWrite(Led6, (BattVoltage >= 14)); 
  }

  if (LowVoltage) {
    digitalWrite(Led1, LOW); 
    digitalWrite(Led2, LOW); 
    digitalWrite(Led3, LOW); 
    digitalWrite(Led4, LOW); 
    digitalWrite(Led5, LOW); 
    digitalWrite(Led6, LOW); 
    digitalWrite(PinOut, LOW); 
    delay(100);
    digitalWrite(Led1, HIGH); 
    delay(100);
  }
  else {
  
    if (not digitalRead(inPlus)) {
      PulseWidth = PulseWidth + StepWidth;
      if (PulseWidth > 500) {
        PulseWidth = 500;
      }
    }
  
    if (not digitalRead(inMinus)) {
      PulseWidth = PulseWidth - StepWidth;
      if (PulseWidth < 0) {
        PulseWidth = 0;
      }
    }
  
    digitalWrite(Led1, HIGH); 
    digitalWrite(Led2, (PulseWidth > 99)); 
    digitalWrite(Led3, (PulseWidth > 199)); 
    digitalWrite(Led4, (PulseWidth > 299)); 
    digitalWrite(Led5, (PulseWidth > 399)); 
    digitalWrite(Led6, (PulseWidth > 499)); 

    digitalWrite(PinOut, HIGH); 
    delay (PulseWidth);
    digitalWrite(PinOut, LOW); 
    delay ((StepWidth * 5) - PulseWidth + 25);
  }
}
