/*
IR sensor: HS0038A2
Remote type: Redmi note 4
app : MI remote version 5.8.5.8
device: Philips TV
Setup steps: TV -> Philips -> (is tv on ?) off-> (checking available configuration
1/9, Power) -> (does device turn of/off ? ) yes

Ir codes 
Vol+ = 0x10, 0x810   //LED Increase intensity 
Vol- = 0x11, 0x811   // LED Decrease intensity 
Ch+ = 0x20, 0x820    //LED MAX
Ch- = 0x21, 0x821    //LED OFF
ok = 0xDD53082F, 0xB411F6DE // Tube On
power = 0xC, 0x80C          // Tube off
*/

#include <Arduino.h>    //remove this line if using official Arduino IDE
#include <IRremote.h>

const int RECV_PIN = 3;       //OUT of HS0038A2
const int controlPin = 5;     //to gate of MOSFET
const int tubePin = 6;
const int stepSize = 20;      //The light will be controlled in 20/2 = 10 steps
IRrecv irrecv(RECV_PIN);
decode_results results;
int intensity = 0;
int outputVal =0;
bool tubeStatus = false;

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn();
  irrecv.blink13(true);
  pinMode(tubePin, OUTPUT);
  digitalWrite(tubePin, LOW);
}

void loop()
{
  if (irrecv.decode(&results))
  {
    //Serial.println(results.value, HEX);
    switch(results.value)
  {
    case 0xC:
    case 0x80C:
    digitalWrite(tubePin, HIGH);
    tubeStatus = false;    
    break;    

    case 0xB411F6DE:
    case 0xDD53082F:
    digitalWrite(tubePin, LOW);    
    tubeStatus = true;
    break;

    case 0x11:    
    case 0x811:
    intensity = intensity -1;
    if(intensity<0)
    {
         intensity=0;
    }
    break;

    case 0x10:
    case 0x810:
    intensity = intensity +1;
    if(intensity>stepSize)
    {
         intensity=stepSize;
    }
    break;

    case 0x20:
    case 0x820:
    intensity = 20;
    break;

    case 0x21:
    case 0x821:
    intensity = 0;
    break;
  }  
    irrecv.resume();
  }

  outputVal = map((intensity*intensity), 0, 400, 0, 255);
  if (tubeStatus == false)
  {
    if((outputVal<255)&&(outputVal>=1))
    {
      outputVal--;
      analogWrite(controlPin, outputVal); //square it to give good non linear response
    }
    else 
    {
      analogWrite(controlPin, outputVal); //square it to give good non linear response 
    }
    
    //Serial.println(outputVal);
  }
  else
  {
    analogWrite(controlPin, 0);
  }
}