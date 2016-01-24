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

ucontext_t scheduler;
static int chan = 0;
static const int None = 0;
static const int Exit = 0xFFFFFFFF;

const int  STACK_SIZE = (1024*64);
ucontext_t* mkroutine(void(*func)(ucontext_t *)) {

	ucontext_t *ctx = (ucontext_t*)malloc(sizeof(ucontext_t));
	char* stack = (char*)malloc(STACK_SIZE);

	getcontext(ctx);
	ctx->uc_stack.ss_sp = stack;
	ctx->uc_stack.ss_size = STACK_SIZE;
	ctx->uc_stack.ss_flags = 0;
	ctx->uc_link = &scheduler;
	makecontext(ctx, (void(*)(void))func, 1, ctx);

	return ctx;
}

int yield(ucontext_t *ctx, int val) {
	chan = val;
	swapcontext(ctx, &scheduler);
	return chan;
}

int send(ucontext_t *to, int val) {
	chan = val;
	swapcontext(&scheduler, to);
	return chan;
}

void close(ucontext_t* routine) {
	send(routine, Exit);
}

void plus_one(ucontext_t* self) {
	int payload = -1;
	printf("starting routine [+1] ...\n");
	while (true) {
		int plused = payload + 1;
		payload = yield(self, plused);
		if (payload == Exit) {
			break;
		}
	}
	printf("routine [+1] done!\n");
}

void multiple_by_two(ucontext_t* self) {
	int payload = -1;
	printf("starting routine [*2]...\n");
	while (true) {
		int multi = payload * 2;
		payload = yield(self, multi);
		if (payload == Exit) {
			break;
		}
	}
	printf("routine [*2] done!\n");
}


int main(int argc, const char *argv[]) {

	ucontext_t* plus  = mkroutine(plus_one);
	ucontext_t* multi = mkroutine(multiple_by_two);

	send(plus,  None); /* run plus  until match yield back here */
	send(multi, None); /* run multi until match yield back here */

	for (int k = 0; k < 4; k++) {
		printf("@k=%d\n", k);
		printf("routine[+1] yield with value=%d\n", send(plus, k));
		printf("routine[*2] yield with value=%d\n", send(multi, chan));
	}

	close(plus);
	close(multi);

	return EXIT_SUCCESS;
}


