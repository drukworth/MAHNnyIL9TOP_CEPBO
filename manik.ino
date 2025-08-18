//Вводим переменные и привязываем библиотеки
#define pot1 0 //o
#define pot2 1 //o 
#define pot3 2 //o
#define pot4 3 //o
#define pot5 6 //o - Указываем порты для подключения потенциометров

#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EncButton.h>
#include <GyverIO.h>


unsigned long last_time;
unsigned long pusk_wait_start_time;
unsigned int zamer_pot1;
unsigned int zamer_pot2;
unsigned int zamer_pot3;
unsigned int zamer_pot4;
unsigned int zamer_pot5;
unsigned int prog_byte;
unsigned int pusk_vibor;
unsigned int pusk_current_step;
unsigned int eeprom_info[512];
unsigned char menu;
unsigned char menu_level;
unsigned char prog_vibor;
unsigned char pusk_povtor;
unsigned char pusk_time_to_wait;
boolean mode_active;
boolean eeprom_readed;
boolean pusk_waiting;
String prog_pot; 

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
  Serial.println("Устройство включено.");

  servo0.attach(10); //+
  servo1.attach(11); //+
  servo2.attach(3); //+
  servo3.attach(9); //+
  servo4.attach(6); //+
  servo5.attach(5); //+ - Указываем порты для подключения сервоприводов
  
  menu = 1;
  menu_level = 1;
  mode_active = false;
  prog_byte = 0;
  prog_vibor = 0;
  eeprom_readed = false;
  pusk_povtor = 0;
  pusk_vibor = 0;
  pusk_current_step = 1;
  pusk_waiting = false;
  pusk_wait_start_time = 0;
  pusk_time_to_wait = 0;
  
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



//Функция в бесконечном цикле самоповтора
void loop() {
  eb.tick(); //Нужно для работы энкодера
  
  zamer_pot1 = analogRead(pot1)/5.68888; //#
  zamer_pot2 = analogRead(pot2)/5.68888; //#
  zamer_pot3 = analogRead(pot3)/5.68888; //#
  zamer_pot4 = analogRead(pot4)/5.68888; //#
  zamer_pot5 = analogRead(pot5)/5.68888; //# - Замер потенциометров и преведение их из 0-1023 к 0-180 для удобства в будущем (как градусы)

 if (menu_level == 1) {
    //Обновление главного меню раз в 1000 миллисекунд
    if (millis() - last_time > 1000) {
      last_time = millis();
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Vvod");
      lcd.setCursor(6, 0);
      lcd.print("Pusk");
      lcd.setCursor(11, 0);
      lcd.print("Prog");
      
      switch(menu) {
        case 1: lcd.setCursor(1, 1); break;
        case 2: lcd.setCursor(6, 1); break;
        case 3: lcd.setCursor(11, 1); break;
        }
      lcd.print("0000");
      }

    //Обработка энкодера, работа меню
    if (eb.right()) {
      menu = (menu % 3) + 1;    
      }
    
    if (eb.left()) {
      menu = (menu + 1) % 3 + 1;    
      }
    if (eb.click()){
      menu_level = 2;
      }
  }


  else if (menu_level == 2 && menu == 1) {
    if (mode_active) {
      //Режим ручного управления сервоприводами Vvod
      servo0.write(zamer_pot1); //q
      servo1.write(zamer_pot2); //q
      servo2.write(zamer_pot2); //q
      servo3.write(zamer_pot3); //q
      servo4.write(zamer_pot4); //q
      servo5.write(zamer_pot5); //q - Двигаем сервоприводы на значения потенциометров
    
      if (millis() - last_time > 1000) {
        last_time = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vvod");                
        lcd.setCursor(1, 1); 
        lcd.print(zamer_pot1);
        lcd.setCursor(4, 0); 
        lcd.print(zamer_pot2);
        lcd.setCursor(7, 1); 
        lcd.print(zamer_pot3);
        lcd.setCursor(10, 0); 
        lcd.print(zamer_pot4);
        lcd.setCursor(13, 1); 
        lcd.print(zamer_pot5);
        }

      if (eb.hold()){//Смена режима ВВОД-ПАУЗА
        mode_active = !mode_active;
        }
      }

    else if (!mode_active) {
      //Режим ПАУЗЫ
      if (millis() - last_time > 1000){
        last_time = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vvod");       
        lcd.setCursor(11, 0);
        lcd.print("Pauza");
        lcd.setCursor(1, 1);
        lcd.print("Hold to change");
        }
      if (eb.click()){//Возврат в главное меню
        menu_level = 1;
        pusk_povtor = eeprom_info[0] + 1;
        }
      if (eb.hold()){
        mode_active = !mode_active;
        }
      }
    } 

else if (menu_level == 2 && menu == 2) {
    if (mode_active) {
      //Режим запуска программы Pusk

      if (!eeprom_readed) {//Читаем все данные с EEPROM 
        for (int i = 0; i < 512; i++) {
          eeprom_info[i] = EEPROM.read(i);
          }
        eeprom_readed = true;
        Serial.println("Память EEPROM прочитана.");
        }

      //Если выполнение программы завершено
      if (pusk_povtor == 0) {
        if (millis() - last_time > 1000) {
          last_time = millis();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pusk");
          lcd.setCursor(0, 1);
          lcd.print("Vse");
          lcd.setCursor(5, 0);
          lcd.print("Click-again");
          lcd.setCursor(8, 1);
          lcd.print("Hold-end");
          }
        } 
      else {
        //Состояние ожидания
        if (pusk_waiting) {
          if (millis() - pusk_wait_start_time >= (pusk_time_to_wait * 1000)) {
            pusk_waiting = false;
            pusk_current_step++; //Переход к следующему шагу
            }
          } 
        else {
          //Обработка текущего шага
          if (pusk_current_step >= 512) {
            //Конец программы - уменьшаем счетчик повторений на 1
            pusk_povtor--;
            
            if (pusk_povtor > 0) {
              pusk_current_step = 1; //Начинаем заново
              }
            } 
          else {
            pusk_vibor = eeprom_info[pusk_current_step];
            
            if (pusk_vibor != 255) {
              if (pusk_current_step % 2 == 0) {
                //Четный шаг - время ожидания (смотрите меню Prog)
                pusk_time_to_wait = pusk_vibor;
                pusk_waiting = true;
                pusk_wait_start_time = millis();
                } 
              else {
                //Нечетный шаг - команда сервоприводу
                switch (pusk_current_step % 10) {
                  case 1:
                    servo0.write(pusk_vibor);
                    break;
                  case 3:
                    servo1.write(pusk_vibor);
                    servo2.write(pusk_vibor);
                    break;
                  case 5:
                    servo3.write(pusk_vibor);
                    break;
                  case 7:
                    servo4.write(pusk_vibor);
                    break;
                  case 9:
                    servo5.write(pusk_vibor);
                    break;
                  }
                pusk_current_step++;
                }
              }
            else {
              // Пропуск пустого шага (если ячейка равна 255(смотрите меню Prog))
              pusk_current_step++;
              }
          }
        }

        if (millis() - last_time > 1000) {
          last_time = millis();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Pusk");
          lcd.setCursor(0, 1);
          
          if (pusk_waiting) {
            lcd.print("Wait:");
            } 
          else {
            if (pusk_current_step % 2 == 0) {
              lcd.print("Time:");
              } 
            else {
              switch (pusk_current_step % 10) {
                case 1: lcd.print("Pot1:"); break;
                case 3: lcd.print("Pot2:"); break;
                case 5: lcd.print("Pot3:"); break;
                case 7: lcd.print("Pot4:"); break;
                case 9: lcd.print("Pot5:"); break;
                }
              }
            }
          
          lcd.setCursor(9, 0);
          lcd.print(pusk_current_step);
          lcd.setCursor(9, 1);
          lcd.print(pusk_vibor);
          lcd.setCursor(13, 1);
          lcd.print(pusk_povtor);
          }
        }

      if (eb.hold()) {
        mode_active = !mode_active;
        }
      if (eb.click()){
        pusk_povtor = eeprom_info[0] + 1;
        }
      } 
    else {
      //Режим ПАУЗЫ
      if (millis() - last_time > 1000) {
        last_time = millis();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Pusk");  
        lcd.setCursor(11, 0);
        lcd.print("Pauza");
        lcd.setCursor(1, 1);
        lcd.print("Hold to change");
        }

      if (eb.click()) {
        menu_level = 1;
        }
      if (eb.hold()) {
        mode_active = !mode_active;
        }
      }
    }
 
 else if (menu_level == 2 && menu == 3) {
      if (mode_active){
        //Режим программирования Prog
        //Условия для записывания 'ходов' в eeprom память: 0 - байт повторения; 1 - ход первого серво(указать угол); 2 - время ожидания; 3 - ход второго серво; 4 - время ожидания; и тд...
        if (prog_byte%2 != 0 || prog_byte != 0){
          switch (prog_byte%10){
              case 1:
                prog_vibor = zamer_pot1;
                prog_pot = "Pot1:";
                servo0.write(zamer_pot1);
                break;
              case 3:
                prog_vibor = zamer_pot2;
                prog_pot = "Pot2:";
                servo1.write(zamer_pot2);
                servo2.write(zamer_pot2);
                break;
              case 5:
                prog_vibor = zamer_pot3;
                prog_pot = "Pot3:";
                servo3.write(zamer_pot3);
                break;
              case 7:
                prog_vibor = zamer_pot4;
                prog_pot = "Pot4:";
                servo4.write(zamer_pot4);
                break;
              case 9:
                prog_vibor = zamer_pot5;
                prog_pot = "Pot5:";
                servo5.write(zamer_pot5);
                break;
              }
          } 

          if (millis() - last_time > 1000){
          last_time = millis();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Prog");
          lcd.setCursor(9, 0);
          lcd.print(prog_byte);
          lcd.setCursor(12, 0);
          lcd.print("/");
          lcd.setCursor(13, 0);
          lcd.print("512");
          lcd.setCursor(9, 1);
          lcd.print(prog_vibor); 
          lcd.setCursor(12, 1);
          lcd.print("/");
          lcd.setCursor(13, 1);
          lcd.print("200");
          
          lcd.setCursor(0, 1); //Условия слова на экране(Povtor/Time/Potx)
          if (prog_byte == 0){ 
            lcd.print("Povtor");
            }
          else if (prog_byte%2==0){
            lcd.print("Time:");
            }
          else{
            lcd.print(prog_pot);
            } 
          }

        //Обработка энкодера
        if (eb.click() && prog_byte < 512){
          EEPROM.update(prog_byte, prog_vibor);
          prog_byte++;
          prog_vibor = 0;
          }

        if (prog_byte%2 == 0 || prog_byte == 0){
          if (eb.left()){
            if (prog_vibor == 0){
              prog_vibor = 200;
              }
            else{
              prog_vibor--;
              }
            }

          else if (eb.right()){
            if (prog_vibor == 200){
              prog_vibor = 0;
              }
            else{
              prog_vibor++;
              }
            }
          } 


        if (eb.hold()){
          mode_active = !mode_active;
          }
        }
 
      
      else if (!mode_active) {
        //Режим ПАУЗЫ
        if (millis() - last_time > 1000){
          last_time = millis();
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Prog");  
          lcd.setCursor(11, 0);
          lcd.print("Pauza");
          lcd.setCursor(1, 1);
          lcd.print("Hold to change");
          }
        if (eb.click()) {
          menu_level = 1;
          eeprom_readed = false;
          Serial.println("EEPROM память была обновлена.");
          for (int i = prog_byte; prog_byte < 1024; prog_byte++){
            EEPROM.update(i, 255);
            }
          pusk_povtor = EEPROM.read(0); 
          prog_byte = 0;
          prog_vibor = 0;
          }
        if (eb.hold()){
          mode_active = !mode_active;
          }
      }
  }
}













