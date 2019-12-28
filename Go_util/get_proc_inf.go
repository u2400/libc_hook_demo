/*
 * @Date: 2019-12-28 02:04:56
 * @LastEditors  : u2400
 * @LastEditTime : 2019-12-28 06:03:49
 */
package Go_util

/*
#cgo linux LDFLAGS: -lrt
#include "../C_util/get_proc_inf.h"
#include "../C_util/get_proc_inf.c"
*/
import "C"
import (
	"fmt"
	"io/ioutil"
	"strings"
	"strconv"
)

type proc_inf struct{
    Path string
    ENV string
    Cmdline string
    pcmdline string
    ppath string
    name string
	pid int
	ppid int
	pgid int
	uid int
	Euid int
	Gid int
	egid int
	sid int
	mode int
	time int
    Modify_time int64
	start_time int64
    run_time int64
    sys_time int64
    fd string
};

var inf proc_inf

func format_proc_path(path string, Pid int) string {
	var f_path string
	S_pid := fmt.Sprintf("%d", Pid)
	if Pid == -1 {
		f_path = fmt.Sprintf(path, "self")
	} else {
		f_path = fmt.Sprintf(path, S_pid)
	}
	return f_path;
}

func Get_proc_inf(Pid int) proc_inf {
	inf = proc_inf{}
	fmt.Println(Pid)
	get_proc_stat(Pid)
	get_proc_cmdline(Pid)
	return inf
}

func get_proc_stat(Pid int) {
	cont, err := ioutil.ReadFile(format_proc_path("/proc/%s/stat", Pid))
	if err != nil {
		panic(err)
	}
	content := string(cont)
	stat := strings.Split(content, " ")
	
	pid, err := strconv.Atoi(stat[0])
	if err != nil {
		panic(err)
	}

	ppid, err := strconv.Atoi(stat[3])
	if err != nil {
		panic(err)
	}

	pgid, err := strconv.Atoi(stat[4])
	if err != nil {
		panic(err)
	}

	sid, err := strconv.Atoi(stat[5])
	if err != nil {
		panic(err)
	}

	inf.pid = pid
	inf.ppid = ppid
	inf.pgid = pgid
	inf.sid = sid
}

func get_proc_cmdline(Pid int) {
	path := fmt.Sprintf("/proc/%d/cmdline", Pid)
	
	cont, err := ioutil.ReadFile(path)
	if err != nil {
		panic(err)
	}

	cmdline := string(cont)
	inf.Cmdline = cmdline
}

func get_proc_path(Pid int) {
	
}