# 三层架构任务系统

这是一个基于C++11 future/promise的三层架构异步任务系统。

## 项目结构

```
future_promise/
├── base_task.h         # 底层回调任务系统接口
├── base_task.cpp       # 底层回调任务系统实现
├── promise_task.h      # 中间层future/promise桥接
├── user_api.h          # 用户层简化接口
├── main.cpp            # 示例代码
├── Makefile            # 编译配置
└── README.md           # 项目说明
```

## 架构说明

### 1. 底层 (base_layer)
- 纯回调的任务系统
- `create_task(extract, callback)` 创建任务
- 任务有 `start()` 接口开始执行
- 例如：每1秒调用一次extract，5次后调用一次callback

### 2. 中间层 (middle_layer) 
- 使用 `std::function` 和 `future/promise` 桥接底层和用户层
- 保存用户的 `user_extract` 和 `user_callback`
- 底层数据准备好后设置promise值并调用用户回调

### 3. 用户层 (user_layer)
- 提供简化的用户接口
- 用户获得 `future`，通过 `get()` 等待数据
- 如果数据未准备好，用户线程会阻塞等待

## 编译和运行

```bash
# 编译
make

# 运行示例
make run

# 清理
make clean

# 调试版本
make debug
```

## 使用示例

```cpp
#include "user_api.h"

// 定义数据提取函数
auto extract_func = []() -> std::string {
    return "提取的数据";
};

// 定义数据处理回调
auto callback_func = [](const std::string& data) {
    std::cout << "处理数据: " << data << std::endl;
};

// 创建异步任务
auto task = user_layer::create_async_task<std::string>(extract_func, callback_func);

// 获取future
auto future = task->get_future();

// 启动任务
task->start();

// 等待结果（会阻塞直到数据准备好）
std::string result = future.get();
```

## 特性

- 基于C++11 future/promise
- 三层架构清晰分离关注点
- 支持任意数据类型（模板）
- 线程安全
- 用户友好的API