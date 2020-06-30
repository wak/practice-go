import (
	"fmt"
	"log"
	"os"
	"runtime"
	"runtime/pprof"
	"syscall"
)

func setMemory(resType int) {
	var rLimit syscall.Rlimit

	err := syscall.Getrlimit(resType, &rLimit)
	if err != nil {
		fmt.Println("Error Getting Rlimit ", err)
	}
	fmt.Println(rLimit)
	rLimit.Max = 20 * 1024 * 1024
	rLimit.Cur = 20 * 1024 * 1024
	err = syscall.Setrlimit(resType, &rLimit)
	if err != nil {
		fmt.Println("Error Setting Rlimit ", err)
	}

	err = syscall.Getrlimit(resType, &rLimit)
	if err != nil {
		fmt.Println("Error Getting Rlimit ", err)
	}
	fmt.Println(rLimit)
}

func writeMemoryProf() {
	memprofile := "mem.prof"

	f, err := os.Create(memprofile)
	if err != nil {
		log.Fatal("could not create memory profile: ", err)
	}
	defer f.Close() // error handling omitted for example
	runtime.GC()    // get up-to-date statistics
	if err := pprof.WriteHeapProfile(f); err != nil {
		log.Fatal("could not write memory profile: ", err)
	}
	fmt.Println("mem prof dumpped.")
}

func t() {
	// setMemory(syscall.RLIMIT_DATA)
	// setMemory(syscall.RLIMIT_AS)
}
