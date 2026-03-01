#pragma once
#include <atomic>
#include <cstdint>

#include "bounded_queue.hpp"
#include "events.hpp"
#include "jsonl_logger.hpp"

struct ConsumerStats {
    std::atomic<std::uint64_t> events{0};
    std::atomic<std::uint64_t> latency_us_sum{0};
    std::atomic<std::uint64_t> latency_us_max{0};
};

void RunConsumer(
    BoundedQueue<QuoteUpdate>& in,
    JsonlLogger& logger,
    std::atomic<bool>& stop_flag,
    ConsumerStats& stats
);