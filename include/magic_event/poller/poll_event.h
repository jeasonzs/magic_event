//
// Created by jeason on 2021/1/21.
//

#ifndef MAGIC_EVENT_POLLER_POLL_EVENT_H
#define MAGIC_EVENT_POLLER_POLL_EVENT_H

#include <bitset>
#include <memory>

namespace magic_event {

class PollEvent {
 public:
  enum EventType {
    kEventRead = 0,
    kEventWrite,
    kEventError,
    kEventClose
  };
  typedef std::bitset<5> EventSet;

  PollEvent(int fd, EventSet event_set)
      : fd_(fd),
        event_set_(event_set) {}

  const EventSet &event_set() const { return event_set_; }

  void even_set(const EventSet &event_set) { event_set_ = event_set; }

  int fd() const { return fd_; }

 private:
  const int fd_;
  EventSet event_set_;
};
}

#endif //MAGIC_EVENT_POLLER_POLL_EVENT_H
