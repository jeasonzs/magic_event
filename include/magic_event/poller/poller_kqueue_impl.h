//
// Created by jeason on 2021/1/15.
//

#ifndef MAGIC_EVENT_POLLER_POLLER_KQUEUE_IMPL_H
#define MAGIC_EVENT_POLLER_POLLER_KQUEUE_IMPL_H
#include "poller.h"

namespace magic_event {

class PollerKqueueImpl : public Poller{
public:
  PollerKqueueImpl();

  ~PollerKqueueImpl();

  virtual void AddEvent(const PollEvent&) override;

  virtual void EditEvent(const PollEvent&) override;

  virtual void RemoveEvent(const PollEvent&) override;

  virtual size_t Poll(std::vector<PollEvent>&, size_t, time_t) override;
 private:
  int kq_;
};

}
#endif //MAGIC_EVENT_POLLER_KQUEUE_IMPL_H
