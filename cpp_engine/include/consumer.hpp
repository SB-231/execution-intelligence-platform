#pragma once
#include <atomic>

#include "bounded_queue.hpp"
#include "events.hpp"
#include "jsonl_logger.hpp"

void RunConsumer(
    BoundedQueue<QuoteUpdate>& in,
    JsonlLogger& logger,
    std::atomic<bool>& stop_flag
);