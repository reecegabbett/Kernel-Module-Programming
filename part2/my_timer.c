#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
// #include <asm-generic/uaccess.h>
#include <linux/kernel.h>
#include <linux/time.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Timer to print current time and elapsed time since last call");

#define ENTRY_NAME "timer"
#define PERMS 0644
#define PARENT NULL
static struct file_operations fops;
static int activated = 0;
static long ns_temp;
static long long s_temp;

// time.h reference
// https://docs.huihoo.com/doxygen/linux/kernel/3.7/include_2linux_2time_8h.html

int my_timer_proc_open(struct inode *sp_inode, struct file *sp_file) {
  // setting up variables for read?
  // printk("proc called open\n");
  return 0;
}

ssize_t my_timer_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset) {
  // this is where we print the current time and time elapsed
  // printk("proc called read\n");
  struct timespec ts = current_kernel_time();
  long ns_time = ts.tv_nsec;
  long long s_time = (long long)(ts.tv_sec);
  printk("current time: %lld.%ld", s_time, ns_time);

  if (activated != 1) {
    activated = 1;
    ns_temp = ns_time;
    s_temp = s_time;
  }
  else {
    printk("elapsed time: %lld.%ld", s_time-s_temp, ns_time-ns_temp);
    ns_temp = ns_time;
    s_temp = s_time;
  }

  return 0;
}

static int my_timer_proc_release(struct inode *sp_inode,
struct file *sp_file) {
  // printk("proc called release\n");
  return 0;
}

int my_timer_init(void) {

  // creating a proc entry /proc/timer
  printk("/proc/%s create\n", ENTRY_NAME);
  fops.open = my_timer_proc_open;
  fops.read = my_timer_proc_read;
  fops.release = my_timer_proc_release;

  if (!proc_create(ENTRY_NAME, PERMS, NULL, &fops)) {
    printk("ERROR! proc_create\n");
    remove_proc_entry(ENTRY_NAME, NULL);
    return -ENOMEM;
  }
  return 0;
}

// not sure if this is needed
module_init(my_timer_init);

void my_timer_exit(void) {
  remove_proc_entry(ENTRY_NAME, NULL);
  printk("Removing /proc/%s.\n", ENTRY_NAME);

  return;
}

// not sure if this is needed
module_exit(my_timer_exit);
