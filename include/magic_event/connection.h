//
// Created by jeason on 2021/1/24.
//

#ifndef MAGIC_EVENT_CONNECTION_H
#define MAGIC_EVENT_CONNECTION_H

#include "magic_event/socket.h"
#include "magic_event/looper.h"
#include "magic_event/channel.h"
#include <string>
#include <functional>
#include <atomic>

namespace magic_event {

class Connection : public Channel::Listener {
 public:
  using ReceiveCallback = std::function<void(Connection *, const std::string &)>;

  using CloseCallback = std::function<void(Connection *)>;

  Connection(LooperPtr, Socket&);

  virtual ~Connection();

  int id() {return id_;}

  LooperPtr looper() {return looper_;}

  Socket& socket() {return socket_;}

  void SetReceiveCallback(ReceiveCallback receiveCallback) {
    receive_callback_ = receiveCallback;
  }

  void SetCloseCallback(CloseCallback closeCallback) {
    close_callback_ = closeCallback;
  }

  int Send(const std::string&);

  bool connected() {return connected_;}

  void Close();

 private:
  virtual void OnRead(Channel*) override;

  virtual void OnWrite(Channel*) override;

  virtual void OnClose(Channel*) override;

  virtual void OnError(Channel*, int) override;

  static volatile std::atomic<int> counter;
  int id_;
  LooperPtr looper_;
  Socket socket_;
  Channel channel_;
  std::deque<std::string> send_queue_;
  ReceiveCallback receive_callback_;
  CloseCallback close_callback_;
  bool connected_;
  volatile size_t send_buf_size_;
};

 using ConnectionPtr = std::shared_ptr<Connection>;

}

#endif //MAGIC_EVENT_CONNECTION_H
