#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <asm-generic/uaccess.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/list.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Implement a scheduling algorithm for a pet elevator.");

#define BUF_LEN 100

static struct proc_dir_entry* proc_entry;

static char msg[BUF_LEN];
static int procfs_buf_len;


//Basic read proc file
static ssize_t procfile_read(struct file* file, char * ubuf, size_t count, loff_t *ppos)
{
	printk(KERN_INFO "proc_read\n");
	procfs_buf_len = strlen(msg);

	if (*ppos > 0 || count < procfs_buf_len)
		return 0;

	if (copy_to_user(ubuf, msg, procfs_buf_len))
		return -EFAULT;

	*ppos = procfs_buf_len;

	printk(KERN_INFO "gave to user %s\n", msg);

	return procfs_buf_len;
}

//Basic write to proc file
static ssize_t procfile_write(struct file* file, const char * ubuf, size_t count, loff_t* ppos)
{
	printk(KERN_INFO "proc_write\n");

	if (count > BUF_LEN)
		procfs_buf_len = BUF_LEN;
	else
		procfs_buf_len = count;

	copy_from_user(msg, ubuf, procfs_buf_len);

	printk(KERN_INFO "got from user: %s\n", msg);

	return procfs_buf_len;
}

//Establishing read and write functions
static struct file_operations procfile_fops = {
	.owner = THIS_MODULE,
	.read = procfile_read,
	.write = procfile_write,
};

//initialize elevator
static int elevator_init(void)
{
	proc_entry = proc_create("elevator", 0666, NULL, &procfile_fops);

	if (proc_entry == NULL)
		return -ENOMEM;
	
	return 0;
}

//exit elevator
static void elevator_exit(void)
{
	proc_remove(proc_entry);
	return;
}



// Basic structs and funcs

struct elevator
{
   /* data */     
// Typedef might be illegal in Kernel programming
typedef struct Elevator {
	Passenger* pass_list;
	pass_list = kmalloc(sizeof(Passenger),__GFP_RECLAIM); 	        /* Passenger list */
	char state[7]; 													/* OFFLINE, IDLE, LOADING, UP, DOWN */
	int cats;														/* Number of cats */
	int current_floor;												/* 1-10 */
	int dogs;														/* Number of dogs */
	int lizards;													/* Number of lizards */
	int passengers;													/* Total number of passengers */
	int serviced;													/* Total number of passengers serviced */
	int weight;														/* Total number of passengers weighting */
} Elevator;

// List implementation guidelines following slides 10
struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

struct Passenger {
    /* Passenger base class */
	int destination_floor;
	int num;					/* Passenger number in current list */
	int type;				
	int weight;			 
	struct list_head list;		/* Allows passenger to be part of a list */
};

/* Create new Passengers using NEW_?(x, y, z) where x is instance name; y is destination floor; z is number in current list */
#define NEW_CAT(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 0, .weight = 15}
#define NEW_DOG(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 1, .weight = 45}
#define NEW_LIZARD(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 2, .weight = 5}

int start_elevator(void) {
    /* Activates the elevator for service. From that point onward, the elevator exists and will begin to 
    service requests. This system call will return 1 if the elevator is already active, 0 for a successful
    elevator start, and -ERRORNUM if it could not initialize (e.g. -ENOMEM if it couldn’t allocate 
    memory). */
	INIT_LIST_HEAD(); 
}

int issue_request (int start_floor, int destination_floor, int type) {
    /* Creates a request for a passenger at start_floor that wishes to go to destination_floor. type is an 
    indicator variable where 0 represents a cat, 1 represents a dog, and 2 represents a lizard. This 
    function returns 1 if the request is not valid (one of the variables is out of range or invalid type),
    and 0 otherwise. */
	
} 

int stop_elevator(void) {

    /* Deactivates the elevator. At this point, the elevator will process no more new requests (that is, 
    passengers waiting on floors). However, before an elevator completely stops, it must offload all 
    of its current passengers. Only after the elevator is empty may it be deactivated (state = 
    OFFLINE). This function returns 1 if the elevator is already in the process of deactivating, and 
    0 otherwise.  */

}


module_init(elevator_init);
module_exit(elevator_exit);