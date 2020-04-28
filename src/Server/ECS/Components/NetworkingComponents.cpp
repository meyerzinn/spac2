#include "NetworkingComponents.h"

namespace spac::server::component {
template <bool SSL>
NetClient<SSL>::NetClient(uWS::WebSocket<SSL, true> *ws) : conn(ws) {}

template <bool SSL>
void NetClient<SSL>::send(uWS::Loop *loop, std::string_view buffer) {
  loop->defer([this, buffer]() {
    if (!closed.get()) this->conn->send(buffer);
  });
}
}  // namespace spac::server::component