#!/bin/js

fmt.oprint('__my__', __my__);

var empty = {};
var sb = {
	sb: 'sb'
};
var Foo = function() {
	this.name = 'foo';
};
var Bar = function() {
	this.name = 'bar';
};
var bar = new Bar();

print('bar.sb=', bar.sb);

Object.setPrototypeOf(Bar.prototype, Foo.prototype);
print('bar.sb=', bar.sb);

Object.setPrototypeOf(Foo.prototype, empty);
print('bar.sb=', bar.sb);

Object.setPrototypeOf(Foo.prototype, sb);
print('bar.sb=', bar.sb);

Foo.prototype.sb = 'not sb';
print('bar.sb=', bar.sb);

Object.setPrototypeOf(Foo.prototype, empty);
print('bar.sb=', bar.sb);

Object.setPrototypeOf(Foo.prototype, sb);
print('bar.sb=', bar.sb);

var obj = Object.create(bar);
print('obj.sb=', obj.sb);
