/*
 * @Date: 2020-01-14 04:54:51
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-15 04:47:13
 */
struct pid_struct {
    int value;
    struct pid_struct *next;
}

static struct pid_struct *linked_list_cycle;

int add_pid(int Pid) {

}

int check_pid(int Pid) {

}

int __init(int num) {
    struct pid_struct *tmp0;
    struct pid_struct *tmp1;
    struct pid_struct *first_node;

    tmp0 = (struct pid_struct *)kmalloc(sizeof(pid_struct), GFP_KERNEL);
    first_node = tmp0;
    for(int i = num; ; i--) {
        if (i == 0) {
            linked_list_cycle = (struct pid_struct *)kmalloc(sizeof(pid_struct), GFP_KERNEL);
            linked_list_cycle->value = -1;
            linked_list_cycle->next = first_node;
        }
        tmp1 = (struct pid_struct *)kmalloc(sizeof(pid_struct), GFP_KERNEL);
        tmp0->value = -1;
        tmp0->next = tmp1;
        tmp0 = tmp1;
    }
}