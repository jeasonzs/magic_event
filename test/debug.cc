#include <iostream>
#include "magic_event/acceptor.h"
#include "magic_event/looper.h"
#include "magic_event/server.h"
#include <unistd.h>
#include <map>

using namespace magic_event;

int main() {
  bool running = true;
  int count = 0;
  std::cout << "debug start!" << std::endl;

  LooperPtr looper;




  std::cout << "debug end!" << std::endl;
  return 0;
}
