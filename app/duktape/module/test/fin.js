#!/bin/js

var stream = require("stream");
var file = new stream.Stream("fin.js", "r");
file=null;

function close (obj) {};

var obj1 = {name:'obj1'};
__js__.destructor(false, obj1, close);
obj1=null;

var Obj2 = function() {};
__js__.destructor(true, Obj2.prototype, close);
var obj2 = new Obj2();
obj2.name = 'obj2';
obj2=null;

stream = null;

