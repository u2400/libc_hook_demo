/*
 * @Date: 2019-12-28 19:16:17
 * @LastEditors  : u2400
 * @LastEditTime : 2020-01-01 22:04:46
 */
package Go_util

import (
	"fmt"
	"errors"
)

func Catch_error(err error, arg ...interface{}) {
	fmt.Println(err)
}

func New_error(estring string) error {
	return errors.New(estring)
}