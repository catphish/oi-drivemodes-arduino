#include <CAN.h>
#include <Metro.h>

#define MODE_NORMAL 0
#define MODE_SPORT 1
#define MODE_ECO 2
#define MODE_OFFROAD 3

#define INPUT_SPORT 0
#define INPUT_OFFROAD 1
#define INPUT_ECO 3
#define RED_SPORT 4
#define RED_OFFROAD 5
#define RED_ECO 6
#define BLUE_SPORT 7
#define BLUE_OFFROAD 8
#define BLUE_ECO 10

void setup() {
  CAN.begin(500000);

  pinMode(INPUT_SPORT, INPUT_PULLUP);
  pinMode(INPUT_OFFROAD, INPUT_PULLUP);
  pinMode(INPUT_ECO, INPUT_PULLUP);
  
  pinMode(RED_SPORT, OUTPUT);
  pinMode(RED_OFFROAD, OUTPUT);
  pinMode(RED_ECO, OUTPUT);
  pinMode(BLUE_SPORT, OUTPUT);
  pinMode(BLUE_OFFROAD, OUTPUT);
  pinMode(BLUE_ECO, OUTPUT);
}

void set_parameter(uint8_t parameter, int32_t value) {
  value = value * 32;
  CAN.beginPacket(0x601); // SDO
  CAN.write(0x40);        // Set
  CAN.write(0x01);        // Parameter by ID
  CAN.write(0x20);        // Parameter by ID
  CAN.write(parameter);   // Parameter ID
  CAN.write(value >> 0);
  CAN.write(value >> 8);
  CAN.write(value >> 16);
  CAN.write(value >> 24);
  CAN.endPacket();
}

void set_throtmax(int32_t value) {
  set_parameter(97, value);
}

void set_brkmax(int32_t value) {
  set_parameter(49, value);
}

Metro can_timer = Metro(500);

void loop() {
  int mode = MODE_NORMAL;
  if(!digitalRead(INPUT_SPORT)) mode = MODE_SPORT;
  if(!digitalRead(INPUT_OFFROAD)) mode = MODE_OFFROAD;
  if(!digitalRead(INPUT_ECO)) mode = MODE_ECO;

  digitalWrite(RED_SPORT, mode == MODE_SPORT);
  digitalWrite(RED_ECO, mode == MODE_ECO);
  digitalWrite(RED_OFFROAD, mode == MODE_OFFROAD);

  digitalWrite(BLUE_SPORT, mode != MODE_SPORT);
  digitalWrite(BLUE_ECO, mode != MODE_ECO);
  digitalWrite(BLUE_OFFROAD, mode != MODE_OFFROAD);

  if (can_timer.check()) {
    if(mode == MODE_SPORT) {
      set_throtmax(100);
      set_brkmax(-20);
    } else if(mode == MODE_ECO) {
      set_throtmax(55);
      set_brkmax(-30);
    } else if(mode == MODE_OFFROAD) {
      set_throtmax(75);
      set_brkmax(-75);
    } else { // MODE_NORMAL
      set_throtmax(75);
      set_brkmax(-30);
    }
  }

}
