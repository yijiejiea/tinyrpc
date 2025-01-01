#include "tinyrpc/net/mutex.h"
#include "tinyrpc/comm/log.h"
#include "tinyrpc/coroutine/coroutine.h"

namespace tinyrpc {

// CoroutineMutex 构造函数和析构函数
CoroutineMutex::CoroutineMutex() = default;

CoroutineMutex::~CoroutineMutex() {
  if (m_lock) {
    unlock();
  }
}

// 协程锁的加锁实现
void CoroutineMutex::lock() {
  if (Coroutine::IsMainCoroutine()) {
    ErrorLog << "Main coroutine can't use coroutine mutex";
    return;
  }

  Coroutine* cor = Coroutine::GetCurrentCoroutine();

  std::unique_lock<std::mutex> lock(m_mutex);  // 使用 std::mutex 来加锁
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

// 协程锁的解锁实现
void CoroutineMutex::unlock() {
  if (Coroutine::IsMainCoroutine()) {
    ErrorLog << "Main coroutine can't use coroutine mutex";
    return;
  }

  std::unique_lock<std::mutex> lock(m_mutex);  // 使用 std::mutex 来加锁
  if (m_lock) {
    m_lock = false;
    if (m_sleep_cors.empty()) {
      return;
    }

    Coroutine* cor = m_sleep_cors.front();
    m_sleep_cors.pop();
    lock.unlock();

    if (cor) {
      // 唤醒等待的协程
      DebugLog << "Coroutine unlocking, resuming coroutine [" << cor->getCorId() << "]";

      tinyrpc::Reactor::GetReactor()->addTask([cor]() {
        tinyrpc::Coroutine::Resume(cor);
      }, true);
    }
  }
}

}  // namespace tinyrpc
