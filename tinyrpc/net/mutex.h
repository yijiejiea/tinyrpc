#ifndef TINYRPC_MUTEX_H
#define TINYRPC_MUTEX_H

#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include "tinyrpc/coroutine/coroutine.h"

namespace tinyrpc {

// 使用标准库的 std::mutex 来替代自定义的 Mutex
class Mutex {
public:
  typedef std::unique_lock<std::mutex> Lock;

  Mutex() = default;
  ~Mutex() = default;

  void lock() {
    m_mutex.lock();
  }

  void unlock() {
    m_mutex.unlock();
  }

  std::mutex& getMutex() {
    return m_mutex;
  }

private:
  std::mutex m_mutex;  // 标准的 mutex
};

// 使用 std::shared_mutex 来替代自定义的 RWMutex
class RWMutex {
public:
  typedef std::unique_lock<std::shared_mutex> WriteLock;
  typedef std::shared_lock<std::shared_mutex> ReadLock;

  RWMutex() = default;
  ~RWMutex() = default;

  void rdlock() {
    m_lock.lock_shared();
  }

  void wrlock() {
    m_lock.lock();
  }

  void unlock() {
    m_lock.unlock();
  }

private:
  std::shared_mutex m_lock;  // 标准的 shared_mutex
};

class CoroutineMutex {
public:
  typedef std::unique_lock<std::mutex> Lock;

  CoroutineMutex() = default;
  ~CoroutineMutex();

  void lock();
  void unlock();

private:
  bool m_lock = false;
  std::mutex m_mutex;  // 使用标准的 mutex
  std::queue<Coroutine*> m_sleep_cors;  // 存放等待的协程队列
};

}  // namespace tinyrpc

#endif  // TINYRPC_MUTEX_H
