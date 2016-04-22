#!/bin/js

var jname = 'test.json';
var cname = 'test.cache.txt';
var dname = 'test.direct.txt';

var binding = require('file/binding');

var j = binding.object(jname, false, JSON.parse, JSON.stringify); j.load();
var c = binding.object(cname); c.load();
var d = binding.object(dname, true);

fmt.oprint(jname, j.content);
print(cname, c.content);
print(dname, d.content);

/* eof */
