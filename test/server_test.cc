//
// Created by jeason on 2021/1/29.
//

#include "magic_event/server.h"
#include <iostream>
#include <unistd.h>
#include <set>

using namespace magic_event;
using namespace std;

int main() {
  cout << "server_test start!" << endl;
  LooperPtr looper = Looper::Create();
  Server server(looper, "", 3120);
  server.SetConnectionCallback([&server](ConnectionPtr connection) {
    if (connection->connected()) {
      server.AddConnection(connection);
    } else {
      server.RemoveConnection(connection);
    }
  });
  server.Start();

  while (true) {
    sleep(1);
  }

  cout << "server_test end!" << endl;
  return 0;
}