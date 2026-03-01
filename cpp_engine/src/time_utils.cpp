#include "time_utils.hpp"
#include <chrono>

std::uint64_t NowNs() {
    using namespace std::chrono;
    return (std::uint64_t)duration_cast<nanoseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}