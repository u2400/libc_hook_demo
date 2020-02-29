/*
 * @Date: 2020-02-04 04:51:34
 * @LastEditors  : u2400
 * @LastEditTime : 2020-02-05 03:43:50
 */

#include <asm/syscall.h>
#include <linux/kallsyms.h>
#include <linux/kprobes.h>

static struct kprobe kp = {
	//设置符号名称
    .symbol_name = "__x64_sys_execve"
};

static void handler_post(struct kprobe *p, struct pt_regs *regs, unsigned long flags) {
	printk(KERN_INFO "post_handler: p->addr = 0x%p\n",
		p->addr);
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr) {
	printk(KERN_INFO "fault_handler: p->addr = 0x%p, trap #%dn",
		p->addr, trapnr);
	return 0;
}

static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
	printk(KERN_INFO "name = %s, pid = %d, uid = %d\n",
    current->comm, current->pid, current->real_cred->uid);
	return 0;
}
 
static int kprobe_init(void) {
	int ret;
	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;
 
	ret = register_kprobe(&kp);
	if (ret < 0) {
		printk(KERN_INFO "register_kprobe failed, returned %d\n", ret);
		return ret;
	}
	printk(KERN_INFO "Planted kprobe success %p\n", kp.addr);
	return 0;
}
 
static void __exit kprobe_exit(void) {
	unregister_kprobe(&kp);
	printk(KERN_INFO "kprobe at %p unregistered\n", kp.addr);
}
 
module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");