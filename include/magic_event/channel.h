//
// Created by jeason on 2021/1/22.
//

#ifndef MAGIC_EVENT_CHANNEL_H
#define MAGIC_EVENT_CHANNEL_H

#include <memory>
#include <functional>

namespace magic_event {
class Channel {
 public:
  using Callback = std::function<void()>;

  class Listener {
   public:
    virtual void OnRead(Channel*) = 0;

    virtual void OnWrite(Channel*) = 0;

    virtual void OnClose(Channel*) = 0;

    virtual void OnError(Channel*, int) = 0;
  };

  Channel(int fd, Listener* listener)
    : fd_(fd),
      listener_(listener) {}

  ~Channel() {}

  int fd() const {return fd_;}

  bool enable_read() const {return enable_read_;}

  bool enable_write() const {return enable_write_;}

  void SetRead(bool read) {enable_read_ = read;}

  void SetWrite(bool write) {enable_write_ = write;}

  void HandleRead() {
    listener_->OnRead(this);
  }

  void HandleWrite() {
    listener_->OnWrite(this);
  }

  void HandleClose() {
    listener_->OnClose(this);
  }

  void HandleError(int error) {
    listener_->OnError(this, error);
  }

 private:
  const int fd_;
  bool enable_read_;
  bool enable_write_;
  Listener* const listener_;
};

using ChannelPtr = std::shared_ptr<Channel>;

}

#endif //MAGIC_EVENT_CHANNEL_H
