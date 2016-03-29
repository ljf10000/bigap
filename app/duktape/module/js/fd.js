#!/bin/js

/*
* module: fd
*/

var mod = this;
var prototype = mod.constructor.prototype;
prototype.$name = prototype.$name || 'fd';

prototype.filetype = 1;
prototype.pipetype = 2;
prototype.socktype = 3;

prototype.SEEK_CUR = __libc__.SEEK_CUR;
prototype.SEEK_SET = __libc__.SEEK_SET;
prototype.SEEK_END = __libc__.SEEK_END;


/* eof */
