#!/bin/js

var loop = {
	inotify: {
		count: 0,
		handle: function (ev) {
			print("inotify times", loop.inotify.count++);

			fmt.oprint(ev);
		}
	},
	signal: {
		count: 0,
		handle: function (siginfo) {
		    print("signal times", loop.signal.count++);

		    fmt.oprint(siginfo);
		},
		sigs: [__libc__.SIGUSR1]
	},
	timer: {
		count: 0,
		handle: function (times) {
			print("timer times:", loop.timer.count++);
		},
		spec: {
			interval:{
				sec:1,nsec:0
			}
		}
	}
};

__my__.loop(loop);

print("errno=", __libc__.errno);