/**
 * @file   ebbchar.c
 * @author Derek Molloy
 * @date   2015 年 4 月 7 日 
 * @version 0.1
 * @brief   一个介绍性的字符设备驱动，作为 Linux 可加载内核驱动系列文章第二篇的示例。
 * 该模块映射到 /dev/ebbchar 文件中，并且提供一个运行于 Linux 用户空间的 C 程序，
 * 来和此内核模块进行交互。
 * @see http://www.derekmolloy.ie/ 查看完整描述和补充描述。
 */

#include <linux/init.h>           // 用于标记函数的宏，如 _init、__exit
#include <linux/module.h>         // 将内核模块加载到内核中的核心头文件 
#include <linux/device.h>         // 支持内核驱动模型的头文件 
#include <linux/kernel.h>         // 包含内核中的类型、宏和函数 
#include <linux/fs.h>             // 支持 Linux 文件系统的头文件 
//#include <asm/uaccess.h>          // 复制到用户用户空间函数需要的头文件
#include <linux/uaccess.h>
#include <linux/slab.h>

#define  DEVICE_NAME "ebbchar"    ///< 使用此值，设备将会展示在 /dev/ebbchar
#define  CLASS_NAME  "ebb"        ///< 设备类名，这是一个字符设备驱动

static int    majorNumber;                  ///< 保存主设备号，这里自动确定 
static char*   message = NULL;           ///< 用于保存从用户空间传输过来字符串的内存
static int  size_of_message;              ///< 用于记录保存的字符串长度
static int    numberOpens = 0;              ///< 用于保存设备打开次数的计数器 
static struct class*  ebbcharClass  = NULL; ///< 设备驱动类结构体指针 
static struct device* ebbcharDevice = NULL; ///< 设备驱动设备结构体指针 

// 字符设备操作的函数原型，必须在结构体定义前定义 
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

/** @brief 设备在内核中被表示为文件结构。 /linux/fs.h 中定义的 file_operations 结构体，
 * 它使用 C99 语法的结构体，列举了文件操作关联的回调函数。
 * 字符设备通常需要实现 open、read、write 和 release 函数。
 */
static struct file_operations fops =
        {
                .open = dev_open,
                .read = dev_read,
                .write = dev_write,
                .release = dev_release,
        };

/** @brief 可加载内核模块初始化函数 
 *  static 关键字限制该函数的可见性在该 C 文件之内。 The __init
 *  __init 宏对于内置驱动（非可加载内核模块）来说，只在初始化时调用，在此之后，该函数将被废弃，内存将被回收。
 *  @return 如果成功返回 0
 */
static int __init ebbchar_init(void) {
    printk(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");

    // 尝试为这个设备动态生成一个主设备号，虽然麻烦一点，但这是值得的
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
        printk(KERN_ALERT "EBBChar failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "EBBChar: registered correctly with major number %d\n", majorNumber);

    // 注册设备类
    ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(ebbcharClass)){                // 如果有错误，清理环境
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(ebbcharClass);          // 对于指针类型返回错误消息正确的方式
    }
    printk(KERN_INFO "EBBChar: device class registered correctly\n");

    // 注册设备驱动
    ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(ebbcharDevice)){               // 如果有错误，清理环境
        class_destroy(ebbcharClass);           // 重复的代码，可选方式是使用 goto 语句
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(ebbcharDevice);
    }
    printk(KERN_INFO "EBBChar: device class created correctly\n"); // 搞定，设备已经初始化
    return 0;
}

/** @brief 可加载内核模块清理函数 
 *  和初始化函数类似，该函数是静态的。__exit 宏标识如果这个代码是使用在内置驱动（非可加载内核模块）中，该函数不需要。
 */
static void __exit ebbchar_exit(void) {
    device_destroy(ebbcharClass, MKDEV(majorNumber, 0));     // 移除设备
    class_unregister(ebbcharClass);                          // 注销设备类
    class_destroy(ebbcharClass);                             // 移除设备类
    unregister_chrdev(majorNumber, DEVICE_NAME);             // 注销主设备号
    printk(KERN_INFO "EBBChar: Goodbye from the LKM!\n");
}

/** @brief 每次设备被代开的时候调用的设备打开函数 
 *  在本例中，该函数只是简单的累加 numberOpens 计数器。
 *  @param inodep 指向 inode 对象的指针（定义在 linux/fs.h 头文件中）
 *  @param filep 指向文件对象指针（定义在 linux/fs.h 头文件中）
 */
static int dev_open(struct inode *inodep, struct file *filep) {
    numberOpens++;
    printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
    return 0;
}

/** @brief 该函数在设备从用户空间读取的时候被调用，即数据从设备向用户空间传输。
 *  在本例中，通过 copy_to_user() 函数将缓冲区中的字符串发送给用户，并且捕获任何异常。
 *  @param filep 指向文件对象的指针（定义在 linux/fs.h 头文件中）
 *  @param buffer 指向本函数写入数据的缓冲区指针 
 *  @param len 缓冲区长度 
 *  @param offset 此次读取在内核缓冲区中的偏移量 
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    unsigned long error_count = 0;
    // copy_to_user 函数参数格式为 ( * to, *from, size)，如果成功返回 0
    error_count = copy_to_user(buffer, message, (unsigned long)size_of_message);

    if (error_count==0) {            // 如果为 true，表示调用成功
        printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
        return (size_of_message = 0);  // 清楚当前位置标记，并且返回 0
    }
    else {
        printk(KERN_INFO "EBBChar: Failed to send %ld characters to the user\n", error_count);
        return -EFAULT;              // 失败，返回无效地址消息（即 -14）
    }
}

/** @brief 该函数在设备想用户空间写入的时候调用，即从数据从用户发往设备。
 *  在此内核模块中，数据通过 sprintf() 函数复制到 message[] 数组中，同时字符串长度被保存到 size_of_message 变量中。
 *  @param filep 指向文件对象的指针 
 *  @param buffer 包含待写入设备字符串的缓冲区 
 *  @param len 传递到 const char 类型缓冲区的数据长度 
 *  @param offset 文件设备当前偏移量 
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
    unsigned long error_count = 0;

    if (message != NULL) {
        kfree(message);
    }

    printk(KERN_INFO "EBBChar: get string from user length:%d\n", (int)len);
    message = (char *)kmalloc(len + 2, GFP_USER);
    memset(message, 0, len + 2);
//    sprintf(message, "%s", buffer);
    error_count = copy_from_user(message, buffer, (unsigned long)len);
    if (error_count != 0) {
        printk(KERN_INFO "EBBChar: dev_write error with code %ld\n", error_count);
    }
    printk(KERN_INFO "EBBChar: get string from user \"%s\"\n", buffer);
    size_of_message = (int)len;                 // 保存当前保存的信息的长度
    printk(KERN_INFO "EBBChar: Received %ld characters from the user\n", len);
    return len;
}

/** @brief 当设备被用户空间程序关闭 / 释放时调用的函数。
 *  @param inodep 指向 inode 对象的指针（定义在 linux/fs.h 头文件中）
 *  @param filep 指向文件对象的指针（定义在 linux/fs.h 头文件中）
 */
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "EBBChar: Device successfully closed\n");
    return 0;
}

/** @brief 内核模块必须使用 linux/init.h 头文件中提供的 module_init()、module_exit() 宏，
 *  在插入和清理的时候标识对应的函数（如上所列）
 */

MODULE_LICENSE("GPL");

module_init(ebbchar_init);
module_exit(ebbchar_exit);