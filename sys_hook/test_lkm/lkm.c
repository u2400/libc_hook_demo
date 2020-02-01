/*
 * @Date: 2020-01-10 06:36:16
 * @LastEditors: u2400
 * @LastEditTime: 2020-01-10 06:36:19
 */
#include <linux/module.h>    
#include <linux/kernel.h>   
#include <linux/init.h>        
 
static int lkm_init(void)
{
    printk("Arciryas:module loaded\n");
    return 0;    
}
 
static void lkm_exit(void)
{
    printk("Arciryas:module removed\n");
}
 
module_init(lkm_init);
module_exit(lkm_exit);