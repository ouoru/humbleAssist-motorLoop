#include <Servo.h>
#include <StopWatch.h>

int pushButtonUp = 4;// YELLOW button, declare which pin is for pushButtonUp
int pushButtonDown = 8;// BLUE button, declare which pin is for pushButtonUp
int buttonEStop = 13;// declare which pin is for buttonEStop

int upButtonState = 0;
int downButtonState = 0;
int eStopButtonState = 0;

long pwmOutput = 3;// declare output pin for pwm to controller

//config
boolean noTimeLimit = true;
long restPower = 1500;
long upPower = 1000;
long downPower = 2000;
int operationTime = 3000;

//operation states
boolean finishedOperation = false;
boolean reachedTop = false;
boolean reachedBottom = true; //ASSUME initial condition is at bottom
long remainingUpTime = operationTime;
long remainingDownTime = operationTime;
boolean firstLoop = true;

Servo myservo;
StopWatch MySW;

void setup() {
  Serial.begin(9600); // initializes the serial communication at 9600 bits per second bw arduino and comp
    
  pinMode (pwmOutput, OUTPUT);
  pinMode (pushButtonUp, INPUT);
  pinMode (pushButtonDown, INPUT);
  pinMode (buttonEStop, INPUT);
  
  myservo.attach(3);
  
  myservo.writeMicroseconds(1500); // set servo to mid-point
}

void loop() {
  firstLoop = true;
  upButtonState = digitalRead(pushButtonUp);
  downButtonState = digitalRead(pushButtonDown);
  eStopButtonState = digitalRead(buttonEStop);
  
  if (firstLoop == true){
    delay(100); // 250 milliseconds delay (1/4 second delay)
  };
  if (MySW.elapsed() != 0){
      Serial.println("RESETTING TIMER");
      MySW.reset();
  }

  while (eStopButtonState === 1) {
    eStopButtonState = digitalRead(buttonEStop);
    myservo.writeMicroseconds(restPower);
  }

  while (upButtonState == 1 && downButtonState != 1 && finishedOperation == false && reachedTop == false) {
    if (firstLoop == true){
      Serial.println("Up");
    }
    firstLoop = false;
    MySW.start();
    reachedBottom = false;
    myservo.writeMicroseconds(upPower);

    upButtonState = digitalRead(pushButtonUp);
    eStopButtonState = digitalRead(buttonEStop);

    if (eStopButtonState === 1) {
      myservo.writeMicroseconds(restPower);
      break;
    }

    //if top position reached
    if (MySW.elapsed() > remainingUpTime && noTimeLimit == false) {
      finishedOperation = true;
      reachedTop = true;
      Serial.println("Held for duration of:");
      Serial.println(MySW.elapsed());
      remainingUpTime = 0;
      remainingDownTime = operationTime;
      myservo.writeMicroseconds(1500);
      Serial.println("User is Fully Standing");
      Serial.println("Remaining Up time");
      Serial.println(remainingUpTime);
      Serial.println("Remaining Down time");
      Serial.println(remainingDownTime);
      MySW.stop();
      break;
    }
    //if interrupted during up process
    else if (upButtonState != 1) {
      Serial.println("Held for duration of:");
      Serial.println(MySW.elapsed());
      remainingUpTime = remainingUpTime - MySW.elapsed();
      remainingDownTime = operationTime - remainingUpTime;
      myservo.writeMicroseconds(restPower);
      Serial.println("User is Partially Standing");
      Serial.println("Remaining Up time");
      Serial.println(remainingUpTime);
      Serial.println("Remaining Down time");
      Serial.println(remainingDownTime);
      MySW.stop();
      break;
    }
  }
  
  while (upButtonState == 1 && downButtonState == 1) {
    //analogWrite (pwmOutput, 60); // need to figure out 0->255 range for pwm output
    Serial.println("both pressed"); // Print out status of Upbuton to the serial monitor
    break;
  }
  
  while(upButtonState != 1 && downButtonState == 1 && finishedOperation == false && reachedBottom == false) {
    if (firstLoop == true){
      Serial.println("Down");
    }
    firstLoop = false;
    MySW.start();
    reachedTop = false;
    myservo.writeMicroseconds(downPower);

    downButtonState = digitalRead(pushButtonDown);
    eStopButtonState = digitalRead(buttonEStop);

    if (eStopButtonState === 1) {
      myservo.writeMicroseconds(restPower);
      break;
    }

    //if bottom position reached
    if (MySW.elapsed() > remainingDownTime && noTimeLimit == false) {
      finishedOperation = true;
      reachedBottom = true;
      Serial.println("Held for duration of:");
      Serial.println(MySW.elapsed());
      remainingUpTime = operationTime;
      remainingDownTime = 0;
      myservo.writeMicroseconds(restPower);
      Serial.println("User is Fully Sitting");
      Serial.println("Remaining Up time");
      Serial.println(remainingUpTime);
      Serial.println("Remaining Down time");
      Serial.println(remainingDownTime);
      MySW.stop();
      break;
    }
    //if interrupted during down process
    else if (downButtonState != 1) {
      Serial.println("Held for duration of:");
      Serial.println(MySW.elapsed());
      remainingDownTime = remainingDownTime - MySW.elapsed();
      remainingUpTime = operationTime - remainingDownTime;
      myservo.writeMicroseconds(restPower);
      Serial.println("User is Partially Sitting");
      Serial.println("Remaining Up time");
      Serial.println(remainingUpTime);
      Serial.println("Remaining Down time");
      Serial.println(remainingDownTime);
      MySW.stop();
      break;
    }
  }

  if (upButtonState == 0 && downButtonState == 0) {
    finishedOperation = false;
    myservo.writeMicroseconds(restPower);
  }
}
