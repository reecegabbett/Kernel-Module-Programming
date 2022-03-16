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
#include <linux/stddef.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Implement a scheduling algorithm for a pet elevator.");

//Global Variables
#define BUF_LEN 500000000
#define MAX_CAPACITY 10
#define MAX_WEIGHT 100
#define OFFLINE 0
#define IDLE 1
#define LOADING 2
#define UP 3
#define DOWN 4
#define TOP_FLOOR 10



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
    bool busy;
    int size;

} Floor;

struct {
    Floor **floor_list;
    int waiting;
    struct mutex mutex;
} Tower;



int add_passenger(int start_floor, int destination_floor, int type);




//Returns string to write to PROC
const char* print_passengers(void) {

    char *buf = kmalloc(sizeof(char) * 100000, __GFP_RECLAIM);
    if (buf == NULL) {
     printk(KERN_ALERT "Error writing data in print_passengers");
     return -ENOMEM;
    }
    char state[10];
    switch(elevator.state){
        case IDLE :
            strcpy(state, "IDLE");
            break;
        case OFFLINE :
            strcpy(state, "OFFLINE");
            break;
        case LOADING :
            strcpy(state, "LOADING");
            break;
        case DOWN :
            strcpy(state, "DOWN");
            break;
        case UP :
            strcpy(state, "UP");
            break;

    }

    strcpy(message, "");

    sprintf(buf, "Elevator state: %s\n", state);
    strcat(message, buf);
    sprintf(buf, "Elevator floor: %d\n", elevator.current_floor+1);
    strcat(message, buf);
    sprintf(buf, "Current weight: %d\n", elevator.weight);
    strcat(message, buf);
    sprintf(buf, "Elevator status: %d C, %d D, %d L\n", elevator.cats, elevator.dogs, elevator.lizards);
    strcat(message, buf);
    sprintf(buf, "Number of passengers: %d\n", elevator.passengers);
    strcat(message, buf);
    sprintf(buf, "Number of passengers waiting: %d\n", Tower.waiting);
    strcat(message, buf);
    sprintf(buf, "Number of passengers serviced: %d\n", elevator.serviced);
    strcat(message, buf);

    int elevator_here;
    elevator_here= elevator.current_floor;
    int i =TOP_FLOOR-1;
    for (i; i >= 0; i--){
        char marker = ' ';
        if (elevator_here==i){
            marker='*';
        }
        int floor = i +1;
        sprintf(buf,"[%c] Floor %d: %d", marker, floor, Tower.floor_list[i]->size);
        strcat(message, buf);
        struct list_head* temp;
        struct list_head* dummy;
        Passenger* tempPass;
        list_for_each_safe(temp, dummy, &Tower.floor_list[i]->waiting_list){

            tempPass = list_entry(temp,Passenger,list);

            if(tempPass->type==0){
                sprintf(buf," C ");
                strcat(message, buf);

            }
            else if(tempPass->type==1){
                sprintf(buf," D ");
                strcat(message, buf);
            }
            else if(tempPass->type==2){
                sprintf(buf," L ");
                strcat(message, buf);
            }

        }


        sprintf(buf,"\n");
        strcat(message, buf);
    }


    //kfree(buf);
    return message;
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
        elevator.state = IDLE;
        return 0;
    }
    return 1;
}

extern long (*STUB_issue_request)(int,int,int);
long issue_request(int start_floor, int destination_floor, int type) {
    if(start_floor >= 0 && start_floor <= TOP_FLOOR && destination_floor >= 0 && destination_floor <= TOP_FLOOR) {
        // write(1, "HERE 1\n", 7);
        printk(KERN_ALERT "inside if statement\n");
        add_passenger(start_floor, destination_floor, type);
        printk(KERN_ALERT "passenger added\n");
        return 0;
    }
    // write(1, "HERE 2 \n", 7);
    printk(KERN_ALERT "did not make it in if statement\n");
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


        if(state==IDLE){
            //if stop_elevator has been ran
            if(elevator_on==false){
                if(mutex_lock_interruptible(&elevator.mutex) == 0) {
                    parameter->state = OFFLINE;
                    mutex_unlock(&elevator.mutex);
                }
            }
            else if (mutex_lock_interruptible(&Tower.mutex) == 0){
                if(Tower.waiting>0){
                  mutex_unlock(&Tower.mutex);

                  // if people are waiting, check which floor they're on and go
                  // in that direction
                  // int i;
                  // for (i = 0; i < TOP_FLOOR; i++) {
                  //   if (Tower.floor_list[i]->size > 0) {
                  //     if (parameter->current_floor == i+1) {
                  //       printk(KERN_ALERT, "idle set load\n");
                  //       parameter->state = LOADING;
                  //       break;
                  //     }
                  //     else if (parameter->current_floor < i+1) {
                  //       printk(KERN_ALERT, "idle set up\n");
                  //       parameter->state = UP;
                  //       break;
                  //     }
                  //     else {
                  //       printk(KERN_ALERT, "idle set down\n");
                  //       parameter->state = DOWN;
                  //       break;
                  //     }
                  //   }
                  // }

                  parameter->state = LOADING;
                  // in LOADING, we'll check what floor has people waiting
                }
                else {
                  //nobody waiting
                  mutex_unlock(&Tower.mutex);
                  ssleep(1);
                }
            }

        }


        else if (state==OFFLINE){
            ssleep(1);
        }


        else if (state==LOADING){
            ssleep(1);
            if(mutex_lock_interruptible(&elevator.mutex) == 0) {
                //Loading and Unloading

                struct list_head *dummy, *temp;
                Passenger *temp_pass;

                struct list_head *dummy2, *temp2;
                Passenger *temp_pass2;

                int count=0;

                //remove the head/passenger who's getting off (FIFO)
                list_for_each_safe(temp, dummy, &parameter->pass_list) {
                  temp_pass = list_entry(temp, Passenger, list);
                  // this is the "first item"

                  int tmp_type=temp_pass->type;
                  list_del(temp);
                  kfree(temp_pass);

                  parameter->passengers--; // decrease total number of passengers
                  parameter->serviced++;
                  if (tmp_type == 0) { // cat
                    parameter->cats--;
                    parameter->weight -= 15;
                  } else if (tmp_type == 1) { // dog
                    parameter->dogs--;
                    parameter->weight -= 45;
                  } else {//lizard
                    parameter->lizards--;
                    parameter->weight -= 5;
                  }
                  break; // only want to unboard first passenger


                }

                // feed in waiting passengers until capacity is full
                // update floor and tower

                list_for_each_safe(temp2, dummy2, &Tower.floor_list[parameter->current_floor-1]->waiting_list) {
                  // will the next passenger still fit the weight limit and capacity limit
                  if (((temp_pass2->weight + parameter->weight) <= 100) && ((parameter->passengers+1)<=10)) {
                    // add this passenger to the elevator
                    list_add_tail(&temp_pass2->list, &parameter->pass_list);
                    // floor and tower stats are updated in the add_passenger
                    // update elevator stats
                    parameter->passengers++;
                    parameter->weight += temp_pass2->weight;
                    if (temp_pass2->type == 0) {
                      parameter->cats++;
                    } else if (temp_pass2->type == 1) {
                      parameter->dogs++;
                    } else {
                      parameter->lizards++;
                    }

                    // make next item the head
                    // LIST_HEAD(temp_pass2->list.next, Passenger);
                    // remove them from the current floor's list
                    list_del(temp2);
                    kfree(temp_pass2);


                  }
                  else {
                    // the elevator is at weight limit or capacity
                    break;
                  }
                }

                // go to next floor
                Passenger * next_pass;
                next_pass = list_first_entry_or_null(&parameter->pass_list, Passenger, list);
                if (next_pass == NULL) {
                  // everyone unboarded here
                  parameter->state = IDLE;
                  return 0;
                }

                if (next_pass->destination_floor > parameter->current_floor) {
                  parameter->state = UP;
                }
                else if (next_pass->destination_floor < parameter->current_floor) {
                  parameter->state = DOWN;
                }
                else {
                  parameter->state = LOADING;
                }


                mutex_unlock(&elevator.mutex);
            }
        }


        else if(state==DOWN){
            //check current floor for passengers
            ssleep(2);
            if(mutex_lock_interruptible(&elevator.mutex) == 0) {
                //move elevator down
                parameter->current_floor--;
                // check if valid floor
                if (parameter->current_floor < 0) {
                  return -1;
                }
                mutex_unlock(&elevator.mutex);
            }
            if(mutex_lock_interruptible(&elevator.mutex) == 0) {
                if(mutex_lock_interruptible(&Tower.mutex) == 0) {
                  Passenger * next_pass;
                  next_pass = list_first_entry_or_null(&parameter->pass_list, Passenger, list);

                    // somehow we got to DOWN even though the list is empty, error
                    if (next_pass == NULL) {
                      parameter->state = IDLE;
                      return 0;
                    }
                    if (next_pass->destination_floor == parameter->current_floor) {
                      parameter->state = LOADING;
                    }
                    //check weight and shutdown before loading

                    //check if bottom floor

                    //set next state
                    mutex_unlock(&Tower.mutex);
                }
                mutex_unlock(&elevator.mutex);
            }
        }


        else if(state==UP){
            //check current floor for passengers
            ssleep(2);
            if(mutex_lock_interruptible(&elevator.mutex) == 0) {
              //move elevator up
              printk(KERN_ALERT, "pre-floor increase\n");
              parameter->current_floor++;
              printk(KERN_ALERT, "floor increased\n");
              // check if valid floor
              if (parameter->current_floor > TOP_FLOOR) {
                return -1;
              }
              mutex_unlock(&elevator.mutex);
            }
            if(mutex_lock_interruptible(&elevator.mutex) == 0) {
                if(mutex_lock_interruptible(&Tower.mutex) == 0) {
                  Passenger * next_pass;
                  next_pass = list_first_entry_or_null(&parameter->pass_list, Passenger, list);
                  printk(KERN_ALERT, "got first entry of pass_list\n");
                  // somehow we got to DOWN even though the list is empty, error
                  if (next_pass == NULL) {
                    parameter->state = IDLE;
                    return 0;
                  }

                    //check if we need to unload at floor
                    printk(KERN_ALERT, "start unload check at UP\n");
                    if (next_pass->destination_floor == parameter->current_floor) {
                      printk(KERN_ALERT, "met unload check at UP\n");
                      parameter->state = LOADING;
                    }
                    //check weight and shutdown before loading
                    //set next state
                    mutex_unlock(&Tower.mutex);
                }
                mutex_unlock(&elevator.mutex);
            }

        }


        else{
            return -1;
        }

    }
    return 0;
}


int add_passenger(int start_floor, int destination_floor, int type){


    Passenger *temp_passenger;

    temp_passenger = kmalloc(sizeof(Passenger)*1, __GFP_RECLAIM);
    start_floor--;
    destination_floor--;
    temp_passenger->type=type;
    temp_passenger->beginning_floor=start_floor;
    temp_passenger->destination_floor=destination_floor;
    if (temp_passenger->type==0){
        temp_passenger->weight=15;
    } else if (temp_passenger->type==1){
        temp_passenger->weight=45;
    } else if (temp_passenger->type==2){
       temp_passenger->weight=5;
    } else {
        return -1;
    }


    if(destination_floor == start_floor){
        return -1;
    }
    INIT_LIST_HEAD(&temp_passenger->list);
    if(mutex_lock_interruptible(&Tower.mutex)==0){
        Tower.floor_list[start_floor]->busy=true;
        list_add_tail(&temp_passenger->list, &Tower.floor_list[start_floor]->waiting_list);
        Tower.floor_list[start_floor]->size++;
        mutex_unlock(&Tower.mutex);
    }
    printk(KERN_ALERT "PASSENGER CREATED\n");
    Tower.waiting++;
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
    Tower.waiting=0;
    for(i; i < TOP_FLOOR; i++) {
        Floor *f = kmalloc(sizeof(Floor)*1, __GFP_RECLAIM);
        INIT_LIST_HEAD(&f->waiting_list);
        Tower.floor_list[i] = f;
        Tower.floor_list[i]->busy=false;
        Tower.floor_list[i]->size=0;
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
