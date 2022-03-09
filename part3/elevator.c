#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/list.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Implement a scheduling algorithm for a pet elevator.");

/********************************************************************/
/*   									 */
/*   						Global Variables   	 */
/*   									 */
/********************************************************************/

static struct proc_dir_entry* proc_entry;
/* Module output */
#define BUF_LEN 100
static char message[BUF_LEN];



/* 'elevator' is a variable implemented as a struct with one instance. */
struct {
    struct list_head pass_list;   			 // Allows passenger to be part of a list; slides 10
    char state[7];    						 // OFFLINE, IDLE, LOADING, UP, DOWN
    int cats;   							 // Number of cats
    int current_floor;   					 // 1-10
    int dogs;   							 // Number of dogs
    int lizards;   						 // Number of lizards
    int passengers;   						 // Total number of passengers
    int serviced;   						 // Total number of passengers serviced
    int weight;   							 // Total number of passengers weighting
} elevator;

/* List implementation */
//struct list_head_ {
//    struct list_head_ *next;
//    struct list_head_ *prev;
//}; //following slides 10

/* Passenger base class. */
struct Passenger {
    int destination_floor;
    int num;   				 // Passenger number in current list
    int type;   			 
    int weight;   		 
    struct list_head list;   	 // Allows passenger to be part of a list; slides 10
};

/* Define global variables to help readability */
#define MAX_CAPACITY 10
#define MAX_WEIGHT 100
#define BUFSIZE 100 //

/* Create new Passengers using NEW_animal(x, y, z) where
 * x is instance name; y is destination floor; z is number in current list */
#define NEW_CAT(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 0, .weight = 15}
#define NEW_DOG(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 1, .weight = 45}
#define NEW_LIZARD(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 2, .weight = 5}

/********************************************************************/
/*   									 */
/*   					 	/proc File I/O   	 */
/*   									 */
/********************************************************************/

static ssize_t elevator_proc_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos) {
    int len = strlen(message);
    
    printk(KERN_DEBUG "elevator_proc_read\n");
    
    if(*ppos > 0 || count < BUFSIZE)
   	 return 0;
//    len += sprintf(message,"irq = %d\n",irq);
//    len += sprintf(message + len,"mode = %d\n",mode);
    
    if(copy_to_user(ubuf, message, len))
   	 return -EFAULT;

    *ppos = len;
    
    return len;
}

static ssize_t elevator_proc_write(struct file *file, const char __user *ubuf, size_t count, loff_t *ppos) {
    int num, c, i, m;
    
    printk(KERN_DEBUG "elevator_proc_write\n");
    
    if(*ppos > 0 || count > BUFSIZE)
   	 return -EFAULT;
    if(copy_from_user(message,ubuf,count))
   	 return -EFAULT;
    
    num = sscanf(message,"%d %d",&i,&m);
    if(num != 2)
   	 return -EFAULT;
    
//    irq = i;
//    mode = m;
    c = strlen(message);
    *ppos = c;
    
    return c;
}

int elevator_proc_release(struct inode *sp_inode, struct file *sp_file)  {
    printk(KERN_DEBUG "elevator_proc_release\n");
    kfree(message);    
    return 0;
}


static struct file_operations procfile_fops = {
    	.owner = THIS_MODULE,
    	.read = elevator_proc_read,
    	.write = elevator_proc_write,
    	.release = elevator_proc_release,
};


/********************************************************************/
/*   									 */
/*   			 Elevator Functions and System Calls   	 */
/*   									 */
/********************************************************************/

int print_passengers(void) {
    
    char *buf = kmalloc(sizeof(char) * 100, __GFP_RECLAIM);
    if (buf == NULL) {
   	 printk(KERN_ALERT "Error writing data in print_passengers");
   	 return -ENOMEM;
    }

    sprintf(buf, "Elevator state: %s\n", elevator.state);  	 
    strcat(message, buf);
    sprintf(buf, "Elevator floor: %d\n", elevator.current_floor);   
    strcat(message, buf);
    sprintf(buf, "Current weight: %d\n", elevator.weight);   
    strcat(message, buf);
    sprintf(buf, "Elevator status: %d C, %d D, %d L\n", elevator.cats, elevator.dogs, elevator.lizards);                          	 
    strcat(message, buf);
    sprintf(buf, "Number of passengers: %d\n", elevator.passengers);   
    strcat(message, buf);
    sprintf(buf, "Number of passengers waiting: ?");   
    strcat(message, buf);
    sprintf(buf, "Number of passengers serviced: %d\n", elevator.serviced);                          	 
    strcat(message, buf);
    
    kfree(buf);
    return 0;
}



int start_elevator(void) {
	/* Activates the elevator for service. From that point onward, the elevator exists and will begin to
	service requests. This system call will return 1 if the elevator is already active, 0 for a successful
	elevator start, and -ERRORNUM if it could not initialize (e.g. -ENOMEM if it couldn’t allocate
	memory). */
    return 0;
}

int issue_request(int start_floor, int destination_floor, int type) {
	/* Creates a request for a passenger at start_floor that wishes to go to destination_floor. type is an
	indicator variable where 0 represents a cat, 1 represents a dog, and 2 represents a lizard. This
	function returns 1 if the request is not valid (one of the variables is out of range or invalid type),
	and 0 otherwise. */
    return 0;
}

int stop_elevator(void) {
	/* Deactivates the elevator. At this point, the elevator will process no more new requests (that is,
	passengers waiting on floors). However, before an elevator completely stops, it must offload all
	of its current passengers. Only after the elevator is empty may it be deactivated (state =
	OFFLINE). This function returns 1 if the elevator is already in the process of deactivating, and
	0 otherwise.  */
    return 0;
}

/********************************************************************/
/*   									 */
/*   						 Initialization   	 */
/*   									 */
/********************************************************************/


// Not sure if this needs to implement more error checking. The prompt says
// and -ERRORNUM if it could not initialize (e.g. -ENOMEM if it couldn’t
// allocate memory). -ENOMEM is implemented, but, is there more? They said e.g.

static int __init elevator_init(void) {
    proc_entry = proc_create("elevator", 0, NULL, &procfile_fops);

    if (proc_entry == NULL)
   	 printk(KERN_ALERT "Erorr in elevator initialization.");
   	 return -ENOMEM;
    
    strcpy(elevator.state, "IDLE");
    elevator.cats = 0;
    elevator.current_floor = 1;
    elevator.dogs = 0;
    elevator.lizards = 0;
    elevator.passengers = 2;
    elevator.serviced = 0;
    elevator.weight = 0;
    INIT_LIST_HEAD(&elevator.pass_list);
    
    return 0;
}

module_init(elevator_init);

static void elevator_exit(void)
{
    proc_remove(proc_entry);
    return;
}
module_exit(elevator_exit);
