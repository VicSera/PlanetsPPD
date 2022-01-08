#include <iostream>

#include "graphics.h"
#include "ThreadPool.h"
#include <binders.h>
#include <functional>

#define FRAMERATE 60
#define NUM_THREADS 10

[[noreturn]] void mainLoop(HWND console, HDC device, ThreadPool& pool)
{
    RECT windowRect;
    GetWindowRect(console, &windowRect);
    int horizontalBorder = windowRect.right - windowRect.left - 20;
    int verticalBorder = windowRect.bottom - windowRect.top - 30;

    std::vector<Planet> planets {
            Planet(50, 100, 5, 1.0f, RGB(255, 0, 0), 1.0f, .0f),
            Planet(250, 100, 10, 10000.0f, RGB(255, 0, 0), 1.0f, .0f),
            Planet(200, 300, 30, 100000.0f, RGB(255, 255, 0), 1.5f, 1.0f)
    };
    std::vector<Force> forces(planets.size());

    auto millisecondsPerLoop = 1000.0f / FRAMERATE;
    auto secondsPerLoop = 1.0f / FRAMERATE;

    while (true)
    {
        pool.resetFinishedJobsCounter();

        // register jobs
        for (auto i = 0; i < planets.size(); ++i)
        {
            pool.addJob([&planets, &forces, i] {
                forces[i] = computeGravity(planets, i);
            });
        }

        // wait for jobs to finish
        pool.unlockAfter((int)planets.size());

        // draw updated planets
        for (auto i = 0; i < planets.size(); ++i)
        {
            applyForce(planets[i], forces[i], secondsPerLoop, horizontalBorder, verticalBorder);
            drawCircle(planets[i], device, console);
        }

        Sleep((int)millisecondsPerLoop);
    }
}

int main() {

    HWND myConsole = GetConsoleWindow();
    HDC mdc = GetDC(myConsole);

    ThreadPool pool;
    std::vector<std::thread> threads;

    for (auto i = 0; i < NUM_THREADS; ++i)
        threads.emplace_back([&pool] { pool.waitForJob(); });

    mainLoop(myConsole, mdc, pool);

    pool.terminate();
}