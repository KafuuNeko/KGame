#pragma once

#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <condition_variable>

#include <cinttypes>

class ThreadPool {
public:
    using Task = std::function<void()>;

    ThreadPool(size_t count) noexcept
        : mCount(count), isRunning(true)
    {
        addThread(count);
    }

    ~ThreadPool() noexcept {
        this->isRunning = false;
        mCv.notify_all();

        for (auto &pool : mPool) {
            if (pool.joinable()) {
                pool.join();
            }
        }
    }

    void addThread(size_t count) noexcept
    {
        if (count == 0) {
            return;
        }

        mPool.emplace_back([=]() {
            while (isRunning) {
                waitAndProcess();
            }
        });

        ++mCount;
        this->addThread(count - 1);
    }

    size_t idleThread() const {
        return mIdleThread;
    }

    template<typename... Args>
    void commit(Args&&... args) {
        mTasks.emplace(std::forward<Args>(args)...);
    }

private:
    size_t mCount;
    std::vector<std::thread> mPool;
    std::queue<Task> mTasks;

    std::mutex mMutex;
    std::condition_variable mCv;

    std::atomic_size_t mIdleThread;

    bool isRunning;

    void waitAndProcess()
    {
        std::unique_lock<std::mutex> lk(mMutex);
        mIdleThread++;
        mCv.wait(lk, [this] {
            return !mTasks.empty() || !isRunning;
        });
        mIdleThread--;

        if (!isRunning) {
            return;
        }

        auto task = mTasks.front();
        mTasks.pop();

        lk.unlock();

        //procee
        task();
    }
};
