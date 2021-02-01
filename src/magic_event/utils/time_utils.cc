//
// Created by jeason on 2021/1/21.
//
#include "magic_event/utils/time_utils.h"
namespace magic_event{

timespec time_point_2_timespec(const TimePoint& time) {
  auto sec = time_point_cast<seconds>(time).time_since_epoch().count();
  auto nano = (time.time_since_epoch() % seconds(1)).count();
  return {sec, nano};
}

}
