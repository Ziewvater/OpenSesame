/*
* Arduino send on/off to SmartThing
*/

#include <SoftwareSerial.h>
#include <SmartThings.h>

#define PIN_THING_RX    3
#define PIN_THING_TX    2

SmartThingsCallout_t messageCallout; // call out function forward decalaration
SmartThings smartthing(PIN_THING_RX, PIN_THING_TX, messageCallout);  // constructor

 //unchanging values:
const int ledPin = 13;      // led connected to digital pin 13
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int knockThreshold = 50;  // threshold value to decide when the detected sound is a knock or not
const int knockEcho = 75; // milliseconds of vibration during a knock
const int knockTimeout = 1000; // after this time, knocks are fully reset


// these variables will change:
int ledState = LOW;         // variable used to store the last LED status, to toggle the light


void setup() {
  // put your setup code here, to run once:
 pinMode(ledPin, OUTPUT); // declare the ledPin as as OUTPUT

 Serial.begin(115200);

}

bool knockListen() {
  
  static int state = 0;
  static unsigned long lastKnock = 0;
  static int knock[8];

  unsigned long time = millis();
  
  if (time - lastKnock < knockEcho) return false;
  
  if (state > 0 && time - lastKnock > knockTimeout) {
    Serial.println("Resetting knock.");
    state = 0;
  }

  int sensorReading = analogRead(knockSensor);
  if (sensorReading >= knockThreshold) {
    knock[state] = time - lastKnock;

    Serial.print("Knock: ");
    Serial.println(knock[state]);

    state++;
    lastKnock = time;
  }

  if (state > 4) {
    state = 0;
    //Serial.print("Testing timing...");
    if (knock[1] < knock[2] && knock[3] < knock[2] && knock[4] < knock[2]) {
      //Serial.print("Ok, 3 is longest");
      knock[2] = knock[2] * 0.5;
      if (knock[1] < knock[2] && knock[3] < knock[2] && knock[4] < knock[2]) {
        Serial.print(" Opening door.");
        return true;
      }
    }
  }
  
  return false;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  smartthing.run();

  if (knockListen()){
    //smartthing.send("held");
    digitalWrite(ledPin, true);
    smartthing.send("on");
    delay(1000);
    digitalWrite(ledPin, false);
    smartthing.send("off");

  }
  

// // read the sensor and store it in the variable sensorReading:
//  sensorReading = analogRead(knockSensor);
//
//  Serial.println(sensorReading);
//  delay(50);  // delay to avoid overloading the serial port buffer
//
//
//  
//
//  // if the sensor reading is greater than the threshold:
//  if (sensorReading >= threshold) {
//    // toggle the status of the ledPin:
//    ledState = !ledState;
//    // update the LED pin itself:
//    digitalWrite(ledPin, ledState);
//    // send the string "Knock!" back to the computer, followed by newline
//    Serial.println("Knock!");
//    if (ledState){
//    smartthing.send("on");
//    Serial.println("Opening"); }
//    else {
//    smartthing.send("off");
//    Serial.println("Closing"); }
//    Serial.println(sensorReading);
//    delay(10);  // delay to avoid overloading the serial port buffer
//  }  



//delay(10000);
//Serial.write("secret initiation");
//Serial.println();
//smartthing.send("on");
//delay(10000);
//Serial.write("secret off sequence");
//Serial.println();
//smartthing.send("off");



}

void messageCallout(String message) {
    Serial.print("Received message: '");
    Serial.print(message);
    Serial.println("' ");
}
