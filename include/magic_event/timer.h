//
// Created by jeason on 2021/1/19.
//

#ifndef MAGIC_EVENT_TIMER_H
#define MAGIC_EVENT_TIMER_H

#include "magic_event/utils/time_utils.h"
#include <functional>
#include <memory>
#include <chrono>

namespace magic_event {

class Timer {
 public:
  typedef std::function<void()> Functor;

  Timer(TimePoint time_point, milliseconds period, Functor functor)
    : time_point_(time_point),
      period_(period),
      functor_(functor) {}

  TimePoint time_point() const { return time_point_; }

  bool restart() {
    if (period_.count() > 0) {
      time_point_ = steady_clock::now() + period_;
      return true;
    }
    return false;
  }

  Functor functor() const { return functor_; }
 private:
  TimePoint time_point_;
  milliseconds period_;
  Functor functor_;
};

typedef std::shared_ptr<Timer> TimerPtr;

}

#endif //MAGIC_EVENT_TIMER_H
