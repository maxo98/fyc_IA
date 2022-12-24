#include "ThreadPool.h"

ThreadPool* ThreadPool::singleton = nullptr;

ThreadPool* ThreadPool::GetInstance()
{
    if (singleton == nullptr) {
        singleton = new ThreadPool();
    }
    return singleton;
}

ThreadPool::~ThreadPool()
{
    waiting = true;
    std::unique_lock<std::mutex> tasks_lock(tasksMutex);
    taskDoneCv.wait(tasks_lock, [this] { return (tasksTotal == 0); });
    waiting = false;

    stop();
}

void ThreadPool::start() 
{
    running = true;

    const uint32_t num_threads = std::thread::hardware_concurrency() - 1; // Max # of threads the system supports
    threads.resize(num_threads);

    for (uint32_t i = 0; i < num_threads; i++) 
    {
        threads.at(i) = std::thread(&ThreadPool::threadLoop, this);
    }
}

void ThreadPool::threadLoop() 
{
    while (running)
    {
        std::function<void()> task;
        std::unique_lock<std::mutex> tasks_lock(tasksMutex);
        
        taskAvailableCv.wait(tasks_lock, [this] { return !tasks.empty() || !running; });

        if (running)
        {
            task = std::move(tasks.front());
            tasks.pop();

            tasks_lock.unlock();
            
            task();
            
            tasks_lock.lock();
            --tasksTotal;
            
            if (waiting)
                taskDoneCv.notify_one();
        }
    }
}

[[nodiscard]] size_t ThreadPool::getTasksTotal() const
{
    return tasksTotal;
}

void ThreadPool::stop() 
{
    running = false;
    taskAvailableCv.notify_all();
    
    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i].join();
    }

    threads.clear();
}