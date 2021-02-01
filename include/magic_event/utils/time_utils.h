//
// Created by jeason on 2021/1/20.
//

#ifndef MAGIC_EVENT_UTILS_TIME_UTILS_H
#define MAGIC_EVENT_UTILS_TIME_UTILS_H

#include <time.h>
#include <chrono>

namespace magic_event {

using std::chrono::steady_clock;
using TimePoint = steady_clock::time_point;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::time_point_cast;

timespec time_point_2_timespec(const TimePoint&);

}

#endif //MAGIC_EVENT_UTILS_TIME_UTILS_H
