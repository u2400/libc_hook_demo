/*
 * @Date: 2019-12-28 19:16:17
 * @LastEditors  : u2400
 * @LastEditTime : 2019-12-28 23:38:41
 */
package Go_util

import (
	"fmt"
)

func Error(err error, arg ...interface{}) {
	fmt.Println(err)
}