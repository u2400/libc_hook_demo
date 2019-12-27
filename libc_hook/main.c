#include <stdio.h>
#include <unistd.h>
#include "mem_share.h"

int main() {
    mem_share_init();
    int Pid = 0;
    while(1) {
        if (Pid == 5000){
            Pid = 0;
        }
        Pid ++;
        push_data(Pid);
    }
}