#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <chrono>

class AsyncTask;

// make it extract for some times then finally callback
using extract_t = std::function<void(AsyncTask *)>;
using callback_t = std::function<void(AsyncTask *)>;

class AsyncTask
{
public:
    AsyncTask(extract_t extract, callback_t callback);
    ~AsyncTask();

    void start();
    void stop();
    bool is_running() const;

private:
    void task_loop();

    extract_t extract_;
    callback_t callback_;
    int extract_interval_ms_;
    int extract_count_;

    std::atomic<bool> running_;
    std::atomic<bool> stopped_;
    std::thread worker_thread_;
};

AsyncTask *create_task(extract_t extract, callback_t callback);

