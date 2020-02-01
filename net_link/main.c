/*
 * @Date: 2020-01-03 05:07:49
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-06 02:55:55
 */
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>
#include "../config.h"

#define MAX_PAYLOAD 1024 // Netlink消息的最大载荷的长度

unsigned int netlink_group_mask(unsigned int group)
{
    return group ? 1 << (group - 1) : 0;
}

int main(int argc)
{
    struct sockaddr_nl src_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    int sock_fd, retval;

    // 创建Socket
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(sock_fd == -1){
        printf("error getting socket: %s", strerror(errno));
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = PF_NETLINK;
    src_addr.nl_pid = 0; // 表示我们要从内核接收多播消息。注意：该字段为0有双重意义，另一个意义是表示我们发送的数据的目的地址是内核。
    src_addr.nl_groups = netlink_group_mask(5); // 多播组的掩码，组号来自我们执行程序时输入的第一个参数

    // 因为我们要加入到一个多播组，所以必须调用bind()。
    retval = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
    if(retval < 0){
        printf("bind failed: %s", strerror(errno));
        close(sock_fd);
        return -1;
    }

    // 为接收Netlink消息申请存储空间
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if(!nlh){
        printf("malloc nlmsghdr error!\n");
        close(sock_fd);
        return -1;
    }

    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    iov.iov_base = (void *)nlh;
    iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    // 从内核接收消息
    printf("waitinf for...\n");
    recvmsg(sock_fd, &msg, 0);
    printf("Received message: %s \n", NLMSG_DATA(nlh));
    
    close(sock_fd);

    return 0;
}