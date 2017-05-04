#include <OBD2.h>
#include <DueTimer.h>
#include "SamNonDuePin.h"

/********************************************************************
This example is built upon the CANAcquisition class and the OBDParmameter class using 11bit (non-extended) OBD2 ID's
This example shows how to set up periodic data acquisition of OBD2 paramters based upon
standard PID's. If you'd like to add another paramter,simply copy one of the definitions and modify it accordingly. 
You may also need to add a new PID to the "OBD_PID" enum in the header file. 
/********************************************************************/

//create the CANport acqisition schedulers
cAcquireCAN CANport0(CAN_PORT_0);

/***** DEFINITIONS FOR OBD MESSAGES ON CAN PORT 0, see https://en.wikipedia.org/wiki/OBD-II_PIDs to add your own ***************/
//char _name[10], char _units[10], OBD_PID pid,  uint8_t OBD_PID_SIZE size, bool _signed, OBD_MODE_REQ mode, float32 slope, float32 offset, cAcquireCAN *, extended ID;

  cOBDParameter OBD_Speed(      "Speed "        , " KPH"    ,  SPEED       , _8BITS,   false,   CURRENT,  1,      0,  &CANport0, false);
  cOBDParameter OBD_EngineSpeed("Engine Speed " , " RPM"    ,  ENGINE_RPM  , _16BITS,  false,   CURRENT,  0.25,   0,  &CANport0, false);
  cOBDParameter OBD_Throttle(   "Throttle "     , " %"      ,  THROTTLE_POS, _8BITS,   false,   CURRENT,  0.3922, 0,  &CANport0, false);
  cOBDParameter OBD_Coolant(    "Coolant "      , " C"      ,  COOLANT_TEMP, _8BITS,   false ,  CURRENT,  1,    -40,  &CANport0, false);
  cOBDParameter OBD_EngineLoad( "Load "         , " %"      ,  ENGINE_LOAD , _8BITS,   false,   CURRENT,  0.3922, 0,  &CANport0, false);
  cOBDParameter OBD_MAF(        "MAF "          , " grams/s"    ,  ENGINE_MAF  , _16BITS,  false,   CURRENT,  0.01,   0,  &CANport0, false);
  cOBDParameter OBD_IAT(        "IAT "          , " C"      ,  ENGINE_IAT  , _8BITS,   false ,  CURRENT,  1,    -40,  &CANport0, false);
  cOBDParameter OBD_FuelLevel("Fuel Level "     , " Percent",  FUEL_LEVEL        , _8BITS,   false,   CURRENT,  1,      0,  &CANport0, false);

//#define Serial SerialUSB
const int LOWPOW = PIN_EMAC_ERX0;

void setup()
{
  pinModeNonDue(LOWPOW, OUTPUT);
  digitalWriteNonDue(LOWPOW, HIGH);  
  pinMode(28, OUTPUT); 
  digitalWrite(28, LOW);    
  

  Serial.begin(115200);

  //debugging message for monitor to indicate CPU resets are occuring
  //Serial.println("System Reset");

  //set up the transmission/reception of messages to occur at 500Hz (2mS) timer interrupt
  Timer3.attachInterrupt(PrintScreen).setFrequency(1).start();
       
  //start CAN ports,  enable interrupts and RX masks, set the baud rate here
  CANport0.initialize(_500K);
}


UINT8 i;
UINT32 maxTime;

void loop()
{
CANport0.run(POLLING); 
}

//this is our timer interrupt handler, called at XmS interval
void PrintScreen()
{
  float a;
  String bbb;
  char result[20];

  //print out our latest OBDII data
  a = OBD_Speed.getData();
  bbb = String(a,0); 
  bbb.toCharArray(result,20);
  Serial.write("Speed=");
  Serial.write(result);
  Serial.write("\n");


  
  a = OBD_EngineSpeed.getData();
  bbb = String(a,0); 
  bbb.toCharArray(result,20);
  Serial.write("Engine RPM=");
  Serial.write(result);
  Serial.write("\n");


  a = OBD_Coolant.getData();
  bbb = String(a,0); 
  bbb.toCharArray(result,20);
  Serial.write("Coolant Temperature=");
  Serial.write(result);
  Serial.write("\n");


  a = OBD_EngineLoad.getData();
  bbb = String(a,0); 
  bbb.toCharArray(result,20);
  Serial.write("Engine Load=");
  Serial.write(result);
  Serial.write("\n");

  Serial.write(";");
   
}
