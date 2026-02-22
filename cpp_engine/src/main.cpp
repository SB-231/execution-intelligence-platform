#include <iostream>
#include <string>

static constexpr const char* kEngineVersion = "0.1.0";

int main(int argc, char** argv) {
    std::string config_path = "configs/dev.yaml";
    if (argc >= 2) {
        config_path = argv[1];
    }

    std::cout << "Execution Intelligence Engine\n";
    std::cout << "Version: " << kEngineVersion << "\n";
    std::cout << "Config: " << config_path << "\n";

    // Week 1 Day 2: parse config + print symbols (AAPL, MSFT, SPY)
    return 0;
}
