# Examples of all kinds of coroutines and routines in different languages.

### infinite.cc
一个使用ucontext.h实现的双函数互相调用的demo, 编译后得到一个twist.run可执行文件

### go_routine.go
一个golang的routine协作程序, 其中有很多常用到的技巧.

### python_coroutine.py
一个python的协程代码, 用于与golang的routine作比较, 一个是系统级并发, 一个是串行的用户级并发;

### c_implemented_yield_and_send.cc
一个c/c++实现的python yield关键字和send函数, 简单封装了ucontext_t, yield可以双向传递数据;

### async.py
一个python3的异步操作demo, 分别处理两项任务.

