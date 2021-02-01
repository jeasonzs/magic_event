//
// Created by jeason on 2021/1/25.
//

#ifndef MAGIC_EVENT_SOCKET_H
#define MAGIC_EVENT_SOCKET_H
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <array>
#include <unistd.h>
#include <fcntl.h>

namespace magic_event {

class Socket {
public:
  Socket()
    : fd_(socket(AF_INET, SOCK_STREAM, 0)) {}

  Socket(const Socket& socket)
    : fd_(socket.fd_) {}

  Socket(int fd)
    : fd_(fd) {}

  int Bind(const std::string& addr, int port, bool reuse = true) {
    in_addr_t in_addr = INADDR_ANY;
    if (!addr.empty()) {
      in_addr = inet_addr(addr.c_str());
    }
    sockaddr_in addr_in {
      .sin_family = AF_INET,
      .sin_port = htons(port),
      .sin_addr = {htonl(in_addr)}
    };
    int opt = reuse;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    return ::bind(fd_, (sockaddr*)(&addr_in), sizeof addr_in);
  }

  int Listen() {
    return ::listen(fd_, 512);
  }

  ssize_t Send(const std::string &message) {
    return ::send(fd_, message.c_str(), message.size(), MSG_DONTWAIT);
  }

  void SetBlock(bool is_block) {
    auto flags = fcntl(fd_, F_GETFL, 0);
    if (is_block) {
      flags &= (~O_NONBLOCK);
    } else {
      flags |= O_NONBLOCK;
    }
    fcntl(fd_, F_SETFL, flags);   //设置成非阻塞模式；
  }

  size_t Recv(char* buffer, size_t size) {
    return ::recv(fd_, buffer, size, MSG_DONTWAIT);
  }

  Socket Accept(std::string& addr, int& port) {
    sockaddr_in addr_in;
    socklen_t size = sizeof addr_in;
    int fd = ::accept(fd_, (sockaddr*)(&addr_in), &size);
    addr = inet_ntoa(addr_in.sin_addr);
    port = ntohs(addr_in.sin_port);
    return Socket(fd);
  }

  int Close() {
    return ::close(fd_);
  }

  operator int() {
    return fd_;
  }

  bool operator < (const Socket& socket) const {
    return this->fd_ < socket.fd_;
  }

private:
  int fd_;
};

}

#endif //MAGIC_EVENT_SOCKET_H
