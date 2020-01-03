#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define  DEVICE_NAME "testdevice"
#define  CLASS_NAME  "test"

static int majorNumber;
static char* message = NULL;
static int size_of_message;
static struct class* test_char_class  = NULL;
static struct device* test_char_dev = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};
 
static int __init dev_init(void) {
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0) {
        return majorNumber;
    }
    
    test_char_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(test_char_class)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        return PTR_ERR(test_char_class);
    }
    
    test_char_dev = device_create(test_char_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(test_char_dev)){
        class_destroy(test_char_class);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        return PTR_ERR(test_char_dev);
    }
    return 0;
}

static void __exit dev_exit(void) {
    device_destroy(test_char_class, MKDEV(majorNumber, 0));
    class_unregister(test_char_class);
    class_destroy(test_char_class);
    unregister_chrdev(majorNumber, DEVICE_NAME);
}

static int dev_open(struct inode *inodep, struct file *filep) {
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    unsigned long error_count = 0;
    error_count = copy_to_user(buffer, message, (unsigned long)size_of_message);
    printk(KERN_INFO "put string to user\"%s\", length: %d\n", message, size_of_message);
    if (error_count == 0) {
        int buf = size_of_message;
        size_of_message = 0;
        return buf;
    }
    else {
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    unsigned long error_count = 0;

    if (message != NULL) {
        kfree(message);
    }

    printk(KERN_INFO "get string from user length:%d\n", (int)len);
    message = (char *)kmalloc(len + 2, GFP_USER);
    memset(message, 0, len + 2);
    error_count = copy_from_user(message, buffer, len);
    if (error_count != 0) {
    }
    printk(KERN_INFO "get string from user \"%s\"\n", message);
    size_of_message = (int)len;
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    return 0;
}

MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);