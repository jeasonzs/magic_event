//
// Created by jeason on 2021/1/14.
//

#include "magic_event/looper.h"
#include "magic_event/utils/time_utils.h"
#include <memory>
#include <sys/socket.h>

namespace magic_event {

using EventType = PollEvent::EventType;

PollEvent::EventSet channel2event_set(const Channel& channel) {
  PollEvent::EventSet set;
  set.set(EventType::kEventRead, channel.enable_read());
  set.set(EventType::kEventWrite, channel.enable_write());
  set.set(EventType::kEventClose);
  set.set(EventType::kEventError);
  return set;
}

Looper::Looper()
  : poller_(create_poller()),
    running_(true) {
  poll_thread_ = std::make_unique<std::thread>([this] { Loop(); });
}

Looper::~Looper() {
  running_ = false;
  poller_->Wakeup();
  if (poll_thread_->joinable()) {
    poll_thread_->join();
  }
}

void Looper::Run(Functor functor) {
  if (InLoop()) {
    functor();
  } else {
    std::unique_lock<std::mutex> lock(mutex_);
    functors_.push_back(functor);
    lock.unlock();
    poller_->Wakeup();
  }
}

TimerPtr Looper::RunDelay(Functor functor, time_t delay) {
  TimePoint time = steady_clock::now() + milliseconds(delay);
  TimerPtr timer = std::make_shared<Timer>(time, milliseconds(0), functor);
  Run([this, timer] {
    timer_list_.Insert(timer);
    poller_->WakeupAt(timer_list_.First()->time_point());
  });
  return timer;
}

TimerPtr Looper::RunEvery(Functor functor, time_t period) {
  auto _period = milliseconds(period);
  TimePoint time = steady_clock::now() + _period;
  TimerPtr timer = std::make_shared<Timer>(time, _period, functor);
  Run([this, timer] {
    timer_list_.Insert(timer);
    poller_->WakeupAt(timer_list_.First()->time_point());
  });
  return timer;
}

void Looper::Cancel(TimerPtr timer) {
  Run([this, timer] {
    timer_list_.Erase(timer);
    if (auto first = timer_list_.First()) {
      poller_->WakeupAt(first->time_point());
    }
  });
}


void Looper::Loop() {
  std::vector<PollEvent> events;
  while (running_) {
    events.clear();
    auto reason = poller_->Poll(events, 100 * 1000);
#ifdef DEBUG
    std::cout << this << " reason: " << reason.reason_set() << std::endl;
#endif
    if (reason.reason_set().test(
        PollReason::ReasonType::kReasonWakeupAt)) {
      auto expired = timer_list_.Expired();
      for (auto& timer : expired) {
        timer->functor()();
        if (timer->restart()) {
          timer_list_.Insert(timer);
        }
      }
      if (auto first = timer_list_.First()) {
        poller_->WakeupAt(first->time_point());
      }
    }
    if (reason.reason_set().test(
        PollReason::ReasonType::kReasonIo)) {
      for (auto event : events) {
        auto iter = channel_map_.find(event.fd());
        if (iter == channel_map_.end()) {
          continue;
        }
        if (event.event_set().test(EventType::kEventRead)) {
          iter->second.HandleRead();
        }
        if (event.event_set().test(EventType::kEventWrite)) {
          iter->second.HandleWrite();
        }
        if (event.event_set().test(EventType::kEventClose)
            && !event.event_set().test(EventType::kEventRead)) {
          iter->second.HandleClose();
        }
        if (event.event_set().test(EventType::kEventError)) {
          int opt_val;
          auto opt_len = static_cast<socklen_t>(sizeof opt_val);
          if (getsockopt(iter->second.fd(), SOL_SOCKET, SO_ERROR, &opt_val, &opt_len) < 0) {
            iter->second.HandleError(errno);
          } else {
            iter->second.HandleError(opt_val);
          }
        }
      }
    }

    std::vector<Functor> functors;
    std::unique_lock<std::mutex> lock(mutex_);
    std::swap(functors, functors_);
    lock.unlock();
    for (auto functor : functors) {
      functor();
    }
  }
}

void Looper::AddChannel(const Channel& channel) {
  Run([this, channel] {
    channel_map_.insert(std::make_pair(channel.fd(), channel));
    PollEvent event(channel.fd(), channel2event_set(channel));
    poller_->AddEvent(event);
  });
}

void Looper::EditChannel(const Channel& channel) {
  Run([this, channel] {
    auto iter = channel_map_.find(channel.fd());
    if (iter != channel_map_.end()) {
      PollEvent event(channel.fd(), channel2event_set(channel));
      poller_->EditEvent(event);
    }
  });
}

void Looper::RemoveChannel(const Channel& channel) {
  Run([this, channel] {
    auto iter = channel_map_.find(channel.fd());
    if (iter != channel_map_.end()) {
      PollEvent event(channel.fd(), channel2event_set(channel));
      poller_->RemoveEvent(event);
      channel_map_.erase(iter);
    }
  });
}

bool Looper::InLoop() {
  return std::this_thread::get_id() == poll_thread_->get_id();
}

}
