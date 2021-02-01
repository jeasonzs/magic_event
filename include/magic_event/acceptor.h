//
// Created by jeason on 2021/1/25.
//

#ifndef MAGIC_EVENT_ACCEPTOR_H
#define MAGIC_EVENT_ACCEPTOR_H

#include "magic_event/socket.h"
#include "magic_event/channel.h"
#include "magic_event/looper.h"
#include <vector>
#include <functional>

namespace magic_event {

class Acceptor : public Channel::Listener {
public:
  using Callback = std::function<void(Socket& socket, const std::string& addr, int port)>;

  Acceptor(const LooperPtr& looper);

  virtual ~Acceptor();

  void SetCallback(Callback callback) {
    callback_ = callback;
  }

  void Listen(const std::string& addr, int port, bool reuse);

private:
  virtual void OnRead(Channel*);

  virtual void OnWrite(Channel*);

  virtual void OnClose(Channel*);

  virtual void OnError(Channel*, int);

  LooperPtr looper_;
  Socket socket_;
  Channel channel_;
  Callback callback_;
};
}

#endif //MAGIC_EVENT_ACCEPTOR_H
