#pragma once
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>

template <typename T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity) : capacity_(capacity) {}

    // Blocks if full. Returns false if queue is closed.
    bool Push(T item) {
        std::unique_lock<std::mutex> lock(mu_);
        cv_not_full_.wait(lock, [&] { return closed_ || q_.size() < capacity_; });
        if (closed_) return false;
        q_.push_back(std::move(item));
        cv_not_empty_.notify_one();
        return true;
    }

    // Blocks if empty. Returns nullopt if closed and empty.
    std::optional<T> Pop() {
        std::unique_lock<std::mutex> lock(mu_);
        cv_not_empty_.wait(lock, [&] { return closed_ || !q_.empty(); });
        if (q_.empty()) return std::nullopt;
        T item = std::move(q_.front());
        q_.pop_front();
        cv_not_full_.notify_one();
        return item;
    }

    void Close() {
        std::lock_guard<std::mutex> lock(mu_);
        closed_ = true;
        cv_not_empty_.notify_all();
        cv_not_full_.notify_all();
    }

    std::size_t Size() const {
        std::lock_guard<std::mutex> lock(mu_);
        return q_.size();
    }

private:
    const std::size_t capacity_;
    mutable std::mutex mu_;
    std::condition_variable cv_not_empty_;
    std::condition_variable cv_not_full_;
    std::deque<T> q_;
    bool closed_ = false;
};