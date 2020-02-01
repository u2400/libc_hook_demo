/*
 * @Date: 2020-01-07 05:12:02
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-14 04:55:31
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/file.h>
#include <asm/paravirt.h>
#include <asm/syscall.h>
#include <linux/sys.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/binfmts.h>
#include <linux/version.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/sysfs.h>
#include <asm/unistd.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/proc_fs.h>
#include <linux/rbtree.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/limits.h>
#include <asm/cacheflush.h>
#include <asm/pgtable_types.h>
#include <linux/sched.h>

unsigned long **sys_call_table_ptr;
unsigned long original_cr0;
void *orig_sys_call_table [NR_syscalls];
typedef asmlinkage long (*origin_execvecall) (
    const char *filename,
    const char *argv,
    const char *envp);

struct sock *syshook_nl_sk = NULL;
#define SYSHOOK_NL_NUM  31

origin_execvecall orig_stub_execve;

unsigned long ** find_sys_call_table(void) {
	/* Lookup address for sys_call_table and set sct_address to it */
	return (unsigned long **)kallsyms_lookup_name("sys_call_table");
}

asmlinkage long monitor_execve_hook(
    const char *filename,
    const char *argv,
    const char *envp) {
    
    char buffer[PATH_MAX+1];
    const struct pt_regs *regs = (const struct pt_regs *)filename;
    strncpy_from_user(buffer, regs->di, strnlen_user(regs->di, PATH_MAX+1));
    printk(KERN_ALERT "%s", buffer);
    printk(KERN_ALERT "%d", current->pid);
    return orig_stub_execve(filename, argv, envp);
}

static int monitor_execve_init(void)
{
    int i = 0;
    if (!(sys_call_table_ptr = find_sys_call_table())){
        printk(KERN_ALERT "Get sys_call_table failed.\n");
        return 0;
    }
    printk(KERN_ALERT "Get sys_call_table success.\n");

    original_cr0 = read_cr0();
    write_cr0(original_cr0 & ~0x00010000);
    printk(KERN_ALERT "Loading module monitor_execve, sys_call_table at %p\n", sys_call_table_ptr);
    
    for(i = 0; i < NR_syscalls - 1; i ++) {
        orig_sys_call_table[i] = sys_call_table_ptr[i];
    }

    orig_stub_execve = (void *)(sys_call_table_ptr[__NR_execve]);
    sys_call_table_ptr[__NR_execve]= (void *)monitor_execve_hook;

    write_cr0(original_cr0);
    return 0;
}

static void monitor_execve_exit(void)
{
    // netlink_kernel_release(syshook_nl_sk);
    // if (!sys_call_table_ptr){
    //     return;
    // }

    write_cr0(original_cr0 & ~0x00010000);
    sys_call_table_ptr[__NR_execve] = (void *)orig_stub_execve;
    write_cr0(original_cr0);
    
    sys_call_table_ptr = NULL;
    printk(KERN_ALERT "unload syshook_execve succ.\n");
}

module_init(monitor_execve_init);
module_exit(monitor_execve_exit);

MODULE_LICENSE("GPL");