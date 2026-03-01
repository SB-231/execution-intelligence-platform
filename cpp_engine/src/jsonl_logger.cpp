#include "jsonl_logger.hpp"
#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

static void OpenOrThrow(std::ofstream& out, const fs::path& p) {
    out.open(p, std::ios::out | std::ios::app);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open log file: " + p.string());
    }
}

JsonlLogger::JsonlLogger(const std::string& session_dir) {
    fs::create_directories(session_dir);

    OpenOrThrow(market_events_, fs::path(session_dir) / "market_events.jsonl");
    OpenOrThrow(engine_metrics_, fs::path(session_dir) / "engine_metrics.jsonl");
}

std::string JsonlLogger::EscapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"':  out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:   out += c; break;
        }
    }
    return out;
}

void JsonlLogger::Log(const QuoteUpdate& q) {
    market_events_
        << "{\"event_type\":\"quote\""
        << ",\"symbol\":\"" << EscapeJson(q.symbol) << "\""
        << ",\"bid_px\":" << q.bid_px
        << ",\"bid_sz\":" << q.bid_sz
        << ",\"ask_px\":" << q.ask_px
        << ",\"ask_sz\":" << q.ask_sz
        << ",\"ts_recv_ns\":" << q.ts_recv_ns
        << ",\"ts_proc_ns\":" << q.ts_proc_ns
        << "}\n";
    market_events_.flush();
}

void JsonlLogger::Log(const TradePrint& t) {
    market_events_
        << "{\"event_type\":\"trade\""
        << ",\"symbol\":\"" << EscapeJson(t.symbol) << "\""
        << ",\"px\":" << t.px
        << ",\"sz\":" << t.sz
        << ",\"ts_recv_ns\":" << t.ts_recv_ns
        << ",\"ts_proc_ns\":" << t.ts_proc_ns
        << "}\n";
    market_events_.flush();
}

void JsonlLogger::Log(const EngineMetric& m) {
    engine_metrics_
        << "{\"event_type\":\"metric\""
        << ",\"name\":\"" << EscapeJson(m.name) << "\""
        << ",\"value\":" << m.value
        << ",\"ts_ns\":" << m.ts_ns
        << "}\n";
    engine_metrics_.flush();
}