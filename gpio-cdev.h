#include <linux/ioctl.h>

int gpio_open(struct inode *inode, struct file *filp);

int gpio_release(struct inode *inode, struct file *filp);

ssize_t gpio_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

ssize_t gpio_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

long int gpio_ioctl(struct file *filp, unsigned int cmd, long unsigned int arg);

static void gpio_pin_on(unsigned int pin);

static void gpio_pin_off(unsigned int pin);