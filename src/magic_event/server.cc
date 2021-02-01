//
// Created by jeason on 2021/1/28.
//

#include "magic_event/server.h"
#include "magic_event/looper.h"

namespace magic_event {

void default_connection_callback(const ConnectionPtr& connection) {
  std::cerr << "default_connection_callback" <<
            ", socket:" << connection->socket().operator int() << std::endl;
}

void default_msg_callback(const ConnectionPtr& connection, const std::string& msg) {
  std::cerr << "default_msg_callback" <<
            ", socket:" << connection->socket().operator int() <<
            ", msg_size:" << msg.size() << std::endl;
}

Server::Server(const LooperPtr& looper,
               const std::string& addr,
               int port,
               bool reuse,
               int worker_size)
  : main_looper_(looper),
    addr_(addr),
    port_(port),
    reuse_(reuse),
    worker_size_(worker_size),
    worker_index_(0),
    connection_callback_(default_connection_callback),
    msg_callback_(default_msg_callback),
    acceptor_(main_looper_) {
  acceptor_.SetCallback(
      [this](Socket& socket, const std::string& addr, int port) {
        OnConnection(socket, addr, port);
  });
  for (int i = 0; i < worker_size_; i++) {
    worker_looper_.emplace_back(Looper::Create());
  }
}

void Server::SetConnectionCallback(ConnectionCallback connection_callback) {
  connection_callback_ = connection_callback;
}

void Server::SetMsgCallback(MsgCallback msg_callback) {
  msg_callback_ = msg_callback;
}

void Server::Start() {
  acceptor_.Listen(addr_, port_, reuse_);
}

LooperPtr& Server::NextWorkerLooper() {
  auto index = worker_index_++;
  worker_index_ %= worker_size_;
  return worker_looper_[index];
}

void Server::AddConnection(const ConnectionPtr &connection) {
  connection->SetReceiveCallback(
      [this, connection](Connection *, const std::string &msg){
        msg_callback_(connection, msg);
  });
  connection->SetCloseCallback(
      [this, connection](Connection *) {
        connection_callback_(connection);
  });
  connection_map_[connection->id()] = connection;
}

void Server::RemoveConnection(const ConnectionPtr &connection) {
  auto iter = connection_map_.find(connection->id());
  if (iter != connection_map_.end()) {
    connection_map_.erase(iter);
  }
}

void Server::OnConnection(Socket &socket, const std::string &addr, int port) {
  ConnectionPtr connection = std::make_shared<Connection>(NextWorkerLooper(), socket);
  connection_callback_(connection);
}

}
