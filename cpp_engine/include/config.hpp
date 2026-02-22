#pragma once
#include <string>
#include <vector>

struct EngineConfig {
    std::string run_mode;
    std::vector<std::string> symbols;
    std::string log_dir;
};

EngineConfig LoadConfigOrThrow(const std::string& path);