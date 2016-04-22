#!/bin/js

var binding = require('file/binding');
var name, f;

name = 'test.json';
f = binding.object(name, false, JSON.parse, JSON.stringify);
f.load();
f.content.age = 30; f.save(); fmt.oprint(name, f.content);
f.content.age = 40; f.save(); fmt.oprint(name, f.content);
f.content.age = 50; f.save(); fmt.oprint(name, f.content);

name = 'test.cache.txt';
f = binding.object(name);
f.load();
f.content = 30; f.save(); print(name, f.content);
f.content = 40; f.save(); print(name, f.content);
f.content = 50; f.save(); print(name, f.content);

name = 'test.direct.txt';
f = binding.object(name, true);
f.content = 30; print(name, f.content);
f.content = 40; print(name, f.content);
f.content = 50; print(name, f.content);

/* eof */
