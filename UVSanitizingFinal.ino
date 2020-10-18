//=============================================================================//
//Name: UVSanitizingFinal
//
//Description: automated clothing disinfectant
//IDE: Arduino, version : 1.8.13
//
//Utilization: Interact with OLED Display to run stepper motor that rotates clothing around while UVC ballasts controlled by relay
//
//Pseudo-code: N/A
//
//Restriction: Open Source
//
//History:
//1-R0 / 2020.08.19 / C. Van Zyl / Creation
//1-R1 / 2020.08.20 / C. Van Zyl / timer function
//1-R2 / 2020.08.21 / C. Van Zyl / Stepper motor & abort activation
//1-R3 / 2020.08.26 / C. Van Zyl / Mega transition & relay addition
//1-R4 / 2020.08.30 / C. Van Zyl / stepper fix & multiple rounds fix
//1-R5 / 2020.08.30 / C. Van Zyl / Final Version
//=============================================================================//

#include <SPI.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define motorInterfaceType 1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//pins adjusted for mega
const int Button = 36;
const int ButtonAbort = 37;
const int relayPinIn1 = 24; //relayPin for inside ballast 1
const int relayPinIn2 = 25; //relayPin for inside ballast 2
const int relayPinOut1 = 28; //relayPin for outside ballast 1
const int relayPinOut2 = 29; //relayPin for outside ballast 2
const int dirPin = 3;//step Pins
const int stepPin = 2;
const int stepsPerRevolution = 200;// Define pin connections & motor's steps per revolution
boolean setFlag = false;//timer completion flag

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin); // use accel to smooth start and ease of run()

// the setup function runs once when you press reset or power the board
void setup() {
  // digital pin relayPins are outputs.
  pinMode(relayPinIn1, OUTPUT); //inside mount relays
  pinMode(relayPinIn2, OUTPUT);
  pinMode(relayPinOut1, OUTPUT);//exterior mount relays
  pinMode(relayPinOut2, OUTPUT);
  // initialize digital pin Button & Button abort as inputs.
  pinMode(Button, INPUT);
  pinMode(ButtonAbort, INPUT);
  //stepper motor pins are outputs
  pinMode(stepPin, OUTPUT);//stepper motor
  pinMode(dirPin, OUTPUT);
  myStepper.setMaxSpeed(24); //set speed for stepper
  myStepper.setSpeed(4);
  Serial.begin(9600);
  digitalWrite(relayPinIn1, HIGH); //lights are opposite this means they are off
  digitalWrite(relayPinIn2, HIGH);
  digitalWrite(relayPinOut1, HIGH);
  digitalWrite(relayPinOut2, HIGH);
  display.begin();
  display.clearDisplay();
  display.display();
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.setCursor(1, 5);
  display.print("Welcome");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  
    //=============================================================//
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(1, 5);
    display.print("Initialization");
    display.display();
    delay(1000);
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(1, 5);
    display.print("Ready");
    display.display();
    delay(3000);
    display.clearDisplay();
    display.display();
    //============================================================//
  
}

void loop() {
  //set time values
  //---------------------------//
  signed int minutes = 20;
  signed int secondes = 1;
  //---------------------------//
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(5, 1);
  display.println("Press the green button");
  display.println("to start");
  display.display();
  setFlag = false;
  int stopPress = digitalRead(ButtonAbort); //reads abort button
  int Press = digitalRead(Button); //reads start button
  if (Press == 1) //if start button has pressed 
  {
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    display.setCursor(5, 8);
    display.println(" The process will");    //split for oled screen linespacing   
    display.println("start in 5 sec!");
    display.display();
    delay(2000);
    display.clearDisplay();
    display.display();   
    timer(minutes, secondes);
    delay(2000); // wait 2 sec to make sure the uvc light is close


    while (digitalRead(Button) != 1 && setFlag == false) //makes sure only to print if job hasnt been aborted and they havent pressed start again
    {
      display.setTextColor(WHITE);
      display.setCursor(1, 4);
      display.setTextSize(1);
      display.print("Job completed");
      display.setCursor(0, 13);
      display.println("Press button to return");
      display.println("to menu");  
      display.display();
      display.display();
    }
    display.clearDisplay();
    display.display();

  }
}

//---------------------------------------------------------------------------
//     @file                   UVSanitizingFinal
//     @version                1.0
//     @brief                  Function Timer
//
//     @internal
//
//     @todo
//
//     <hr width="100%" height="5">
//     <b>Historique</b>
//     <hr width="100%" height="1">
//     @date                   2020-08-20
//     @mod                    2020-08-30
//     @author                 Cornelius Van Zyl                     
//     @note                   Set whatever time necessary
//     <hr width="100%" height="1">
//
//---------------------------------------------------------------------------

int timer(int minutes, int secondes)
{
  startMillis = millis(); //used to ensure time is exact
  digitalWrite(dirPin, HIGH); //set direction of stepper
  while (minutes >= 0 && secondes >= 0) {
    myStepper.runSpeed(); //start moving must be called multiple times
    digitalWrite(relayPinIn1, LOW);//TURN LIGHT ON
    digitalWrite(relayPinIn2, LOW);
    digitalWrite(relayPinOut1, LOW);
    digitalWrite(relayPinOut2, LOW);
    if(digitalRead(ButtonAbort) == 1){ //check at start so button press response instant 
        setFlag = true; //timer aborted flag
        break;//exit while loop
      }
    currentMillis = millis();
    if (currentMillis - startMillis >= period) {//if 1 second has not finished
      secondes--;
      display.setTextSize(3);
      display.setCursor(20, 5);
      if (minutes < 10) {
        display.print(0);
      }
      myStepper.runSpeed();
      display.print(minutes);
      display.setCursor(53, 5);
      display.print(":");
      display.setCursor(68, 5);
      if (secondes < 10) {
        display.print(0);
      }
      display.print(secondes);
      display.display();
      display.clearDisplay();
      if (secondes <= 0) {
        minutes--;
        secondes = 60;
      }
      if(setFlag != true){//stops it from setting flag back to false and printing job completed
      setFlag = false; // timer has not been aborted
      }
      startMillis = currentMillis;
      myStepper.runSpeed();
    }
  }
  digitalWrite(relayPinIn1, HIGH); //shut off lights normally 
  digitalWrite(relayPinIn2, HIGH);
  digitalWrite(relayPinOut1, HIGH);
  digitalWrite(relayPinOut2, HIGH);
  if(setFlag = true){
    display.clearDisplay();
    display.display();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(5,8);
    display.print("process aborted");
    display.display();
    delay(2000);
    display.clearDisplay();
  }
  
}
