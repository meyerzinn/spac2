#include "ConnectionManager.h"

namespace spac::client::net {
void ConnectionManager::connect(std::string host, std::string port) {
  if (client_) {
    client_->close();
  }
  client_ = WebsocketClient::create(host, port);
}
}  // namespace spac::client::net