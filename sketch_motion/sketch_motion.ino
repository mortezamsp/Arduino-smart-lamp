#include "Wire.h" // I2C Library
#include "BH1750FVI.h" // Sensor Library
BH1750FVI LightSensor;

//motion module setup
#define MotionPin  7
#define RelayPin  8
#define calibrationTime 10 //give yourself time to leave the room       

//handle continious time
long unsigned int starttime = 0;
long unsigned int last = 100000;
//
bool lampON = false;
#define MaxLightOn  300
#define MinLightOn 10
#define additiveLightOnTime 20
#define continuseMeasure 30
//
long unsigned int pause = MinLightOn;
long unsigned int lastTimePuseDecremented = 0;
#define PuseChangingThreshold continuseMeasure+5


void setup()
{
  
  //Serial.begin(9600);
  
  LightSensor.begin();
  LightSensor.SetAddress(Device_Address_H);//Address 0x5C
  LightSensor.SetMode(Continuous_H_resolution_Mode);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
  
  pinMode(MotionPin, INPUT);
  digitalWrite(MotionPin, HIGH);
  //Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      //Serial.print(".");
      delay(1000);
      }
    //Serial.println(" done");
    //Serial.println("SENSOR ACTIVE");
  }

void loop()
{
  uint16_t lux = LightSensor.GetLightIntensity();// Get Lux value
  //Serial.print("lux=");
  //Serial.print(lux);
  //Serial.println(" ");
  if(lux > 15 && !lampON)
    return;
  
  if(digitalRead(MotionPin) == HIGH)
  {
    if(!lampON)
    {
      //turn lamp ON
      digitalWrite(RelayPin, LOW);
      starttime = millis()/1000;
      lampON = true;
      //Serial.println("motion started");
      //delay(pause*1000);
      
      //update lighting time, if motion occures sequentially
      if(starttime - last < continuseMeasure)
      {
        pause += additiveLightOnTime;
        if(pause > MaxLightOn)
          pause = MaxLightOn;
          //Serial.print("increasing to ");
          //Serial.print(pause);
          //Serial.println(" ");
      }
      last = starttime;
    }
    else
    {
      //update start time
      starttime = millis()/1000;
      //Serial.println("motion updated");
      delay(2000);
    } 
  }
  
  if(lampON)
  {
    long unsigned int dur = (millis() / 1000) - starttime;
    //Serial.print("dur = ");
    //Serial.print(dur);
    //Serial.println();
    
    if(dur > pause)
    { 
      lampON = false;
      digitalWrite(RelayPin, HIGH);
      //Serial.print("motion stoped after");
      //Serial.print(dur);
      //Serial.print("secconds.");
      //Serial.println();
      last = millis() / 1000;
      delay(200);
    }
  }

  if(!lampON && digitalRead(MotionPin) == LOW)
  {
    //this 'if' denies furthor errors
    if(last > lastTimePuseDecremented)
      lastTimePuseDecremented = last;

   //decrease 'pause' in periods
    long unsigned int now = millis()/1000;
    if(now - lastTimePuseDecremented > PuseChangingThreshold)
    {
      //Serial.print("decreasing from ");
          //Serial.print(pause);
          //Serial.print(" ");
      if(pause >= additiveLightOnTime)
        pause -= additiveLightOnTime;
      if(pause < MinLightOn)
        pause = MinLightOn;
      lastTimePuseDecremented = now;
      
          //Serial.print(" to ");
          //Serial.print(pause);
          //Serial.println(" ");
    }
  }

  delay(200);
}
