<!--
 * @Date: 2020-02-04 22:05:54
 * @LastEditors  : u2400
 * @LastEditTime : 2020-02-05 03:48:05
 -->

#kprobe-demo

一个简单的kprobe的demo, 这里只hook了系统调用execve函数, 其中handler_pre(pre_handler)函数在函数执行前调用, post_handler在函数执行后被调用, handler_fault函数据文章描述是调用失败时调用该函数

参考文章:
https://www.ibm.com/developerworks/cn/linux/l-kprobes.html
https://onestraw.github.io/linux/linux-struct-cred/
https://blog.csdn.net/andy205214/article/details/77148573
https://blog.csdn.net/gatieme/article/details/78310036
