//
// Created by jeason on 2021/1/20.
//

#ifndef MAGIC_EVENT_TIMER_LIST_H
#define MAGIC_EVENT_TIMER_LIST_H

#include "magic_event/timer.h"
#include <set>
#include <vector>

namespace magic_event {

class TimerList {
typedef std::pair<TimePoint, TimerPtr> Item;
typedef std::set<Item> List;
public:
  void Insert(TimerPtr);

  void Erase(TimerPtr);

  const std::vector<TimerPtr>& Expired();

  TimerPtr First();

private:
  List list_;
  std::vector<TimerPtr> expired_;
};

}

#endif //MAGIC_EVENT_TIMER_LIST_H
