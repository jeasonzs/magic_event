//
// Created by jeason on 2021/1/28.
//

#ifndef MAGIC_EVENT_SERVER_H
#define MAGIC_EVENT_SERVER_H

#include "magic_event/looper.h"
#include "magic_event/connection.h"
#include "magic_event/acceptor.h"
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace magic_event {

using std::vector;

class Server {
 public:
  using ConnectionCallback = std::function<void(const ConnectionPtr& connection)>;
  using MsgCallback = std::function<void(const ConnectionPtr& connection, const std::string& msg)>;

  Server(const LooperPtr& looper,
         const std::string& addr,
         int port,
         bool reuse_port = true,
         int worker_size = 4);

  void SetConnectionCallback(ConnectionCallback connection_callback);

  void SetMsgCallback(MsgCallback msg_callback);

  void Start();

  void AddConnection(const ConnectionPtr& connection);

  void RemoveConnection(const ConnectionPtr& connection);

 private:
  LooperPtr& NextWorkerLooper();

  void OnConnection(Socket& socket, const std::string& addr, int port);

  LooperPtr main_looper_;
  std::string addr_;
  int port_;
  bool reuse_;
  int worker_size_;
  int worker_index_;
  ConnectionCallback connection_callback_;
  MsgCallback msg_callback_;
  Acceptor acceptor_;
  vector<LooperPtr> worker_looper_;
  std::map<int, ConnectionPtr> connection_map_;
};

}

#endif //MAGIC_EVENT_SERVER_H
