#include <iostream>

#include "graphics.h"
#include <chrono>
#include <fstream>

#define FRAMERATE 60

std::ofstream out("main-logs.txt");

[[noreturn]] void mainLoop(HWND console, HDC device)
{
    RECT windowRect;
    GetWindowRect(console, &windowRect);
    int horizontalBorder = windowRect.right - windowRect.left - 20;
    int verticalBorder = windowRect.bottom - windowRect.top - 30;


    std::vector<Planet> planets {
            Planet(50, 100, 5, 1.0f, 1.0f, .0f),
            Planet(250, 100, 10, 10000.0f, 1.0f, .0f),
            Planet(200, 300, 30, 100000.0f, 1.5f, 1.0f)
    };

    std::vector<Force> forces(planets.size());

    auto millisecondsPerLoop = 1000.0f / FRAMERATE;
    auto secondsPerLoop = 1.0f / FRAMERATE;

    while (true)
    {
        auto const beginTime = std::chrono::high_resolution_clock::now();

        for (auto i = 0; i < planets.size(); ++i)
        {
            forces[i] = computeGravity(planets, i);
        }

        auto const endTime = std::chrono::high_resolution_clock::now();

        out << "Loop took " << duration_cast<std::chrono::milliseconds>(endTime-beginTime).count() << "milliseconds" << std::endl;

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

    mainLoop(myConsole, mdc);
}