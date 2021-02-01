//
// Created by jeason on 2021/1/14.
//

#ifndef MAGIC_EVENT_LOOPER_H
#define MAGIC_EVENT_LOOPER_H

#include "magic_event/timer.h"
#include "magic_event/poller/poller.h"
#include "magic_event/timer_list.h"
#include "magic_event/channel.h"
#include <memory>
#include <iostream>
#include <thread>
#include <sys/epoll.h>
#include <queue>
#include <vector>
#include <mutex>
#include <set>
#include <map>

namespace magic_event {

class Looper {
 public:
  typedef Timer::Functor Functor;

  static std::shared_ptr<Looper> Create() {
    return std::shared_ptr<Looper>(new Looper());
  }

  ~Looper();

  void Run(Functor);

  TimerPtr RunDelay(Functor, time_t);

  TimerPtr RunEvery(Functor, time_t);

  void Cancel(TimerPtr);

  void AddChannel(const Channel&);

  void EditChannel(const Channel&);

  void RemoveChannel(const Channel&);

  bool InLoop();
 private:
  Looper();

  void Loop();

  std::vector<Functor> functors_;
  std::mutex mutex_;
  TimerList timer_list_;
  PollerPtr poller_;
  bool running_;
  std::unique_ptr<std::thread> poll_thread_;
  std::map<int, Channel> channel_map_;
};

using LooperPtr = std::shared_ptr<Looper>;

}

#endif //MAGIC_EVENT_LOOPER_H
