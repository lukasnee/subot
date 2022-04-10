#pragma once

#include <IRremote.hpp>

#include <Arduino.h>

namespace SuBot {

struct Remote {
public:
  enum Strategy { none, A, B, C, D, E, F, G, H, I };

  struct Strategies {
    Strategy start = 0, main = 0;
  };

  Remote(byte receivePin, byte feedbackLedPin);

  void init();

  Strategies getStrategies(std::uint32_t timeoutInMs = 10 * 3600);

private:
  void blinkLed();

  IRrecv irrecv;
  const byte feedbackLedPin;
};

} // namespace SuBot
