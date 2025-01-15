#ifndef TINYRPC_MUTEX_H
#define TINYRPC_MUTEX_H

#include <memory>
#include <shared_mutex>
#include <mutex>
#include <queue>
#include "tinyrpc/coroutine/coroutine.h"

// this file copy form sylar

namespace tinyrpc {

template <class T>
struct ScopedLockImpl
{
public:
  ScopedLockImpl(T &mutex)
      : m_mutex(mutex)
  {
    m_mutex.lock();
    m_locked = true;
  }

  ~ScopedLockImpl()
  {
    unlock();
  }

  void lock()
  {
    if (!m_locked)
    {
      m_mutex.lock();
      m_locked = true;
    }
  }

  void unlock()
  {
    if (m_locked)
    {
      m_mutex.unlock();
      m_locked = false;
    }
  }

private:
  /// mutex
  T &m_mutex;
  /// 是否已上锁
  bool m_locked;
};

template <class T>
struct ReadScopedLockImpl
{
public:
  ReadScopedLockImpl(T &mutex)
      : m_mutex(mutex)
  {
    m_mutex.rdlock();
    m_locked = true;
  }

  ~ReadScopedLockImpl()
  {
    unlock();
  }

  void lock()
  {
    if (!m_locked)
    {
      m_mutex.rdlock();
      m_locked = true;
    }
  }

  void unlock()
  {
    if (m_locked)
    {
      m_mutex.unlock();
      m_locked = false;
    }
  }

private:
  /// mutex
  T &m_mutex;
  /// 是否已上锁
  bool m_locked;
};

/**
 * @brief 局部写锁模板实现
 */
template <class T>
struct WriteScopedLockImpl
{
public:
  WriteScopedLockImpl(T &mutex)
      : m_mutex(mutex)
  {
    m_mutex.wrlock();
    m_locked = true;
  }

  ~WriteScopedLockImpl()
  {
    unlock();
  }

  void lock()
  {
    if (!m_locked)
    {
      m_mutex.wrlock();
      m_locked = true;
    }
  }

  void unlock()
  {
    if (m_locked)
    {
      m_mutex.unlock();
      m_locked = false;
    }
  }

private:
  T &m_mutex;
  bool m_locked;
};

class Mutex
{
public:
  typedef ScopedLockImpl<Mutex> Lock;

  void lock()
  {
    m_mutex.lock();
  }

  void unlock()
  {
    m_mutex.unlock();
  }

private:
  std::mutex m_mutex;
};

class RWMutex
{
public:
  /// 局部读锁
  typedef ReadScopedLockImpl<RWMutex> ReadLock;

  typedef WriteScopedLockImpl<RWMutex> WriteLock;

  void rdlock()
  {
    m_mutex.lock_shared();
  }

  void unrdlock()
  {
    m_mutex.unlock_shared();
  }
  void wrlock()
  {
    m_mutex.lock();
  }

  void unlock()
  {
    m_mutex.unlock();
  }

private:
  std::shared_mutex m_mutex;
};


class CoroutineMutex {
 public:
  typedef ScopedLockImpl<CoroutineMutex> Lock;

  CoroutineMutex();

  ~CoroutineMutex();

  void lock();

  void unlock();
 private:
  bool m_lock {false};
  Mutex m_mutex;
  std::queue<Coroutine*> m_sleep_cors;
};


}
#endif
