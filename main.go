/*
 * @Date: 2019-12-26 02:51:20
 * @LastEditors  : u2400
 * @LastEditTime : 2019-12-28 06:02:01
 */
package main

import (
	// "HIDS-agent/agent"
	"HIDS-agent/Go_util"
	"os"
	"fmt"
)

func main() {
	// agent.Init();
	inf := Go_util.Get_proc_inf(os.Getpid())
	fmt.Printf("%+v", inf)
}