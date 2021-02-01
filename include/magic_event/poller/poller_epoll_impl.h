//
// Created by jeason on 2021/1/15.
//

#ifndef MAGIC_EVENT_POLLER_POLLER_EPOLL_IMPL_H
#define MAGIC_EVENT_POLLER_POLLER_EPOLL_IMPL_H

#include "magic_event/poller/poller.h"
#include <memory>
#include <sys/epoll.h>

namespace magic_event {

class PollerEpollImpl : public Poller{
public:
  PollerEpollImpl();

  ~PollerEpollImpl();

  virtual void AddEvent(const PollEvent&) override;

  virtual void EditEvent(const PollEvent&) override;

  virtual void RemoveEvent(const PollEvent&) override;

  virtual PollReason Poll(std::vector<PollEvent>&, time_t) override;

  virtual void Wakeup() override;

  virtual void WakeupAt(const TimePoint);

private:
  int ep_fd_;
  int event_fd_;
  int timer_fd_;
  std::unique_ptr<epoll_event[]> ep_events_;
};

}

#endif //MAGIC_EVENT_POLLER_POLLER_EPOLL_IMPL_H
