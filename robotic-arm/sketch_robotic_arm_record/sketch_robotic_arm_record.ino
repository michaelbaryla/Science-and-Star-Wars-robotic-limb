#include <ax12.h>
#include <BioloidController.h>
#include "poses.h"


boolean doAutoStandby = true;
boolean isSleepNow = false;


BioloidController bioloid = BioloidController(1000000);

const int SERVOCOUNT = 8;
boolean doDisplayMenu = 0;



// ---------------------------------------------------------------------- SETUP
void setup()
{
  //open serial port
  Serial.begin(9600);
  delay (500);   
  Serial.println("SETUP: chk1");
  
  doDisplayMenu = 0;
  
  Serial.println("SETUP: chk2");
  MoveStandby();


  delay(200);
  GripperClose();

  Serial.println("SETUP: chk3");  
  delay(2000);
  MenuOptions();
  doDisplayMenu = 1;
     
}

// ---------------------------------------------------------------------- LOOP
void loop()
{
 
  //
  while(Serial.available())
  {
    
    char inChar = (char)Serial.read();
    
    if ( inChar == '0')
    {
       MoveSleep();
       delay(500);
    }
    else if ( inChar == '1')
    {
      MoveStandby();
    }
    else if ( inChar == '2')
    {
      MoveLightsaber();
    }
    else if ( inChar == 'q')
    {
      StartRecord();
    }
    else if ( inChar == 'w')
    {
      StopRecord();
    }
    else if ( inChar == 'e')
    {
      
      RecordCurrentMove();
    }
    else if ( inChar == 's')
    {
      PlayRecordedMove();
    }
    else if ( inChar == 'z')
    {
      GripperOpen();
    }
    else if ( inChar == 'x')
    {
      GripperClose();
    }
    else if ( inChar == 'a')
    {
      MoveLightsaber();
    }
  }
  
}
  
  
  



void StartRecord()
{
  RelaxServos();
  
}


void StopRecord()
{
  TorqueOnServos();
  
}

void RecordCurrentMove()
{
  TorqueOnServos();
  
  delay(100);
  
  
  ReadCurrentMove();
  
  
  //
  RelaxServos();

 
}


void ReadCurrentMove()
{
  
  
  PROGMEM prog_uint16_t currentPos[] = {8, 512, 512, 512, 512, 512, 512, 512, 142};
  
  int id = 1;
  int pos;
  while (id <= SERVOCOUNT)
  {
    pos =  GetPosition(id);    

    if (pos > 0) currentPos[id] = pos;
    else currentPos[id] = 255;

    id = id++;
    delay(200);
  }
     
  Serial.print("{8");
  for( int i = 1; i<= SERVOCOUNT; i++)
  {
      Serial.print(", ");
      Serial.print(currentPos[i]);
  }
  
  Serial.println("},");
  
}



void PlayRecordedMove()
{
  delay(100);  // recommended pause at least '100'
  
  
  // STAND BY
  bioloid.loadPose(Standby);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
  delay(10);
  LoopMoving(1000);
  delay(200);
  
  
  
  
  //
  int total = (sizeof(POS_ARRAY)/sizeof(prog_uint16_t))/9;

  for( int i =0; i< total; i++)
  {
      
    bioloid.loadPose(POS_ARRAY[i]);   // load the pose from FLASH, into the nextPose buffer
    bioloid.readPose();            // read in current servo positions to the curPose buffer
    
    Serial.print("#### Moving servos: ");
    Serial.print(i);
    Serial.print(" - ");
    delay(5);
    
    int time_move = TIME_ARRAY[i][0];
    int time_delay = TIME_ARRAY[i][1];
    
    Serial.print(time_move);
    Serial.print(" / ");
    Serial.println(time_delay);
    
    LoopMoving(time_move);
    delay(time_delay);

  }
 
}


void LoopMoving(int inInter)
{
   bioloid.interpolateSetup(inInter); // setup for interpolation from current->next over 1/2 a second
   while(bioloid.interpolating > 0){  // do this while we have not reached our new pose
    bioloid.interpolateStep();     // move servos, if necessary. 
    delay(3);
  }
}




//
// ================================================================================= MOVE: STANDBY
//
void MoveSleep()
{
  delay(100);  // recommended pause at least '100'
  
  bioloid.loadPose(Sleep);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  Serial.println("#### Moving servos to sleep");
  delay(1000);
  bioloid.interpolateSetup(1000); // setup for interpolation from current->next over 1/2 a second
  while(bioloid.interpolating > 0){  // do this while we have not reached our new pose
    bioloid.interpolateStep();     // move servos, if necessary. 
    delay(3);
  }
  
  isSleepNow = true;
    
  // Display Menu
  if (doDisplayMenu == 1) MenuOptions();


  
  
}

void MoveStandby()
{
  delay(100);
  
  bioloid.loadPose(Standby);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  Serial.println("#### Moving servos to standby");
  delay(1000);
  bioloid.interpolateSetup(1000); // setup for interpolation from current->next over 1/2 a second
  while(bioloid.interpolating > 0){  // do this while we have not reached our new pose
    bioloid.interpolateStep();     // move servos, if necessary. 
    delay(3);
  }
  
  isSleepNow = false;
    
  // Display Menu
  if (doDisplayMenu == 1) MenuOptions();

}

void MoveLightsaber()
{
  delay(100);
  
  bioloid.loadPose(Lightsaber);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  Serial.println("#### Moving servos to centered position");
  delay(1000);
  bioloid.interpolateSetup(1000); // setup for interpolation from current->next over 1/2 a second
  while(bioloid.interpolating > 0){  // do this while we have not reached our new pose
    bioloid.interpolateStep();     // move servos, if necessary. 
    delay(3);
  }
  
  isSleepNow = false;
    
  // Display Menu
  if (doDisplayMenu == 1) MenuOptions();

}

void GripperOpen()
{
  Serial.println("open");
  
  int pos = GetPosition(8);
  
  while(pos < 512)
  {
    pos = pos++;  
    SetPosition(8, pos);
  
    delay(5);
  }
  
}

void GripperClose()
{
  Serial.println("close");

  int pos = GetPosition(8);
  
  while(pos > 146)
  {
    pos = pos--; 
    SetPosition(8, pos);
    
    delay(5);
  }
  
}






























void RelaxServos()
{
  int id = 1;
//  Serial.println("### Relaxing Servos.");
  while(id <= SERVOCOUNT)
  {
    Relax(id);
    id = id++;
    delay(50);
  }
 
}


void TorqueOnServos()
{
   int id = 1;
//  Serial.println("### Torque On Servos.");   
  while(id <= SERVOCOUNT)
  {
    TorqueOn(id);
    id = id++;
    delay(50);
  }
}








void delayBeforeAction()
{
  if(doAutoStandby)
  {
    if(isSleepNow)
    {
      MoveStandby();
      isSleepNow = false;
    }
  }
  
  
  Serial.println("delay before action");
  delay(100); 
//  LEDFeedback();
}

void LEDFeedback()
{
//  digitalWrite(LED_PIN, LOW);
  delay(200);
//  digitalWrite(LED_PIN, HIGH);
  delay(200);
//  digitalWrite(LED_PIN, LOW);
  delay(200);
//  digitalWrite(LED_PIN, HIGH);


}




    
    
    
    
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void MenuOptions()
{
  
    Serial.println("###########################"); 
    Serial.println("Please enter option 1-5 to run individual tests again.");     
    Serial.println("1) Move Standby"); 
    Serial.println("3) Move Servos to Home");     
    Serial.println("4) Relax Servos");            
    Serial.println("5) Perform Movement Sign Test");                
    Serial.println("6) Check System Voltage");  
    Serial.println("7) Relax Servos");    
    Serial.println("8) Servo Scanning Test");        
    Serial.println("9) Do Test");        
    Serial.println("###########################"); 
}




