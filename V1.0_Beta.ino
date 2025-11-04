#include "ssd1306.h"
int pLM[4], pRM[4];
unsigned long LM = 5000, RM = 5000, targetR, targetL;
bool conf = 0;
void setup() {
  targetR = RM;
  targetL = LM;
  Serial.begin(9600);
  ssd1306_128x64_i2c_init();
  ssd1306_fillScreen(0x00);
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_printFixedN(0,  16, "LM:", STYLE_NORMAL, 1);
  ssd1306_printFixed(0,  32, "Target L:", STYLE_ITALIC);
  ssd1306_printFixed(55,  32, "off", STYLE_ITALIC);
  ssd1306_printFixedN(0,  40, "RM:", STYLE_NORMAL, 1);
  ssd1306_printFixed(0,  56, "Target R:", STYLE_ITALIC);
  ssd1306_printFixed(55,  56, "off", STYLE_ITALIC);
  //  Serial.println("\n-= New Gen =-");
  //  step( 8, 0); Serial.println("---------------");
  //  step(-8, 0); Serial.println("--------------R");
  //  step( 8, 1); Serial.println("--------------R");
  //  step(-8, 1);
  for (byte i = 2; i < 10; i++)pinMode(i, INPUT_PULLUP);
  for (byte i = 14; i < 17; i++)pinMode(i, INPUT_PULLUP);
  //attachInterrupt(0, stop, FALLING);
  config();
}
void loop() {
  //step(1,0);step(-1,1);delay(2000);
  kostil();
}

void step(int s, byte r) {

  if (r) {
    if (!conf)targetR = RM + s;
    else targetR += s;
  }
  else {
    if (!conf)targetL = LM + s;
    else targetL += s;
  }
  for (int n = 0; n < abs(s); n++) {
    if (r)RM += s < 0 ? -1 : 1;
    else LM += s < 0 ? -1 : 1;
    stepone();
    motoring();
    monitoring(r);
    kostil();
  }
}

void stepone()   {
  for (int i = 0; i < 4; i++) {
    pLM[i] = ((i + LM) / 2) % 2;
    pRM[i] = ((i + RM) / 2) % 2;
  }
}
void motoring()  {
  for (int i = 0; i < 4; i++) {
    digitalWrite(3 + i, pLM[i]);
    digitalWrite(7 + i, pRM[i]);
  }
}
void monitoring(byte r) {
  //  for (int i = 0; i < 4; i++)Serial.print(pLM[i] ? 'M' : '-');
  //  Serial.print(" "); Serial.println(LM);
  //  for (int i = 0; i < 4; i++)Serial.print(pRM[i] ? 'M' : '-');
  //  Serial.print(" "); Serial.print(RM); Serial.println(".");
  char f[11];
  if (r) {
    if (!conf) {
      for (byte i = 0; i < 11; i++)f[i] = 0;
      ssd1306_printFixedN(37,  40, ltoa(RM, f, 10), STYLE_NORMAL, 1);
    }
    for (byte i = 0; i < 11; i++)f[i] = 0;
    ssd1306_printFixed(55,  56, ltoa(targetR, f, 10), STYLE_ITALIC);
  }
  else {
    if (!conf) {
      for (byte i = 0; i < 11; i++)f[i] = 0;
      ssd1306_printFixedN(37,  16, ltoa(LM, f, 10), STYLE_NORMAL, 1);
    }
    for (byte i = 0; i < 11; i++)f[i] = 0;
    ssd1306_printFixed(55,  32, ltoa(targetL, f, 10), STYLE_ITALIC);
  }
}

void monitoring(void) {
  //  for (int i = 0; i < 4; i++)Serial.print(pLM[i] ? 'M' : '-');
  //  Serial.print(" "); Serial.println(LM);
  //  for (int i = 0; i < 4; i++)Serial.print(pRM[i] ? 'M' : '-');
  //  Serial.print(" "); Serial.print(RM); Serial.println(".");
  char f[11];
  for (byte i = 0; i < 11; i++)f[i] = 0;
  if (!conf) {
    ssd1306_printFixedN(37,  40, ltoa(RM, f, 10), STYLE_NORMAL, 1);
    for (byte i = 0; i < 11; i++)f[i] = 0;
    ssd1306_printFixedN(37,  16, ltoa(LM, f, 10), STYLE_NORMAL, 1);
    for (byte i = 0; i < 11; i++)f[i] = 0;
  }
  ssd1306_printFixed(55,  56, ltoa(targetR, f, 10), STYLE_ITALIC);
  for (byte i = 0; i < 11; i++)f[i] = 0;
  ssd1306_printFixed(55,  32, ltoa(targetL, f, 10), STYLE_ITALIC);
}

void stop() {
  for (byte i = 2; i < 10; i++)digitalWrite(i, 0);
  ssd1306_printFixed (0,  0, "Emergency stop", STYLE_BOLD);
  ssd1306_printFixed(55,  32, "off", STYLE_ITALIC);
  ssd1306_printFixed(55,  56, "off", STYLE_ITALIC);
  //  Serial.println("Emergency stop");
  while (1) {};
}
void kostil() {
  if (!digitalRead(2))stop();
}
void config() {
  conf = 1;
  targetL = 0;
  targetR = 0;
  ssd1306_printFixed (0,  0, "Homing mode", STYLE_BOLD);
  ssd1306_printFixedN(37,  40, "0, homing", STYLE_NORMAL, 1);
  ssd1306_printFixedN(37,  16, "0, homing", STYLE_NORMAL, 1);
  monitoring();
  while (1) {
    if (!digitalRead(A3))step(100, 0);
    if (!digitalRead(A2))step(-100, 0);
    if (!digitalRead(A1))step(100, 1);
    if (!digitalRead(A0))step(-100, 1);
    if (!digitalRead(2)) {
      LM = 5000;
      RM = 5000;
      ssd1306_clearBlock(0, 0, 66, 8);
      ssd1306_clearBlock(37, 40, 108, 32);
      conf = 0;
      delay(300);
      return;
    }
  }
}
