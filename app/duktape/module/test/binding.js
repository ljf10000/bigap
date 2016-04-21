#!/bin/js

var jname = 'test.json';
var cname = 'test.cache.txt';
var dname = 'test.direct.txt';

var bd = require('file/binding');

var j = new bd.Cache(jname, JSON.parse, JSON.stringify);
var c = new bd.Cache(cname);
var d = new bd.Direct(dname);

fmt.oprint(jname, j.content);
print(cname, c.content);
print(dname, d.content);

/* eof */
