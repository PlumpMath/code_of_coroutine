#!/usr/local/bin/python3.5
#encoding:utf-8

import sys
import time
import types
import selectors
from bisect import insort
from collections import deque
from functools import partial

#def fib(n):
#    return fib(n - 1) + fib(n - 2) if n > 1 else n

def fib(n):
    if n <= 1:
        yield n
    else:
        a = yield fib(n - 1)
        b = yield fib(n - 2)
        yield a + b
    #print("Exit{}".format(n))


#coroutine 1
def read_input():
    while True:
        line = yield sys.stdin
        n = int(line)
        fib_n = yield fib(n)
        print("fib({}) = {}".format(n, fib_n))

#coroutine 2
def print_every(message, interval):
    while True:
        print("{} - {}".format(int(time.time()), message))
        yield sleep_for_seconds(interval)

class sleep_for_seconds(object):
    def __init__(self, wait_time):
        self._wait_time = wait_time

class EventLoop(object):
    def __init__(self, *tasks):
        self._running = False
        self._selector = selectors.DefaultSelector()
        self._selector.register(sys.stdin, selectors.EVENT_READ)
        self._timers = []
        self._tasks = deque(tasks)
        # (coroutine, stack) pair of tasks waiting for input from stdin
        self._tasks_waiting_on_stdin = []

    def resume_task(self, coroutine, value=None, stack=()):
        result = coroutine.send(value)
        if isinstance(result, types.GeneratorType):
            self.schedule(result, None, (coroutine, stack))
        elif isinstance(result, sleep_for_seconds):
            self.schedule(coroutine, None, stack, time.time() + result._wait_time)
        elif result is sys.stdin:
            self._tasks_waiting_on_stdin.append((coroutine, stack))
        elif stack:
            self.schedule(stack[0], result, stack[1])
            try:
                # trigger a exception to make sure "coroutine" return.
                coroutine.send("just run to the end.  please!")
            except (GeneratorExit, StopIteration):
                pass



    def schedule(self, coroutine, value=None, stack=(), when=None):
        task = partial(self.resume_task, coroutine, value, stack)
        if when:
            insort(self._timers, (when, task))
        else:
            self._tasks.append(task)

    def run_forever(self):
        self._running = True
        while self._running:
            # First check for available IO input
            for key, mask in self._selector.select(0):
                line = key.fileobj.readline().strip()
                for task, stack in self._tasks_waiting_on_stdin:
                    self.schedule(task, line, stack)
                self._tasks_waiting_on_stdin.clear()

            # Next, run the next task
            if self._tasks:
                task = self._tasks.popleft()
                task()

            # Finally run time scheduled tasks
            while self._timers and self._timers[0][0] < time.time():
                task = self._timers[0][1]
                del self._timers[0]
                task()

        self._running = False


def main():
    loop = EventLoop()
    hello_task = print_every('Hello world!', 3)
    fib_task = read_input()
    loop.schedule(hello_task)
    loop.schedule(fib_task)
    loop.run_forever()

if __name__ == '__main__':
    main()

