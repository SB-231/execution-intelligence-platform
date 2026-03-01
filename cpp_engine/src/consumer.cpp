#include "consumer.hpp"
#include "time_utils.hpp"

static void AtomicMax(std::atomic<std::uint64_t>& a, std::uint64_t v) {
    std::uint64_t cur = a.load(std::memory_order_relaxed);
    while (cur < v && !a.compare_exchange_weak(cur, v, std::memory_order_relaxed)) {}
}

void RunConsumer(
    BoundedQueue<QuoteUpdate>& in,
    JsonlLogger& logger,
    std::atomic<bool>& stop_flag,
    ConsumerStats& stats
) {
    (void)stop_flag;

    while (true) {
        auto item = in.Pop();
        if (!item.has_value()) break; // closed + empty

        const auto now = NowNs();
        const auto latency_ns = now - item->ts_recv_ns;
        const std::uint64_t latency_us = latency_ns / 1000;

        stats.events.fetch_add(1, std::memory_order_relaxed);
        stats.latency_us_sum.fetch_add(latency_us, std::memory_order_relaxed);
        AtomicMax(stats.latency_us_max, latency_us);

        logger.Log(item.value());
    }
}