
#include <Servo.h> 
 
Servo leftservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
Servo rightservo;

//digital pins
int leftservopin =0;
int rightservopin =1;
int buttonPin = 2;
int signalLED = 3;//visible sign that the sensor detect edge of table
//int probeLED = 9; //needs to be on to for sensor to be measuring. possibly just connect the sensor to power instead.

//analog pins
int joystickX = A0;
int joystickY = A1;
#define SENSOR_NUM 2
int sensorPin[SENSOR_NUM] = {A5,A4}; 



int leftspeed = 90;    // variable to store the servo speed. 90 is non-moving. 180 is full one direction, 0 is full backwards.
int rightspeed = 90;

// Define the number of samples to keep track of.  The higher the number,
// the more the readings will be smoothed, but the slower the output will
// respond to the input.  Using a constant rather than a normal variable lets
// use this value to determine the size of the readings array.
const int numReadings = 10;
//Joystick readings
int readingsX[numReadings];      // the readings from the analog input
int readingsY[numReadings];
int index = 0;                  // the index of the current reading
int totalX = 0; 
int totalY = 0;// the running total
int averageX = 0;                // the average
int averageY = 1;
int midX=493;
int midY=499;


boolean on = false;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 1500;    // the debounce time; increase if the output flickers
//Infrared Distance Sensors for testing if the edge of the table is near (or, alternately, if a wall is in the way)
int sensorValue[SENSOR_NUM]={0,0};
int sensorThreshold[SENSOR_NUM] = {150,150};
int sensorHigh = 500;
int sensorLow =450;


int scaleJoystickInput(int input,int center){
  if (input >=center){
  //  Serial.println( sqrt(5*(input-500))/2);
   return sqrt(5*(input-center))/2;
  } else {
  //  Serial.println(- sqrt(-5*(input-500))/2);
    return - sqrt(-5*(input-center))/2;
  }
}

void setup() 
{ 
  leftservo.attach(leftservopin);  // attaches the servo on pin 9 to the servo object 
  rightservo.attach(rightservopin); 
   // initialize serial communication with computer:
  Serial.begin(9600);   
  // initialize all the readings to half of their full values, this is the rest position of the joystick: 
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readingsX[thisReading] = 0;//(fullright-fullleft)/2;    
    readingsY[thisReading] = 0;//(fullback-fullforward)/2; 
  }
  pinMode(buttonPin, INPUT);
  pinMode(signalLED, OUTPUT);
  
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
  Serial.print("X: ");
    Serial.print(averageX);
    Serial.print(", Y: ");
    Serial.println(averageY);
  if( averageX >=midX){
    leftupdate = 90 - scaleJoystickInput(averageY,midY) - scaleJoystickInput(averageX,midX);
    rightupdate = 95 - scaleJoystickInput(averageY,midY) + scaleJoystickInput(averageX,midX);
  } else { //this inverts left and right when going backwards. The tradeoffs: feel more natural when thinking in reverse, but can be very problematic when just trying to turn (can swtich directions with small accidental shifts in y
    leftupdate = 90  + scaleJoystickInput(averageY,midY) - scaleJoystickInput(averageX,midX);
    rightupdate = 95 + scaleJoystickInput(averageY,midY) + scaleJoystickInput(averageX,midX);
  }
    // Serial.print(rightupdate);
    // Serial.println(" right servo speed.");
    
 // IR proximity sensor pointed at the floor. If signal disappears, we came to the edge of the table, so don't let the robot move forward.
 // I also have some signal leds wired up to change from green to red.
for (unsigned i=0; i< SENSOR_NUM;i++)
{
 sensorValue[i] = analogRead(sensorPin[i]);
 Serial.print(i);
 Serial.print(":");
 Serial.print(sensorLow);
 Serial.print("---");
 Serial.print(sensorValue[i]);
 Serial.print("---");
 Serial.println(sensorHigh);
// if (sensorValue[i] < sensorLow && sensorValue >0){
//  sensorLow = sensorValue[i]; 
//  sensorThreshold = sensorLow + (sensorLow +sensorHigh)/5;
// } else if (sensorValue[i] > sensorHigh){
//   sensorHigh = sensorValue[i];
// }
}
  if ( sensorValue[1] < sensorThreshold[1]){
    digitalWrite(signalLED, HIGH); 
    if (leftupdate < 90){ leftupdate = 90;}
    if (rightupdate > 95){ rightupdate = 95;}
  } else if ( sensorValue[0] < sensorThreshold[0]){
    digitalWrite(signalLED, HIGH); 
    if (leftupdate > 90){ leftupdate = 90;}
    if (rightupdate < 95){ rightupdate = 95;}
  } else{
    digitalWrite(signalLED,LOW); 
  }
 
 //turns the servos only if the on/off toggle button is on.
 
  if (on) {
    Serial.print("left: ");
    Serial.print(leftupdate);
    Serial.print(", right: ");
    Serial.println(rightupdate);
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

