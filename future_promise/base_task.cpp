#include <iostream>
#include <unistd.h>
#include "base_task.h"

#define EXTRACT_INTERVAL_MS	1000
#define EXTRACT_COUNT		5

AsyncTask::AsyncTask(extract_t extract, callback_t callback)
    : extract_(extract)
    , callback_(callback)
    , extract_interval_ms_(EXTRACT_INTERVAL_MS)
    , extract_count_(EXTRACT_COUNT)
    , running_(false)
    , stopped_(false) {
}

AsyncTask::~AsyncTask()
{
    stop();
}

void AsyncTask::start()
{
    if (running_.load())
        return;
    
    stopped_.store(false);
    running_.store(true);
    
    worker_thread_ = std::thread(&AsyncTask::task_loop, this);
}

void AsyncTask::stop()
{
    if (!running_.load())
        return;

    stopped_.store(true);
    
    if (worker_thread_.joinable())
        worker_thread_.join();

    running_.store(false);
}

bool AsyncTask::is_running() const
{
    return running_.load();
}

// fake task loop : here is 5 times extract() and 1s for each
void AsyncTask::task_loop()
{
    int count = 0;
    
    while (!stopped_.load() && count < extract_count_)
	{
        if (extract_)
            extract_(this);
        
        if (!stopped_.load() && callback_)
            callback_(this);
        
        count++;
        if (!stopped_.load() && count < extract_count_)
			usleep(extract_interval_ms_ * 1000);
    }
}

AsyncTask* create_task(extract_t extract, callback_t callback)
{
    return new AsyncTask(extract, callback);
}

