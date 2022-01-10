/*1.Verify User Read/Write operations using IOCTL functionality.
a)Write a Simple character driver with Open , Read ,Write , Close and Ioctl Functionality.
b)Initialize Structure in Kernel space which records kernel buffer data , size of the buffer and Recent activity(Read = 0 /Write = 1).
    struct stats
		{
                  int size;
                  char buff[];
                  int r_w;
	        };
c)Write 3 separate user programs for Read, Write and Ioctl in User application.
d)Check the status of driver using Ioctl program with command GETSTATS, which should return structure(struct stats) from kernel space.
e)User should run write application first with any string and check stats using Ioctl program and then run Read application and check stats using Ioctl program and verify the string and recent activity(r_w).
*/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/types.h> 
#include <linux/fs.h>  
#include <linux/cdev.h> //device registration
#include <linux/ioctl.h> //ioctl 
#include <linux/uaccess.h> //copy_to /from_user
#include "ioctl.h" 
#define chr IOCTL

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAJENDRA");

int chr_open(struct inode *inode,struct file *filp);
int chr_release(struct inode *inode,struct file *filp);
ssize_t chr_write(struct file *filp,const char __user *Ubuff, size_t count, loff_t *offp);
ssize_t chr_read(struct file *filp,char __user *Ubuff, size_t count, loff_t *offp);
long chr_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

struct file_operations fops=
{
 .owner  	  = THIS_MODULE,
 .open   	  = chr_open,
 .read   	  = chr_read, //read
 .write   	  = chr_write, //write
 .unlocked_ioctl  = chr_ioctl, //ioctl structure
 .release	  = chr_release,
 };
 
 struct cdev *my_cdev;
 struct Default_Data{
	int Baudrate;
	int StopBits;
	int Parity;
};

 dev_t Mydev=0;
 
 static int __init chardevice_init(void)
{

 int result;
 int MAJOR,MINOR;
 result=alloc_chrdev_region(&Mydev,0,1,"IOCTL");
 MAJOR=MAJOR(Mydev);
 MINOR=MINOR(Mydev);
 printk(KERN_INFO "\nThis is init function\n");
 printk(KERN_ALERT "\nThe Major no.= %d and Minor no.=%d\n",MAJOR,MINOR);
 if(result<0)
 {
 printk(KERN_ALERT "\n The region is not obtain \n");
 return(-1);
 }
 my_cdev = cdev_alloc();
 my_cdev ->ops = &fops;
  
 result= cdev_add(my_cdev,Mydev,1);
 if(result<0)
 {
 printk(KERN_ALERT "\n The char driver as not been created \n");
 unregister_chrdev_region(Mydev,1);
 }
 return 0;
 }
 
 void __exit chardevice_exit(void)
{
int MAJOR,MINOR;
MAJOR=MAJOR(Mydev);
MINOR=MINOR(Mydev);
printk(KERN_ALERT "The Major no.= %d and Minor no.=%d",MAJOR,MINOR);
unregister_chrdev_region(Mydev,1);
cdev_del(my_cdev);
printk(KERN_ALERT "\n Deallocated stuff GOODBYE!! \n");
}
//open fuction
int chr_open(struct inode *inode,struct file *filp)
{
printk(KERN_ALERT "\nThis is kernel open call\n");
return 0;
}

//write function
ssize_t chr_write(struct file *filp,const char __user *Ubuff, size_t count, loff_t *offp)
{
 char kbuff[100];   //kernel buff[100]
 unsigned long result;
 ssize_t retvalue;
 result=copy_from_user((char *)kbuff,(char *)Ubuff,count);
 if(result==0)
 {
 printk(KERN_ALERT "\nMessage from user to kernel:\n %s \n",kbuff);
 retvalue=count;
 return retvalue;
 }
 else
 {
 printk(KERN_ALERT "\n CANNOT OPEN FILE IN KERNEL\n");
 retvalue =-EFAULT;
 return retvalue;
 }
 }

 //read function
ssize_t chr_read(struct file *filp,char __user *Ubuff, size_t count, loff_t *offp)
{
char kbuff[]="Read from kernel to User\n";
unsigned long result;
ssize_t retvalue;
result=copy_to_user((char *)Ubuff,(char*)kbuff,sizeof(kbuff));
if(result==0)
 {
 printk(KERN_ALERT "\n Data Read Success\n");
 retvalue =count;
 return retvalue;
 }
 else
 {
 printk(KERN_ALERT "\n CANNOT OPEN FILE IN KERNEL\n");
 retvalue =-EFAULT;
 return retvalue;
 }
}
 

//release function
int chr_release(struct inode *inode,struct file *filp)
{
printk(KERN_ALERT "\nThis is kernel release call\n");
return 0;
}

//ioctl function
long chr_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
unsigned int value, STOP_BITS;
char Ubuff[20];
struct Default_Data *ToSend;
printk(KERN_INFO "\n IOCTL FUNCTION" );
switch(cmd) {
 	case SET_BAUDRATE:
		get_user (value,(int __user *)arg);
		printk("\n BAUDRATE SET = %d",value);
		value=10;
		put_user(value,(int __user *)arg);
		break;
	case SET_DIRECTION_WRITE:
		printk("\n The direction is set to be write\n");
		break;
	case SET_NON_STOP_BITS:
		get_user(STOP_BITS,(int __user *)arg);
		printk("\n STOP_BITS = %d",STOP_BITS);
		break;

	case DEFAULT_DATA:
		ToSend =(struct Default_Data *)Ubuff;
		copy_from_user (Ubuff ,(struct Default_Data *)arg,sizeof(struct Default_Data));
		printk("\n Default Data = %d , %d , %d",ToSend->Baudrate,ToSend->StopBits,ToSend->Parity);
		break;
	default :
		printk("\n Command not found");
		return (-EINVAL);
}

return 0;
}
module_init(chardevice_init);
module_exit(chardevice_exit);

