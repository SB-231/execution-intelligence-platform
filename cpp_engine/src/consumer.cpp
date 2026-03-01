#include "consumer.hpp"

void RunConsumer(
    BoundedQueue<QuoteUpdate>& in,
    JsonlLogger& logger,
    std::atomic<bool>& stop_flag
) {
    (void)stop_flag; // shutdown is driven by Close() + empty queue
    while (true) {
        auto item = in.Pop();
        if (!item.has_value()) break; // closed + empty
        logger.Log(item.value());
    }
}