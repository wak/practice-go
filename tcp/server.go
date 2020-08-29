package main

import (
	"fmt"
	"io"
	"net"
	"sync"
	"time"
)

var mutex sync.Mutex
var nr_connections int = 0
var nr_concurrencies int = 0
var nr_base int = 0
var beg0 = time.Now()
var beg = time.Now()

func measure() {
	tick := time.Tick(2 * time.Second)
	for {
		<-tick
		fmt.Printf("%4.0f/s, %4.0f/s\n",
			(float64(nr_connections) / time.Now().Sub(beg0).Seconds()),
			(float64(nr_connections-nr_base) / time.Now().Sub(beg).Seconds()))
		nr_base = nr_connections
		beg = time.Now()
	}
}

func main() {
	go measure()
	ln, err := net.Listen("tcp", ":8080")

	if err != nil {
		fmt.Println("FAILED: " + err.Error())
		return
	}
	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println("FAILED: " + err.Error())
			fmt.Println(nr_connections)
		} else {
			go handleConnection(conn)
		}
	}
}

func handleConnection(c net.Conn) {
	mutex.Lock()
	nr_connections++
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
