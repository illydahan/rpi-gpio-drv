#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "gpio-cdev.h"
#define MAJOR_VER 42
#define MAX_MINORS 3
#define MAX_MSG_SIZE 1024
#define BCM2711_PERI_BASE 0xFE000000
#define GPIO_BASE (BCM2711_PERI_BASE + 0X200000)

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("MRSLabs");

static unsigned int *gpio_virt_addr = NULL;

struct device_data
{
    struct cdev dev;
};

struct device_data devs[MAX_MINORS];

const struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .open = gpio_open,
    .read = gpio_read,
    .write = gpio_write,
    .release = gpio_release,
    .unlocked_ioctl = gpio_ioctl};

int init_module(void)
{
    int i, err;
    err = register_chrdev_region(MKDEV(MAJOR_VER, 0), MAX_MINORS,
                                 "my_device_driver");
    if (err != 0)
    {
        /* report error */
        return err;
    }

    for (i = 0; i < MAX_MINORS; i++)
    {
        /* initialize devs[i] fields */
        cdev_init(&devs[i].dev, &my_fops);
        cdev_add(&devs[i].dev, MKDEV(MAJOR_VER, i), 1);
    }

    gpio_virt_addr = (int *)ioremap(GPIO_BASE, PAGE_SIZE);
    if (gpio_virt_addr == NULL)
    {
        printk("gpio_cdev: Failed mapping gpio to virtual address\n");
        return -1;
    }
    return 0;
}

void cleanup_module(void)
{
    int i;

    for (i = 0; i < MAX_MINORS; i++)
    {
        /* release devs[i] fields */
        cdev_del(&devs[i].dev);
    }
    unregister_chrdev_region(MKDEV(MAJOR_VER, 0), MAX_MINORS);
}

int gpio_open(struct inode *inode, struct file *filp)
{
    printk("gpio-cdev-opend\n");
    // todo
    return 0;
}

int gpio_release(struct inode *inode, struct file *filp)
{
    printk("gpio-cdev-release\n");
    // todo
    return 0;
}

ssize_t gpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    printk("gpio-cdev-read\n");
    // todo
    return 0;
}

ssize_t gpio_write(struct file *filp, const char *buff, size_t count, loff_t *f_pos)
{
    printk("gpio-cdev-write\n");
    // todo

    long cmd_str_len = strnlen_user(buff, MAX_MSG_SIZE) - 1;

    int pin = 0;
    int value = 0;
    copy_from_user((void *)&pin, buff, sizeof(int));
    copy_from_user((void *)&value, buff + sizeof(int), sizeof(int));

    printk("value: %d, pin %d", value, pin);
    if (value != 0 && value != 1)
    {
        printk("gpio: invalid value\n");
        return -1;
    }
    else if (pin < 0 || pin > 30)
    {
        printk("gpio: invalid pin index\n");
        return -1;
    }

    if (value == 0)
    {
        printk("toggling off\n");
        gpio_pin_off(pin);
    }
    else if (value == 1)
    {
        printk("toggling on\n");
        gpio_pin_on(pin);
    }
    else
    {
        printk("toggling error\n");
        return -1;
    }
    return count;
}

long int gpio_ioctl(struct file *filp, unsigned int cmd, long unsigned int arg)
{
    // todo

    return 0;
}

static void gpio_pin_on(unsigned int pin)
{
    unsigned int fsel_index = pin / 10;
    unsigned int fsel_bitpos = pin % 10;
    unsigned int *gpio_fsel = gpio_virt_addr + fsel_index;
    unsigned int *gpio_on_register = (unsigned int *)((char *)gpio_virt_addr + 0x1c);

    *gpio_fsel &= ~(7 << (fsel_bitpos * 3));
    *gpio_fsel |= (1 << (fsel_bitpos * 3));
    *gpio_on_register |= (1 << pin);

    return;
}

static void gpio_pin_off(unsigned int pin)
{
    unsigned int *gpio_off_register = (unsigned int *)((char *)gpio_virt_addr + 0x28);
    *gpio_off_register |= (1 << pin);
    return;
}