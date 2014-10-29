// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo leftservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
Servo rightservo;
 
int leftspeed = 90;    // variable to store the servo speed. 90 is non-moving. 180 is full one direction, 0 is full backwards.
int rightspeed = 90;

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int dimmerReadings[numReadings];
int index = 0;                  // the index of the current reading
int total = 0; 
int dimmerTotal = 0;// the running total
int average = 0;                // the average
int dimmerAverage = 1;
int fullleft = 0;
int fullright = 1050;
int fullscale= fullright - fullleft;
int inputPin = A0;
int dimmerPin = A1;
int buttonPin = 4;
boolean on = false;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 500;    // the debounce time; increase if the output flickers



void setup() 
{ 
  leftservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  rightservo.attach(10); 
   // initialize serial communication with computer:
  Serial.begin(9600);   
  // initialize all the readings to 0: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings[thisReading] = 0;    
    dimmerReadings[thisReading] = 0; 
  }
  pinMode(buttonPin, INPUT);
} 
 
 
void loop() 
{ 
  int reading = digitalRead(buttonPin);
  
  if (reading == HIGH) {
    if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
      on = !on;
      leftservo.write(90);
      rightservo.write(95);
      lastDebounceTime = millis();
      Serial.println("toggled power");
    }
  }
  //Serial.println(on);
  // subtract the last reading:
  total= total - readings[index];         
  // read from the sensor:  
  readings[index] = analogRead(inputPin); 
  // add the reading to the total:
  total= total + readings[index];       
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  average = total / numReadings;         
  // send it to the computer as ASCII digits
  Serial.println(average);   
//  delay(1);        // delay in between reads for stability
  int leftUpdateTemp = 0;
  int rightUpdateTemp = 0;
  
  int servoSpeeds[7] = {30,30,30,30,20,10,-5};
  int servoIndex = 7*(average - fullleft) / fullscale;
  leftUpdateTemp = servoSpeeds[servoIndex];
  rightUpdateTemp = servoSpeeds[6 - servoIndex];
  
  int leftupdate = 90 - (leftUpdateTemp * 1) * dimmerAverage;
  int rightupdate = 95 + (rightUpdateTemp * 1) * dimmerAverage;
    
  
  if (on) {
    if (leftupdate != leftspeed){
     leftservo.write(leftupdate);  
     Serial.println("left servo updated");
    }
    if (rightupdate != rightspeed){
     rightservo.write(rightupdate);
     Serial.println("left servo updated");
    }
    rightspeed = rightupdate;
    leftspeed = leftupdate;
  } else {
    
  }
  delay(75);                       // waits 15ms for the servo to reach the position 

 // for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  //{                                  // in steps of 1 degree 
  //  myservo.write(pos);              // tell servo to go to position in variable 'pos' 
  //  delay(15);                       // waits 15ms for the servo to reach the position 
  //} 
  //for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  //{                                
   // myservo.write(pos);              // tell servo to go to position in variable 'pos' 
   // delay(15);                       // waits 15ms for the servo to reach the position 
  //} 
} 
