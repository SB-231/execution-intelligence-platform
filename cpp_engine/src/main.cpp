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

static constexpr const char* kEngineVersion = "0.2.1";

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

        std::thread producer([&] { RunDummyFeed(cfg.symbols, q, stop); });
        std::thread consumer([&] { RunConsumer(q, logger, stop); });

        // Run for a fixed duration (simple MVP control)
        constexpr int kRunSeconds = 10;
        std::cout << "Running dummy pipeline for " << kRunSeconds << " seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(kRunSeconds));

        // Shutdown
        stop.store(true, std::memory_order_relaxed);
        q.Close();

        producer.join();
        consumer.join();

        EngineMetric m;
        m.name = "shutdown_ok";
        m.value = 1.0;
        m.ts_ns = NowNs();
        logger.Log(m);

        std::cout << "Shutdown complete.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}