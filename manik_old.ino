//Вводим переменные и привязываем библиотеки
#define pot1 0 //о
#define pot2 1 //о
#define pot3 2 //о
#define pot4 3 //о
#define pot5 6 //о - Инициализируем порты для 

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Активируем сервоприводы
Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
LiquidCrystal_I2C lcd(0x27, 16, 2);

int zamer_pot1;
int zamer_pot2;
int zamer_pot3;
int zamer_pot4;
int zamer_pot5;
unsigned long last_time;




//Функция при запуске
void setup() {
  lcd.init(); //*
  lcd.backlight(); //* - Для включения LCD-дисплея
  
  Serial.begin(9600); //Активируем Терминал Ардуинки
  Serial.println("Устройство включено.");
  
  servo1.attach(11); //+
  servo2.attach(10); //+
  servo3.attach(9); //+
  servo4.attach(6); //+
  servo5.attach(5); //+
  servo0.attach(3); //+ - Указываем порты для подключения сервоприводов
}

//Функция в бесконечном цикле самоповтора
void loop() {
zamer_pot1 = analogRead(pot1)/5.68888; //#
zamer_pot2 = analogRead(pot2)/5.68888; //#
zamer_pot3 = analogRead(pot3)/5.68888; //#
zamer_pot4 = analogRead(pot4)/5.68888; //#
zamer_pot5 = analogRead(pot5)/5.68888; //# - Замер потенциометров и преведение их из 0-1023 к 0-180 для удобства в будущем (как градусы)

servo2.write(zamer_pot1); //q
servo0.write(zamer_pot2); //q
servo1.write(zamer_pot2); //q
servo3.write(zamer_pot3); //q
servo4.write(zamer_pot4); //q
servo5.write(zamer_pot5); //q - Двигаем сервоприводы на значения потенциометров

if (millis() - last_time > 1000){
  //Обновление главного меню раз в 1000 миллисекунд
  last_time = millis();
  
  Serial.println("          ");
  Serial.println("          ");
  Serial.println("          ");
  Serial.print("servo1= "); Serial.println(zamer_pot1);
  Serial.print("servo2= "); Serial.println(zamer_pot2);
  Serial.print("servo3= "); Serial.println(zamer_pot3);
  Serial.print("servo4= "); Serial.println(zamer_pot4);
  Serial.print("servo5= "); Serial.println(zamer_pot5);

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
