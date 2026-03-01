#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "bounded_queue.hpp"
#include "config.hpp"
#include "consumer.hpp"
#include "dummy_feed.hpp"
#include "events.hpp"
#include "jsonl_logger.hpp"
#include "session.hpp"
#include "time_utils.hpp"

static constexpr const char* kEngineVersion = "0.3.0";

int main(int argc, char** argv) {
    std::string config_path = "configs/dev.yaml";
    if (argc >= 2) config_path = argv[1];

    std::cout << "Execution Intelligence Engine\n";
    std::cout << "Version: " << kEngineVersion << "\n";

    try {
        EngineConfig cfg = LoadConfigOrThrow(config_path);

        std::cout << "Run mode: " << cfg.run_mode << "\n";
        std::cout << "Log dir: " << cfg.log_dir << "\n";
        std::cout << "Symbols:\n";
        for (const auto& s : cfg.symbols) std::cout << "  - " << s << "\n";

        const std::string session_dir = MakeSessionDir(cfg.log_dir);
        std::cout << "Session dir: " << session_dir << "\n";

        JsonlLogger logger(session_dir);

        // Week 2 Day 2: internal pipeline
        BoundedQueue<QuoteUpdate> q(1024);
        std::atomic<bool> stop{false};

        // Week 2 Day 3: baseline metrics
        ConsumerStats stats;
        std::atomic<std::uint64_t> queue_depth_max{0};

        auto AtomicMaxQ = [&](std::uint64_t v) {
            std::uint64_t cur = queue_depth_max.load(std::memory_order_relaxed);
            while (cur < v &&
                   !queue_depth_max.compare_exchange_weak(cur, v, std::memory_order_relaxed)) {}
        };

        std::thread producer([&] { RunDummyFeed(cfg.symbols, q, stop); });
        std::thread consumer([&] { RunConsumer(q, logger, stop, stats); });

        std::thread metrics([&] {
            using namespace std::chrono;

            std::uint64_t last_events = 0;

            while (!stop.load(std::memory_order_relaxed)) {
                std::this_thread::sleep_for(seconds(1));

                // Queue high-water mark
                AtomicMaxQ(q.Size());

                const std::uint64_t ev = stats.events.load(std::memory_order_relaxed);
                const std::uint64_t delta = ev - last_events;
                last_events = ev;

                const std::uint64_t sum = stats.latency_us_sum.load(std::memory_order_relaxed);
                const std::uint64_t max_us = stats.latency_us_max.load(std::memory_order_relaxed);
                const double avg_us = (ev > 0) ? (double)sum / (double)ev : 0.0;

                logger.Log(EngineMetric{"events_per_sec", (double)delta, NowNs()});
                logger.Log(EngineMetric{"latency_us_avg", avg_us, NowNs()});
                logger.Log(EngineMetric{"latency_us_max", (double)max_us, NowNs()});
                logger.Log(EngineMetric{
                    "queue_depth_max",
                    (double)queue_depth_max.load(std::memory_order_relaxed),
                    NowNs()
                });
            }
        });

        // Run for a fixed duration (simple MVP control)
        constexpr int kRunSeconds = 10;
        std::cout << "Running dummy pipeline for " << kRunSeconds << " seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(kRunSeconds));

        // Shutdown
        stop.store(true, std::memory_order_relaxed);
        q.Close();

        producer.join();
        consumer.join();
        metrics.join();

        logger.Log(EngineMetric{"shutdown_ok", 1.0, NowNs()});

        std::cout << "Shutdown complete.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}