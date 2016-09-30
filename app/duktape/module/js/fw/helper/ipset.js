#!/bin/js

var mod = this;

const name = 'fw/helper/ipset';

function create (name, type, range) {
	var cmd = 'ipset create ' + name + ' ' + type + ' range ' + range;

	__my__.shell(cmd);
}

function destroy (name) {
	var cmd = 'ipset destroy ' + name;

	__my__.shell(cmd);
}

function swap (a, b) {
	var cmd = 'ipset swap' + a + ' ' + b;

	__my__.shell(cmd);
}

function add (name, entry) {
	var cmd = 'ipset add ' + name + ' ' + entry;

	__my__.shell(cmd);
}

function del (name, entry) {
	var cmd = 'ipset del ' + name + ' ' + entry;

	__my__.shell(cmd);
}

mod.create = create;
mod.destroy = destroy;
mod.add = add;
mod.del = del;
mod.swap = swap;
