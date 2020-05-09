#include <linux/module.h>     /* Needed by all modules */ 
//#include <linux/kernel.h>     /* Needed for KERN_INFO */ 
#include <linux/init.h>       /* Needed for the macros */ 
MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void){
	printk(KERN_ALERT "======================>Hello world\n");
	return 0;
}

static void hello_exit(void){
	printk(KERN_ALERT "<=================== Goodbye, cruel world\n");
}

module_init(hello_init);
module_exit(hello_exit);