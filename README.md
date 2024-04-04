# COP4520 Programming Assignment 3

### [Documentation](#Program-Documentation)

### [Compile Instructions](#Compile-Instructions1)
Note: I recommend using Eustis3 for testing since it's the same environment I tested on.
- #### [Windows on Eustis3](#Compiling-for-Windows-on-Eustis3)
- #### [Mac/Linux on Eustis3](#Compiling-for-Linux-or-Mac-on-Eustis3)
- #### [Local Machine](#Compiling-for-Local-Machine)


## Program Documentation
### Problem 1
There are a bunch of different solutions for a concurrent linked list. 
However, the goal of this problem is to simply ensure that the correct amount of cards are written for each present.
In the original problem, the servants weren't able to write the correct number of cards for each present in the bag, 
which likely resulted from issues with attaching and removing from the chain of presents at the same time.
In my solution, I used lock coupling/hand-over-hand locking to address the issue with adding/removing presents at the same time.
By using this method, I was able to guarantee that each present is appropriately added and removed, even when multiple threads access the linked list.

In my linked list, I have an empty node for the head and tail.
Each node in my linked list also contains a mutex, which allows for locking/unlocking.
To add a present to the linked list, I locked two nodes at the same time, prev and curr.
In doing this, I could validate that there wouldn't be any interference from any other threads, and I could insert a present to the chain safely.
After locking two nodes, I continued to traverse the list until I found the correct position to insert at.
Throughout this traversal, I locked/unlocked nodes as necessary, so only two nodes are locked at the same time.

For removing presents, I was able to simplify the process by only looking at the first two nodes.
If the second node was the tail, I didn't do anything, but if it was anything other than the tail, I was able to remove it.
I also utilized a mutex to ensure only one thread had access to modify the two nodes, so that no issues would occur when another thread tries to modify a pointer.

For searching for a present, there are no write operations involved, so it is less prone to errors.
In searching, I simply traversed the list to check if a value exists in the chain.
However, since the number of presents is quite large at 500k and there are multiple adds/removes occuring at the same time, it is rare to find a specific present in the chain.

There are some runtime concerns with lock coupling, specifically due to thread blocking.
Some threads may block the progress of other threads by obtaining locks, which potentially slows the program down.
However, this does not change the validity of the program/concurrent linked list, as it still allows for proper add/removes and the correct number of cards written.
This method of a parallel linked list is starvation-free and deadlock-free, since a thread trying to lock a node will eventually succeed.
More information about this strategy is found in "Art of Multiprocessor Programming" book, and it's validity is explained in depth there.

### Problem 2
The solution for this problem revolves around ensuring that the 8 threads are able to take a temperature scan at the appropriate time.
In my solution, I utilized the main thread as a timer, which increases a counter every x milliseconds.
Whenever the counter increases, it also sets a flag to false, which allows a thread to scan for the temperature.
Once a thread scans the temeprature, it resets the flag to true, disallowing other threads from scanning until the next minute occurs.
Each thread continuously checks if it is allowed to scan the temperature, and they all stop once the timer goes above 60 minutes.
I utilize various mutexes to ensure only one thread is allowed to write to a variable at a time.
I also used a shared lock, which allows multiple threads to read from a variable at the same time.
In my program, I use a vector for shared memory, and it is protected with a mutex to prevent data corruption if more than one thread writes to it.
My program outputs a timer, the generated temperature scanned during that time, as well as the compiled report.

## Compile Instructions1
These instructions assume you have a UCF account. To compile without one, check out the third section.

1. If you're not on the campus WiFi, set up the UCF VPN.
2. Download and install Cisco AnyConnect from https://secure.vpn.ucf.edu/.
3. Open Cisco AnyConnect and type in `https://secure.vpn.ucf.edu`.
4. Login to establish a VPN connection.


## Compiling for Windows on Eustis3

0. (If necessary) Set up the UCF VPN.
1. Download and open MobaXterm from https://mobaxterm.mobatek.net/.
2. Establish a new SSH session by clicking `Session --> SSH`.
3. For remote host, input `eustis3.eecs.ucf.edu`.
4. Click `Specify Username` and input your NID (2 letters + numbers).
5. Leave port as 22 and click OK.
6. Double Click `eustis3.eecs.ucf.edu` in the side bar, and login using your password.
7. Download problem1.cpp/problem2.cpp from this repository and drag it into the sidebar.
8. Run `g++ problem1.cpp` and then run `./a.out` on the command line.
9. Run `g++ problem2.cpp` and then run `./a.out` on the command line.



## Compiling for Linux or Mac on Eustis3

0. (If necessary) Set up the UCF VPN.
1. Open a terminal window and type `YOURNID@eustis3.eecs.ucf.edu` to connect to eustis3. Use your actual NID instead of YOURNID.
2. Enter your password when prompted for one.
3. Download problem1.cpp and problem2.cpp from this repository.
4. Open a NEW terminal, and `cd` into the directory with problem1.cpp/problem2.cpp.
5. Transfer that file to eustis3 by doing `scp problem1.cpp YOUR_NID@eustis3.eecs.ucf.edu:~/`. Do the same for problem2.cpp. Retype password when prompted.
6. On the terminal connected to eustis3, `cd` into the directory with problem1.cpp/problem2.cpp.
7. Run `g++ problem1.cpp` and then run `./a.out` on the command line.
8. Run `g++ problem2.cpp` and then run `./a.out` on the command line.


## Compiling for Local Machine
This section assumes you don't have a UCF account, and want to compile/run the program.

**Windows/Mac/Linux**
1. Install a g++ compiler to compile C++ files.
2. Download problem1.cpp and problem2.cpp from this repository.
3. Use `g++ problem1.cpp` to compile the program and `./a.out` to run it.
4. Use `g++ problem2.cpp` to compile the program and `./a.out` to run it.

**Quick guide for installing g++:**

Windows --> Enable WSL, setup Ubuntu, run: `sudo apt-get install g++` on its command line.

Linux --> Run `sudo apt-get install g++` on the command line.

MacOS --> Google: `how to install g++ on mac` and follow those steps.


**Other OS**
1. Figure out a way to install the g++ compiler on your OS.
2. Compile the .cpp files and run them.