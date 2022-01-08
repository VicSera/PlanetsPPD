//
// Created by vsera on 1/8/2022.
//

#include "ThreadPool.h"

[[noreturn]] void ThreadPool::waitForJob()
{
    while (true)
    {
        Job job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);

            condition.wait(lock, [this](){
                return !queue.empty() || terminatePool;
            });
            job = queue.front();
            queue.pop();
        }

        job();
        {
            std::unique_lock<std::mutex> lock(counterMutex);
            ++finishedJobsCount;
        }
    }
}

void ThreadPool::terminate()
{
    terminatePool = true;
}

void ThreadPool::addJob(Job job)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        queue.push(job);
    }
    condition.notify_one();
}

void ThreadPool::resetFinishedJobsCounter()
{
    {
        std::unique_lock<std::mutex> lock(counterMutex);
        finishedJobsCount = 0;
    }
}

void ThreadPool::unlockAfter(int numberOfJobs)
{
    while(true)
    {
        {
            std::unique_lock<std::mutex> lock(counterMutex);
            if (finishedJobsCount == numberOfJobs)
                return;
        }
    }
}
