// COMP-10184 – Mohawk College
// AlarmSystem
// Program description:
// This project consist on creating a home alarm system by using a PIR sensor connected to a ESP8266 microcontroller.
//
// I, Jorge Isaza, student number 000811486, certify that all code submitted is my own work; 
// that I have not copied it from any other source. 
// I also certify that I have not allowed my work to be copied by others.

#include <Arduino.h>
#include <time.h>
#include <stdio.h>

// digital input pin definitions
#define PIN_PIR D5
#define PIN_BUTTON D6
#define PROJECT_STAGE 5  // This variable sets the Stage Functionality


// Global variables
bool bLastReading = false;
bool bAlarmSystemNeedReSet = false;
bool bAlarmEnabled = true;
int iLastButtonRestartInput = 0;
bool bSensorGettingDownWarning = false;

// *************************************************************
// In this mode, the alarm will blink the led on for 10 seconds 
// if a motion event is detected. (4 blinks per second)
// At the end of the 10 seconds the led will turned on
// And the alarm won't response to any event and will operate again
// just by resetting the device
// If the button of the alarm is prest while the LED is blinking,
// the alarm will be turned OFF, and then, to turned ON again
// the user needs to press the button again

void Stage5(){
  time_t rawtime;
  String sCurrentTime = "";
  int iblinksPerSecond = 4;
  int itotalTimeBlinkingMiliSecs = 10000;
  int iButton; 
  int ilastButtonInput = 0;
  bool bPushResetButton = false;
 

  // Setting the time on each state of the Led (ON/OFF)
  // The half of the time will be ON, and the other OFF
  int timePerBlinkStage = (1000/(2 * iblinksPerSecond));
  int cursorTime = 0;

  time ( &rawtime ); 
  sCurrentTime = ctime(&rawtime);
  Serial.println("Stage 5 Alarm triggered " + String(sCurrentTime));

  while(cursorTime < itotalTimeBlinkingMiliSecs){

    //Serial.println("Counting..." + String(cursorTime));
    iButton = digitalRead(PIN_BUTTON);

    if(iButton == 0 && ilastButtonInput == 1){
      
      bPushResetButton = !bPushResetButton;
      
    }

    // Serial.println("Button Stage: " + String(bPushResetButton));

    ilastButtonInput = iButton;

    if(bPushResetButton){
     

      Serial.println("\t\t >>>>> Stage 5 - Disableing Alarm");
      digitalWrite(LED_BUILTIN, true);
      Serial.println("\n\n Alarm disabled. To start it again, press button\n");
      bAlarmEnabled = false;
      iLastButtonRestartInput = 0;
      cursorTime = itotalTimeBlinkingMiliSecs;
      break;
      return;

    }
    else{
      
      digitalWrite(LED_BUILTIN, false);
      delay(timePerBlinkStage);
      digitalWrite(LED_BUILTIN, true);
      delay(timePerBlinkStage);

      cursorTime = cursorTime + (2 * timePerBlinkStage);
    }
  }

  if(!bPushResetButton){
    time ( &rawtime ); 
    sCurrentTime = ctime(&rawtime);
    Serial.println("Stage 5 Alarm ended " + String(sCurrentTime));
    Serial.println("System Alarm needs to be reset");

    digitalWrite(LED_BUILTIN, false);
    bAlarmSystemNeedReSet = true;
    bPushResetButton = false;

  }
 
} 
// *************************************************************
// This function is intended to collect the imputs from the button and PIR
// and also process those signals and deside if the due to those the signals
// the alarm should be triggered 
bool collectInputs(){
  bool bPIR = false;
  bool triggerAction = false;
  int iButton; 


  // Reads the input from the button
  iButton = digitalRead(D6);

  // If the button was prest when the alarm is disabled, turn it ON
  if(iButton == 0 && iLastButtonRestartInput == 1 && !bAlarmEnabled){
      bAlarmEnabled = true; 
      bAlarmSystemNeedReSet = false;
     
      Serial.println("Alarm System is turning ON... Please wait");
      // Some times the sensor is getting a movement signal just when starts,
      // so, to avoid that wrong signal, the system waits until its state is relaxed again 
      bSensorGettingDownWarning = true;
      if(!bPIR){
        Serial.println("Alarm System is turned ON");
      } 
      
  }

  iLastButtonRestartInput = iButton;

  if(!bAlarmEnabled){
    return false;
  }

  // read PIR sensor (true = Motion detected!). As long as there
  // is motion, this signal will be true. About 2.5 seconds after
  // motion stops, the PIR signal will become false.
  bPIR = digitalRead(PIN_PIR);
  

  if(bLastReading != bPIR && bPIR){
    triggerAction = true;
    // Serial.println("Alarm Activated>> " + String(bPIR) + " | Needs Reset: " + String(alarmSystemNeedResSet) +  " | Trigger Action: " + String(triggerAction));
  }

  // Shows a warning saying the Alarm System needs to starts with its sensor in relaxed state,
  // so it is needed to wait
  if(bLastReading && bPIR && !triggerAction && bAlarmEnabled && !bAlarmSystemNeedReSet && bSensorGettingDownWarning){
    Serial.println("Sensor is getting its relaxed state. Please don't move...");
    bSensorGettingDownWarning = false;
  }

  // When the sensor comes to a relaxed state from the starting point, the system says that
  // is ready to work
  if(bLastReading && !bPIR && bAlarmEnabled && !bAlarmSystemNeedReSet){
    Serial.println("Sensor ready");
    Serial.println("Alarm System is turned ON");
  }
  
  bLastReading = bPIR;
  // Serial.println("Checking State: PIR: " + String(bPIR) + " | Needs Reset: " + String(alarmSystemNeedResSet) +  " | Trigger Action: " + String(triggerAction) + " | AlarmEnabled: " + String(alarmEnabled));
  return triggerAction;
}

// *************************************************************
void setup() {
  // configure the USB serial monitor
  Serial.begin(115200);
  // configure the LED output
  pinMode(LED_BUILTIN, OUTPUT);
  // PIR sensor is an INPUT
  pinMode(PIN_PIR, INPUT);
  // Button is an INPUT
  pinMode(PIN_BUTTON, INPUT_PULLUP);

   Serial.println("\n\n Alarm system Activated \n\n");
   digitalWrite(LED_BUILTIN, true);

  bAlarmSystemNeedReSet = false;
  bLastReading = false;
  }
// *************************************************************
void loop() {
  
  bool triggerAction = collectInputs();

  // If the alarm is turned ON and doesn't need to be re-started, activate the alarm
  if(!bAlarmSystemNeedReSet && bAlarmEnabled){
   if(triggerAction){
          Stage5();
        }
  }  
}





