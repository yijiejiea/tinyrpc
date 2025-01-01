#ifndef TINYRPC_MUTEX_H
#define TINYRPC_MUTEX_H

#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include "log.h"
#include "tinyrpc/coroutine/coroutine.h"
#include "reactor.h"

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
  ~CoroutineMutex() {
    if (m_lock) {
      unlock();
    }
  }

  void lock() {
    if (Coroutine::IsMainCoroutine()) {
      ErrorLog << "Main coroutine can't use coroutine mutex";
      return;
    }

    Coroutine* cor = Coroutine::GetCurrentCoroutine();

    // 使用 std::mutex 来加锁
    Lock lock(m_mutex);
    if (!m_lock) {
      m_lock = true;
      DebugLog << "Coroutine successfully acquired coroutine mutex";
      lock.unlock();
    } else {
      m_sleep_cors.push(cor);
      auto tmp = m_sleep_cors;
      lock.unlock();

      DebugLog << "Coroutine yielding, pending on coroutine mutex, current sleep queue size: "
               << tmp.size() << " coroutines";

      Coroutine::Yield();
    }
  }

  void unlock() {
    if (Coroutine::IsMainCoroutine()) {
      ErrorLog << "Main coroutine can't use coroutine mutex";
      return;
    }

    Lock lock(m_mutex);
    if (m_lock) {
      m_lock = false;
      if (m_sleep_cors.empty()) {
        return;
      }

      Coroutine* cor = m_sleep_cors.front();
      m_sleep_cors.pop();
      lock.unlock();

      if (cor) {
        // Wake up the first coroutine in the sleep queue
        DebugLog << "Coroutine unlocking, resuming coroutine [" << cor->getCorId() << "]";

        tinyrpc::Reactor::GetReactor()->addTask([cor]() {
          tinyrpc::Coroutine::Resume(cor);
        }, true);
      }
    }
  }

private:
  bool m_lock = false;
  std::mutex m_mutex;  // 使用标准的 mutex
  std::queue<Coroutine*> m_sleep_cors;  // 存放等待的协程队列
};

}  // namespace tinyrpc

#endif  // TINYRPC_MUTEX_H
