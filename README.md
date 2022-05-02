# Discrete Event Simulation (Part 2)

## Introduction

Now that you have completed the first part and can read the specification of a discrete event system, it is time to turn our attention to the simulation per se. The objective of a simulation is to collect statistics such as the average queue length or the average waiting time in any given queue. Once a simulation is complete, you can get a sense of whether the system is "over" or "under" provisioned (if it is under-provisioned, some of the queues will grow in length as the length of the simulation increases while correctly provisioned systems would reach a stable state regardless of the length of the simulation).

Our objective, of course, is not to build an industrial-strength simulator but instead to experience what a C++ design looks like. Therefore, we are only interested in collecting a handful of statistics to demonstrate the architecture. In particular, we want the system to produce

- average length of each queue
- average waiting time in each queue
- average service time on each processor

As you will quickly realize, once the *right* architecture is in place, extending the statistics to become far more comprehensive is fairly straightforward. 

### Example

If you consider the model

    A=FIFO()
    B=ServerExp(0.08)
    C=FIFO()
    D=ServerExp(0.2)
    E=FIFO()
    F=ServerExp(0.025)
    G=FIFO()
    H=ServerExp(0.25)
    X=Exit
    K=Poisson(0.05)
    L=Poisson(0.125)
    I=Dispatch([(A,0.2),(E,0.4),(G,0.4)])
    J=Dispatch([(X,0.8),(C,0.2)])
    A->B
    C->D
    E->F
    G->H
    K->A
    L->C
    B->I
    D->G
    F->X
    H->J
    
A simulation lasting 1,000,000 time units (we stop generating arrivals once the clock is larger than 1,000,000) should produce an output similar to:

    A-Queue Len(125928)   : 3.85247
    A-wait Time(62964)    : 45.3938
    B-service time(62964) : 12.4685
    C-Queue Len(324214)   : 4.58229
    C-wait Time(162107)   : 21.996
    D-service time(162107) : 5.00355
    E-Queue Len(50390)    : 137.971
    E-wait Time(25195)    : 5518.68
    F-service time(25195) : 39.9483
    G-Queue Len(374510)   : 3.22186
    G-wait Time(187255)   : 11.8537
    H-service time(187255): 4.00137

The printout shows that the queue `A`, saw 125928 (arrivals and departures) and that the queue contained, on average, 3.85 clients. The average wait time for a client in that queue was 45.39 units (e.g., seconds). It shows that the `B` server fed by queue `A` had an average service time of 12.46 units (recall that this server yields running times distributed according to an exponential with rate 0.08. 

### Your objective

Your task is to augment your implementation with the simulation capabilities needed to run a simulation, collect samples and produce the statistics shown above. Naturally, two consecutive runs should produce different outputs as the simulator heavily relies upon random numbers to do its chore. 

### C++ 

STL has a number of classes that will prove useful to build a simulator.


####Containers
For instance, it provides ```std::queue, std::list``` for building containers


####Randomization

But it also offers ```std::random_device and std::mt19937``` for generating random numbers, 
```std::exponential_distribution<double>,std::normal_distribution<double>, std::uniform_real_distribution<double>``` to produce samples that follow specific distributions. You should take some time to pour over the documentation of those classes from the STL. Familiarity with those classes will be quite effective to write the necessary code since the servers must produce service time that follow specific distributions. 

####Poisson inter-arrival time. 

One aspect that is not directly available, but easy to build is a generator for the inter-arrival time of Poisson distribution. For illustration's sake, a partial implementation of my own Poisson class is shown below:

    //Header File
    class Poisson :public Generator {
      double                              _lambda;
      std::random_device                      _rd;
      std::mt19937                           _gen;
      std::uniform_real_distribution<double> _dis;
      ...
      double genInterArrival();
    public:
      Poisson(const std::string& n,double r);
      ~Poisson() {}
      ...
    };
    // CPP File
    Poisson::Poisson(const std::string& n,double r)
      : Generator(n),
        _rd(),
        _gen(_rd()),
        _dis(0.0,1.0)
    {
      _lambda = r;
    }
   
    double Poisson::genInterArrival()
    {
      double rnd = _dis(_gen);
      return - logf(1.0f - rnd) / _lambda;
    }
   
The class simply holds onto the generation rate `_lambda`  and creates instance variables `_rd,_gen` and `_dis` to handle the generation of 
random numbers. Namely, it needs a random device as a source of entropy, 
a generator of 'plain' random numbers (`_gen`) and an object to produce
a uniform distribution `_dis`.  The constructor initializes all those 
attributes and states that the distribution must produce numbers (uniformly distributed) in the range [0 .. 1].  The method `genInterArrival` shown last uses the uniform distribution to obtain a sample (`rnd`) and exploit that sample to generate an inter-arrival time that follows the Poisson process with specified rate `lambda`. The way it does it simply exploits the analytical definition of the density function (an exponential) and computes its inverse (with a logarithm). Similar logics are buried into the other C++ distributions that you can use. 

**Note**: remember that this is an *abridged*  version of the generator. I omitted some methods!

#### Discrete Distribution

When you are told that a random variable `X` can have 4 events `A,B,C,D` with probabilities 0.2,0.3,0.1,0.4 and you wish to generate events that conform with these probabilities, the task can be easily accomplished with a uniform distribution as well. It suffices to work with the cumulative distribution. Indeed, one must simply generate (uniformly at random) a number `a` between 0 and 1 and then recognize that:

- 0.0 ≤ a < 0.2  : You have event `A`
- 0.2 ≤ a < 0.5  : You have event `B`
- 0.5 ≤ a < 0.6  : You have event `C`
- 0.6 ≤ a ≤ 1.0  : You have event `D`

Namely, you pick an order for the event and add up the probabilities to be in the cumulative space. Then you check which range is 'hit' by the random value you generated. The range you hit gives you the event you generate. 

With these two tricks, alongside with the C++ distributions for normals and exponentials, you have the ability to generate any random value you wish. 

## Solution of previous week

I am sharing the solution of the previous week. This is not meant to replace your own code. It is actually advisable to use your own code and possibly fix it up after consulting the solution to address any shortcomings that have become apparent. I enjoin all of you to read the solution attentively and start by 'enhancing' your own solution to have a good starting point. 

## Assignment

### Concepts

The simulator needs a handful of new concepts to create an implementation. 

#### Clients

A generator (e.g., Poisson) must create new `Clients` that enter the system. 
It should do so by simply taking the time of last arrival `l`, generate an inter-arrival time `i` as discussed above and create a brand new client entering the system at time `l+i` (don't forget to update the last arrival for the next generation). 

#### Events
A simulation is a sequence of events. The creation of a new client is a primordial event of course. Other events include

- entering a buffer
- leaving a buffer
- being processed on a server
- completion on a server
- leaving the system

All these events involve a client and some of the resources in the system being simulated. The *occurrence* of an event must trigger some actions (e.g., to update some state, for instance: remove a client from a queue) and may *schedule* zero or more events into the future (e.g., when a client is worked on at a server at time `t`, one can schedule a completion event at time `t+d` where `d` is a sampled processing time on the server. 
 
##### Simulation Queue
Since a simulation is a trace of events, the simulator must maintain a *priority queue* of events (the priority is the time at which the event is supposed to occur). The simulator then has a simple task:

- it pulls the highest priority event from the queue.
- it defines `now` as being the time at which the pulled event must occur
- it executes the behavior mandated by the event
- the execution of that behavior may schedule other events.

The process repeats until the event queue is empty. 

##### Termination
To keep the execution tractable, `generators` stop creating new clients past some point in time. To provide that time, the simulator is run with a given `horizon` on the command line. For instance, one may run the simulator with the command:

    ./sim  model0.q  1000000
    
which will run the simulator on the model specified in file `model0.q` for 1 million time step at most. Namely, once the 'clock' is larger than 1,000,000 generators stop creating new clients and the system will eventually purge itself of all events. 

##### Samples

Each time an event occurs on a resource, it may be time to collect a measurement. For instance, when a client enters a buffer, you should record the time of arrival into the buffer for that client. When the client is pulled out of the buffer, you can record the time he leaves. Therefore having now both the arrival and departure time, you can compute the wait time for this client in that specific queue and record that waiting time as one sample. As the simulation proceed, samples will accumulate. At the end, you can simply loop over all the resources and compute the required statistics (average queue length and average wait time). Note that the length of the queue changes twice: once when a client enters and once when a client leaves. So if you have, at the end of the simulation, `x` samples for waiting time in a given queue, then you should have `2 * x` samples for queue length on that same queue. 

 
### C++

Once again, the code you will produce is bound to use 

- delegation
- inheritance
- polymorphism
- encapsulation

You need to start pulling all the punches to get a nice tidy code base. 
For your information, my code base has:

- 15 files
- 894 lines of readable code
- 23.3Kb of source code.

Those 894 lines is what it takes to complete the entire project (namely, it contains the solution of both the first and second week).


## A word of wisdom...

To complete this project, I strongly encourage you to add printout statements in your code to confirm that it does what you expect at every point in time. Make sure you have no leaks. Make sure the code does not crash. Make sure the results coming out are meaningful. While a little bit of variation is to be expected, you should produce simulation results similar to those provided in the driving example at the top. Do start by testing on a small instance (one queue, one server!). 

## And, above all, Have Fun!




