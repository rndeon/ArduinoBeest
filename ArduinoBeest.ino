
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

int readingsX[numReadings];      // the readings from the analog input
int readingsY[numReadings];
int index = 0;                  // the index of the current reading
int totalX = 0; 
int totalY = 0;// the running total
int averageX = 0;                // the average
int averageY = 1;
int midX=489;
int midY=489;
int joystickX = A0;
int joystickY = A1;
int buttonPin = 0;
boolean on = false;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 500;    // the debounce time; increase if the output flickers

int scaleJoystickInput(int input){
  if (input >=500){
  //  Serial.println( sqrt(5*(input-500))/2);
   return sqrt(5*(input-midX))/2;
  } else {
  //  Serial.println(- sqrt(-5*(input-500))/2);
    return - sqrt(-5*(input-midX))/2;
  }
}

void setup() 
{ 
  leftservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  rightservo.attach(10); 
   // initialize serial communication with computer:
  Serial.begin(9600);   
  // initialize all the readings to half of their full values, this is the rest position of the joystick: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readingsX[thisReading] = 0;//(fullright-fullleft)/2;    
    readingsY[thisReading] = 0;//(fullback-fullforward)/2; 
  }
  pinMode(buttonPin, INPUT);
} 
 
 
void loop() 
{ 
  int reading = digitalRead(buttonPin);
  
  if (reading == LOW) {
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
  totalX= totalX - readingsX[index]; 
  totalY= totalY - readingsY[index];   
  // read from the sensor:  
  readingsX[index] = analogRead(joystickX); 
  readingsY[index] = analogRead(joystickY); 
  
  // add the reading to the total:
  totalX= totalX + readingsX[index];  
  totalY= totalY + readingsY[index];  
  // advance to the next position in the array:  
  index = index + 1;                    

  // if we're at the end of the array...
  if (index >= numReadings)              
    // ...wrap around to the beginning: 
    index = 0;                           

  // calculate the average:
  averageX = totalX / numReadings;
  averageY = totalY / numReadings;

  int leftupdate=0;
  int rightupdate=0;
  //if( averageY >=500){
    leftupdate = 90 + scaleJoystickInput(averageX) + scaleJoystickInput(averageY);
    rightupdate = 95 +scaleJoystickInput(averageX) - scaleJoystickInput(averageY);
 // } else { //this inverts left and right when going backwards. The tradeoffs: feel more natural when thinking in reverse, but can be very problematic when just trying to turn (can swtich directions with small accidental shifts in y
  //  leftupdate = 90 - scaleJoystickInput(averageX) + scaleJoystickInput(averageY);
  //  rightupdate = 95 -scaleJoystickInput(averageX) - scaleJoystickInput(averageY);
  //}
     Serial.print(rightupdate);
     Serial.println(" right servo speed.");
  if (on) {
    if (leftupdate != leftspeed){
     leftservo.write(leftupdate);
     
    }
    if (rightupdate != rightspeed){
     rightservo.write(rightupdate);
     
    }
    rightspeed = rightupdate;
    leftspeed = leftupdate;
 } else {
    Serial.println("off");
 }
  delay(75);       // waits 15ms for the servo to reach the position 
} 

