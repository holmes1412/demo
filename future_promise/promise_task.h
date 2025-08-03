#pragma once

#include "base_task.h"
#include "chunk.h"
#include <future>
#include <functional>

template<typename T>
class PromiseTask
{
public:
    using user_extract_t = std::function<T()>;
    using user_callback_t = std::function<void(const T&)>;

    PromiseTask(user_extract_t extract, user_callback_t callback);
    ~PromiseTask();

    Chunk<T>* get_chunk();
    void start();
    void stop();
    bool is_running() const;

private:
    void middle_extract();
    void middle_callback();

    user_extract_t user_extract_;
    user_callback_t user_callback_;

    AsyncTask *base_task_;
    
    Chunk<T>* head_chunk_;
    Chunk<T>* current_chunk_;
    int extract_count_;
    bool task_finished_;

    T extracted_data_;
    bool data_ready_;
};

template<typename T>
PromiseTask<T>::PromiseTask(user_extract_t extract, user_callback_t callback)
    : user_extract_(extract)
    , user_callback_(callback)
    , base_task_(nullptr)
    , head_chunk_(nullptr)
    , current_chunk_(nullptr)
    , extract_count_(0)
    , task_finished_(false)
    , data_ready_(false)
{ 
    head_chunk_ = new Chunk<T>();
    current_chunk_ = head_chunk_;

    auto extract_wrapper = [this](AsyncTask*){ this->middle_extract(); };

    auto callback_wrapper = [this](AsyncTask*){ this->middle_callback(); };

    base_task_ = create_task(extract_wrapper, callback_wrapper);
}

template<typename T>
PromiseTask<T>::~PromiseTask()
{
    stop();
    delete base_task_;
    if (head_chunk_) {
        delete head_chunk_;
    }
}

template<typename T>
Chunk<T>* PromiseTask<T>::get_chunk()
{
    return head_chunk_;
}

template<typename T>
void PromiseTask<T>::start()
{
    if (base_task_)
        base_task_->start();
}

template<typename T>
void PromiseTask<T>::stop()
{
    if (base_task_)
        base_task_->stop();
}

template<typename T>
bool PromiseTask<T>::is_running() const
{
    return base_task_ ? base_task_->is_running() : false;
}

template<typename T>
void PromiseTask<T>::middle_extract()
{
    if (user_extract_ && !task_finished_)
    {
        extracted_data_ = user_extract_();
        data_ready_ = true;
        extract_count_++;
        
        // 模拟底层AsyncTask一共回来5次
        if (extract_count_ >= 5) {
            task_finished_ = true;
        }
    }
}

template<typename T>
void PromiseTask<T>::middle_callback()
{
    if (data_ready_)
    { 
        if (user_callback_)
            user_callback_(extracted_data_);

        // 设置当前chunk的数据
        current_chunk_->set_data(extracted_data_);
        
        if (task_finished_) {
            current_chunk_->set_finished(true);
        } else {
            // 为下次数据创建新的chunk
            current_chunk_->create_next_chunk();
            current_chunk_ = current_chunk_->get_next();
        }
        
        data_ready_ = false;
    }
}

template<typename T>
PromiseTask<T>* create_promise_task(
    typename PromiseTask<T>::user_extract_t extract,
    typename PromiseTask<T>::user_callback_t callback,
    Chunk<T>** chunk)
{
    auto task = new PromiseTask<T>(extract, callback);
    *chunk = task->get_chunk();
    return task;
}

