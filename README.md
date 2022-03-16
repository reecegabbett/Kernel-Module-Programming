# Kernel-Module-Programming
Operating Systems Project 2 Group 27
Code written by: Dylan Dalal, Reece Gabbett, Finley Talley

## Division of Labor
### Dylan:
- Structure Implementation
- Initialization Functions
- Troubleshooting
- Bug fixing compilation errors.
### Reece:
- Elevator Makefile Implementation
- Elevator Test Code Implementation
- Elevator Proc Read Function
- Elevator Proc Write Function
- Printing Passengers Function
- start_elevator Function and Syscall
- stop_elevator Function and Syscall
- issue_request Function and Syscall
- Add Passengers Function
- Modifying files for syscall compilation
- Scheduler Threading Locks Setup
- Added to Readme
### Finley:
- Part 1 File Structure
- Trace Files
- Setup my_timer with Functions and Headers
- Timer Makefile Implementation
- Print Timer Function
- Timer Proc Read Function
- Timer Proc Write Function
- Added most of Scheduler functionality
- Added Loading and unloading functionality
- Bug fixing
- Started and contributed to Readme

## How to Run Part 1
1. Compile the empty and part 1 files with `gcc empty.c -o empty.x` and `gcc part1.c -o part1.x` respectively. There is no makefile for part 1.
2. Obtain the strace files with `strace empty.c -o empty.x` and `strace part1.c -o part1.x`

## How to Run Part 2
1. We will assume you have installed the necessary kernel and headers.
2. Put all files of `part2/` into directory `/usr/src/linux/my_timer`
3. Enter the directory in step 2, and compile the kernel module with  `sudo make`
4. Install the kernel module with `sudo insmod my_timer.ko`
5. Run cat `/proc/timer`. Second and subsequent runs will introduced the "elapsed time" between reads

You can uninstall this kernel module with `rmmod my_timer`.

## How to Run Part 3
1. We will assume you have installed kernel 4.19 and the necessary headers.
2. Put all files of `part3/` into directory `/usr/src/linux-4.19.98/elevator`
3. Edit the necessary table files, Makefile, and syscall files to account for the three new syscalls
4. Log into root with `sudo -s` and enter te directory in step 3
5. Compile the kernel module with `make`
6. Install the kernel module with `insmod elevator.ko`
7. Compile the producer and consumer files with `gcc producer.c -o producer.x` and `gcc consumer.c -o consumer.x` respectively
8. Start the elevator with `./consumer.x --start`
9. Issue requests to the elevator with passengers with `./producer.x <number of requests>`
10. To read from proc, first update the proc file `echo > /proc/elevator` and then run `cat /proc/elevator`
11. We recommend a script for reading from proc, such as:
```
#!/bin/bash

./consumer.x --start
./producer.x 10

while :
do
    echo > /proc/elevator
    cat /proc/elevator
    echo -e "----------"
    sleep 1
done
```

Additionally, you can stop the elevator with `./consumer.x --stop`.  
You can uninstall the kernel module with `rmmod elevator`.   
We recommend that you reset your machine between starts of the elevator.

## Contents of .tar
### part1
- empty.c: an empty C file
- empty.trace: the trace file for empty.c
- part1.c: a C file with 4 system calls
- part1.trace: the trace file for part1.c

### part2
- my_timer.c: a kernel module for an elapsed seconds timer and seconds since Epoch
- Makefile: makefile for the kernel module above

### part3
- elevator.c: elevator kernel module
- start_elevator.c: start_elevator() system call
- stop_elevator.c: stop_elevator() system call
- issue_request.c: issue_request() system call
- Makefile: makefile for elevator kernel module

## Bugs

### Stalling of elevator: part 3
Type: Runtime error

When running the elevator kernel module, and it will stall and not make progress past floor 2. While we could not fully locate the source of the error, after double checking the logic of Loading, Unloading, and moving the elevator Up and Down, we are concluding the error is likely related to threading and locks. Specifically, we suspect that variables are changing while we have improperly handled the locks and other deadlocking situations.

## Data Structs in Part 3

### Elevator

- Mutex/Kthread/Pass_List: Setting up the necessary components for Threading with Locking and the Linked List.
- State: Tracks the current state of the elevator.                          
- Cats: Tracks the current number of cats on the elevator.
- Current_floor: Tracks the current floor the elevator is on.
- Dogs: Tracks the current number of dogs on the elevator.
- Lizards: Tracks the current number of lizards on the elevator.
- Passengers: Tracks the current number of passengers on the elevator.
- Serviced: Tracks the total number of passengers that have been on the elevator.
- Weight: Tracks the current weight of the elevator.

### Passenger
- Destination_floor: Stores the desired floor of the passenger.
- Beginning_floor: Stores where the passenger wants to be picked up from the elevator.
- Type: Stores which type of animal the passenger is.
- Weight: Stores how much the passenger weighs.
- List: Allows the passenger to be added to a list.

### Floor
- Waiting_list: A list that tracks which passengers are waiting to be serviced.
- Busy: A boolean that turns true when someone needs to be serviced on the elevator.
- Size: The number of people waiting on this floor to be serviced.

### Tower
- Floor_list: A list of all the floors the elevator can reach to.
- Waiting: Tracks how many people are currently waiting.
- Mutex: Allows for thread locking.

## Part 2 Functions

### print_timer()
- Brief: returns a message with the necessary timer info when called
- Parameters: void
- Return type: char*

## Part 3 Functions

### print_passengers()
- Brief: Reads all of the elevator and tower data and returns a string that contains the desired output table.
- Parameters: void
- Return type: const char*

Buffer length is limited to 0.5 GB.

### add_passenger()
- Brief: Creates a passenger, fills it's variables with the appropriate data, and adds the to the waiting list on their starting floor.
- Parameters: the start floor, the desired floor, and the type of animal
- Return type: int

### elevator_proc_read()
- Brief: When called through cat, this will read the /proc/elevator text file and print it to the console.
- Parameters: The file to read, the user buffer, the message length, and the current buffer position.
- Return type: ssize_t

### elevator_proc_write()
- Brief: When called through echo, this will write to the /proc/elevator text file the return string from the print_passengers() function.
- Parameters: The file to write, the user buffer, the message length, and the current buffer position.
- Return type: ssize_t

### elevator_proc_release()
- Brief: Releases the saved proc message.
- Parameters: file system object and the desired file.
- Return type: int

### start_elevator()
- Brief: A syscall to start the elevator, and set the state to idle.
- Parameters: void
- Return type: long

### issue_request()
- Brief: A syscall to add a passenger on a floor that makes a request to the elevator.
- Parameters: void
- Return type: long

### stop_elevator()
- Brief: A syscall to stop the elevator, and set the "on" global to false.
- Parameters: void
- Return type: long

### scheduler()
- Brief: The function that is being threaded which schedules the elevator to it's next step and will loop until the elevator is shut off.
- Parameters: a pointer to void
- Return type: int

### thread_init()
- Brief: Initializes the thread with the elevator's parameters and then runs the thread.
- Parameters: the Elevator's parameters
- Return type: void

### elevator_init()
- Brief: Sets up the syscalls and the tower's floor lists and runs the thread initializer.
- Parameters: void
- Return type: int

### elevator_exit()
- Brief: deactivates syscalls and removes any data, threads, and locks before killing the elevator.
- Parameters: void
- Return type: void
