#pragma once
#include <fstream>
#include <string>

#include "events.hpp"

class JsonlLogger {
public:
    explicit JsonlLogger(const std::string& session_dir);

    void Log(const QuoteUpdate& q);
    void Log(const TradePrint& t);
    void Log(const EngineMetric& m);

private:
    std::ofstream market_events_;
    std::ofstream engine_metrics_;

    static std::string EscapeJson(const std::string& s);
};