#include "magic_event/looper.h"
#include "magic_event/acceptor.h"
#include "magic_event/connection.h"
#include <iostream>
#include <unistd.h>
#include <set>

using namespace magic_event;
using namespace std;

int main() {
  cout << "acceptor_test start!" << endl;
  LooperPtr looper = Looper::Create();
  LooperPtr io_looper = Looper::Create();

  ConnectionPtr connection;
  auto recv_callback = [] (Connection *connection, const std::string &msg) {
    cout << "recv_callback:" << msg << endl;
    connection->Send("You input is:" + msg);
  };

  Acceptor acceptor(looper);
  acceptor.SetCallback([&](Socket& socket, const std::string& addr, int port) {
    cout << "new socket accepted fd:" << socket << ", addr:" << addr << ":" << port << endl;
    connection = std::make_shared<Connection>(io_looper, socket);
    connection->SetReceiveCallback(recv_callback);
    connection->Send("hello this is server!");
  });
  acceptor.Listen("", 3120, true);

  while (true) {
    sleep(1);
  }

  cout << "acceptor_test end!" << endl;
  return 0;
}
