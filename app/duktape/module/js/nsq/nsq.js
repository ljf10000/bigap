#!/bin/js

var mod  = this,
    proto  = {
		type: ["sh", "js"],
	    cache: ["none", "memory", "forever"],
	    scope: ["global", "topic"]
    };

/*
{
    "type": "sh/js",
    "content": "content",
    "filename": "filename",
    "url": "url",
    "md5": "md5 string",
    "argument": "arg1 arg2 arg3 ...",
    "cache": "none/memory/forever",
    "scope": "global/topic",
    "id": "GUID",
    "reply": "COMMAND",

    "instance": {
        "name": "name",
        "cache": "global cache",
        "topic": "topic"
    }
}
*/

function is_good_field(nsq, name) {
	var spec = proto[name];
	var field= nsq[name];
	var len = spec.length;
	var i;

	for (i=0; i<len; i++) {
		if (field===spec[i]) {
			return true;
		}
	}

	return false;
}

function is_good_spec(nsq) {
	return is_good_field(nsq, "type")
		&& is_good_field(nsq, "cache")
		&& is_good_field(nsq, "scope");
}

function is_good(nsq) {
	return is_good_spec(nsq);
}

function run(nsq) {

}

function main() {
	var json=__my__.argv[0];
	var nsq;

	try {
		nsq = JSON.parse(json);

		if (is_good(nsq)) {
			run(nsq);
		}
	} catch(e) {
		__libc__.exit(1);
	}
}

main();