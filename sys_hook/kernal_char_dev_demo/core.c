/*
 * @Date: 2020-01-03 00:38:31
 * @LastEditors  : u2400
 * @LastEditTime : 2020-02-02 21:25:16
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ip.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/netlink.h> 
#include <linux/netlink.h> 
#include<linux/kthread.h>
// #include "net_link.c"
#include "../config.h"

static int majorNumber;
static char* message = NULL;
static int size_of_message;
static struct class* test_char_class  = NULL;
static struct device* test_char_dev = NULL;
unsigned long gsize = 4096 * 10;
unsigned char *mem_msg_buf = NULL;

struct sock *nl_sk = NULL;

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

extern struct net init_net;
void sendnlmsg(char *message) {
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int len = NLMSG_SPACE(MAX_MSGSIZE);
    int slen = 0;

    if(!message || !nl_sk) {
        return;
    }

    // 为新的 sk_buffer申请空间
    skb = alloc_skb(len, GFP_KERNEL);
    if(!skb) {
        printk(KERN_ERR "my_net_link: alloc_skb Error./n");
        return;
    }

    slen = strlen(message)+1;

    //用nlmsg_put()来设置netlink消息头部
    nlh = nlmsg_put(skb, 0, 0, 0, MAX_MSGSIZE, 0);

    // 设置Netlink的控制块里的相关信息
    // NETLINK_CB(skb).pid = 0; // 消息发送者的id标识，如果是内核发的则置0
    NETLINK_CB(skb).dst_group = 5; //多播组号为5，但置成0好像也可以。

    message[slen] = '\0';
    memcpy(NLMSG_DATA(nlh), message, slen+1);

    //通过netlink_unicast()将消息发送用户空间由dstPID所指定了进程号的进程
    //netlink_unicast(nl_sk,skb,dstPID,0);
    netlink_broadcast(nl_sk, skb, 0,5, GFP_KERNEL); //发送多播消息到多播组5，这里我故意没有用1之类的“常见”值，目的就是为了证明我们上面提到的多播组号和多播组号掩码之间的对应关系
    printk("send OK!\n");
    return;
}

struct netlink_kernel_cfg cfg = {
    .groups = 5,
};

static int __init dev_init(void) {
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &cfg);

    if(!nl_sk){
        printk(KERN_ERR "my_net_link: create netlink socket error.\n");
        return 1;
    }

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

    struct task_struct *mythread = kthread_create_on_node(test, NULL, -1, "thread_sender");
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
    char message[10] = {0};
    int pid = 0;

    if (len > 9) {
        return len;
    }

    error_count = copy_from_user(message, buffer, len);
    
    printk(KERN_INFO "get string from user \"%d\"\n", pid);
    size_of_message = (int)len;
    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    return 0;
}

MODULE_LICENSE("GPL");

module_init(dev_init);
module_exit(dev_exit);