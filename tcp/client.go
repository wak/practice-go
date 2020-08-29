package main

import (
	"fmt"
	"net"
	"time"
)

var nr_connections = 0
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
	go measure()

	for {
		// b := time.Now()
		conn, err := net.DialTimeout("tcp", "localhost:8080", 2000*time.Millisecond)
		// wait_ns += time.Now().Sub(b).Nanoseconds()

		if err != nil {
			fmt.Println("FAILED: " + err.Error())
			continue
		}
		nr_connections++
		conn.Close()
	}
}
