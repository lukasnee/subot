#pragma once

/**
 * @brief Software timer for writing non-blocking "super-loop" code.
 * @author Lukas Neverauskis <lukas.neverauskis@gmail.com>
 * @link https://github.com/lukasnee
 */

namespace SuBot {

class Timer {
public:
  enum class Mode {
    normal,     /* when time elapses, `isTriggered()` always returns true. User
                   shall `reset()` manually whenever needed. */
    singleShot, /* when time elapses, `isTriggered()` returns true only once.
                   User shall `reset()` manually whenever needed. */
    autoReset /* when time elapses, `isTriggered()` returns true, and resets. */
  };
  Timer(Mode mode, unsigned long duration) : mode(mode), duration(duration) {
    this->reset();
  }

  bool isTriggered() {
    if (this->durationHasElapsed()) {
      if (this->mode == Mode::normal) {
        return true;
      } else if (this->mode == Mode::singleShot) {
        if (!this->triggered) {
          this->triggered = true;
          return true;
        }
      } else if (this->mode == Mode::autoReset) {
        this->reset();
        return true;
      }
    }
    return false;
  }
  void reset() {
    this->startTime = millis();
    this->triggered = false;
  }

private:
  bool durationHasElapsed() {
    unsigned long millisElapsed = millis() - this->startTime;
    return millisElapsed > this->duration;
  }

  Mode mode = Mode::normal;
  bool triggered = false;
  unsigned long duration = 0;
  unsigned long startTime = 0;
};

} // namespace SuBot