// газированная вода - 1; мятный сироп - 2; апельсиновый сок - 3; лимонад “Мятный” - 4;
// Лимонад “Заводной апельсин” - 5; Лимонад ‘Тройной” - 6; работает только диспенсер - 8; 
// работает только доставка - *; разрешить работу диспенсера - 9(режим 'только диспенсер');
// разрешить работу доставки - 7(режим 'только доставка')

#include <SimpleKeypad.h>
#include <ServoSmooth.h>



ServoSmooth servo; 

uint32_t tmr1;    
uint32_t tmr2;
uint32_t wait_time;
uint32_t micro_tmr1;    
uint32_t micro_tmr2;

#define time_for_up 75
#define time_for_other 80

float const_need_deg = 23;
int order = 0;
int count = 1;
int intkey = 0;
int order_count = 0;
float need_deg = 1;
bool need_for_motion = false;
int count_of_orders = 0;
bool dispenser_only = false;
bool delivery_only = false;
bool wait = false;
int current_order = 0;
int const_need_up = 0;     
int const_need_orange_juice = 0;
int const_need_mint_syrup = 0;
int need_up = 0;
int need_orange_juice = 0;
int need_mint_syrup = 0;
int time_for_motion = 2600;
int count_of_orders2 = 0;

#define light_or_dark 800 // 800
#define KP_ROWS 4
#define KP_COLS 3
#define dispenser A0
#define delivery A1
#define relay_sparkling_water 36
#define relay_orange_juice 40
#define relay_mint_syrup 30
#define light_delivery 650

// пины подключения (по порядку штекера)
byte colPins[KP_COLS] = {4, 3, 2};
byte rowPins[KP_ROWS] = {8, 7, 6, 5};
// массив имён кнопок
char keys[KP_ROWS][KP_COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

// создаём клавиатуру
SimpleKeypad pad((char*)keys, rowPins, colPins, KP_ROWS, KP_COLS);


void setup() {
  Serial.begin(9600);
  pinMode(dispenser, INPUT);
  pinMode(delivery, INPUT);
  pinMode(relay_sparkling_water, OUTPUT); //верхняя бутылка
  pinMode(relay_orange_juice, OUTPUT);
  pinMode(relay_mint_syrup, OUTPUT);
  pinMode(43,OUTPUT);

  servo.attach(49,2);  // "плавно" движемся к нему
  servo.setSpeed(25);
  tmr1 = -10001;
  servo.smoothStart();

  

}
void loop() {
  while (intkey != -13 && (count_of_orders!=4) & (((millis() - tmr1) > count_of_orders2 * 1000 + 4000))) { // пока не нажата кнопка "подтвердить заказ" (кнопка 9) И пока не сделано четыре заказа
    char key = pad.getKey();
    if (count == -1 || count == -2) {
    count = 1;
    need_deg = const_need_deg;
    }
    intkey = key - 48;
    if (intkey == -6) delivery_only = true;
    if (intkey == 8) dispenser_only = true;
    if (key && intkey < 7 && -1 < intkey ) {
      order = order + intkey * count;
      count = count * 10;
      order_count ++;
      count_of_orders ++;
    }
    if (intkey == -13 && count_of_orders <= 0) intkey = -100;
    if ((intkey == -13 && count_of_orders > 0) || count_of_orders == 4) {
      count_of_orders2 = count_of_orders;
      need_deg = const_need_deg;
    }
    Serial.println(dispenser_only);
  }
  if (dispenser_only) {
      char key = pad.getKey();
      if (key - 48 == 9) {
        need_for_motion = false;

      }
  }






  if (delivery_only) {
      char key = pad.getKey();
      if (key - 48 == 7) {
        order_count = order_count - 1; 
        need_for_motion = true;
        tmr1 = millis();
        servo.setTargetDeg(need_deg);

      }
  }


  if (order_count && !need_for_motion && !delivery_only) {  
    if (!current_order ) {
      current_order = order % 10;
    }
    if ((analogRead(dispenser) > light_or_dark) && !wait) {
      if (!const_need_up && !const_need_mint_syrup && !const_need_orange_juice) {
        if (current_order == 1) {
          const_need_up = 50;
          need_up = 50;
        }

        if (current_order == 2) {
          const_need_mint_syrup = 10;
          need_mint_syrup = 10;
        }

        if (current_order == 3) {
          const_need_orange_juice = 40;
          need_orange_juice = 40;
        }

        if (current_order == 4) {
          const_need_up = 80;
          need_up = 80;
          const_need_mint_syrup = 20;
          need_mint_syrup = 20;
        }

        if (current_order == 5) {
          const_need_up = 30;
          need_up = 30;
          const_need_orange_juice = 50;
          need_orange_juice = 50;
        }

        if (current_order == 6) {
          const_need_up = 35;
          need_up = 35;
          const_need_orange_juice = 45;
          need_orange_juice = 45;
          const_need_mint_syrup = 10;
          need_mint_syrup = 10;
        }
      micro_tmr1 = millis();
      tmr2 = millis();
      }
      
    }
    if ((analogRead(dispenser) > light_or_dark) && wait) {
      wait = false;
      wait_time = millis() - wait_time;
    }
    if (analogRead(dispenser) > light_or_dark && !wait) {
      if (need_orange_juice && !need_mint_syrup && !need_up) {
        if (millis()-micro_tmr1 < time_for_other) digitalWrite(relay_orange_juice, 1);
        if (millis()-micro_tmr1 >= time_for_other) {
          need_orange_juice --;
          micro_tmr1 = millis();
          if (!need_orange_juice) digitalWrite(relay_orange_juice, 0);
        }
      }

      if (need_mint_syrup && !need_up) {
        if (millis()-micro_tmr1 < time_for_other) digitalWrite(relay_mint_syrup, 1);
        if (millis()-micro_tmr1 >= time_for_other) {
          need_mint_syrup --;
          micro_tmr1 = millis();
          if (!need_up) digitalWrite(relay_mint_syrup, 0);
        }
      }

      if (need_up) {
        if (millis()-micro_tmr1 < time_for_up) digitalWrite(relay_sparkling_water, 1);
        if (millis()-micro_tmr1 >= time_for_up) {
          need_up --;
          micro_tmr1 = millis();
          if (!need_up) digitalWrite(relay_sparkling_water, 0);
        }
      }
    }
    if ((analogRead(dispenser) <= light_or_dark && !wait)) {

      digitalWrite(relay_sparkling_water, 0);
      digitalWrite(relay_mint_syrup, 0);
      digitalWrite(relay_orange_juice, 0);
      wait_time = millis();
      wait = true;
      tmr2 = millis();

    }

    long int true_time = millis();
    long int true_tmr2 = tmr2;
    long int true_wait_time = wait_time;
    if (!wait && (true_time - true_tmr2 - true_wait_time) >= (const_need_up * time_for_up + const_need_mint_syrup * time_for_other + const_need_orange_juice * time_for_other + 1000)) {

        order = (order - current_order) / 10;
        order_count = order_count - 1; 
        need_for_motion = true;
        tmr1 = millis();
        wait_time = 0;
        wait = false;
        current_order = 0;
        const_need_up = 0;
        const_need_mint_syrup = 0;
        const_need_orange_juice = 0;
        servo.setTargetDeg(need_deg);
        if (dispenser_only & order_count == 0) {
          order = 1;
          intkey = 0;
          order_count = 0;
          need_for_motion = false;
          count_of_orders = 0;
          dispenser_only = false;
          delivery_only = false;
          wait = false;
          current_order = 0;
          const_need_up = 0;
          const_need_orange_juice = 0;
          const_need_mint_syrup = 0;
          need_up = 0;
          need_orange_juice = 0;
          need_mint_syrup = 0;
          count = -2;
        }
    } 
  }

  if (count == -1 & !dispenser_only)  {
    servo.setTargetDeg(need_deg);
    servo.tick();
    order = 0;
    intkey = 0;
    order_count = 0;
    need_for_motion = false;
    count_of_orders = 0;
    dispenser_only = false;
    delivery_only = false;
    wait = false;
    current_order = 0;
    const_need_up = 0;
    const_need_orange_juice = 0;
    const_need_mint_syrup = 0;
    need_up = 0;
    need_orange_juice = 0;
    need_mint_syrup = 0;


  }
  if (!order_count && count > 1 && !dispenser_only) {
    Serial.println(1);
    if (need_deg == const_need_deg) {
      servo.setTargetDeg(28);
      Serial.println(1);
    }
    servo.tick();
    if ((millis()-tmr1)>(time_for_motion-100) && ((analogRead(delivery) < light_delivery) || need_deg < 5*const_need_deg)) {

      if (need_deg >= (const_need_deg*4+(count_of_orders-1)*const_need_deg) && analogRead(delivery) < light_delivery) {
        count = -1;
        need_deg = (-1)*const_need_deg;
      } 
      if (analogRead(delivery) < light_delivery && need_deg >= (const_need_deg*4-1)) {
        need_deg += const_need_deg;
        servo.setTargetDeg(need_deg);
        tmr1 = millis();
      }
      if (need_deg < const_need_deg*4) {
        need_deg += const_need_deg;
        servo.setTargetDeg(need_deg);
        tmr1 = millis();
      }


    }


  }
  
  if ((need_for_motion && order_count && delivery_only && !dispenser_only)) { //движение (наливание)
    servo.tick();

    if ((millis()-tmr1)>time_for_motion){
      need_for_motion = !need_for_motion;
      need_deg = need_deg + const_need_deg;
      servo.setTargetDeg(need_deg);
      tmr1 = millis();


    }
  }


  if ((need_for_motion && order_count && !dispenser_only && !delivery_only)) { //движение (наливание)
    servo.tick();
    if ((millis()-tmr1)>time_for_motion){
      need_for_motion = false;
      need_deg = need_deg + const_need_deg;
      servo.setTargetDeg(need_deg);
      tmr1 = millis();
      tmr2 = millis();


    }
  }
  
}

