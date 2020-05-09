#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>   /* printk() */
#include <linux/slab.h>     /* kmalloc() */
#include <linux/fs.h>       /* everything... */
#include <linux/errno.h>    /* error codes */
#include <linux/types.h>    /* size_t */
#include <linux/fcntl.h>    /* O_ACCMODE */
#include <linux/cdev.h>
#include <asm/uaccess.h>    /* copy_*_user */
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Hcamael");
int scull_major =   0;
int scull_minor =   0;
int scull_nr_devs = 4;
int scull_quantum = 4000;
int scull_qset = 1000;
//struct scull_qset {
  //  void **data;
    //struct scull_qset *next;
//};

struct scull_dev {
  //  struct scull_qset *data;  /* Pointer to first quantum set. */
    //int quantum;              /* The current quantum size. */
    //int qset;                 /* The current array size. */
    //unsigned long size;       /* Amount of data stored here. */
    //unsigned int access_key;  /* Used by sculluid and scullpriv. */
    struct mutex mutex;       /* Mutual exclusion semaphore. */
    struct cdev cdev;     /* Char device structure. */
};
struct scull_dev *scull_devices;    /* allocated in scull_init_module *//*
 * Follow the list.
 */

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos)
{
    ssize_t retval = 0;
    return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)
{
    ssize_t retval = -ENOMEM; /* Value used in "goto out" statements. */    
    return retval;
}
/* Beginning of the scull device implementation. *//*
 * Empty out the scull device; must be called with the device
 * mutex held.
 */

struct file_operations scull_fops = {
    //.owner =    THIS_MODULE,
    //.llseek =   scull_llseek,
    .read =     scull_read,
    .write =    scull_write,
    // .unlocked_ioctl = scull_ioctl,
    //.open =     scull_open,
    //.release =  scull_release,
};/*
 * Set up the char_dev structure for this device.
 */


static void scull_setup_cdev(struct scull_dev *dev, int index)
{
    int err, devno = MKDEV(scull_major, scull_minor + index);    
    cdev_init(&dev->cdev, &scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &scull_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    /* Fail gracefully if need be. */
    if (err)
        printk(KERN_NOTICE "Error %d adding scull%d", err, index);
    else
        printk(KERN_INFO "scull: %d add success\n", index);
}
void scull_cleanup_module(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);    /* Get rid of our char dev entries. */
    if (scull_devices) {
        for (i = 0; i < scull_nr_devs; i++) {
            //scull_trim(scull_devices + i);
            cdev_del(&scull_devices[i].cdev);
        }
        kfree(scull_devices);
    }    /* cleanup_module is never called if registering failed. */
    unregister_chrdev_region(devno, scull_nr_devs);
    printk(KERN_INFO "scull: cleanup success\n");
}
int scull_init_module(void)
{
    int result, i;
    dev_t dev = 0;    /*
     * Get a range of minor numbers to work with, asking for a dynamic major
     * unless directed otherwise at load time.
     */
    if (scull_major) {
        dev = MKDEV(scull_major, scull_minor);
        result = register_chrdev_region(dev, scull_nr_devs, "scull");
    } else {
        result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
        scull_major = MAJOR(dev);
    }
    if (result < 0) {
        printk(KERN_WARNING "scull: can't get major %d\n", scull_major);
        return result;
    } else {
        printk(KERN_INFO "scull: get major %d success\n", scull_major);
    }        /*
     * Allocate the devices. This must be dynamic as the device number can
     * be specified at load time.
     */
    scull_devices = kmalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
    if (!scull_devices) {
        result = -ENOMEM;
        goto fail;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(struct scull_dev));        /* Initialize each device. */
    for (i = 0; i < scull_nr_devs; i++) {
        //scull_devices[i].quantum = scull_quantum;
        //tscull_devices[i].qset = scull_qset;
        mutex_init(&scull_devices[i].mutex);
        scull_setup_cdev(&scull_devices[i], i);
    }    return 0; /* succeed */  fail:
    scull_cleanup_module();
    return result;

}
module_init(scull_init_module);
module_exit(scull_cleanup_module);