#pragma once

#include <uwebsockets/App.h>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>
#include <unordered_set>

namespace spac::server::component {

    template<bool SSL>
    struct NetClient {
        uWS::WebSocket<SSL, true> *conn;
        std::unordered_set<entt::entity> knownEntities;
    };

}


