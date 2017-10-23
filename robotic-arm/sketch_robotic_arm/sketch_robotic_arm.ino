#include <ax12.h>
#include <BioloidController.h>
#include "controlKey.h"
#include "poses.h"



// ================================================================================= start of MOVEMENT RANGE SETUP
// BODY RIGTH/LEFT
const int MOVE_BODY_RIGHT_LEFT_STEP = 128;
const int MOVE_BODY_RIGHT_LEFT_MIN = 0;
const int MOVE_BODY_RIGHT_LEFT_MAX = 1023;

// BODY UP/DOWN_UP_DOWN
const int MOVE_BODY_UP_DOWN_STEP = 80;
const int MOVE_BODY_UP_DOWN_MIN = 310;
const int MOVE_BODY_UP_DOWN_MAX = 680;

// ARM UP/DOWN
const int MOVE_ARM_UP_DOWN_STEP = 80;
const int MOVE_ARM_UP_DOWN_MIN = 190;
const int MOVE_ARM_UP_DOWN_MAX = 680;

// HAND UP/DOWN
const int MOVE_HAND_UP_DOWN_STEP = 128;
const int MOVE_HAND_UP_DOWN_MIN = 190;
const int MOVE_HAND_UP_DOWN_MAX = 818;


const int MOVE_HAND_GRIPPER_OPEN = 512;
const int MOVE_HAND_GRIPPER_CLOSE = 146;
// ================================================================================= end of MOVEMENT RANGE SETUP




const int INTER_TIME_DEFAULT = 850;


const int LED_PIN = 0;
const int SERVOCOUNT = 8;

boolean doAutoStandby = true;
boolean isSleepNow = false;
boolean IDCheck;




BioloidController bioloid = BioloidController(1000000);




// Continuos Mode
boolean isContinuousMoveActive = false;
boolean doContinuousMove = false;
char continuos_mode_num = '0';

int continuos_servo_num;
int continuos_trg_pos;
int continuos_current_pos, continuos_current_pos_2;





//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
void setup()
{  
  
  //open serial port
  Serial.begin(9600);
  delay (500);   
  
    
  // Set LED
  pinMode(LED_PIN, OUTPUT);  
  digitalWrite(LED_PIN, HIGH);
    
  //initialize variables 
  IDCheck = 1;
   
  //Serial.println("[setup] Start action.");
  MoveStandby(INTER_TIME_DEFAULT);
  
  delay(200);
  MoveSleep(INTER_TIME_DEFAULT);


  MenuOptions();
  //Serial.println("[setup] End of setup.");
  
}


//
void loop()
{
 
 // 
 // read the sensor;  
 //
  while(Serial.available())
  {
    
    char inChar = (char)Serial.read();
//    Serial.println(inChar);

    

    
    //KEY_BODY_RIGHT
    // Control part by part    
    //
    if(isContinuousMoveActive == false)
    {
      // NON-CONTINUOUS
      if ( inChar == KEY_BODY_RIGHT) MoveBodyRight();
      else if ( inChar == KEY_BODY_LEFT) MoveBodyLeft();
      else if ( inChar == KEY_BODY_UP) MoveBodyUp();
      else if ( inChar == KEY_BODY_DOWN) MoveBodyDown();
      else if ( inChar == KEY_ARM_UP) MoveArmUp();
      else if ( inChar == KEY_ARM_DOWN) MoveArmDown();
      else if ( inChar == KEY_HAND_UP) MoveHandUp();
      else if ( inChar == KEY_HAND_DOWN) MoveHandDown();
      else if ( inChar == KEY_GRIPPER_OPEN) GripperOpen();
      else if ( inChar == KEY_GRIPPER_CLOSE) GripperClose();
    }
    else
    {
      // CONTINUOUS
      if ( inChar == KEY_BODY_RIGHT || inChar == KEY_BODY_LEFT || inChar == KEY_BODY_UP || inChar == KEY_BODY_DOWN || inChar == KEY_ARM_UP || inChar == KEY_ARM_DOWN || inChar == KEY_HAND_UP || inChar == KEY_HAND_DOWN || inChar == KEY_GRIPPER_OPEN || inChar == KEY_GRIPPER_CLOSE)
      {
        continuos_mode_num = inChar;
        checkBodyPartBeforeMovingContinuously();
        //Serial.print("== start continuos num: ");
        //Serial.println(continuos_mode_num);
      }
      else
      {
        //Serial.println("== pressed non-continuos part");
      }
    }
    
    
    
    
    
    
    
    
    
    //
    // Control pose by pose
    //
    if ( inChar == KEY_MOVE_SLEEP) MoveSleep(INTER_TIME_DEFAULT);
    else if ( inChar == KEY_MOVE_STANDBY) MoveStandby(INTER_TIME_DEFAULT);
    else if ( inChar == 'a') MoveLightsaberStandbyDown(INTER_TIME_DEFAULT);
    else if ( inChar == 's') MoveLightsaberStandbyUp(INTER_TIME_DEFAULT);
    
    
    
    //
    // Sequence pose
    //
    if ( inChar == KEY_PLAY_SEQ_MOVE_1) PlaySequenceMove(1);
    else if ( inChar == KEY_PLAY_SEQ_MOVE_2) PlaySequenceMove(2);
    else if ( inChar == KEY_PLAY_SEQ_MOVE_3) PlaySequenceMove(3);
    
    
    //
    // Utility for recording
    //
    if ( inChar == KEY_START_RECORD) StartRecord();
    else if ( inChar == KEY_READ_CURRENT_POSE) RecordCurrentMove();
    

    
    //
    // Control Continuos Movement
    //
    if ( inChar == KEY_ACTIVATE_CONTINUOUS_MODE)
    {
      
      //Serial.println("<--- TURN ON CONTINUOUS");
      continuos_mode_num = '0';
      isContinuousMoveActive = true;
    }
    else if( inChar == KEY_INACTIVATE_CONTINUOUS_MODE)
    {
      //Serial.println("<--- TURN OFF CONTINUOUS");
      isContinuousMoveActive = false;
      
      continuos_mode_num = '0';
    }
    else if( inChar == KEY_RELEASED_CONTINUOUS_MODE)
    {
      //Serial.println("<--- STOP CONTINUOUS BY RLEASE");
      continuos_mode_num = '0';
    }
    
  }
  
  
  
  
 
  
  
  
  
  
  
  
  //
  // -------------------------------------------------------------- LOOP FOR CONTINUOS
  //
  if(isContinuousMoveActive && continuos_mode_num != '0')
  {
    moveBodyPartContinuosly();
   
  }
 
}



//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// ================================================================================= START OF POSE RECORDING FUNS

void StartRecord()
{
  RelaxServos();
}

void RecordCurrentMove()
{
  TorqueOnServos();  
  delay(100);
  ReadCurrentMove();
  
  RelaxServos();
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

void ReadCurrentMove()
{
  
  
  PROGMEM prog_uint16_t currentPos[] = {8, 512, 512, 512, 512, 512, 512, 512, 512};
  
  int id = 1;
  int pos;
  while (id <= SERVOCOUNT)
  {
    pos =  GetPosition(id);    

    if (pos > 0) currentPos[id] = pos;
    else currentPos[id] = 512;

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

// ================================================================================= END OF POSE RECORDING FUNS
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// ================================================================================= START OF CONTROL POSE BY POSE

void MoveSleep(int inInterTime)
{
  delay(100);  // recommended pause at least '100'
  
  bioloid.loadPose(Sleep);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  delay(100);
  LoopMoving(inInterTime);
  
  isSleepNow = true;
      
}

void MoveStandby(int inInterTime)
{
  delay(100);
  
  bioloid.loadPose(Standby);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  delay(100);
//  Serial.print("playSound: ");
//  Serial.println(1);
  LoopMoving(inInterTime);
  
  isSleepNow = false;
  
  
  
   
}


void MoveLightsaberStandbyDown(int inInterTime)
{
  delay(100);
  
  
//  prog_uint16_t pos = GetPosition(8);
//  Lightsaber_Standby_Down[8] = pos;
  
  bioloid.loadPose(Lightsaber_Standby_Down);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  delay(100);
  LoopMoving(inInterTime);
 
}

void MoveLightsaberStandbyUp(int inInterTime)
{
  delay(100);
  
  bioloid.loadPose(Lightsaber_Standby_Up);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
    
  delay(100);
  LoopMoving(inInterTime);

}

void PlaySequenceMove(int inIndex)
{
  delay(500);  // recommended pause at least '100'
  
/*  
  // STAND BY
  bioloid.loadPose(Standby);   // load the pose from FLASH, into the nextPose buffer
  bioloid.readPose();            // read in current servo positions to the curPose buffer
  delay(100);
  LoopMoving(INTER_TIME_DEFAULT);
  delay(200);
*/  
  
  //
  int total;
  if( inIndex == 1) total = (sizeof(SEQUENCE_POS_1)/sizeof(prog_uint16_t))/9;
  else if( inIndex == 2) total = (sizeof(SEQUENCE_POS_2)/sizeof(prog_uint16_t))/9;
  else if( inIndex == 3) total = (sizeof(SEQUENCE_POS_3)/sizeof(prog_uint16_t))/9;
  else
  {
    Serial.println("ERROR: can't find seq pose");
    return;
  }

  for( int i =0; i< total; i++)
  {
    if( inIndex == 1) bioloid.loadPose(SEQUENCE_POS_1[i]);
    else if( inIndex == 2) bioloid.loadPose(SEQUENCE_POS_2[i]);
    else if( inIndex == 3) bioloid.loadPose(SEQUENCE_POS_3[i]);
    
    bioloid.readPose();            // read in current servo positions to the curPose buffer
    
    Serial.print("#### Moving servos: ");
    Serial.print(i);
    Serial.print(" - ");
    delay(5);
    
    int time_move; 
    int time_delay; 
    if(inIndex == 1)
    {
      time_move = SEQUENCE_TIME_1[i][0];
      time_delay = SEQUENCE_TIME_1[i][1];
    }
    else if(inIndex == 2)
    {
      time_move = SEQUENCE_TIME_2[i][0];
      time_delay = SEQUENCE_TIME_2[i][1];
    }
    else if(inIndex == 3)
    {
      time_move = SEQUENCE_TIME_3[i][0];
      time_delay = SEQUENCE_TIME_3[i][1];
    }
    
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

// ================================================================================= END OF CONTROL POSE BY POSE
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// ================================================================================= START OF CONTROL PART BY PART

void MoveBodyRight()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos = GetPosition(1);
  
  // set target position
  int trg_pos = pos - MOVE_BODY_RIGHT_LEFT_STEP;
  if(trg_pos < MOVE_BODY_RIGHT_LEFT_MIN) trg_pos = MOVE_BODY_RIGHT_LEFT_MIN;
  
  // loop moving until moving to target position
  while(pos > trg_pos)
  {
    pos == pos--;
    SetPosition(1, pos);
    
    delay(10);
  }
 
}

void MoveBodyLeft()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos = GetPosition(1);
  
  // set target position
  int trg_pos = pos + MOVE_BODY_RIGHT_LEFT_STEP;
  if(trg_pos > MOVE_BODY_RIGHT_LEFT_MAX) trg_pos = MOVE_BODY_RIGHT_LEFT_MAX;
  
  // loop moving until moving to target position
  while(pos < trg_pos)
  {
    pos == pos++;
    SetPosition(1, pos);
    
    delay(10);
  }
  
}



void MoveBodyUp()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos_1 = GetPosition(2);
  int pos_2 = GetPosition(3);
  
  Serial.println(pos_1);
  Serial.println(pos_2);
  
  // set target position
  int trg_pos = pos_2 + MOVE_BODY_UP_DOWN_STEP;
  if(trg_pos > MOVE_BODY_UP_DOWN_MAX) trg_pos = MOVE_BODY_UP_DOWN_MAX;
  
  // loop moving until moving to target position
  while(pos_2 < trg_pos)
  {
    pos_1 == pos_1--;
    pos_2 = pos_2++;
    
    SetPosition(2, pos_1);
    SetPosition(3, pos_2);
    
    delay(10);
  }
  

}

void MoveBodyDown()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  
  // read current servo position
  int pos_1 = GetPosition(2);
  int pos_2 = GetPosition(3);
  
  Serial.println(pos_1);
  Serial.println(pos_2);
  
  // set target position
  int trg_pos = pos_2 - MOVE_BODY_UP_DOWN_STEP;
  if(trg_pos < MOVE_BODY_UP_DOWN_MIN) trg_pos = MOVE_BODY_UP_DOWN_MIN;
  
  // loop moving until moving to target position
  while(pos_2 > trg_pos)
  {
    pos_1 == pos_1++;
    pos_2 = pos_2--;
    
    SetPosition(2, pos_1);
    SetPosition(3, pos_2);
    
    delay(10);
  }
  
}



void MoveArmUp()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos_1 = GetPosition(4);
  int pos_2 = GetPosition(5);
  
  Serial.println(pos_1);
  Serial.println(pos_2);
  
  // set target position
  int trg_pos = pos_2 - MOVE_ARM_UP_DOWN_STEP;
  if(trg_pos < MOVE_ARM_UP_DOWN_MIN) trg_pos = MOVE_ARM_UP_DOWN_MIN;
  
  // loop moving until moving to target position
  while(pos_2 > trg_pos)
  {
    pos_1 == pos_1++;
    pos_2 = pos_2--;
    
    SetPosition(4, pos_1);
    SetPosition(5, pos_2);
    
    delay(10);
  }
  
}

void MoveArmDown()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos_1 = GetPosition(4);
  int pos_2 = GetPosition(5);
  
  Serial.println(pos_1);
  Serial.println(pos_2);
  
  // set target position
  int trg_pos = pos_2 + MOVE_ARM_UP_DOWN_STEP;
  if(trg_pos > MOVE_ARM_UP_DOWN_MAX) trg_pos = MOVE_ARM_UP_DOWN_MAX;
  
  // loop moving until moving to target position
  while(pos_2 < trg_pos)
  {
    pos_1 == pos_1--;
    pos_2 = pos_2++;
    
    SetPosition(4, pos_1);
    SetPosition(5, pos_2);
    
    delay(10);
  }
  
}



void MoveHandUp()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos = GetPosition(6);
  
  // set target position
  int trg_pos = pos + MOVE_HAND_UP_DOWN_STEP;
  if(trg_pos > MOVE_HAND_UP_DOWN_MAX) trg_pos = MOVE_HAND_UP_DOWN_MAX;
  
  // loop moving until moving to target position
  while(pos < trg_pos)
  {
    pos == pos++;
    SetPosition(6, pos);
    
    delay(10);
  }
  
}

void MoveHandDown()
{
  delayBeforeAction();  // recommended pause at least '100'
  
  // read current servo position
  int pos = GetPosition(6);
  
  // set target position
  int trg_pos = pos - MOVE_HAND_UP_DOWN_STEP;
  if(trg_pos < MOVE_HAND_UP_DOWN_MIN) trg_pos = MOVE_HAND_UP_DOWN_MIN;
  
  // loop moving until moving to target position
  while(pos > trg_pos)
  {
    pos == pos--;
    SetPosition(6, pos);
    
    delay(10);
  }
  
}

void GripperOpen()
{
  int pos = GetPosition(8);
 
  while(pos < MOVE_HAND_GRIPPER_OPEN)
  {
    pos = pos++;  
    SetPosition(8, pos);
  
    delay(5);
  }
  
}

void GripperClose()
{
  int pos = GetPosition(8);
  
  while(pos > MOVE_HAND_GRIPPER_CLOSE)
  {
    pos = pos--; 
    SetPosition(8, pos);
    
    delay(5);
  }
  
}



//
// HANDLE CONTINUOUSLY
//

void checkBodyPartBeforeMovingContinuously()
{
  
  delayBeforeAction();  // recommended pause at least '100'
  

  if(continuos_mode_num == KEY_BODY_RIGHT || continuos_mode_num == KEY_BODY_LEFT) continuos_servo_num = 1; // body right / left
  else if(continuos_mode_num == KEY_BODY_UP || continuos_mode_num == KEY_BODY_DOWN) continuos_servo_num = 2; // body up/down
  else if(continuos_mode_num == KEY_ARM_UP || continuos_mode_num == KEY_ARM_DOWN) continuos_servo_num = 4; // arm up/down
  else if(continuos_mode_num == KEY_HAND_UP || continuos_mode_num == KEY_HAND_DOWN) continuos_servo_num = 6; // hand up/down
  else if(continuos_mode_num == KEY_GRIPPER_OPEN || continuos_mode_num == KEY_GRIPPER_CLOSE) continuos_servo_num = 8; // gripper open/close

  // read current servo position
  continuos_current_pos = GetPosition(continuos_servo_num);
  // read 2nd servo pos
  if(continuos_mode_num == KEY_BODY_UP || continuos_mode_num == KEY_BODY_DOWN || continuos_mode_num == KEY_ARM_UP || continuos_mode_num == KEY_ARM_DOWN)
  {
    continuos_current_pos_2 = GetPosition(continuos_servo_num+1);
  }
  
  
  
  
  
  
  
  if(continuos_mode_num == KEY_BODY_RIGHT) continuos_trg_pos = MOVE_BODY_RIGHT_LEFT_MIN; // body right
  else if(continuos_mode_num == KEY_BODY_LEFT) continuos_trg_pos = MOVE_BODY_RIGHT_LEFT_MAX; // body left
  else if(continuos_mode_num == KEY_BODY_UP) continuos_trg_pos = MOVE_BODY_UP_DOWN_MAX;// body up
  else if(continuos_mode_num == KEY_BODY_DOWN) continuos_trg_pos = MOVE_BODY_UP_DOWN_MIN;// body down
  else if(continuos_mode_num == KEY_ARM_UP) continuos_trg_pos = MOVE_ARM_UP_DOWN_MIN; // arm up
  else if(continuos_mode_num == KEY_ARM_DOWN) continuos_trg_pos = MOVE_ARM_UP_DOWN_MAX; // arm down
  else if(continuos_mode_num == KEY_HAND_UP) continuos_trg_pos = MOVE_HAND_UP_DOWN_MAX; // arm up
  else if(continuos_mode_num == KEY_HAND_DOWN) continuos_trg_pos = MOVE_HAND_UP_DOWN_MIN; // arm down
  else if(continuos_mode_num == KEY_GRIPPER_OPEN) continuos_trg_pos = MOVE_HAND_GRIPPER_OPEN; // gripper open
  else if(continuos_mode_num == KEY_GRIPPER_CLOSE) continuos_trg_pos = MOVE_HAND_GRIPPER_CLOSE; // gripper close

}



void moveBodyPartContinuosly()
{
//  Serial.print("continuos loop mode: ");
//  Serial.println(continuos_mode_num);
  
     
  if(continuos_mode_num == KEY_BODY_RIGHT)
  {
    // body right
    if(continuos_current_pos > continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos--;
      SetPosition(continuos_servo_num, continuos_current_pos);
    }
  }
  else if(continuos_mode_num == KEY_BODY_LEFT)
  {
    // body left
    if(continuos_current_pos < continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos++;
      SetPosition(continuos_servo_num, continuos_current_pos);

    }
  }
  else if(continuos_mode_num == KEY_BODY_UP)
  {
    // body up
    if(continuos_current_pos_2 < continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos--;
      continuos_current_pos_2 = continuos_current_pos_2++;
      
      SetPosition(continuos_servo_num, continuos_current_pos);
      SetPosition(continuos_servo_num+1, continuos_current_pos_2);
      
    }
  }
  else if(continuos_mode_num == KEY_BODY_DOWN)
  {
    // body down
    if(continuos_current_pos_2 > continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos++;
      continuos_current_pos_2 = continuos_current_pos_2--;
      
      SetPosition(continuos_servo_num, continuos_current_pos);
      SetPosition(continuos_servo_num+1, continuos_current_pos_2);
     
    }
  }
  else if(continuos_mode_num == KEY_ARM_UP)
  {
    // arm up
    if(continuos_current_pos_2 > continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos++;
      continuos_current_pos_2 = continuos_current_pos_2--;
      
      SetPosition(continuos_servo_num, continuos_current_pos);
      SetPosition(continuos_servo_num+1, continuos_current_pos_2);
      
    }
  
  }
  else if(continuos_mode_num == KEY_ARM_DOWN)
  {
    // arm down
    if(continuos_current_pos_2 < continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos--;
      continuos_current_pos_2 = continuos_current_pos_2++;
      
      SetPosition(continuos_servo_num, continuos_current_pos);
      SetPosition(continuos_servo_num+1, continuos_current_pos_2);
    }
    
  }
  else if(continuos_mode_num == KEY_HAND_UP)
  {
    // hand up
    if(continuos_current_pos < continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos++;
      SetPosition(continuos_servo_num, continuos_current_pos);
    }
  
  }
  else if(continuos_mode_num == KEY_HAND_DOWN)
  {
    // hand down
    if(continuos_current_pos > continuos_trg_pos)
    {
      continuos_current_pos == continuos_current_pos--;
      SetPosition(continuos_servo_num, continuos_current_pos);
    }
  
  }
  else if(continuos_mode_num == KEY_GRIPPER_OPEN)
  {
    // gripper open
    if(continuos_current_pos < continuos_trg_pos)
    {
      continuos_current_pos = continuos_current_pos++;  
      SetPosition(continuos_servo_num, continuos_current_pos);
    }
    
  }
  else if(continuos_mode_num == KEY_GRIPPER_CLOSE)
  {
    // gripper close
    if(continuos_current_pos > continuos_trg_pos)
    {
      continuos_current_pos = continuos_current_pos--; 
      SetPosition(continuos_servo_num, continuos_current_pos);
     
    }
  
  }

  
  if(continuos_mode_num == KEY_GRIPPER_OPEN || continuos_mode_num == KEY_GRIPPER_CLOSE) delay(5);
  else delay(10);
  


}


// ================================================================================= END OF CONTROL PART BY PART
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
// ================================================================================= START OF UTILITY FUNCTIONS

void MenuOptions()
{
  Serial.println("###########################"); 
  Serial.println("--- Basic Control");
  Serial.println("0) Sleep"); 
  Serial.println("1) Standby");     
  Serial.println("2-5) Body R/L/Up/Dn");            
  Serial.println("6-7) Arm Up/Dn");                
  Serial.println("8-9) Hand Up/Dn");  
  Serial.println("o/p) Gripper Open/Close");    
  Serial.println("--- Lightsaber Control");
  Serial.println("a) LightSaber Standby");
  Serial.println("s) LightSaber Side Up");
  Serial.println("--- Record Utility");
  Serial.println("q) Start Record");        
  Serial.println("w) Record Current Move");        
  Serial.println("###########################"); 
}


void delayBeforeAction()
{
  if(doAutoStandby)
  {
    if(isSleepNow)
    {
      MoveStandby(INTER_TIME_DEFAULT);
      isSleepNow = false;
    }
  }
  
  
  delay(100); 

}


void LEDFeedback()
{
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);

}


// ================================================================================= END OF UTILITY FUNCTIONS

