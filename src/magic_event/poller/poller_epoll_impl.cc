//
// Created by jeason on 2021/1/15.
//
#include "magic_event/poller/poller_epoll_impl.h"
#include "magic_event/utils/time_utils.h"
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

namespace magic_event {

constexpr int kEventSize = 128;
using EventType = PollEvent::EventType;

epoll_event poll_event_2_ep_event(const PollEvent& event) {
  uint32_t events = EPOLLET;
  if (event.event_set().test(EventType::kEventRead)) {
    events |= EPOLLIN;
  }
  if (event.event_set().test(EventType::kEventWrite)) {
    events |= EPOLLOUT;
  }
  if (event.event_set().test(EventType::kEventClose)) {
    events |= EPOLLHUP;
  }
  if (event.event_set().test(EventType::kEventError)) {
    events |= EPOLLERR;
  }
  epoll_event ep_event = {
      events,
      {.fd = event.fd()}
  };
  return ep_event;
}

PollEvent ep_event_2_poll_event(const epoll_event& event) {
  PollEvent::EventSet set;
  set.set(EventType::kEventRead, event.events & EPOLLIN);
  set.set(EventType::kEventWrite, event.events & EPOLLOUT);
  set.set(EventType::kEventClose, event.events & EPOLLHUP);
  set.set(EventType::kEventError, event.events & EPOLLERR);
  return PollEvent(event.data.fd, set);
}

PollerPtr create_poller() {
  return std::make_shared<PollerEpollImpl>();
}

PollerEpollImpl::PollerEpollImpl()
    : ep_fd_(epoll_create(1024)),
      event_fd_(eventfd(0, 0)),
      timer_fd_(timerfd_create(CLOCK_MONOTONIC, 0)),
      ep_events_(new epoll_event[kEventSize]) {
  epoll_event wakeup_event = {
      EPOLLIN | EPOLLET,
      {.fd = event_fd_}
  };
  epoll_event timer_event = {
      EPOLLIN | EPOLLET,
      {.fd = timer_fd_}
  };
  epoll_ctl(ep_fd_, EPOLL_CTL_ADD, event_fd_, &wakeup_event);
  epoll_ctl(ep_fd_, EPOLL_CTL_ADD, timer_fd_, &timer_event);
}

PollerEpollImpl::~PollerEpollImpl() {
  close(timer_fd_);
  close(event_fd_);
  close(ep_fd_);
}

void PollerEpollImpl::AddEvent(const PollEvent& event) {
  auto ep_event = poll_event_2_ep_event(event);
  epoll_ctl(ep_fd_, EPOLL_CTL_ADD, event.fd(), &ep_event);
}

void PollerEpollImpl::EditEvent(const PollEvent& event) {
  auto ep_event = poll_event_2_ep_event(event);
  epoll_ctl(ep_fd_, EPOLL_CTL_MOD, event.fd(), &ep_event);
}

void PollerEpollImpl::RemoveEvent(const PollEvent& event) {
  epoll_ctl(ep_fd_, EPOLL_CTL_DEL, event.fd(), nullptr);
}

PollReason PollerEpollImpl::Poll(std::vector<PollEvent>& events, time_t timeout) {
  PollReason::ReasonSet reason_set;
  auto size = epoll_wait(ep_fd_, ep_events_.get(), kEventSize, 100 * 1000);
  auto io_size = size;
  for (int i = 0; i < size; i++) {
    auto& event = ep_events_[i];
    auto fd = event.data.fd;
    if (fd == event_fd_) {
      eventfd_t value;
      eventfd_read(fd, &value);
      reason_set.set(PollReason::ReasonType::kReasonWakeup);
      io_size -= 1;
    } else if (fd == timer_fd_) {
      uint64_t exp = 0;
      read(fd, &exp, sizeof(uint64_t));
      reason_set.set(PollReason::ReasonType::kReasonWakeupAt);
      io_size -= 1;
    } else {
      events.push_back(ep_event_2_poll_event(event));
    }
  }
  if (io_size > 0) {
    reason_set.set(PollReason::ReasonType::kReasonIo);
  }
  return PollReason(reason_set, io_size);
}

void PollerEpollImpl::Wakeup() {
  eventfd_write(event_fd_, 100);
}

void PollerEpollImpl::WakeupAt(const TimePoint tp) {
  itimerspec new_value {
      .it_interval = {0},
      .it_value = time_point_2_timespec(tp)
  };
  itimerspec old_value{0};
  timerfd_settime(timer_fd_, 1, &new_value, &old_value);
}

}
