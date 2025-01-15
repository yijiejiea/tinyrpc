#include <thread>
#include <queue>
#include <functional>
#include <vector>
#include <condition_variable>
#include <mutex>
#include "tinyrpc/comm/thread_pool.h"

namespace tinyrpc {

void ThreadPool::MainFunction() {
  while (!m_is_stop) {
    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_condition.wait(lock, [this] { return m_is_stop || !m_tasks.empty(); });
      if (m_is_stop && m_tasks.empty()) {
        return;
      }
      task = std::move(m_tasks.front());
      m_tasks.pop();
    }
    task();
  }
}

ThreadPool::ThreadPool(int size) : m_size(size), m_is_stop(false) {
  for (int i = 0; i < m_size; ++i) {
    m_threads.emplace_back(&ThreadPool::MainFunction, this);
  }
}

void ThreadPool::start() {
  // Threads are already started in the constructor
}

void ThreadPool::stop() {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_is_stop = true;
  }
  m_condition.notify_all();
  for (std::thread &thread : m_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void ThreadPool::addTask(std::function<void()> cb) {
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_tasks.push(std::move(cb));
  }
  m_condition.notify_one();
}

ThreadPool::~ThreadPool() {
  stop();
}
} 