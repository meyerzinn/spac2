#include "Session.h"
#include "WebsocketSession.h"
#include <utility>

namespace spac::server::component {

SessionComponent::SessionComponent(std::shared_ptr<spac::server::net::WebsocketSession> session) : conn(std::move(session)) {}

}  // namespace spac::server::component