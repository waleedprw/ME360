#include <Servo.h>

Servo myservo;

#define motor 3               //motor power pin on motor shield, controls on/off
#define motorDirection 12     //direction control pin on motor shield
#define brakePin 9            //brake pin on the motor shield

#define servo 10              //pin 10 is automatic servo connection

#define laserEmitter 8        //Laser diode control pin
#define laserSensor 7         //Laser reciever control pin

#define ultrasonicIn A0       //Ultrasonic sensor echo input
#define ultrasonicOut 5       //Ultrasonic sensor trigger output

#define ledPin 13

const int dispTime = 2000;  //ms        Threshold for how long the motor should run to dispense 1 mask
const int dispSpeed = 255;  //analog    How fast the mask is dispensed, from 0 to 255
const int triggerDist = 20; //cm        An object must be within this many cm away from the distance sensor before it triggers 
const int doorLift = 20;    //degrees   Parameter that controls angle the servo will rotate when opening the door. Higher val means door opens more
const int cooldown = 3000;  //ms        How long must the dispenser wait before trying to dispense another mask

float duration, distance; 

boolean triggered = false;  //Boolean value for whether mask needs to be dispensed
boolean dispensed = true;   //Boolean for keeping track of if the mask was dispensed

void setup() {
  pinMode(motor,OUTPUT);          //Changes whether motor is on/off
  pinMode(motorDirection, OUTPUT); //Changes direction of motor with high and low
  pinMode(brakePin,OUTPUT);       //Brake used to stop the motor fast, dont really need this but its automatically wired onto motor shield, so we'll use it
  pinMode(laserEmitter, OUTPUT);  //Changes whether laser is on/off
  pinMode(laserSensor, INPUT);    //Reads high or low based on laser touching it
  pinMode(ledPin, OUTPUT);        //Change LED indicator if mask is empty
  pinMode(ultrasonicIn, INPUT);   //Ultrasonic sensor input pin
  pinMode(ultrasonicOut, OUTPUT); //Ultrasonic sensor output pin
  myservo.attach(servo);          // attach servo pin to board
  
  myservo.write(0);                   //set servo to 0
  digitalWrite(motor, LOW);           //turn off motor
  digitalWrite(brakePin,LOW);         //turn off brake
  digitalWrite(motorDirection, LOW);  //Sets the motor direction. Change HIGH/LOW if motor is spinning wrong direction
  digitalWrite(laserEmitter, HIGH);   //turn on the laser
  delay(200);                         //wait to start loop
}

void loop() {
  MeasureDistance();
  if (triggered==true){
    RaiseDoor();
    DispenseMask();
    LowerDoor(); 
  }
  delay(100); //Wait 200ms between checking if mask must be dispensed
}

void MeasureDistance(){
  digitalWrite(ultrasonicOut, LOW);  //Turn off transmitter
  delayMicroseconds(2);       //Wait a little
  digitalWrite(ultrasonicOut, HIGH); //Turn on the trasmitter
  delayMicroseconds(10);      // It is high for 10 microseconds as it sends out an 8 cycle burst from the transmitter which bounces off an object and hits the reciever
  digitalWrite(ultrasonicOut, LOW);  //Turn back off after 10us

  duration = pulseIn(ultrasonicIn, HIGH);   //Stores how long the pulse took to travel from the transmitter to reciever
  distance=(duration*.0343)/2;              //Speed of sound is in cm per microsecond. Waves hits objects and comes back
                                            //We know the distance from the sensor to the front edge of the dispenser.
                                            //Goal is to calculate the average time it takes for the waves to hit one's hand at the bottom of the dispenser
  
  if(distance<triggerDist) triggered = true;   //If something moves into the trigger distance range, change the triggered value to true
  else triggered = false;                     //If no change stay false
}


void DispenseMask(){
  analogWrite(motor, dispSpeed);        //turn on motor
  delay(dispTime/2);                    //Wait for half of the dispense time
  
  if(digitalRead(laserSensor) == LOW){  //If there is something blocking the laser reciever (i.e. a mask)
    dispensed = true;                   //tell the machine something was dispensed
  }
  else dispensed = false;               //If no change in laser reciever, nothing was dispened.

  delay(dispTime/2);                    //Wait for second half of the dispense time
  digitalWrite(motor, LOW);             //turn motor off
  digitalWrite(brakePin, HIGH);         //brake on
  delay(50);                            //wait till motor stops
  digitalWrite(brakePin, LOW);          //brake off
  ChangeLed();
}


void ChangeLed(){
  if(dispensed == false) digitalWrite(ledPin, HIGH); //If mask isn't dispensed, turn on the LED  
}


void RaiseDoor(){
  myservo.write(doorLift);  //Servo goes to doorlift angle
  delay(50);                //Small delay before starting to dispense mask
}


void LowerDoor(){
  myservo.write(0); //Servo goes to doorlift angle
  delay(cooldown);        //Small delay before starting to dispense mask
}
