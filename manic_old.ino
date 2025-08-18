#define pot1 0
#define pot2 1
#define pot3 2
#define pot4 3
#define pot5 6

////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////

Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
LiquidCrystal_I2C lcd(0x27, 16, 2);

///////////////////////////////////////////////////////////////////////////////////////////////////////

int zamer_pot1;
int zamer_pot2;
int zamer_pot3;
int zamer_pot4;
int zamer_pot5;
unsigned long last_time;

///////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);
  servo1.attach(11);
  servo2.attach(10);
  servo3.attach(9);
  servo4.attach(6);
  servo5.attach(5);
  servo0.attach(3);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
zamer_pot1 = analogRead(pot1)/5.68888;
zamer_pot2 = analogRead(pot2)/5.68888;
zamer_pot3 = analogRead(pot3)/5.68888;
zamer_pot4 = analogRead(pot4)/5.68888;
zamer_pot5 = analogRead(pot5)/5.68888;

///////////////////////////////////////////////////////////////////////////////////////////////////////

servo2.write(zamer_pot1);
servo0.write(zamer_pot2);
servo1.write(zamer_pot2);
servo3.write(zamer_pot3);
servo4.write(zamer_pot4);
servo5.write(zamer_pot5);

///////////////////////////////////////////////////////////////////////////////////////////////////////

if (millis() - last_time > 1000){
  last_time = millis();

///////////////////////////////////////////////////////////////////////////////////////////////////////
  
  Serial.println("          ");
  Serial.println("          ");
  Serial.println("          ");
  Serial.print("servo1= "); Serial.println(zamer_pot1);
  Serial.print("servo2= "); Serial.println(zamer_pot2);
  Serial.print("servo3= "); Serial.println(zamer_pot3);
  Serial.print("servo4= "); Serial.println(zamer_pot4);
  Serial.print("servo5= "); Serial.println(zamer_pot5);

///////////////////////////////////////////////////////////////////////////////////////////////////////

  lcd.clear();                
  lcd.setCursor(0, 0); 
  lcd.print(zamer_pot1);
  lcd.setCursor(3, 1); 
  lcd.print(zamer_pot2);
  lcd.setCursor(6, 0); 
  lcd.print(zamer_pot3);
  lcd.setCursor(9, 1); 
  lcd.print(zamer_pot4);
  lcd.setCursor(12, 0); 
  lcd.print(zamer_pot5);
}
}