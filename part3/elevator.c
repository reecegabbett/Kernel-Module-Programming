#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/random.h>
 
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Implement a scheduling algorithm for a pet elevator.");
 
//Global Variables
#define BUF_LEN 1000
#define MAX_CAPACITY 10
#define MAX_WEIGHT 100
#define OFFLINE 0
#define IDLE 1
#define LOADING 2
#define UP 3
#define DOWN 4
#define TOP_FLOOR 10
 
#define NEW_CAT(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 0, .weight = 15}
#define NEW_DOG(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 1, .weight = 45}
#define NEW_LIZARD(x, y, z) struct Passenger x = {.destination_floor = y, .num = z, .type = 2, .weight = 5}
 
static struct proc_dir_entry* proc_entry;
static char message[BUF_LEN];
static int len;
bool elevator_on;
 
//Structures
struct Elevator{
    struct list_head pass_list;              // Allows passenger to be part of a list; slides 10
    struct task_struct *kthread;
    struct mutex mutex;
    int state;                           // OFFLINE, IDLE, LOADING, UP, DOWN
    int cats;                                // Number of cats
    int current_floor;                       // 1-10
    int dogs;                                // Number of dogs
    int lizards;                         // Number of lizards
    int passengers;                          // Total number of passengers
    int serviced;                            // Total number of passengers serviced
    int weight;                              // Total number of passengers weighting
};
 
struct Elevator elevator;
 
typedef struct {
    int destination_floor;
    int beginning_floor;
    int type;                
    int weight;          
    struct list_head list;       // Allows passenger to be part of a list; slides 10
} Passenger;
 
typedef struct {
    struct list_head waiting_list;
} Floor;
 
struct {
    Floor **floor_list;
 
    struct mutex mutex;
} Tower;
 
 
int add_passenger(int start_floor, int destination_floor, int type);
 
 
 
 
//Returns string to write to PROC
const char* print_passengers(void) {
   
    char *buf = kmalloc(sizeof(char) * 1000, __GFP_RECLAIM);
    if (buf == NULL) {
     printk(KERN_ALERT "Error writing daita in print_passengers");
     return -ENOMEM;
    }
    /*
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
    sprintf(buf, "Number of passengers waiting: ?\n");  
    strcat(message, buf);
    sprintf(buf, "Number of passengers serviced: %d\n", elevator.serviced);                              
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 10:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 9:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 8:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 7:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 6:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 5:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 4:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 3:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 2:\n");
    strcat(message, buf);
    sprintf(buf,"[ ] Floor 1:\n");
    strcat(message, buf);
   */
    //kfree(buf);
    return buf;
}
 
//Reads PROC
static ssize_t elevator_proc_read(struct file* file, char * ubuf, size_t count, loff_t *ppos)
{
    printk(KERN_INFO "proc_read\n");
    len = strlen(message);
 
    if (*ppos > 0 || count < len)
        return 0;
 
    if (copy_to_user(ubuf, message, len))
        return -EFAULT;
 
    *ppos = len;
 
    printk(KERN_INFO "gave to user %s\n", message);
 
    return len;
}
 
//Writes PROC
static ssize_t elevator_proc_write(struct file* file, const char * ubuf, size_t count, loff_t* ppos)
{
    printk(KERN_INFO "proc_write\n");
 
    if (count > BUF_LEN)
        len = BUF_LEN;
    else
        len = count;
 
    copy_from_user(print_passengers(), ubuf, len);
 
    printk(KERN_INFO "got from user: %s\n", message);
 
    return len;
}
 
//Releases PROC
int elevator_proc_release(struct inode *sp_inode, struct file *sp_file)  {
    printk(KERN_DEBUG "elevator_proc_release\n");
    //kfree(message);    
    return 0;
}
 
//PROC File Operations
static struct file_operations procfile_fops = {
    .owner = THIS_MODULE,
    .read = elevator_proc_read,
    .write = elevator_proc_write,
    .release = elevator_proc_release,
};
 
 
//Syscalls
extern long (*STUB_start_elevator)(void);
long start_elevator(void) {
    elevator_on=true;
    if(elevator.state == OFFLINE) {
        return 0;
    }
    return 1;
}
 
extern long (*STUB_issue_request)(int,int,int);
long issue_request(int start_floor, int destination_floor, int type) {
    if(start_floor >= 0 && start_floor < TOP_FLOOR && destination_floor >= 0 && destination_floor < TOP_FLOOR) {
        add_passenger(start_floor, destination_floor, type);
        return 0;
    }
    return 1;
}
 
extern long (*STUB_stop_elevator)(void);
long stop_elevator(void) {
    if(elevator_on == true){
        elevator_on=false;
        return 0;
    }
    return 1;
}
 
//Scheduler and Helper Functions
int scheduler(void *data) {
    printk(KERN_ALERT "SUCCESS\n");
    struct Elevator *parameter = data;
    int state = parameter->state;
    while(!kthread_should_stop()) {
        if(mutex_lock_interruptible(&elevator.mutex) == 0) {
            state = parameter->state;
            mutex_unlock(&elevator.mutex);
        }
    }
    return 0;
}
 
int add_passenger(int start_floor, int destination_floor, int type){
    return 0;
}
 
//Initializations
void thread_init(struct Elevator *parameters){
   
    parameters->state = OFFLINE;                           // OFFLINE, IDLE, LOADING, UP, DOWN
    parameters->cats = 0;                                // Number of cats
    parameters->current_floor = 0;                       // 1-10
    parameters->dogs = 0;                                // Number of dogs
    parameters->lizards = 0;                         // Number of lizards
    parameters->passengers = 0;                          // Total number of passengers
    parameters->serviced = 0;                            // Total number of passengers serviced
    parameters->weight = 0;    
    mutex_init(&parameters->mutex);
    INIT_LIST_HEAD(&parameters->pass_list);              // Allows passenger to be part of a list; slides 10
    parameters->kthread = kthread_run(scheduler, parameters, "elevator");
   
}
 
 
 
 
static int elevator_init(void)
{
    STUB_start_elevator = start_elevator;
    STUB_issue_request = issue_request;
    STUB_stop_elevator = stop_elevator;
    proc_entry = proc_create("elevator", 0660, NULL, &procfile_fops);
    printk(KERN_ALERT "proc file created\n");
 
    if (proc_entry == NULL){
        return -ENOMEM;
    }
 
    elevator_on=true;
    mutex_init(&Tower.mutex);
    Tower.floor_list = kmalloc_array(TOP_FLOOR, sizeof(Floor*), __GFP_RECLAIM);
    int i =0;
    for(i; i < TOP_FLOOR; i++) {
        Floor *f = kmalloc(sizeof(Floor)*1, __GFP_RECLAIM);
        INIT_LIST_HEAD(&f->waiting_list);
        Tower.floor_list[i] = f;
    }
    thread_init(&elevator);
    return 0;
}
 
static void elevator_exit(void)
{
    STUB_start_elevator = NULL;
    STUB_issue_request = NULL;
    STUB_stop_elevator = NULL;
 
    if(mutex_lock_interruptible(&Tower.mutex) == 0) {
        Floor *f;
        int i;
        for(i = 0; i < TOP_FLOOR; i++) {
            f = Tower.floor_list[i];
            kfree(f);
        }
    }  
    proc_remove(proc_entry);
    kthread_stop(elevator.kthread);
    mutex_destroy(&elevator.mutex);
 
    printk(KERN_ALERT "exiting\n");
    return;
}
 
 
 
module_init(elevator_init);
module_exit(elevator_exit);

