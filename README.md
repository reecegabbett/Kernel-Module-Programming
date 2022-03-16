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
- Added Loading functionality
- Bug fixing
- Started Readme

## Contents of .tar
### part1
- empty.c
- empty.trace
- part1.c
- part1.trace

### part2
- my_timer.c
- Makefile

### part3
- elevator.c
- start_elevator.c
- stop_elevator.c
- issue_request.c
- Makefile

## Bugs

### Bug name: Part
Bug details here

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

## Part 3 Functions

### print_passengers()
- Brief: Reads all of the elevator and tower data and returns a string that contains the desired output table.
- Parameters: void
- Return type: const char*

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

Additional details or limitations
