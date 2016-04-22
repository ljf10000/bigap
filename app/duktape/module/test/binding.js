#!/bin/js

var binding = require('file/binding');
var name, f;

name = 'test.json';
f = binding.object(name, 'cache', JSON.parse, JSON.stringify);
f.load();
f.content.age = 30; f.save(); print(name, ':', __my__.readtxt(name));
f.content.age = 40; f.save(); print(name, ':', __my__.readtxt(name));
f.content.age = 50; f.save(); print(name, ':', __my__.readtxt(name));

name = 'test.cache.txt';
f = binding.object(name, 'cache');
f.load();
f.content = 30; f.save(); print(name, ':', __my__.readtxt(name));
f.content = 40; f.save(); print(name, ':', __my__.readtxt(name));
f.content = 50; f.save(); print(name, ':', __my__.readtxt(name));

name = 'test.direct.txt';
f = binding.object(name, 'direct');
f.content = 30; print(name, ':', __my__.readtxt(name));
f.content = 40; print(name, ':', __my__.readtxt(name));
f.content = 50; print(name, ':', __my__.readtxt(name));

name = 'test.append.txt';
f = binding.object(name, 'append', JSON.parse, JSON.stringify);
f.content.age = 30; f.save(); print(name, ':', __my__.readtxt(name));
f.content.age = 40; f.save(); print(name, ':', __my__.readtxt(name));
f.content.age = 50; f.save(); print(name, ':', __my__.readtxt(name));

/* eof */
