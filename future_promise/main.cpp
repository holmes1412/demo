#include "promise_task.h"
#include <iostream>
#include <chrono>
#include <string>
#include <random>
#include <thread>

int main() {
    std::cout << "=== 三层架构任务系统演示 ===" << std::endl;
    
    int data_counter = 0;
    
    auto extract_func = [&data_counter]() -> std::string {
        data_counter++;
        std::string result = "数据_" + std::to_string(data_counter);
        std::cout << "[Extract] 提取数据: " << result << std::endl;
        return result;
    };
    
    auto callback_func = [](const std::string& data) {
        std::cout << "[Callback] 处理数据: " << data << std::endl;
    };
    
    std::cout << "\n1. 创建异步任务..." << std::endl;
    Chunk<std::string>* chunk;
    auto task = create_promise_task<std::string>(extract_func, callback_func, &chunk);
    
    std::cout << "2. 启动任务..." << std::endl;
    task->start();
    
    std::cout << "3. 流式处理数据..." << std::endl;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 2);
    
    while (chunk) {
        std::cout << "[用户] 等待数据..." << std::endl;
        std::string data = chunk->get();
        std::cout << "[用户] 获得数据: " << data << std::endl;
        
        if (chunk->is_finished()) {
            std::cout << "[用户] 数据流结束" << std::endl;
            break;
        }
        
        // 模拟处理逻辑，随机sleep 1s或2s
        int sleep_time = dis(gen);
        std::cout << "[用户] 处理数据中，睡眠 " << sleep_time << "s..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(sleep_time));
        
        chunk = chunk->get_next();
    }
    
    std::cout << "4. 停止任务..." << std::endl;
    task->stop();
    delete task;
    
    std::cout << "\n=== 演示完成 ===" << std::endl;
    
    std::cout << "\n=== 并发测试 ===" << std::endl;
    
    auto extract_func2 = []() -> int {
        std::cout << "[Extract2] 正在计算..." << std::endl;
        return 42;
    };
    
    auto callback_func2 = [](const int& data) {
        std::cout << "[Callback2] 计算结果: " << data << std::endl;
    };
    
    Chunk<int>* chunk2;
    auto task2 = create_promise_task<int>(extract_func2, callback_func2, &chunk2);
    
    task2->start();
    
    std::cout << "第二个流式任务开始..." << std::endl;
    
    while (chunk2) {
        std::cout << "[任务2] 等待数据..." << std::endl;
        int data = chunk2->get();
        std::cout << "[任务2] 获得数据: " << data << std::endl;
        
        if (chunk2->is_finished()) {
            std::cout << "[任务2] 数据流结束" << std::endl;
            break;
        }
        
        // 模拟处理
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        chunk2 = chunk2->get_next();
    }
    
    delete task2;
    
    return 0;
}
