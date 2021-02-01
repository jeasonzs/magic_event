//
// Created by jeason on 2021/1/25.
//
#include "magic_event/acceptor.h"
#include <iostream>

namespace magic_event {

void default_connection_callback(Socket& socket, const std::string& addr, int port) {
  std::cerr << "default_connection_callback" <<
      ", socket:" << socket.operator int() <<
      ", addr:" << addr <<
      ", port:" << port << std::endl;
}

Acceptor::Acceptor(const LooperPtr &looper)
  : looper_(looper),
    socket_(),
    channel_(socket_, this),
    callback_(default_connection_callback) {}

Acceptor::~Acceptor() {
  looper_->RemoveChannel(channel_);
  socket_.Close();
}

void Acceptor::Listen(const std::string& addr, int port, bool reuse) {
  socket_.Bind(addr, port);
  socket_.Listen();
  channel_.SetRead(true);
  looper_->AddChannel(channel_);
}

void Acceptor::OnRead(Channel*) {
  std::string addr;
  int port;
  auto socket = socket_.Accept(addr, port);
  callback_(socket, addr, port);
}

void Acceptor::OnWrite(Channel*) {

}

void Acceptor::OnClose(Channel*) {

}

void Acceptor::OnError(Channel*, int) {

}

}
