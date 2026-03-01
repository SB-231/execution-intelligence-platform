#pragma once
#include <atomic>
#include <string>
#include <vector>

#include "bounded_queue.hpp"
#include "events.hpp"

void RunDummyFeed(
    const std::vector<std::string>& symbols,
    BoundedQueue<QuoteUpdate>& out,
    std::atomic<bool>& stop_flag
);