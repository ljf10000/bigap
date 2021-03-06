#!/bin/js

var inotify = {
	count: 0,
	handle: function (ev) {
		print("inotify times", loop.inotify.count++);

		fmt.oprint(ev);
	},
	param: [
		{
			path: "/home/liujingfei/work/big",
			mask: __libc__.IN_OPEN | __libc__.IN_ACCESS
		}
	]
};

var signal = {
	count: 0,
	handle: function (siginfo) {
	    print("signal times", loop.signal.count++);

	    fmt.oprint(siginfo);
	},
	param: [
		__libc__.SIGUSR1,
		__libc__.SIGUSR2,
		__libc__.SIGINT,
		__libc__.SIGHUP,
		__libc__.SIGTERM,
		__libc__.SIGABRT,
		__libc__.SIGBUS,
		__libc__.SIGILL,
		__libc__.SIGFPE,
		__libc__.SIGSEGV
	]
};

var timer = {
	count: 0,
	handle: function (now) {
		print("timer times:", loop.timer.count++, "now:", now);
	},
	param: {
		interval:{
			sec:5,
			nsec:0
		}
	}
};

var loop = {
	inotify: inotify,
	signal: signal,
	timer: timer
};

__my__.loop(loop);

print("errno=", __libc__.errno);
