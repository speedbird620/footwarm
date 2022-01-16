/*
  Footwarm verion - the code
  The manual: to be written
  The source code: https://github.com/speedbird620/footwarm
  The webpage: https://skyracer.net/

  Revision 0.1, first released version
  
  avionics@skyracer.net
*/

// Confugurable parameters for adjustment for each application
int ShuntFactor = 100;  // To be set between 0 and 100%
int BatteryType = 1;    // 1 = 2S LiPo/LiIon, 2 = 3S LiPo/LiIon, 3 = 2S LiFePo4, 4 = 3S LiFePo4, 5 = 4S LiFePo4, 6 = Lead 12v
int ButtonTime = 600;   // The time in ms for a button to be pressed before notching up/down

// Pls do not alter any of the values below

// Defenition the I/O pin
// Please note: pin mapping is set to be reversed in the compiler
int PinOut = 1;     // To MOSFET, pin 3 @ATtiny841
int inPlus = 8;     // Button notch up, pin 11 @ATtiny841
int inMinus = 10;   // Button notch down, pin 13 @ATtiny841
int voltIN = A1;    // Analog voltage battery in, pin 12 @ATtiny841
int Led1 = 7;       // Pin 10 @ATtiny841, connected to LED 1 (green)
int Led2 = 6;       // Pin 9 @ATtiny841, connected to LED 2 (green)
int Led3 = 5;       // Pin 8 @ATtiny841, connected to LED 3 (green)
int Led4 = 4;       // Pin 7 @ATtiny841, connected to LED 4 (orange)
int Led5 = 3;       // Pin 6 @ATtiny841, connected to LED 5 (red)
int Led6 = 2;       // Pin 5 @ATtiny841, connected to LED 6 (red)
int Pin10 = 0;      // Pin 2 @ATtiny841, not connected but can be bused to indicate status when debugging

// Parameters
int StepWidth = 100;                  // The pulse lengths will be multiples of this value 
int PulseLength = 0;                  // The pulse length of the ouput
unsigned long FlankPlusMillis = 0;    // Used to indicate flank when the notch up button is pressed
unsigned long FlankMinusMillis = 0;   // Used to indicate flank when the notch down button is pressed
unsigned long Sync = 0;               // Time of the last real time action
int PulseLengthLow, PulseLengthHigh;  // Internal values for calculated pulsemodulation
bool LowVoltage = LOW;                // If low voltage hade been detected, thie value is set and will interlock any output
float BattVoltage;                    // The voltage of the incoming power.
float VoltThr1, VoltThr2, VoltThr3, VoltThr4, VoltThr5;   // The voltage threshold for indication of the incoming voltage 

// The setup routine runs upon boot
void setup() {
  // initialize the input and output pins
  pinMode(PinOut, OUTPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);
  pinMode(Led4, OUTPUT);
  pinMode(Led5, OUTPUT);
  pinMode(Led6, OUTPUT);
  pinMode(Pin10, OUTPUT);
  pinMode(inPlus, INPUT);
  pinMode(inMinus, INPUT);

  // Applying voltage values for the different battery types.
  // Note, These figues does only apply on batteries at 25°C and the measurement is not precise. It shall only be used as an indication.
  if (BatteryType == 1) {   // 2S LiPo/LiIon
    VoltThr1 = 7.53;        // 30%
    VoltThr2 = 7.63;        // 45%
    VoltThr3 = 7.75;        // 60%
    VoltThr4 = 7.97;        // 75%
    VoltThr5 = 8.22;        // 90%
  }
  if (BatteryType == 2) {   // 3S LiPo/LiIon
    VoltThr1 = 11.3;        // 30%
    VoltThr2 = 11.45;       // 45%
    VoltThr3 = 11.62;       // 60%
    VoltThr4 = 11.95;       // 75%
    VoltThr5 = 12.33;       // 90%
  }
  if (BatteryType == 3) {   // 2S LiFePo4
    VoltThr1 = 6.5;         // 30%
    VoltThr2 = 6.54;        // 45%
    VoltThr3 = 6.59;        // 60%
    VoltThr4 = 6.62;        // 75%
    VoltThr5 = 6.65;        // 90%
  }
  if (BatteryType == 4) {   // 3S LiPo/LiIon
    VoltThr1 = 9.75;        // 30%
    VoltThr2 = 9.8;         // 45%
    VoltThr3 = 9.88;        // 60%
    VoltThr4 = 9.92;        // 75%
    VoltThr5 = 9.98;        // 90%
  }
  if (BatteryType == 5) {   // 4S LiFePo4
    VoltThr1 = 13.00;       // 30%
    VoltThr2 = 13.07;       // 45%
    VoltThr3 = 13.17;       // 60%
    VoltThr4 = 13.23;       // 75%
    VoltThr5 = 13.3;        // 90%
  }
  if (BatteryType == 6) {   // 12v Lead acid
    VoltThr1 = 11.75;       // 30%
    VoltThr2 = 11.98;       // 45%
    VoltThr3 = 12.2;        // 60%
    VoltThr4 = 12.37;       // 75%
    VoltThr5 = 12.50;       // 90%
  }
}

// A function used for picking up the voltage from the incoming power
float GetVoltage() {
  float Result;
  Result = (analogRead(voltIN) * (8.95/512));   // 512@8.95v
  return Result;
}

// the loop routine runs over and over again forever:
void loop() {

  // Cecking the incoming voltage
  BattVoltage = GetVoltage();   // Measure the incoming voltage
  if (BattVoltage < VoltThr1){  
    // If the incoming voltage is below 30% the footwarmer shall inhibit operation in order to spare the battery from damage
    LowVoltage = HIGH;
  }

  // Both button are pressed at the same time, time to indicate the incoming voltage and reset the low voltage inhibation
  if (not digitalRead(inPlus) and not digitalRead(inMinus)){
    LowVoltage = LOW;                                                           // Reset of low voltage inhibation
    BattVoltage = GetVoltage();                                                 // Measure the incoming voltage
    digitalWrite(Led6, (BattVoltage < VoltThr1));                               // Light LED 1 if voltage is < 30%
    digitalWrite(Led5, (BattVoltage >= VoltThr1 and BattVoltage < VoltThr2));   // Light LED 2 if voltage is 30% to 45%
    digitalWrite(Led4, (BattVoltage >= VoltThr2 and BattVoltage < VoltThr3));   // Light LED 3 if voltage is 45% to 60%
    digitalWrite(Led3, (BattVoltage >= VoltThr3 and BattVoltage < VoltThr4));   // Light LED 4 if voltage is 60% to 75%
    digitalWrite(Led2, (BattVoltage >= VoltThr4 and BattVoltage < VoltThr5));   // Light LED 5 if voltage is 75% to 90%
    digitalWrite(Led1, (BattVoltage >= VoltThr5));                              // Light LED 6 if voltage is > 90%
  }
  // Inhibition of operation
  else if (LowVoltage) {
    // Shut down all of the LED
    digitalWrite(Led1, LOW);    
    digitalWrite(Led2, LOW); 
    digitalWrite(Led3, LOW); 
    digitalWrite(Led4, LOW); 
    // Shut down the output as well
    digitalWrite(PinOut, LOW); 
    // Except the LED 5 and 6 what shall be blinking in order to indicate the detection of low voltage
    digitalWrite(Led5, HIGH); 
    digitalWrite(Led6, LOW); 
    delay(100);
    digitalWrite(Led5, LOW); 
    digitalWrite(Led6, HIGH); 
    delay(100);
  }
  else {
    // Calculating the pulselength where the sole shall be energized
    PulseLengthHigh = (PulseLength - ((PulseLength/StepWidth)* (100-ShuntFactor)));

    // Calculating the pulselength where the sole shall be de-energized
    PulseLengthLow = (StepWidth * 5) - PulseLengthHigh;

    // Detecting when the time of setting the output high (yes, it is done in a complicated way, but this is real time programming)
    if (not digitalRead(PinOut) and millis() > (Sync + PulseLengthLow)) {
      // Setting a new time stamp
      Sync = millis();
      // Setting the ouput
      digitalWrite(PinOut, HIGH); 
    }

    // Detecting when the time of setting the output low
    if (digitalRead(PinOut) and millis() > (Sync + PulseLengthHigh)) {
      // Setting a new time stamp
      Sync = millis();
      // Setting the ouput
      digitalWrite(PinOut, LOW); 
    }

  // Lets indicate how much power is used to fry the soles 
  digitalWrite(Led1, HIGH);                               // Just an indication that we got power to the footwarmer
  digitalWrite(Led2, (PulseLength >= StepWidth));         // Frying the soles with 20%
  digitalWrite(Led3, (PulseLength >= (StepWidth * 2)));   // Frying the soles with 40%
  digitalWrite(Led4, (PulseLength >= (StepWidth * 3)));   // Frying the soles with 60%
  digitalWrite(Led5, (PulseLength >= (StepWidth * 4)));   // Frying the soles with 80%
  digitalWrite(Led6, (PulseLength >= (StepWidth * 5)));   // Frying the soles with 100%
  }    

  // The button for notch up is pressed
  if (not digitalRead(inPlus)) {
    // Is this the first scan cycle where the putton is pressed or has the variable been reset?
    if (FlankPlusMillis == 0) {
      // Yes it is, then set a time stamp
      FlankPlusMillis = millis();
    }
  }
  // The button for notch up is un-pressed
  else {
      // Reset the time stamp
      FlankPlusMillis = 0;
  }

  // The button for notch down is pressed
  if (not digitalRead(inMinus)) {
    // Is this the first scan cycle where the putton is pressed or has the variable been reset?
    if (FlankMinusMillis == 0) {
      // Wohooo! Let´s set a time stamp
      FlankMinusMillis = millis();
    }
  }
  // The button for notch up is un-pressed
  else {
      // Reset the time stamp
      FlankMinusMillis = 0;
  }

  // Cheking if the button has been pressed longer than the time specified by the parameter "ButtonTime"
  if (FlankPlusMillis != 0 and millis() > (FlankPlusMillis + ButtonTime)) {
    // The button has been pressed for longer than the parameter "ButtonTime", resetting the flank detection
    FlankPlusMillis = 0;
    // Notching up the pulse length
    PulseLength = PulseLength + StepWidth;
    // ... but not too much ...
    if (PulseLength > (5 * StepWidth)) {
      // ... not more than 5 times the pulse length
      PulseLength = (5 * StepWidth);
    }
  }

  // Checking if the button has been pressed longer than the time specified by the parameter "ButtonTime"
  if (FlankMinusMillis != 0 and millis() > (FlankMinusMillis + ButtonTime)) {
    // The button has been pressed for longer than the parameter "ButtonTime", resetting the flank detection
    FlankMinusMillis = 0;
    // Notching down the pulse length
    PulseLength = PulseLength - StepWidth;
    // But not lower than 0 (zero)
    if (PulseLength < 0) {
      // There you go. Now carry on!
      PulseLength = 0;
    }
  }  
}
