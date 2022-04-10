#include "remote.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace SuBot {

Remote::Remote(byte receivePin, byte feedbackLedPin)
    : irrecv(receivePin), feedbackLedPin(feedbackLedPin){};

void SuBot::Remote::init() { irrecv.enableIRIn(); }

enum class Button {
  num1 = 0xFFA25D,
  num2 = 0xFF629D,
  num3 = 0xFFE21D,
  num4 = 0xFF22DD,
  num5 = 0xFF02FD,
  num6 = 0xFFC23D,
  num7 = 0xFFE01F,
  num8 = 0xFFA857,
  num9 = 0xFF906F,
  num0 = 0xFF9867,
  Ok = 0xFF38C7,
};

static Remote::Strategy getStrategyFromButtonCode(Button button) {
  const std::array<std::pair<Remote::Button, Remote::Strategy>> map{{
      std::make_pair(Button::num1, Strategy::A),
      std::make_pair(Button::num2, Strategy::B),
      std::make_pair(Button::num3, Strategy::C),
      std::make_pair(Button::num4, Strategy::D),
      std::make_pair(Button::num5, Strategy::E),
      std::make_pair(Button::num6, Strategy::F),
      std::make_pair(Button::num7, Strategy::G),
      std::make_pair(Button::num8, Strategy::H),
      std::make_pair(Button::num9, Strategy::I),
  }};
  const auto pIt = std::find_if(map.cbegin(), map.cend(), [](const auto pair) {
    return pair.first == button;
  });
  if (pIt != map.cend()) {
    return pIt->second;
  }
  return Strategy::none;
}

void Remote::blinkLed() {
  digitalWrite(this->feedbackLedPin, LOW);
  delay(100);
  digitalWrite(this->feedbackLedPin, HIGH);
  delay(100);
  digitalWrite(this->feedbackLedPin, LOW);
  delay(100);
}

Remote::Strategies Remote::getStrategies(std::uint32_t timeoutInMs) {

  Strategies strategies;

  enum InputStage {
    pickStartStrategy,
    pickMainStrategy,
  } inputStage = InputStage::pickStartStrategy;

  const unsigned long initialMillis = millis();
  while (millis() - initialMillis < timeoutInMs) {
    decode_results cr;
    if (this->irrecv.decode(&cr) && cr.decode_type == NEC) {
      if (inputStage == InputStage::pickStartStrategy) {
        strategies.start = getStrategyFromButtonCode(cr.value);
        digitalWrite(this->feedbackLedPin,
                     strategies.start == Strategy::none ? LOW : HIGH);
        if (cr.value == Button::Ok) {
          blinkLed();
          inputStage == InputStage::pickMainStrategy;
        }
      } else if (inputStage == InputStage::pickMainStrategy) {
        strategies.main = getStrategyFromButtonCode(cr.value);
        digitalWrite(this->feedbackLedPin,
                     strategies.main == Strategy::none ? LOW : HIGH);
        if (cr.value == Button::Ok) {
          blinkLed();
          break;
        }
      }
      irrecv.resume();
    }
  }
  return strategies;
}

} // namespace SuBot
