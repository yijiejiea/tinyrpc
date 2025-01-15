#include <iostream>
#include <thread>
#include <chrono>
#include "tinyrpc/net/mutex.h"
#include "tinyrpc/coroutine/coroutine.h"

tinyrpc::Coroutine::ptr cor1;
tinyrpc::Coroutine::ptr cor2;

// 测试 Mutex
tinyrpc::Mutex mutex;

void testMutex(int id)
{
    for (int i = 0; i < 5; ++i)
    {
        {
            tinyrpc::Mutex::Lock lock(mutex);
            std::cout << "Thread " << id << " is working." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 测试 RWMutex
tinyrpc::RWMutex rwmutex;

void readTask(int id)
{
    for (int i = 0; i < 5; ++i)
    {
        {
            tinyrpc::RWMutex::ReadLock lock(rwmutex);
            std::cout << "Reader " << id << " is reading." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void writeTask(int id)
{
    for (int i = 0; i < 5; ++i)
    {
        {
            tinyrpc::RWMutex::WriteLock lock(rwmutex);
            std::cout << "Writer " << id << " is writing." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 测试 CoroutineMutex
class Test
{
public:
    tinyrpc::CoroutineMutex m_coroutine_mutex;
    int a = 1;
};
Test test_;

void fun1()
{
    std::cout << "cor1 ---- now first resume fun1 coroutine" << std::endl;
    {
        tinyrpc::CoroutineMutex::Lock lock(test_.m_coroutine_mutex);
        std::cout << "cor1 ---- coroutine lock on test_, sleep 5s begin" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "cor1 ---- sleep 5s end, now back coroutine lock" << std::endl;
    }
    tinyrpc::Coroutine::Yield();
    std::cout << "cor1 ---- fun1 coroutine back, now end" << std::endl;
}

void fun2()
{
    std::cout << "cor222 ---- now first resume fun2 coroutine" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "cor222 ---- coroutine2 want to get coroutine lock of test_" << std::endl;
    {
        tinyrpc::CoroutineMutex::Lock lock(test_.m_coroutine_mutex);
        std::cout << "cor222 ---- coroutine2 get coroutine lock of test_ succ" << std::endl;
    }
}

int main()
{
    //   // 测试 Mutex
    //   std::cout << "Testing Mutex:" << std::endl;
    //   std::thread t1(testMutex, 1);
    //   std::thread t2(testMutex, 2);
    //   t1.join();
    //   t2.join();

    //   // 测试 RWMutex
    //   std::cout << "\nTesting RWMutex:" << std::endl;
    //   std::thread readers[3];
    //   std::thread writers[2];

    //   for (int i = 0; i < 3; i++) {
    //     readers[i] = std::thread(readTask, i + 1);
    //   }

    //   for (int i = 0; i < 2; i++) {
    //     writers[i] = std::thread(writeTask, i + 1);
    //   }

    //   for (int i = 0; i < 3; i++) {
    //     readers[i].join();
    //   }

    //   for (int i = 0; i < 2; i++) {
    //     writers[i].join();
    //   }

    // 测试 CoroutineMutex
    std::cout << "\nTesting CoroutineMutex:" << std::endl;

    std::cout << "main begin" << std::endl;
    int stack_size = 128 * 1024;
    char *sp1 = reinterpret_cast<char *>(malloc(stack_size));
    if (!sp1)
    {
        std::cerr << "Failed to allocate stack memory for coroutine 1" << std::endl;
        return -1;
    }
    cor1 = std::make_shared<tinyrpc::Coroutine>(stack_size, sp1, fun1);

    char *sp2 = reinterpret_cast<char *>(malloc(stack_size));
    if (!sp2)
    {
        std::cerr << "Failed to allocate stack memory for coroutine 2" << std::endl;
        free(sp1);
        return -1;
    }
    cor2 = std::make_shared<tinyrpc::Coroutine>(stack_size, sp2, fun2);

    std::cout << "Main coroutine created" << std::endl;

    tinyrpc::Coroutine::Resume(cor1.get());
    tinyrpc::Coroutine::Resume(cor2.get());

    while (cor1->getIsInCoFunc() || cor2->getIsInCoFunc())
    {
        if (cor1->getIsInCoFunc())
        {
            tinyrpc::Coroutine::Resume(cor1.get());
        }
        if (cor2->getIsInCoFunc())
        {
            tinyrpc::Coroutine::Resume(cor2.get());
        }
    }

    free(sp1);
    free(sp2);

    std::cout << "main end" << std::endl;

    return 0;
}
