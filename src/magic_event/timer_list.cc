//
// Created by jeason on 2021/1/20.
//
#include "magic_event/timer_list.h"
#include <iostream>

namespace magic_event {

void TimerList::Insert(TimerPtr timer) {
  list_.insert(std::make_pair(timer->time_point(), timer));
}

void TimerList::Erase(TimerPtr timer) {
  auto item = std::make_pair(timer->time_point(), timer);
  list_.erase(item);
}

const std::vector<TimerPtr>& TimerList::Expired() {
  auto now = std::make_pair(steady_clock::now(), nullptr);
  auto end = list_.lower_bound(now);
  expired_.clear();
  for (auto iter = list_.begin(); iter != end; iter++) {
    expired_.push_back(iter->second);
  }
  list_.erase(list_.begin(), end);
  return expired_;
}

TimerPtr TimerList::First() {
  if (list_.empty()) {
    return nullptr;
  }
  return list_.begin()->second;
}

};
