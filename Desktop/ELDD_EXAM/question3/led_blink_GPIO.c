/*3.	LED Operation Using GPIOs and Kernel Timers.
a)	Write a character driver using ioctl functionality, kernel timer.
b)	Use GPIO_23 to interface led on raspberry pi and blink the led periodically using kernel timer.
c)	Use ioctl functionality to change the periodicity of led blinking
*/
// Toogle pin 23 after every 500ms interval //
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/ioctl.h> //ioctl
#include <linux/gpio.h> //GPIO


MODULE_LICENSE("GPL");
MODULE_AUTHOR("RAJENDRA");
MODULE_DESCRIPTION("Linux Kernel Timer");

#define LED_PIN 23

void simple_timer_function(struct timer_list *);

struct timer_list simple_timer;

static int  __init simple_timer_module_init (void)
{
printk(KERN_INFO "\nInit module\n");
timer_setup(&simple_timer,simple_timer_function,0);
mod_timer(&simple_timer, jiffies + msecs_to_jiffies(500));
gpio_direction_output(LED_PIN,1);
return 0;
}

void simple_timer_function(struct timer_list *timer)
{

static int count;

printk(KERN_INFO" Timerfunction \n");
printk(KERN_INFO" HZ: %d \n",HZ);
mod_timer (&simple_timer, jiffies + ( msecs_to_jiffies(500)));
if (count) 
{
gpio_direction_output(LED_PIN,1);
count = 0;
}
else 
{
gpio_direction_output(LED_PIN,0);
count = 1;
}
}


static void __exit simple_timer_module_exit (void)
{
printk(KERN_INFO "\nEND\n");
del_timer(&simple_timer);
}


module_init(simple_timer_module_init);
module_exit(simple_timer_module_exit);
