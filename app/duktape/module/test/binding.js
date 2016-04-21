#!/bin/js

var jname = 'test.json';
var cname = 'test.cache.txt';
var dname = 'test.direct.txt';

var binding = require('file/binding');

var j = binding.cache(jname, JSON.parse, JSON.stringify); j.load();
var c = binding.cache(cname); c.load();
var d = binding.direct(dname);

fmt.oprint(jname, j.content);
print(cname, c.content);
print(dname, d.content);

/* eof */
