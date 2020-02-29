/*
 * @Date: 2019-12-28 02:04:56
 * @LastEditors  : u2400
 * @LastEditTime : 2020-02-01 03:01:24
 */
package Go_util

import (
	"fmt"
	"strings"
	"os"
)

type Proc_inf struct {
    Path string
    ENV []string
    Cmdline string
    Pcmdline string
    PPath string
	Name string
	Mode string
	Pid int
	PPid int
	Pgid int
	Uid int
	Euid int
	Gid int
	Egid int
	Sid int
	// Time int
	Owner_uid int
	Owner_gid int
	Create_time int
    Modify_time int64
	Start_time int64
    Run_time int64
    Sys_time int64
    Fd map[int]string
};

var inf Proc_inf

func Get_Proc_inf(Pid int) Proc_inf {

	defer func () {
		if err := recover(); err != nil {
			Catch_error(err.(error))
		}
	}()
	inf = Proc_inf{}
	fmt.Println(Pid)
	inf.Pid = Pid
	s_stat, stat := get_proc_status(Pid)
	inf.PPid = stat[0]
	inf.Uid = stat[1]
	inf.Euid = stat[2]
	inf.Gid = stat[3]
	inf.Egid = stat[4]
	inf.Name = s_stat[0]
	inf.Mode = s_stat[1]

	cmdline := get_proc_cmdline(Pid)
	pcmdline := get_proc_cmdline(inf.PPid)
	inf.Cmdline = cmdline
	inf.Pcmdline = pcmdline

	path := get_proc_path(Pid)
	ppath := get_proc_path(Pid)
	inf.Path = path
	inf.PPath = ppath

	mod_time, err := get_modify_time(path)
	if mod_time == -1 {	
		Catch_error(err)
	}
	inf.Modify_time = mod_time

	env := get_proc_env(Pid)
	inf.ENV = env
	
	// inf.Fd = get_proc_fd(Pid)
	return inf
}		

func get_proc_cmdline(Pid int) string {
	path := fmt.Sprintf("/proc/%d/cmdline", Pid)
	cmdline := read_file_to_string(path)
	return cmdline
}

func get_modify_time(path string) (int64, error) {
	fp, err := os.Open(path)
	if err != nil {
		return -1, New_error("file has been deleted")
	}
	defer fp.Close()

	stat, err := fp.Stat()
	if err != nil {
		return -1, New_error("get stat error")
	}

	return stat.ModTime().Unix(), nil
}

func get_proc_path(Pid int) string {
	path := fmt.Sprintf("/proc/%d/exe", Pid)
	
	proc_path, err := os.Readlink(path)
	if err != nil {
		Catch_error(err)
	}
	return proc_path
}

func get_proc_env(Pid int) []string {
	path := fmt.Sprintf("/proc/%d/environ", Pid)
	content := read_file_to_string(path)
	env_arr := strings.Split(content, "\u0000")
	return env_arr
}

func get_proc_status(Pid int) ([]string, []int) {
	path := fmt.Sprintf("/proc/%d/status", Pid)
	content := read_file_to_string(path)

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

func get_proc_fd(Pid int) map[string]string {
	path := fmt.Sprintf("/proc/%d/fd", Pid)
	names := read_dir(path)

	fds := make(map[string]string, len(names))
	for _, i := range names {
		fd, err := os.Readlink(path + "/" + i)
		if err != nil {
			Catch_error(err)
		}
		fds[i] = fd
	}
	// fmt.Println(fds)
	return fds
}