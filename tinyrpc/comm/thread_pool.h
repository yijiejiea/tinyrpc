#ifndef TINYRPC_THREAD_THREADPOOL_H
#define TINYRPC_THREAD_THREADPOOL_H

#include <thread>
#include <queue>
#include <functional>
#include <vector>
#include <condition_variable>
#include <mutex>

namespace tinyrpc {

class ThreadPool {
 public:
  ThreadPool(int size);
  ~ThreadPool();

  void start();
  void stop();
  void addTask(std::function<void()> cb);

 private:
  void MainFunction();

 private:
  int m_size;
  std::vector<std::thread> m_threads;
  std::queue<std::function<void()>> m_tasks;
  std::mutex m_mutex;
  std::condition_variable m_condition;
  bool m_is_stop;
};

} // namespace tinyrpc

#endif // TINYRPC_THREAD_THREADPOOL_H