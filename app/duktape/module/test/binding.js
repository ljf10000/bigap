#!/bin/js

var binding = require('file/binding');
var name, f, obj;

function bprint(name) {
	print(name, ':', __my__.readtxt(name));
}

// json cache write
name = 'test.cache_w.json';
f = binding.cache_w(name, JSON.parse, JSON.stringify);
f.load();
f.content.age = 30; f.save(); bprint(name);
f.content.age = 40; f.save(); bprint(name);
f.content.age = 50; f.save(); bprint(name);

// json cache append
name = 'test.cache_a.json';
f = binding.cache_a(name, JSON.parse, JSON.stringify);
f.content = {name: 'liujf', age: 30};
f.content.age = 30; f.save(); bprint(name);
f.content.age = 40; f.save(); bprint(name);
f.content.age = 50; f.save(); bprint(name);

// json cache line
name = 'test.cache_a.json';
f = binding.cache_l(name, JSON.parse, JSON.stringify);
f.load();
fmt.oprint(name, f);

// json direct write
name = 'test.direct_w.json';
f = binding.direct_w(name, JSON.parse, JSON.stringify);
obj = {name: 'liujf', age: 30};
obj.age = 30; f.content = obj; bprint(name);
obj.age = 40; f.content = obj; bprint(name);
obj.age = 50; f.content = obj; bprint(name);

// json direct append
name = 'test.direct_a.json';
f = binding.direct_a(name, JSON.parse, JSON.stringify);
obj = {name: 'liujf', age: 30};
obj.age = 30; f.content = obj; bprint(name);
obj.age = 40; f.content = obj; bprint(name);
obj.age = 50; f.content = obj; bprint(name);

///////////////////////////////////////////////////////////////////////
// txt cache write
name = 'test.cache_w.txt';
f = binding.cache_w(name);
f.load();
f.content = 30; f.save(); bprint(name);
f.content = 40; f.save(); bprint(name);
f.content = 50; f.save(); bprint(name);

// txt cache append
name = 'test.cache_a.txt';
f = binding.cache_a(name);
f.content = 30; f.save(); bprint(name);
f.content = 40; f.save(); bprint(name);
f.content = 50; f.save(); bprint(name);

// txt cache line
name = 'test.cache_a.txt';
f = binding.cache_l(name);
f.load();
fmt.oprint(name, f);

// txt direct write
name = 'test.direct_w.txt';
f = binding.direct_w(name);
f.content = 30; bprint(name);
f.content = 40; bprint(name);
f.content = 50; bprint(name);

// txt direct append
name = 'test.direct_a.txt';
f = binding.direct_a(name);
f.content = 30; bprint(name);
f.content = 40; bprint(name);
f.content = 50; bprint(name);

/* eof */
