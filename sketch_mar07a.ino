#include "ZMPT101B.h"
#include "ACS712.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

ZMPT101B voltageSensor(34);
ACS712 currentSensor(ACS712_20A, 36);

float P=0;
float U=0;
float I=0;
long dt=0;
float CulmPwh=0;
float units=0;
long changeScreen=0;
float lastSample=0;

unsigned long lasttime=0;
long ScreenSelect = 0;


void setup()
{
  Serial.begin(9600);
  Serial.println("Hello");
  delay(100);
  voltageSensor.setSensitivity(0.0025);
  voltageSensor.setZeroPoint(1000);
 
  currentSensor.setZeroPoint(2953);
  currentSensor.setSensitivity(0.15);

  //Caliberation Commands Need To Be Run On First Upload.  
  //CalibCurrent();
  CalibVoltage();

}

void loop()
{
  // To measure voltage/current we need to know the frequency of voltage/current
  // By default 50Hz is used, but you can specify desired frequency
  // as first argument to getVoltageAC and getCurrentAC() method, if necessary
  
  U = voltageSensor.getVoltageAC();
    if(U<55)
      {
        U=0;
        CulmPwh=0;
       }
  
  I = currentSensor.getCurrentAC();
  dt = micros()- lastSample;
  
    if(I<0.15)
      {
        I=0;
        CulmPwh=0;
      }
  
  //Calculating power
  P = U * I;
  
  CulmPwh = CulmPwh + P*(dt/3600);///uWh
  units= CulmPwh/1000;

  if(millis()-changeScreen>5000)
    {
      ScreenSelect+=1;
      changeScreen=millis();
    }
    
  if(millis()-lasttime>500)
    {
      if((ScreenSelect%4)==0)
      { displayVoltCurrent(); }//Volts and Current
  
      else if( (ScreenSelect%4)==1)
      { displayInstPower();   }//Instantaenous Power
  
      else if( (ScreenSelect%4)==2)
      { displayEnergy();      } //Energy
      
      else if( (ScreenSelect%4)==3)
      { displayUnits();       } //Units
    }
  lastSample=micros();
}

void displayVoltCurrent()
{
  Serial.println(String(U)+"V");
  Serial.println(String(I)+"A");
  lasttime=millis();
}

void displayInstPower()
{
  Serial.println("Power"); 
  
     if(P>1000)
      {
        Serial.println(String(P/1000)+"kW");
      }
    else
      {
        Serial.println(String(P)+"W");
      }
  lasttime=millis();
} 

void displayEnergy()
{
   if(CulmPwh>1000000000)
     {
        Serial.print("Energy kWh");
        Serial.println(String(CulmPwh/1000000000));
     }
   else if(CulmPwh<1000000000 && CulmPwh>1000000)
     {
        Serial.print("Energy Wh");
        Serial.println(String(CulmPwh/1000000));
     }
   else if(CulmPwh<1000000 && CulmPwh>1000)
     {
        Serial.print("Energy mWh");
        Serial.println(String(CulmPwh/1000));
     }
   else
     {
        Serial.print("Energy uWh");
        Serial.println(String(CulmPwh));
     }
  lasttime=millis();
 }

void displayUnits()
{
    if(units>1000000)
      {
        Serial.println("Units"); 
        Serial.println(String(units/1000000));
      }
    else if(units<1000000 && units>1000)
      {
        Serial.println("MilliUnits");
        Serial.println(String(units/1000));
      }
  else
      {
        Serial.println("MicroUnits");
        Serial.println(String(units)); 
      }
  lasttime=millis();
 }
 
void CalibCurrent()
{ 
  while(1)
  {
    currentSensor.calibrate();  
    Serial.print("Zero Point Current :");
    Serial.println(currentSensor.getZeroPoint());
    Serial.print("Current Zero Point :");
    Serial.println(currentSensor.getZeroPoint());
    delay(500);
  }
  
}

void CalibVoltage()
{ 
  while(1)
  {
    voltageSensor.calibrate();  
    Serial.print("Zero Point Voltage :");
    Serial.println(voltageSensor.getZeroPoint());
    Serial.print("Voltage Zero Point :");
    Serial.println(voltageSensor.getZeroPoint());
    delay(500);
  }
  
}

//void displayCenter(String text, int line) 
//{
  //int16_t x1;
  //int16_t y1;
  //uint16_t width;
  //uint16_t height;

  //display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
  // display on horizontal center
  //display.setCursor((SCREEN_WIDTH - width) / 2, line);
  //display.println(text); // text to display
  //display.display();
//}
