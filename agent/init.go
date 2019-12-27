package agent

/*
#cgo linux LDFLAGS: -lrt
#include "../C_util/get_proc_inf.h"
#include "../C_util/util.h"
#include "../C_util/util.c"
#include "../config.h"
*/
import "C"
import (
	"fmt"
	"sync"
	"time"
	"unsafe"
)

var data_mem *[20]C.int
var inf_mem *[2]C.int
var pid_chan chan int

func Init() {
	wait_group := new(sync.WaitGroup)
	wait_group.Add(1)
	pid_ptr, err := new_share_mem(C.share_mem_name, C.int_size * 21)
	if (err == -1) {
		fmt.Println("new share mem error!")
		return
	}

	inf, err := new_share_mem(C.share_mem_inf_name, C.int_size * 3)
	if (err == -1) {
		fmt.Println("new share mem error!")
		return
	}

	pid_chan = make(chan int, 20)
	data_mem = (*[20]C.int)(pid_ptr)
	inf_mem = (*[2]C.int)(inf)
	inf_mem[0] = C.int(0)
	inf_mem[1] = C.int(3)
	data_mem[1] = 10
	data_mem[0] = 5
	data_mem[2] = 20
	fmt.Println(data_mem)
	fmt.Println(inf_mem)
	go data_checker()

	wait_group.Wait()
}

func data_checker() {
	for {
		if inf_mem[0] > 0 {
			inf_mem[0] --
			for _, pid := range data_mem {
				if (pid > 0) {
					fmt.Println(pid)
					pid_chan <- int(pid)
				}
			}
		}
		time.Sleep(5);
	}
}

func new_share_mem(name string, size int) (unsafe.Pointer,int) {
	ptr, err := C.new_share_mem(C.CString(name), C.ulong(size))

	if err != nil {
		fmt.Println(err);
		return nil,-1
	}

	return ptr,0
}