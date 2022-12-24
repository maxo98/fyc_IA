#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <queue>
#include <type_traits>
#include <iostream>

class ThreadPool 
{
public:
    ThreadPool(ThreadPool& other) = delete;

    void operator=(const ThreadPool&) = delete;

    static ThreadPool* GetInstance();

    void start();

    template <class _Fn, class... _Args>
    void queueJob(_Fn&& task, _Args&&... args)
    {
        std::function<void()> task_function = std::bind(std::forward<_Fn>(task), std::forward<_Args>(args)...);
        
        {
            const std::scoped_lock tasks_lock(tasksMutex);
            tasks.push(task_function);
        }
        
        ++tasksTotal;
        taskAvailableCv.notify_one();
    }


    void stop();
    size_t getTasksTotal() const;

protected:
    ThreadPool() {};

    ~ThreadPool();

private:
    static ThreadPool* singleton;

    void threadLoop();

    std::atomic<bool> running = false;           // Tells threads to stop looking for jobs
    std::mutex tasksMutex;                  // Prevents data races to the job queue
    std::condition_variable taskAvailableCv = {}; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::atomic<size_t> tasksTotal = 0;
    std::atomic<bool> waiting = false;
    std::condition_variable taskDoneCv = {};
};