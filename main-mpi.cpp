#include <iostream>
#include <mpi.h>
#include <chrono>
#include <fstream>

#include "graphics.h"

#define FRAMERATE 60

#define TAG_START_INDEX 0
#define TAG_END_INDEX 1
#define TAG_NUM_PLANETS 2
#define TAG_PLANETS 3
#define TAG_FORCES 4

#define VERBOSE_LOGGING true

std::ofstream out("main-mpi-logs.txt");

[[noreturn]] void worker(int me)
{
    int startIndex, endIndex, numPlanets;
    MPI_Status status;

    // Receive initial data
    MPI_Recv(&startIndex, 1, MPI_INT, 0, TAG_START_INDEX, MPI_COMM_WORLD, &status);
    MPI_Recv(&endIndex, 1, MPI_INT, 0, TAG_END_INDEX, MPI_COMM_WORLD, &status);
    MPI_Recv(&numPlanets, 1, MPI_INT, 0, TAG_NUM_PLANETS, MPI_COMM_WORLD, &status);

    auto numForces = endIndex - startIndex;

    std::vector<Planet> planets(numPlanets);
    std::vector<Force> forces(numForces);

    while (true)
    {
        // Receive updated planet data
        MPI_Recv(planets.data(), (int)sizeof(Planet) * numPlanets, MPI_BYTE, 0, TAG_PLANETS, MPI_COMM_WORLD, &status);

        // For each planet, compute gravity
        for (auto i = startIndex; i < endIndex; ++i)
        {
            auto force = computeGravity(planets, i);

#if VERBOSE_LOGGING
            std::cout << "Process " << me << " calculated force for planet " << i << ": ";
            force.print();
            std::cout << std::endl;
#endif

            forces[i - startIndex] = force;
        }

        MPI_Ssend(forces.data(), (int)sizeof(Force) * numForces, MPI_BYTE, 0, TAG_FORCES, MPI_COMM_WORLD);
    }
}

[[noreturn]] void mainLoop(int numProcesses)
{
    int horizontalBorder = 500;
    int verticalBorder = 500;

    std::vector<Planet> planets {
            Planet(50, 100, 5, 1.0f, 1.0f, .0f),
            Planet(250, 100, 10, 10000.0f, 1.0f, .0f),
            Planet(200, 300, 30, 100000.0f, 1.5f, 1.0f)
    };

    auto numPlanets = (int)planets.size();
    std::vector<Force> forces(numPlanets);

    auto millisecondsPerLoop = 1000.0f / FRAMERATE;
    auto secondsPerLoop = 1.0f / FRAMERATE;

    // Before the loop, inform each process which indices it will have to compute and how many planets it should expect
    for (auto i = 1; i < numProcesses; ++i)
    {
        int startIndex = (i * numPlanets) / numProcesses;
        int endIndex = ((i + 1) * numPlanets) / numProcesses;

        // Send start index and end index to process
        MPI_Ssend(&startIndex, 1, MPI_INT, i, TAG_START_INDEX, MPI_COMM_WORLD);
        MPI_Ssend(&endIndex, 1, MPI_INT, i, TAG_END_INDEX, MPI_COMM_WORLD);
        MPI_Ssend(&numPlanets, 1, MPI_INT, i, TAG_NUM_PLANETS, MPI_COMM_WORLD);
    }

    MPI_Status status;

    while (true)
    {
        auto const beginTime = std::chrono::high_resolution_clock::now();
        // Update planet data
        for (auto process = 1; process < numProcesses; ++process)
        {
            MPI_Ssend(planets.data(), (int)sizeof(Planet) * numPlanets, MPI_BYTE, process, TAG_PLANETS, MPI_COMM_WORLD);
        }

        for (auto idx = 0; idx < numPlanets / numProcesses; ++idx)
        {
            forces[idx] = computeGravity(planets, idx);
        }

        // Wait for calculations
        for (auto process = 1; process < numProcesses; ++process)
        {
            int startIndex = (process * numPlanets) / numProcesses;
            int endIndex = ((process + 1) * numPlanets) / numProcesses;

#if VERBOSE_LOGGING
            std::cout << "Receiving forces: " << forces.data() + startIndex << " to " << forces.data() + endIndex << " from process " << process << std::endl;
#endif

            MPI_Recv(&forces[startIndex], (int)sizeof(Force) * (endIndex - startIndex), MPI_BYTE, process, TAG_FORCES, MPI_COMM_WORLD, &status);
        }

        auto const endTime = std::chrono::high_resolution_clock::now();

        out << "Loop took " << duration_cast<std::chrono::milliseconds>(endTime-beginTime).count() << "milliseconds" << std::endl;

        for (auto idx = 0; idx < numPlanets; ++idx)
        {
            auto expectedForce = computeGravity(planets, idx);
            if (!expectedForce.check(forces[idx]))
            {
#if VERBOSE_LOGGING
                std::cout << "Failed check: " << idx << "-> Expected: ";
                expectedForce.print();
                std::cout << "; Received: ";
                forces[idx].print();
                std::cout << std::endl;
#endif
            }
        }

        for (auto idx = 0; idx < numPlanets; ++idx)
        {
            applyForce(planets[idx], forces[idx], secondsPerLoop, horizontalBorder, verticalBorder);
        }

        Sleep((int)millisecondsPerLoop);
    }
}

int main()
{
    MPI_Init(nullptr, nullptr);
    int me, numProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    if (me == 0)
        mainLoop(numProcesses);
    else
        worker(me);

    MPI_Finalize();
}