#include "dummy_feed.hpp"
#include "time_utils.hpp"

#include <chrono>
#include <thread>

void RunDummyFeed(
    const std::vector<std::string>& symbols,
    BoundedQueue<QuoteUpdate>& out,
    std::atomic<bool>& stop_flag
) {
    // Simple deterministic price movement per symbol
    double base = 100.0;

    std::size_t i = 0;
    while (!stop_flag.load(std::memory_order_relaxed)) {
        const std::string& sym = symbols[i % symbols.size()];
        double mid = base + (double)((i % 200) - 100) * 0.001; // small drift
        double bid = mid - 0.005;
        double ask = mid + 0.005;

        QuoteUpdate q;
        q.symbol = sym;
        q.bid_px = bid;
        q.bid_sz = 100 + (int)(i % 400);
        q.ask_px = ask;
        q.ask_sz = 120 + (int)((i * 3) % 400);
        q.ts_recv_ns = NowNs();
        q.ts_proc_ns = q.ts_recv_ns;

        // If queue closes, stop.
        if (!out.Push(std::move(q))) {
            break;
        }

        ++i;
        // Control rate (Week 2: keep simple + stable)
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // ~100 events/sec total
    }
}