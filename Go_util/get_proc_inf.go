/*
 * @Date: 2019-12-28 02:04:56
 * @LastEditors  : u2400
 * @LastEditTime : 2019-12-30 04:46:28
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
	"strings"
	"os"
)

type proc_inf struct {
    Path string
    ENV []string
    Cmdline string
    Pcmdline string
    PPath string
    name string
	Pid int
	PPid int
	pgid int
	uid int
	Euid int
	Gid int
	Egid int
	sid int
	mode string
	time int
    Modify_time int64
	start_time int64
    run_time int64
    sys_time int64
    fd []string
};

var inf proc_inf

func Get_proc_inf(Pid int) proc_inf {
	inf = proc_inf{}
	fmt.Println(Pid)
	inf.Pid = Pid
	s_stat, stat := get_proc_status(Pid)
	inf.PPid = stat[0]
	inf.uid = stat[1]
	inf.Euid = stat[2]
	inf.Gid = stat[3]
	inf.Egid = stat[4]
	inf.name = s_stat[0]
	inf.mode = s_stat[1]

	cmdline := get_proc_cmdline(Pid)
	pcmdline := get_proc_cmdline(inf.PPid)
	inf.Cmdline = cmdline
	inf.Pcmdline = pcmdline

	path := get_proc_path(Pid)
	ppath := get_proc_path(Pid)
	inf.Path = path
	inf.PPath = ppath

	env := get_proc_env(Pid)
	inf.ENV = env
	return inf
}

func get_proc_cmdline(Pid int) string {
	path := fmt.Sprintf("/proc/%d/cmdline", Pid)
	cmdline := Read_file_to_string(path)
	return cmdline
}

func get_proc_path(Pid int) string {
	path := fmt.Sprintf("/proc/%d/exe", Pid)
	
	proc_path, err := os.Readlink(path)
	if err != nil {
		Error(err)
	}
	return proc_path
}

func get_proc_env(Pid int) []string {
	path := fmt.Sprintf("/proc/%d/environ", Pid)
	content := Read_file_to_string(path)
	env_arr := strings.Split(content, "\u0000")
	return env_arr
}

func get_proc_status(Pid int) ([]string, []int) {
	path := fmt.Sprintf("/proc/%d/status", Pid)
	content := Read_file_to_string(path)

	arr := strings.Split(content, "\n")
	arr_uid := strings.Split(arr[8], "	")
	uid := String_to_int(arr_uid[1])
	Euid := String_to_int(arr_uid[2])

	arr_gid := strings.Split(arr[9], "	")
	Gid := String_to_int(arr_gid[1])
	Egid := String_to_int(arr_gid[2])

	arr_PPid := strings.Split(arr[6], "	")
	ppid := String_to_int(arr_PPid[1])

	arr_name := strings.Split(arr[0], "	")
	name := arr_name[1]
	
	arr_mask := strings.Split(arr[1], "	")
	mask := arr_mask[1]

	return []string{name, mask}, []int{ppid, uid, Euid, Gid, Egid}
}