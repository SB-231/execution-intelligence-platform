#include <iostream>
#include <string>
#include "config.hpp"

static constexpr const char* kEngineVersion = "0.1.0";

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
    }
    catch (const std::exception& e) {
        std::cerr << "Config error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}