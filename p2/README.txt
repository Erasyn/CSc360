This assignment was written in C++ using the std::thread library.
This was done with permission from professor Jianpang.

Overall, I did not deviate from my initial design much, with
the exception of the mutexes, and convars to an extent as I feel
I didn't understand them as well as I do now before the assignment.

I used three mutex, one for the main track, and another for the scheduler.
The last one was used to make sure that all trains started at the same time.
There were convars for allowing all threads to start at the same time, for
threads on the main track, and notifiers after a train was off main, and when
the threads were all created. It was done with the use of one convar.

The algorithm was similar to my initial one. A train loaded in their thread,
and then the thread was told to wait by a convar. In the case of the first
train to finish loading, it would take the mutex and go on main. Notifying
a thread when it was done. Since the threads grabbed trains from the
scheduling queue, it didn't matter what thread was woken up.