//
// Created by vsera on 1/8/2022.
//

#ifndef PLANETS_THREADPOOL_H
#define PLANETS_THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include "physics.h"

//typedef std::function<void(const std::vector<Planet>&, int, const std::vector<Force>&)> Job;
typedef std::function<void()> Job;

class ThreadPool
{
private:
    std::mutex queueMutex;
    std::mutex counterMutex;
    std::condition_variable condition;
    std::queue<Job> queue;
    bool terminatePool;
    int finishedJobsCount = 0;

public:
    [[noreturn]] void waitForJob();

    void terminate();

    void addJob(Job job);

    void resetFinishedJobsCounter();

    void unlockAfter(int numberOfJobs);
};

#endif //PLANETS_THREADPOOL_H
