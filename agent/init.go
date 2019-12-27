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

var data_mem *[200]C.int
var inf_mem *[2]C.int
var pid_chan chan int
var data_mem_size int

func Init() {
	data_mem_size = C.data_mem_size
	wait_group := new(sync.WaitGroup)
	wait_group.Add(1)
	pid_ptr, err := new_share_mem(C.share_mem_name, C.int_size * data_mem_size)
	if (err == -1) {
		fmt.Println("new share mem error!")
		return
	}

	inf, err := new_share_mem(C.share_mem_inf_name, C.int_size * 3)
	if (err == -1) {
		fmt.Println("new share mem error!")
		return
	}

	pid_chan = make(chan int, data_mem_size)
	data_mem = (*[200]C.int)(pid_ptr)

	/*
	inf_mem[0] singal
	inf_mem[1] free_point
	*/
	inf_mem = (*[2]C.int)(inf)
	inf_mem[0] = C.int(0)
	inf_mem[1] = C.int(0)
	go data_checker()
	go data_getter()

	wait_group.Wait()
}

func data_checker() {
	for {
		if inf_mem[0] > 0 {
			for i := (inf_mem[1] - 1); i >= 0; i-- {
				pid := data_mem[i]
				pid_chan <- int(pid)
			}
			inf_mem[0] = 0
			inf_mem[1] = 0
		}
		time.Sleep(10000000);
	}
}

func data_getter() {
	for {
		Pid := <- pid_chan
		fmt.Println(Pid)
	}
}

func new_share_mem(name string, size int) (unsafe.Pointer,int) {
	var ptr unsafe.Pointer
	var err0, err1 error
	ptr, err0 = C.new_share_mem(C.CString(name), C.ulong(size))
	if err0 != nil {
		ptr, err1 = C.open_share_mem(C.CString(name), C.ulong(size))
		if err1 != nil {
			fmt.Println(err0)
			fmt.Println(err1)
		}
		return nil,-1
	}
	return ptr,0
}