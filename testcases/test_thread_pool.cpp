#include <iostream>
#include <chrono>
#include "tinyrpc/comm/thread_pool.h"

void exampleTask(int id) {
  std::cout << "Task " << id << " is starting." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "Task " << id << " is completed." << std::endl;
}

int main() {
  tinyrpc::ThreadPool pool(4); // 创建一个包含4个线程的线程池
  pool.start();

  // 向线程池添加10个任务
  for (int i = 0; i < 10; ++i) {
    pool.addTask([i] { exampleTask(i); });
  }

  // 等待所有任务完成
  std::this_thread::sleep_for(std::chrono::seconds(12));

  pool.stop();
  return 0;
}
