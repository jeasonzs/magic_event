//
// Created by jeason on 2021/1/25.
//
#include "magic_event/connection.h"
#include <sys/socket.h>
#include <fcntl.h>

namespace magic_event {

constexpr size_t kMaxSendBufSize = 8 * 1024 * 1024;

void default_recv_callback(Connection *, const std::string &data) {
  std::cerr << "default_recv_callback called!!" << std::endl;
}
void default_close_callback(Connection* connection) {
  std::cerr << "default_close_callback called!!" << connection << std::endl;
}

volatile std::atomic<int> Connection::counter(0);

Connection::Connection(LooperPtr looper, Socket& socket)
  : id_(Connection::counter.fetch_add(1)),
    looper_(looper),
    socket_(socket),
    channel_(socket_, this),
    receive_callback_(default_recv_callback),
    close_callback_(default_close_callback),
    connected_(true),
    send_buf_size_(0) {
  socket_.SetBlock(false);
  channel_.SetRead(true);
  looper_->AddChannel(channel_);
}

Connection::~Connection() {
  Close();
}

int Connection::Send(const std::string &data) {
  if (!connected_) {
    return -1;
  }
  looper_->Run([this, data]{
    if (send_buf_size_ > kMaxSendBufSize) {
      std::cerr << "The send buffer size:" << send_buf_size_
          << ", over than:" << kMaxSendBufSize;
      return;
    }
    if (!send_queue_.empty()) {
      send_queue_.push_back(data);
      send_buf_size_ += data.size();
    } else {
      ssize_t size = socket_.Send(data);
      if (size != data.size()) {
        std::string left(data, size, data.size() - size);
        send_queue_.push_back(left);
        send_buf_size_ += left.size();
      }
    }
    if (!send_queue_.empty()) {
      channel_.SetWrite(true);
      looper_->EditChannel(channel_);
    }
  });
  return data.size();
}

void Connection::Close() {
  looper_->Run([this] {
    if (connected_) {
      connected_ = false;
      looper_->RemoveChannel(channel_);
      socket_.Close();
    }
  });
}

void Connection::OnRead(Channel*) {
  std::string data;
  ssize_t size = 0;
  char buf[8192];
  while ((size = socket_.Recv(buf, 8192)) > 0) {
    data.append(buf, size);
  }
  if (!data.empty()) {
    receive_callback_(this, data);
  } else {
    Close();
    close_callback_(this);
  }
}

void Connection::OnWrite(Channel*) {
  while (!send_queue_.empty()) {
    auto data = send_queue_.front();
    send_queue_.pop_front();
    auto size = socket_.Send(data);
    send_buf_size_ -= size;
    if (size != data.size()) {
      std::string left(data, size, data.size() - size);
      send_queue_.push_front(left);
      break;
    }
  }
  if (send_queue_.empty()) {
    channel_.SetWrite(false);
    looper_->EditChannel(channel_);
  }
}

void Connection::OnClose(Channel*) {
  Close();
  close_callback_(this);
}

void Connection::OnError(Channel*, int) {

}

}
