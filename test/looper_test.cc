#include "magic_event/looper.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <condition_variable>

using namespace magic_event;
using namespace std;

constexpr int kReadSize = 4096;
size_t rd_size = 0;
size_t wr_size = 0;

class Listener : public Channel::Listener {
  virtual void OnRead(Channel* channel) {
    char buf[kReadSize] = {0};
    int size = 0;
    while ((size = read(channel->fd(), buf, kReadSize)) > 0) {
      rd_size += size;
    }
  }

  virtual void OnWrite(Channel* channel) {
    cout << "OnWrite!" << endl;
  }

  virtual void OnClose(Channel* channel) {
    cout << "OnClose!" << endl;
  }

  virtual void OnError(Channel* channel, int) {
    cout << "OnError!" << endl;
  }
};


int main() {
  cerr << "looper_test start!" << endl;
  size_t mb = 1024 * 1024;
  LooperPtr looper = Looper::Create();
  Listener listener;
  bool running_ = true;
  int fds[2];
  pipe(fds);
  fcntl(fds[0], F_SETFL, O_NONBLOCK);
  Channel rd_channel(fds[0], &listener);
  rd_channel.SetRead(true);
  looper->AddChannel(rd_channel);

  char str[128];
  while (running_) {
    int size = write(fds[1], str, sizeof str);
    wr_size += size;
    if (size != sizeof(str)) {
      cerr << "wr error!" << endl;
      usleep(100 * 1000);
    }
    if (wr_size % mb == 0) {
      cout << "wr_mb:" << wr_size / mb << ", rd_mb:" << rd_size / mb << endl;
      if (wr_size / mb > 512) {
        running_ = false;
      }
    }
  }
  cout << "looper_test end!" << endl;
  return 0;
}
