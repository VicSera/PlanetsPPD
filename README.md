# Orbiting planets

## Physics
### Vector2:
- x
- y

    >This is a user defined object in order to represent the current speed of a planet of the 
x and y axes.


### Planet:
>This is the main participant in our experiment. We will try to simulate the orbiting of planets in the Universe.

- radius: float
    > The distance between the center of the planet and the edge of the planet
- mass: float
    > The mass of the planet will be used to compute its gravity. It is computed with the formula:
     ``mass = PI * radius * radius * density``
- color: COLORREF
    > The color which will be used in the GUI to represent the planet
- x,y: float
    > The current position of the planet in the 2D GUI.
- oldX, oldY: float
    > The old position of the planet in the 2D GUI(used for drawing purposes in the GUI)
- speed: Vector2 
    > The current speed of the planet.


### Force
> 
* dirX, dirY: float
* magnitude: float

### Logic
* computeAcceleration(p: Planet, f: Force): Vector2
> Given a planet and a force, this method will compute the acceleration for that planet. It will be computed with the formula: 
``a = f.magnitude / p.mass``

* computeAttraction(p1: Planet, p2: Planet): Force
> Given two planets, the method will return a force which represents the attraction between the two.

* applyForce(p: Planet, f: Force, seconds: float, horizontalBorder: int, verticalBorder: int)
> Given a planet, a force, the number of seconds for which we should compute, we will compute the new speed
> for the given planet.<br> 
> The force is needed for computing the acceleration.<br>
> We also need the horizontal/vertical borders because otherwise our planets will fall out of space. When a planet hits a border
> we will invert the direction and will reduce the speed by half.

* addForces(forces: vector<Force>): Force
> Given a vector of forces, the method will compute a resulting force from those one (using physics laws of computing forces)

* computeGravity(planets: vector<Planet>, planetNumber: int): Force
> For a given planet from a list of planets, we will try to compute the "gravity" between the chosen 
> planet and all the other planets, respectively. With all these "gravities", we will build a vector of
> forces which will apply to the chosen planet. After that, we will compute a final force from the vector of forces,
> with the help of addForces(...) method which will give a final, resulting force, which will be the main driver for 
> the speed and direction for the given planet in the chosen timespan


## Main

For our example, we chose 3 planets as point of reference:
* Planet1: 
  * xStart: 50 || yStart: 100
  * radius: 5
  * density: 1
  * xSpeed: 1 || ySpeed: 0
* Planet2:
  * xStart: 250 || yStart: 100
  * radius: 10
  * density: 10000
  * xSpeed: 1 || ySpeed: 0
* Planet3: 
  * xStart: 200 || yStart: 300
  * radius: 30
  * density: 100000
  * xSpeed: 1.5 || ySpeed: 1


> We have an infinite loop for the motion effect.<br>
>The flow of the program is the following:<br>
> For each of the planets we do:
>1. We compute for each planet the gravity. This will give us back a driver force.
>2. We will apply the driver force for the current planet in order change its position
>3. We draw on the GUI the new positions for creating the motion effect 


### Threaded approach:

For the threaded approach, everything will be the same but the gravity between the planets will be 
computed on different threads.

In order to achieve this, we implemented another entity:

#### Thread pool:

##### fields:
* typedef std::function<void()> Job
  >A new user defined type in order to represent a "Job" (a runnable piece of code which can be executed by a thread)

* queueMutex: mutex
  > Mutex on the queue for protecting against popping/pushing on different threads
* counterMutex: mutex
  > A mutex on the counter which keeps track of the finished jobs on the current session 
* condition: condition_variable
  >
* queue: queue<Job>
  > The queue with the jobs which must be run by the ThreadPool
* terminatePool: bool
  > A boolean which tells whether the pool has to be terminated or not
* finishedJobCount
  > Determines after how many jobs a session of the thread pool ends.

##### methods:

* addJob(job: Job)
> Method which adds a job to the queue to be executed by the pool. When it added a 
> job in the queue, it notifies the pool (through a condition_variable) that he can consume a job

* resetFinishedJobCounter()
> It resets the number of finished jobs on the session to 0

* unlockAfter(numberOfJobs: int)
> Method which sets the number of jobs after which the thread pool should finish

* waitForJob()
> The main structure of the ThreadPool:<br>
> We will create an infinite loop which waits for jobs to run. The way we achieve this is by
> waiting for a condition to be notified when someone adds a job on the queue to be consumed.<br>
> After the job is consumed, we update the counter for the jobs we run on this session.<br>


#### Threaded-main

We start by initializing the thread pool with an arbitrary number of threads and we will set them all to be waiting 
for a job (pool.waitForJob())<br>
After that, the logic will be the same as in the non threaded approach, but:
> In the first step, we will send to the thread pool to compute the gravity 
> for a planet with the help of addJob() method<br>
> We will also call pool.unlockAfter(numberOfPlanets) after we send to compute the gravity(in this way, we force the main thread 
> to wait for all the gravities to be computed before drawing to the GUI)
