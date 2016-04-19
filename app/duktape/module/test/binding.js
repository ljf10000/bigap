#!/bin/js

fmt.separator('binding test begin');
var binding = require('file/binding');

fmt.separator('binding txtDirectBinding test');
var direct = binding.txtDirectBinding('/tmp/txtDirectBinding');
print(direct.content);
direct.content = 'txtDirectBinding';
print(direct.content);

fmt.separator('binding jsonBinding test');
var json = binding.jsonBinding('/tmp/json_binding');
json.content.a = 1;
json.content.b = 2;
json.save();

json = binding.jsonBinding('/tmp/json_binding');
json.load();
fmt.oprint('json', json);

fmt.separator('binding test end');
