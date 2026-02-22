#include "config.hpp"
#include <yaml-cpp/yaml.h>
#include <stdexcept>

static std::string RequireString(const YAML::Node& n, const char* key) {
    if (!n[key] || !n[key].IsScalar()) {
        throw std::runtime_error(std::string("Missing or invalid key: ") + key);
    }
    return n[key].as<std::string>();
}

static std::vector<std::string> RequireStringList(const YAML::Node& n, const char* key) {
    if (!n[key] || !n[key].IsSequence()) {
        throw std::runtime_error(std::string("Missing or invalid key: ") + key);
    }

    std::vector<std::string> result;
    for (const auto& item : n[key]) {
        result.push_back(item.as<std::string>());
    }
    return result;
}

EngineConfig LoadConfigOrThrow(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);

    EngineConfig cfg;
    cfg.run_mode = RequireString(root, "run_mode");
    cfg.symbols = RequireStringList(root, "symbols");
    cfg.log_dir = RequireString(root, "log_dir");

    return cfg;
}