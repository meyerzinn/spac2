#pragma once

#include <functional>
#include <boost/lockfree/queue.hpp>

namespace spac::server {
    using QueuedTask = std::function<void()>;
    using TaskQueue = boost::lockfree::queue<QueuedTask *>;
}