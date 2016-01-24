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


#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

const int max_count = 10;
static ucontext_t uc[2];

void loop1() {
	for (int cnt = 0; cnt < max_count; cnt++){
		printf("switch to [2] when cnt=%d\n", cnt);
		swapcontext(&uc[0], &uc[1]);
	}
}

void loop2() {
	for (int cnt = 0; cnt < max_count; cnt++){
		printf("switch to [1] when cnt=%d\n", cnt);
		swapcontext(&uc[1], &uc[0]);
	}
}

int main(int argc, char *argv[]){

	ucontext_t main;

	char stack1[1024*128];
	char stack2[1024*128];

	getcontext(&uc[0] );
	uc[0].uc_link = &main;
	uc[0].uc_stack.ss_sp = stack1;
	uc[0].uc_stack.ss_size = sizeof stack1;
	makecontext(&uc[0], loop1, 0);

	getcontext(&uc[1] );
	uc[1].uc_link = &main;
	uc[1].uc_stack.ss_sp = stack2;
	uc[1].uc_stack.ss_size = sizeof(stack2);
	makecontext(&uc[1], loop2, 0);

	/* start with loop1 */
	swapcontext(&main, &uc[0]);

	return EXIT_SUCCESS;
}
