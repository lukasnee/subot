/*
  Sumo robot basic strategy

  Description TODO

  created 2022-03-28
  by Lukas Neverauskis
*/

#include "suBot.hpp"

Struct Config {
  constexpr byte remoteFeedbackLedPin = 13;
  constexpr byte remoteReceivePin = 20;
};

SuBot::Timer timerShort(SuBot::Timer::Mode::normal, 100);
SuBot::Timer timerLong(SuBot::Timer::Mode::normal, 1000);

void setup() {
  Serial.begin(9600);
  SuBot::Remote remote(remoteFeedbackLedPin, Config::remoteReceivePin);
  SuBot::Remote::Strategies strategies = remote.getStrategies();
}

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
