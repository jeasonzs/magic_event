//
// Created by jeason on 2021/1/15.
//

#ifndef MAGIC_EVENT_POLLER_POLLER_H
#define MAGIC_EVENT_POLLER_POLLER_H

#include "magic_event/utils/time_utils.h"
#include "magic_event/poller/poll_event.h"
#include <bitset>
#include <memory>
#include <vector>
#include <chrono>

namespace magic_event {

class PollReason {
 public:
  enum ReasonType {
    kReasonWakeup = 0,
    kReasonWakeupAt,
    kReasonIo
  };
  typedef std::bitset<3> ReasonSet;

  PollReason(const ReasonSet& reason_set,
             int event_size)
    : reason_set_(reason_set),
      event_size_(event_size) {}

  const ReasonSet& reason_set() {return reason_set_;}

  int event_size() {return event_size_;}
 private:
  ReasonSet reason_set_;
  int event_size_;
};

class Poller {
public:
  virtual ~Poller() {}

  virtual void AddEvent(const PollEvent&) = 0;

  virtual void EditEvent(const PollEvent&) = 0;

  virtual void RemoveEvent(const PollEvent&) = 0;

  virtual PollReason Poll(std::vector<PollEvent>&, time_t) = 0;

  virtual void Wakeup() = 0;

  virtual void WakeupAt(const TimePoint) = 0;
};

typedef std::shared_ptr<Poller> PollerPtr;
PollerPtr create_poller();

}

#endif //MAGIC_EVENT_POLLER_POLLER_H
