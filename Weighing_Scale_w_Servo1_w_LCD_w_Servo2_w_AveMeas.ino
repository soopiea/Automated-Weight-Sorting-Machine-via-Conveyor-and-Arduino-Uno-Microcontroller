#ifndef HX711_H_
#define HX711_H_
#include "Arduino.h"
#include <Servo.h>
#include<Wire.h>
#include<LiquidCrystal_I2C.h>


Servo myservo1;
Servo myservo2;

int a = 0;
int i = 0;
int pos = 10;   
float mul = 0.18;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);













class Hx711
{
public:
  Hx711(uint8_t pin_din, uint8_t pin_slk);
  virtual ~Hx711();
  long getValue();
  long averageValue(byte times = 32);
  void setOffset(long offset);
  void setScale(float scale = 742.f);
  float getGram();

private:
  const uint8_t _pin_dout;
  const uint8_t _pin_slk;
  long _offset;
  float _scale;
};

#endif /* HX711_H_ */









Hx711::Hx711(uint8_t pin_dout, uint8_t pin_slk) :
    _pin_dout(pin_dout), _pin_slk(pin_slk)
{
  pinMode(_pin_slk, OUTPUT);
  pinMode(_pin_dout, INPUT);

  digitalWrite(_pin_slk, HIGH);
  delayMicroseconds(100);
  digitalWrite(_pin_slk, LOW);

  averageValue();
  this->setOffset(averageValue());
  this->setScale();
}

Hx711::~Hx711()
{

}

long Hx711::averageValue(byte times)
{
  long sum = 0;
  for (byte i = 0; i < times; i++)
  {
    sum += getValue();
  }

  return sum / times;
}

long Hx711::getValue()
{
  byte data[3];

  while (digitalRead(_pin_dout))
    ;

  for (byte j = 0; j < 3; j++)
  {
    for (byte i = 0; i < 8; i++)
    {
      digitalWrite(_pin_slk, HIGH);
      bitWrite(data[2 - j], 7 - i, digitalRead(_pin_dout));
      digitalWrite(_pin_slk, LOW);
    }
  }

  digitalWrite(_pin_slk, HIGH);
  digitalWrite(_pin_slk, LOW);

  return ((long) data[2] << 16) | ((long) data[1] << 8) | (long) data[0];
}

void Hx711::setOffset(long offset)
{
  _offset = offset;
}

void Hx711::setScale(float scale)
{
  _scale = scale;
}

float Hx711::getGram()
{
  long val = (averageValue() - _offset);
  return (float) val / _scale;
}







// Hx711.DOUT - pin #A1
// Hx711.SCK - pin #A0



Hx711 scale(A1, A0);

void setup() {

  Serial.begin(9600);
  myservo2.attach(10);
  lcd.begin(16,2);
  myservo1.attach(8);
  myservo1.write(pos);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WEIGHT:");
  lcd.setCursor(8,0);
  lcd.setCursor(0,1);
  lcd.print("STATUS:");

  

}

void loop() {

    delay(5000);
//servo1 sweep
    for (pos = 10; pos <= 50; pos += 1) 
    { 
      myservo1.write(pos);              
      delay(30);                       
    }
    
//Weighing Sensor Reading

    delay(1000);
    
    float first_reading= (((scale.getGram())*2)+(scale.getGram()*(mul)));
    Serial.println(first_reading, 2);
//    Serial.print("\t");

     //Put loop here until ilan ka times until maging final output
    delay(1000);
    float second_reading= (((scale.getGram())*2)+(scale.getGram()*(mul)));
    Serial.println(second_reading, 2);
//    Serial.print("\t");
//    
    float third_reading= (((scale.getGram())*2)+(scale.getGram()*(mul)));
    Serial.println(third_reading, 2);
//    Serial.print("\t");

    float fourth_reading= (((scale.getGram())*2)+(scale.getGram()*(mul)));
    Serial.println(fourth_reading, 2);

    float finale_reading= ((second_reading+third_reading+fourth_reading)/3);
    Serial.print("finale: ");
    Serial.println(finale_reading, 2);
//    Serial.print("\t");

    


//LCD Code 
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WEIGHT:");
    lcd.setCursor(8,0);
    lcd.print(finale_reading);
    lcd.print(" g");
    lcd.setCursor(0,1);
    lcd.print("STATUS:");
    delay(500);

   
//Servo2 repoition to center
    myservo2.write(90);

//Servo2 postition to either waste or correct box
    if( 5.4<= finale_reading && finale_reading <= 6.6 )
    {
      myservo2.write(130);
      lcd.setCursor(8,1);
      lcd.print("Passed");
//      delay(2000);
    }
    else
    {
      myservo2.write(60);
      lcd.setCursor(8,1);
      lcd.print("Waste");
//      delay(2000); 
     }
  
//Servo1 sweep to drop cube  
    for (pos = 50; pos <= 100; pos += 1) 
    { 
      myservo1.write(pos);              
      delay(30);                       
    }
    
    delay(2000);

//Servo 1 sweep back to 0 degree position
    for (pos = 100; pos >= 10; pos -= 1) 
    { 
      myservo1.write(pos);              
      delay(30);                       
    }


  
}
