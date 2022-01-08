#include <iostream>

#include "graphics.h"

#define FRAMERATE 60

[[noreturn]] void mainLoop(HWND console, HDC device)
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

    auto millisecondsPerLoop = 1000.0f / FRAMERATE;
    auto secondsPerLoop = 1.0f / FRAMERATE;

    while (true)
    {
        for (auto i = 0; i < planets.size(); ++i)
        {
            auto force = computeGravity(planets, i);
            applyForce(planets[i], force, secondsPerLoop, horizontalBorder, verticalBorder);
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