#pragma once

#include <future>
#include <memory>

template<typename T>
class Chunk {
public:
    Chunk();
    ~Chunk();
    
    // 等待并获取数据
    T get();
    
    // 检查是否是最后一个chunk
    bool is_finished() const;
    
    // 获取下一个chunk
    Chunk<T>* get_next();
    
    // 内部方法：设置数据
    void set_data(const T& data);
    void set_finished(bool finished);
    
    // 内部方法：创建下一个chunk
    void create_next_chunk();

private:
    std::promise<T> promise_;
    std::future<T> future_;
    bool finished_;
    bool data_set_;
    Chunk<T>* next_chunk_;
    T data_;
};

template<typename T>
Chunk<T>::Chunk() 
    : finished_(false)
    , data_set_(false)
    , next_chunk_(nullptr) {
    future_ = promise_.get_future();
}

template<typename T>
Chunk<T>::~Chunk() {
    if (next_chunk_) {
        delete next_chunk_;
    }
}

template<typename T>
T Chunk<T>::get() {
    if (!data_set_) {
        data_ = future_.get();
        data_set_ = true;
    }
    return data_;
}

template<typename T>
bool Chunk<T>::is_finished() const {
    return finished_;
}

template<typename T>
Chunk<T>* Chunk<T>::get_next() {
    return next_chunk_;
}

template<typename T>
void Chunk<T>::set_data(const T& data) {
    if (!data_set_) {
        data_ = data;
        promise_.set_value(data);
        data_set_ = true;
    }
}

template<typename T>
void Chunk<T>::set_finished(bool finished) {
    finished_ = finished;
}

template<typename T>
void Chunk<T>::create_next_chunk() {
    if (!next_chunk_) {
        next_chunk_ = new Chunk<T>();
    }
}