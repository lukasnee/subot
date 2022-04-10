/*
  Sumo robot basic strategy

  Description TODO

  created 2022-03-28
  by Lukas Neverauskis
*/

#include "suBot.hpp"

SuBot::Timer timerShort(SuBot::Timer::Mode::normal, 100);
SuBot::Timer timerLong(SuBot::Timer::Mode::normal, 1000);

void setup() { Serial.begin(9600); }

void loop(void) {
  if (!timerShort.isTriggered()) {
    Serial.println("short");
  } else {
    if (!timerLong.isTriggered()) {
      Serial.println("long");
    } else {
      timerShort.reset();
      timerLong.reset();
    }
  }
}
