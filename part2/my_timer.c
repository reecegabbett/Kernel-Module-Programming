#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/gfp.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group 27");
MODULE_DESCRIPTION("Timer to print current time and elapsed time since last call");

#define ENTRY_NAME "timer"
#define PERMS 0644
#define PARENT NULL
#define BUF_LEN 2048

static struct file_operations fops;
static int activated = 0;
static long ns_temp;
static long long s_temp;
static char message[BUF_LEN];
static int len;

// time.h reference
// https://docs.huihoo.com/doxygen/linux/kernel/3.7/include_2linux_2time_8h.html

char* print_timer(void) {

  char *buf = kmalloc(sizeof(char) * 1000, GFP_USER);
  if (buf == NULL) {
   printk(KERN_ALERT "Error writing timer info");
   return -ENOMEM;
  }

  strcpy(message, "");

  struct timespec ts = current_kernel_time();
  long ns_time = ts.tv_nsec;
  long long s_time = (long long)(ts.tv_sec);
  sprintf(buf, "current time: %lld.%ld\n", s_time, ns_time);
  strcat(message, buf);

  if (activated != 1) {
    activated = 1;
    ns_temp = ns_time;
    s_temp = s_time;
  }
  else {
    int elapsed_ns = ns_time-ns_temp;
    if (elapsed_ns < 0) {
      elapsed_ns = 1-elapsed_ns;
    }
    sprintf(buf, "elapsed time: %lld.%ld\n", s_time-s_temp, elapsed_ns);
    strcat(message, buf);
    ns_temp = ns_time;
    s_temp = s_time;
  }

  printk("print timer done\n");
  return message;
}

int my_timer_proc_open(struct inode *sp_inode, struct file *sp_file) {
  printk("proc called open\n");
  strcpy(message, print_timer());
  if (message == NULL) {
    printk("Error, proc_open");
    return -ENOMEM;
  }
  return 0;
}

ssize_t my_timer_proc_read(struct file *sp_file, char *ubuf, size_t size, loff_t *offset) {
  // this is where we print the current time and time elapsed

  printk(KERN_INFO "proc_read\n");
  len = strlen(message);

  if (*offset > 0 || size < len)
      return 0;

  if (copy_to_user(ubuf, message, len))
      return -EFAULT;

  *offset = len;

  printk(KERN_INFO "gave to user %s\n", message);

  return len;

}

// static ssize_t elevator_proc_write(struct file* sp_file, const char * ubuf, size_t size, loff_t* offset)
// {
//     printk(KERN_INFO "proc_write\n");
//
//     if (size > BUF_LEN)
//         len = BUF_LEN;
//     else
//         len = size;
//
//     copy_from_user(print_timer(), ubuf, len);
//
//     printk(KERN_INFO "got from user: %s\n", message);
//
//     return len;
// }


static int my_timer_proc_release(struct inode *sp_inode,
struct file *sp_file) {
  printk("proc called release\n");
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

module_init(my_timer_init);

void my_timer_exit(void) {
  remove_proc_entry(ENTRY_NAME, NULL);
  printk("Removing /proc/%s.\n", ENTRY_NAME);

  return;
}

module_exit(my_timer_exit);
