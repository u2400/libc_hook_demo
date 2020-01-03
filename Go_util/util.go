/*
 * @Date: 2019-12-29 02:15:54
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-02 00:54:57
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
		Catch_error(err)
	}
	return _int
}

func format_proc_path(path string, Pid int) string {
	var f_path string
	f_path = fmt.Sprintf(path, Pid)
	return f_path;
}

func read_file_to_string(path string) string {
	cont, err := ioutil.ReadFile(path)
	if err != nil {
		Catch_error(err)
	}
	content := string(cont)
	return content
}

func read_dir(path string) []string {
	test, err := ioutil.ReadDir(path)
	if err != nil {
		Catch_error(err)
	}

	file_names := make([]string, 0, 10)
	for _, n := range test {
		file_names = append(file_names, n.Name())
	}
	return file_names
}