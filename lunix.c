#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/wrapper.h>
#include <linux/slab.h>

#include <asm/uaccess.h>

#include "lunix.h"

#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
	#include <linux/modversions.h>
	#define MODVERSIONS
#endif

static char *lunix_mem;
static unsigned int lunix_mem_size;
static stats lunix_stats;
static unsigned short lunix_is_open;
static unsigned int lunix_pos; 

static ssize_t lunix_read(struct file *file, char *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "Lunix read: \n");
	lunix_stats.reads++;
	if(lunix_pos+length > lunix_mem_size) return -EINVAL; 
	copy_to_user(buffer, lunix_mem+lunix_pos, length);
	lunix_pos+=length;
 	return length;
}

static ssize_t lunix_write(struct file *file, const char *buffer, size_t length, loff_t *offset) {
	printk(KERN_ALERT "Lunix write: \n");
	lunix_stats.writes++;
	if(lunix_pos+length > lunix_mem_size) return -EINVAL;
	copy_from_user(lunix_mem+lunix_pos, buffer, length);
	lunix_pos+=length;
	return length;
}

static loff_t lunix_llseek(struct file *file, loff_t offset, int whence) {
	printk(KERN_ALERT "Lunix llseek, current position is %d, ", lunix_pos);
	lunix_stats.lseeks++;
	switch(whence) {
		case 0: /* To offset metraei apo thn arxh */
			if( (offset>lunix_mem_size)||(offset<0) ) return -EINVAL;
			lunix_pos=offset;
			printk(KERN_ALERT "new position is %d.\n", lunix_pos);
			return lunix_pos;
		break;
		case 1: /* To offset metraei apo thn trexousa 9esh */
			if( (offset+lunix_pos>lunix_mem_size) || (offset+lunix_pos> lunix_mem_size) ) return -EINVAL;
			lunix_pos+=offset;
			printk(KERN_ALERT "new position is %d.\n", lunix_pos);
			return lunix_pos;
		break;
		case 2: /* To offset metraei apo to telos */
			if( (offset>0)||(-offset>lunix_mem_size) ) return -EINVAL;
			lunix_pos=lunix_mem_size+offset;
			printk(KERN_ALERT "new position is %d.\n", lunix_pos);
			return lunix_pos;
		break;
	}
	return -EINVAL;
}

static int lunix_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) {
	int i;
	switch(ioctl_num) {
		case LUNIX_INIT:
			printk(KERN_ALERT "lunix init message received.\n");
			kfree(lunix_mem);
			lunix_mem=(char *)kmalloc(1000*sizeof(char), GFP_KERNEL );
			lunix_mem_size=1000;
			for(i=0;i<1000;i++) lunix_mem[i]=0;
			return 0;
		break;
		case LUNIX_GETSIZE:
			printk(KERN_ALERT "lunix getsize message received, mem size is %d.\n",lunix_mem_size);
			*(int *)ioctl_param=lunix_mem_size;
			return 0;
		break;
		
		case LUNIX_SETSIZE:
			printk(KERN_ALERT "lunix setsize message received, new size is %d.\n", (int)ioctl_param);
			if(ioctl_param<0) return -EINVAL;
			kfree(lunix_mem);
			lunix_mem_size=ioctl_param;
			lunix_mem=(char *)kmalloc(lunix_mem_size*sizeof(char), GFP_KERNEL);
			for(i=0;i<lunix_mem_size;i++) lunix_mem[i]=0;
			return 0;
		break;
		case LUNIX_GETSTATS:
			printk(KERN_ALERT "lunix getstats message received\n");
			copy_to_user((stats *)ioctl_param, &lunix_stats, sizeof(stats));
			return 0;
		break;
		
	}	
	return -ENOTTY;
}

static int lunix_open(struct inode *inode, struct file *file) {
	if(lunix_is_open!=0) return -EINVAL;
	lunix_is_open++;
	lunix_stats.opens++;
	lunix_pos=0;
	MOD_INC_USE_COUNT;
	printk(KERN_ALERT "lunix is opened.\n");
	return 0;
}

static int lunix_release(struct inode *inode, struct file *file) {
	lunix_is_open--;
	lunix_stats.closes++;
	MOD_DEC_USE_COUNT;
	printk(KERN_ALERT "lunix is closed.\n");
	return 0;
}

static struct file_operations lunix_fops = {
	.open	= lunix_open,
	.release= lunix_release,
	.read	= lunix_read,
	.write	= lunix_write,
	.llseek	= lunix_llseek,
	.ioctl	= lunix_ioctl,
};

int init_module(void) {
	int res,i;
	res=register_chrdev(LUNIX_MAJOR, "lunix", &lunix_fops);
	if(res<0) {
		printk("ERROR");
		return -1;
	}
	
	lunix_mem=(char *)kmalloc(1000*sizeof(char),GFP_KERNEL);
	for(i=0;i<1000;i++) lunix_mem[i]=0;
	lunix_mem_size=1000;
	lunix_stats.opens=0;
	lunix_stats.closes=0;
	lunix_stats.reads=0;
	lunix_stats.writes=0;
	lunix_stats.lseeks=0;
	lunix_is_open=0;
	printk(KERN_ALERT "Lunix driver loaded.\n");
	return 0;
}

void cleanup_module(void) {
	kfree(lunix_mem);
	unregister_chrdev(LUNIX_MAJOR, "lunix");
	printk(KERN_ALERT "Lunix driver unloaded.\n");
}

MODULE_LICENSE("GPL"); 
MODULE_SUPPORTED_DEVICE("lunix");

