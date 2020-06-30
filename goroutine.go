//
// goroutineがGCの対象外というような話がWeb上にあったため、その事実確認のためのプログラム。
//
// このプログラムは、無限にgoroutineを生成する。
// 実行したところ、メモリ使用率は一定である。
//
// したがって、goroutineのメモリはしっかり再利用されている。
//
// 104,138,175個のgoroutienを生成して、RSSは2,440、VSZは4,371,412。
//
package main

import (
	"fmt"
	"runtime"
	"sync"
	"time"
)

func generateGoroutines(c chan int) {
	generate_count := 0

	go func() {
		var mu sync.Mutex

		for {
			// Goroutineの終了を待つ。
			for runtime.NumGoroutine() > 2 {
				runtime.Gosched()
			}

			go func() {
				mu.Lock()
				generate_count++
				c <- generate_count
				mu.Unlock()
			}()
		}
	}()
}

func main() {

	c := make(chan int)
	tick := time.Tick(1 * time.Second)

	generateGoroutines(c)
	var generated int
	for {
		select {
		case i, _ := <-c:
			generated = i
		case <-tick:
			fmt.Printf("%d generated, %d active\n",
				generated, runtime.NumGoroutine())
		}
	}
}
