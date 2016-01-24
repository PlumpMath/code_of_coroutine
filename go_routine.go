/*
The MIT License (MIT)

Copyright (c) [2015] [liangchengming]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

package main

import (
	"fmt"
	"runtime"
	"sync"
)

func main() {

	var in  = make(chan int)
	var out = make(chan int)

	var await sync.WaitGroup
	runtime.GOMAXPROCS(runtime.NumCPU())


	/* 2 calculation threads */
	for i := 0; i < 2; i++ {
		await.Add(1)
		go func(in chan int, out chan int) {
			for v := range in {
				fmt.Printf("performing calculation <SUM> result = %d\n", v+1)
				out <- v+1
			}
			await.Done()
		}(in, out)
	}

	/* 1 thread providing */
	go func() {
		for i := 0; i < 10; i++ {
			in <- i
			fmt.Printf("providing payload [%d]\n", i)
		}
		close(in)
	}()

	/* 1 detecting thread */
	go func() {
		await.Wait()
		close(out)
	}()

	/* main routine, retriving all results from out[chan] */
	for _ = range out {
		/* we dont care */
	}

}
