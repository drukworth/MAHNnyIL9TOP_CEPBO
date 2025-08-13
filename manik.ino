//Вводим переменные и привязываем библиотеки
#define pot1 0 //o
#define pot2 1 //o 
#define pot3 2 //o
#define pot4 3 //o
#define pot5 6 //o - Указываем порты для подключения потенциометров

#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EncButton.h>
#include <Arduino.h>
#include <GyverIO.h>
#include <EEPROM.h>

int zamer_pot1;
int zamer_pot2;
int zamer_pot3;
int zamer_pot4;
int zamer_pot5;
unsigned long last_time;
byte menu;
byte menu_level;
boolean mode_active;

//Активируем сервоприводы, LCD-Дисплей и Энкодер. Работа с энкодером взята у AlexGyver.
Servo servo0;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
LiquidCrystal_I2C lcd(0x27, 16, 2);
EncButton eb(0, 1, 2);






//Функция при запуске
void setup() {
  lcd.init(); //*
  lcd.backlight(); //* - Для  включения lcd-дисплея

  Serial.begin(9600); //Активируем Терминал Ардуинки

  servo1.attach(11); //+
  servo2.attach(10); //+
  servo3.attach(9); //+
  servo4.attach(6); //+
  servo5.attach(5); //+
  servo0.attach(3); //+ - Указываем порты для подключения сервоприводов

  Serial.println("Устройство включено.");

  menu = 1;
  menu_level = 1;
  mode_active = false;

  eb.setBtnLevel(LOW); //z
  eb.setClickTimeout(500); //z
  eb.setDebTimeout(50); //z
  eb.setHoldTimeout(600); //z 
  eb.setStepTimeout(200); //z
  eb.setTimeout(1000); //z
  eb.setEncReverse(0); //z
  eb.setEncType(EB_STEP4_LOW); //z
  eb.setFastTimeout(30); //z
  eb.counter = 0; //z - Дефолтные настройки для работы с энкодером
}




void loop() {
  eb.tick(); //Нужно для работы энкодера

  // Обработка энкодера, работа меню
  if (eb.left() && menu_level == 1) {
    menu = (menu % 3) + 1;    
  }
  
  if (eb.right() && menu_level == 1) {
    menu = (menu + 1) % 3 + 1;    
  }

  if (eb.hold()) {
    mode_active = !mode_active;   
  }

  if (eb.click()) {
    if (menu_level == 1){
      menu_level = 2;
    } else {
      menu_level = 1;
    }
    mode_active = false;
  }

  zamer_pot1 = analogRead(pot1)/5.68888; //#
  zamer_pot2 = analogRead(pot2)/5.68888; //#
  zamer_pot3 = analogRead(pot3)/5.68888; //#
  zamer_pot4 = analogRead(pot4)/5.68888; //#
  zamer_pot5 = analogRead(pot5)/5.68888; //# - Замер потенциометров и преведение их из 0-1023 к 0-180 для удобства в будущем (как градусы)

 if (menu_level == 1) {
    // Обновление главного меню раз в 1000 миллисекунд
    if (millis() - last_time > 1000) {
      last_time = millis();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Vvod");
      lcd.setCursor(5, 0);
      lcd.print("Pusk");
      lcd.setCursor(10, 0);
      lcd.print("Prog");
      
      switch(menu) {
        case 1: lcd.setCursor(0, 1); break;
        case 2: lcd.setCursor(5, 1); break;
        case 3: lcd.setCursor(10, 1); break;
      }
      lcd.print("0000");
    }
  }


  else if (menu_level == 2 && menu == 1) {
    if (mode_active) {
      //Режим ручного управления сервоприводами Vvod
      servo2.write(zamer_pot1);
      servo0.write(zamer_pot2);
      servo1.write(zamer_pot2);
      servo3.write(zamer_pot3);
      servo4.write(zamer_pot4);
      servo5.write(zamer_pot5);
    
    if (millis() - last_time > 1000) {
      last_time = millis();
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
    else if (!mode_active) {
        //Режим ПАУЗЫ
        if (millis() - last_time > 1000){
        last_time = millis();
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("Pauza");
        lcd.setCursor(3, 1);
        lcd.print("Hold to change");
        }
      }
    } 


 else if (menu_level == 2 && menu == 2) {
      if (mode_active) {
        //Режим запуска программы
        lcd.clear();
      } 
      
      else if (!mode_active){
        //Режим ПАУЗЫ
        if (millis() - last_time > 1000){
        last_time = millis();
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("Pauza");
        lcd.setCursor(3, 1);
        lcd.print("Hold to change");
        }
      }
  } 
 
 else if (menu_level == 2 && menu == 3) {
      if (mode_active){  
        //Режим программирования 
        lcd.clear();
      } 
      
      else if (!mode_active) {
        //Режим ПАУЗЫ
        if (millis() - last_time > 1000){
        last_time = millis();
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.print("Pauza");
        lcd.setCursor(3, 1);
        lcd.print("Hold to change");
        }
      }
  }
}



