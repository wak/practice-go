package main

import (
	"fmt"
	"io"
	"net"
	"os"
	"sync"
	"time"
)

var mutex sync.Mutex
var nr_connections = 0
var nr_concurrencies int = 0
var wait_ns int64 = 0
var beg = time.Now()

func measure() {
	tick := time.Tick(5 * time.Second)
	for {
		<-tick
		fmt.Printf("%4.0f/s\n", (float64(nr_connections) / time.Now().Sub(beg).Seconds()))
		nr_connections = 0
		wait_ns = 0
		beg = time.Now()
	}
}

func main() {
	target := "localhost:8080"
	if len(os.Args) > 1 {
		target = os.Args[1]
	}

	go measure()

	set := make(map[string]struct{})

	for {
		// b := time.Now()
		conn, err := net.DialTimeout("tcp", target, 2000*time.Millisecond)
		// wait_ns += time.Now().Sub(b).Nanoseconds()

		if err != nil {
			fmt.Println("FAILED: " + err.Error())
			continue
		}

		t := conn.LocalAddr().String()
		if _, ok := set[t]; ok {
			fmt.Printf("reused %s\n", t)
		} else {
			set[t] = struct{}{}
		}
		nr_connections++
		// activeClose(conn)
		go passiveClose(conn)
	}
}

func activeClose(c net.Conn) {
	c.Close()
}

func passiveClose(c net.Conn) {
	mutex.Lock()
	nr_concurrencies++
	mutex.Unlock()

	var buf = make([]byte, 10)
	for {
		_, error := c.Read(buf)
		if error != nil {
			if error == io.EOF {
				break
			} else {
				panic(error)
			}
		}
	}
	c.Close()
	mutex.Lock()
	nr_concurrencies--
	mutex.Unlock()
}
