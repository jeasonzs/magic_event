//
// Created by jeason on 2021/1/15.
//
#include "magic_event/poller/poller_kqueue_impl.h"
#include <sys/event.h>

namespace magic_event {

PollerPtr create_poller() {
  return std::make_shared<PollerKqueueImpl>();
}

PollerKqueueImpl::PollerKqueueImpl()
  : kq_(kqueue()) {}

PollerKqueueImpl::~PollerKqueueImpl() {}

void PollerKqueueImpl::AddEvent(const PollEvent& event) {
  struct kevent evt;
  EV_SET(&evt, event.fd(), EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

}

void PollerKqueueImpl::EditEvent(const PollEvent& event) {

}
void PollerKqueueImpl::RemoveEvent(const PollEvent& event) {

}

size_t PollerKqueueImpl::Poll(std::vector<PollEvent>& events, size_t size, time_t timeout) {
  return 0;
}

}
