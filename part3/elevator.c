#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
// #include <asm-generic/uaccess.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/list.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Implement a scheduling algorithm for a pet elevator.");

struct elevator
{
   /* data */     
};

typedef struct 
{
    /* data */
} Passenger ;

int start_elevator(void){

    /* Activates the elevator for service. From that point onward, the elevator exists and will begin to 
    service requests. This system call will return 1 if the elevator is already active, 0 for a successful
    elevator start, and -ERRORNUM if it could not initialize (e.g. -ENOMEM if it couldn’t allocate 
    memory). */

}

int issue_request (int start_floor, int destination_floor, int type){

    /* Creates a request for a passenger at start_floor that wishes to go to destination_floor. type is an 
    indicator variable where 0 represents a cat, 1 represents a dog, and 2 represents a lizard. This 
    function returns 1 if the request is not valid (one of the variables is out of range or invalid type),
    and 0 otherwise. */

}

int stop_elevator(void){

    /* Deactivates the elevator. At this point, the elevator will process no more new requests (that is, 
    passengers waiting on floors). However, before an elevator completely stops, it must offload all 
    of its current passengers. Only after the elevator is empty may it be deactivated (state = 
    OFFLINE). This function returns 1 if the elevator is already in the process of deactivating, and 
    0 otherwise.  */

}