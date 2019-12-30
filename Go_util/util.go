/*
 * @Date: 2019-12-29 02:15:54
 * @LastEditors  : u2400
 * @LastEditTime : 2019-12-30 02:54:30
 */
package Go_util

import(
	"fmt"
	"strconv"
	"io/ioutil"
)

func String_to_int(str string) int {
	_int, err := strconv.Atoi(str)
	if err != nil {
		Error(err)
	}
	return _int
}

func Format_proc_path(path string, Pid int) string {
	var f_path string
	S_pid := fmt.Sprintf("%d", Pid)
	if Pid == -1 {
		f_path = fmt.Sprintf(path, "self")
	} else {
		f_path = fmt.Sprintf(path, S_pid)
	}
	return f_path;
}

func Read_file_to_string(path string) string {
	cont, err := ioutil.ReadFile(path)
	if err != nil {
		Error(err)
	}
	// fmt.Println(cont)
	content := string(cont)
	return content
}