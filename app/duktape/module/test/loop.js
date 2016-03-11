#!/bin/js

var count = 0;

var param = {
	signal: {
		handle: function (siginfo) {
			if (siginfo.signo==__libc__.SIGUSR1) {
		                print("recv signal SIGUSR1 times", count++);
		                fmt.oprint(siginfo)
		        }
		},
		sigs: [__libc__.SIGUSR1]
	},
	timer: {
		handle: function (times) {
		        print("timer times:", times + count++);
		        print("signalfd =", param.signal.fd);
		        print("timerfd  =", param.timer.fd);
		        print("epollfd  =", param.epoll.fd);
		        print("inotifyfd=", param.inotify.fd);
		},
		spec: {interval:{sec:1,nsec:0}}
	}
};

__my__.loop(param);

print("errno=", __libc__.errno)