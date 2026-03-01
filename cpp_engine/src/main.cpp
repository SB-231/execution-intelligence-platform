#include <iostream>
#include <string>

#include "config.hpp"
#include "events.hpp"
#include "jsonl_logger.hpp"
#include "session.hpp"
#include "time_utils.hpp"

static constexpr const char* kEngineVersion = "0.2.0";

int main(int argc, char** argv) {
    std::string config_path = "configs/dev.yaml";
    if (argc >= 2) {
        config_path = argv[1];
    }

    std::cout << "Execution Intelligence Engine\n";
    std::cout << "Version: " << kEngineVersion << "\n";

    try {
        EngineConfig cfg = LoadConfigOrThrow(config_path);

        std::cout << "Run mode: " << cfg.run_mode << "\n";
        std::cout << "Log dir: " << cfg.log_dir << "\n";
        std::cout << "Symbols:\n";
        for (const auto& s : cfg.symbols) {
            std::cout << "  - " << s << "\n";
        }

        const std::string session_dir = MakeSessionDir(cfg.log_dir);
        std::cout << "Session dir: " << session_dir << "\n";

        JsonlLogger logger(session_dir);

        // Day 1: write a few dummy events to validate schema & logger
        for (const auto& sym : cfg.symbols) {
            QuoteUpdate q;
            q.symbol = sym;
            q.bid_px = 100.0;
            q.bid_sz = 500;
            q.ask_px = 100.01;
            q.ask_sz = 600;
            q.ts_recv_ns = NowNs();
            q.ts_proc_ns = NowNs();
            logger.Log(q);
        }

        EngineMetric m;
        m.name = "startup_ok";
        m.value = 1.0;
        m.ts_ns = NowNs();
        logger.Log(m);

        std::cout << "Wrote dummy events.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Config error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}