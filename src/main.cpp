#include <Arduino.h>
#include "BasicStepperDriver.h"
#include "MultiDriver.h"
#include "SyncDriver.h"
#include <SoftwareSerial.h>

// Screen Pins
#define SERIAL_RX       2
#define SERIAL_TX       3
// Stepper Pins
#define DIRECTION_X_PIN 14
#define STEP_X_PIN      15
#define DIRECTION_Y_PIN 16
#define STEP_Y_PIN      17
// IO Pins
#define ENABLE_PIN 12

// General Defines
#define MICROSTEPS  16
#define MOTOR_STEPS 200
// Objects
SoftwareSerial hmiScreen(SERIAL_RX, SERIAL_TX);
BasicStepperDriver stepperX(MOTOR_STEPS, DIRECTION_X_PIN, STEP_X_PIN);
BasicStepperDriver stepperY(MOTOR_STEPS, DIRECTION_Y_PIN, STEP_Y_PIN);
SyncDriver controller(stepperX, stepperY);
// variables
int A = 0;  // wire dia
int B = 0;  // coil width
int C = 0;  // number of turns
int L = 40; // lenght
int state = 0;
String message;
int QTY = 0;
int numMessages = 0;
int endBytes = 0;
byte inByte;
int flag = 0;
int count = 0;

void setup(){ 
// Serial Terminal  
  Serial.begin(9600);
// Screen Start
  hmiScreen.begin(9600);
// Stepper Start
  stepperX.begin(250, MICROSTEPS);
  stepperY.begin(250, MICROSTEPS);
// Setup Pins
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, HIGH);
// what is this delay for?
  delay(500);
}

void data() {
  if (state == 0){
    if (numMessages == 1){ //Did we receive the anticipated number of messages? In this case we only want to receive 1 message.
      A = QTY;
      Serial.println(A);    //See what the important message is that the Arduino receives from the Nextion
      numMessages = 0;      //Now that the entire set of data is received, reset the number of messages received
      state = 1;
    }
  }
  if (state == 1){
    if(numMessages == 1){ //Did we receive the anticipated number of messages? In this case we only want to receive 1 message.
      B = QTY;
      Serial.println(B);  //See what the important message is that the Arduino receives from the Nextion
      numMessages = 0;    //Now that the entire set of data is received, reset the number of messages received
      state = 2;
    }
  }
  if(state == 2){
    if (numMessages == 1){ //Did we receive the anticipated number of messages? In this case we only want to receive 1 message.
      C = QTY;
      Serial.println(C);   //See what the important message is that the Arduino receives from the Nextion
      numMessages = 0;     //Now that the entire set of data is received, reset the number of messages received
      state = 0;
    }
  }
    
  if (hmiScreen.available()) { //Is data coming through the serial from the Nextion?
    inByte = hmiScreen.read();

    // Serial.println(inByte); //See the data as it comes in

    if (inByte > 47 && inByte < 58){  //Is it data that we want to use?
      message.concat(char(inByte));   //Cast the decimal number to a character and add it to the message
    }
     else if (inByte == 255) {        //Is it an end byte?
      endBytes = endBytes + 1;        //If so, count it as an end byte.
    }

    if (inByte == 255 && endBytes == 3) { //Is it the 3rd (aka last) end byte?
      QTY = message.toInt();            //Because we have now received the whole message, we can save it in a variable.
      message = "";                     //We received the whole message, so now we can clear the variable to avoid getting mixed messages.
      endBytes = 0;                     //We received the whole message, we need to clear the variable so that we can identify the next message's end
      numMessages  = numMessages + 1;   //We received the whole message, therefore we increment the number of messages received.
      //Now lets test if it worked by playing around with the variable.
    }
  }
}

void updateScreen(int count){
  hmiScreen.print("n2.val=");
  hmiScreen.print(count);
  hmiScreen.write(0xff);
  hmiScreen.write(0xff);
  hmiScreen.write(0xff);
}

void loop(){
  data();
  if (A > 0 && B > 0 && C > 0) {
    digitalWrite(ENABLE_PIN, LOW);
    if (count <= C){
      for (int i = 0; i <= L; i++){
        if (count > C){
          break;
        }
        controller.rotate(360,40);
        updateScreen(count);
        count++;  
      }

      for (int i = 0; i <= L; i++){
        if (count > C){
          break;
        }
        controller.rotate(360,-40);  
        updateScreen(count);
        count++;
      }
    }   
  }
}