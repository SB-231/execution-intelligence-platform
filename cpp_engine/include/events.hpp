#pragma once
#include <cstdint>
#include <string>

struct QuoteUpdate {
    std::string symbol;
    double bid_px = 0.0;
    int bid_sz = 0;
    double ask_px = 0.0;
    int ask_sz = 0;
    std::uint64_t ts_recv_ns = 0;
    std::uint64_t ts_proc_ns = 0;
};

struct TradePrint {
    std::string symbol;
    double px = 0.0;
    int sz = 0;
    std::uint64_t ts_recv_ns = 0;
    std::uint64_t ts_proc_ns = 0;
};

struct EngineMetric {
    std::string name;
    double value = 0.0;
    std::uint64_t ts_ns = 0;
};